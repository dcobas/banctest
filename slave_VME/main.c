#include <stdio.h>
#include <types.h>
#include <errno.h>
#include "1553def.h"

#define SR	0x0c00

#define CR	0x0008

#define BAD(str) {_errmsg(0,str,errno);}
/********************** Control parameters ****************************/
int erreur = 0;
int go_on = 1;
int loop = 1;
int nsi = 0;
int key = -1;
int ex_code = 0;
int XILINX = 0;
/********************** Configuration parameters **********************/
int stop_after_err = 1;
int trace_all_tests = 0;
int count = 1;
int testing_error = 1;
/**********************************************************************/
char e00[] = "Erreur systeme de lecture du CSR";
char e01[] = "Error02"; 
char e02[] = "Error03";
char e03[] = "Error04";
static char *Err_mess[] = {e00,e01,e02,e03};

inter() {go_on=0;}

unsigned short buff[4200];
unsigned short data_code = 1;

int test_remote();
int test_csr();
int test_buff();
int full_buff();

void intr(sig)
int sig;
{
switch (sig) {
    case 2:
	nsi = 2;
	go_on = 0;
	loop = 0;
/*	r_t_bpr();		*/
	break;
    case 3:
	nsi = 3;
	ex_code = 1;
	printf ("\nTerminated by Your request. Cup of tea ? cofe ? ...\n");
	printf (" - O.K.  Pall Mall, double Scotch with ice & 'Morrison Hotel', please !\n");
	exit(0);
    default:
	break;
    }
}

int Err_mil(e_code)			
int e_code;
{
unsigned short csr,status;
int res;

csr = INV;
if (key == 6) {
    if ((res = set_csr(1,1,&csr,&status)) != 0) 
	printf ("Can't set INV-bit, erreur de SetCsr.");
    } else {
	if (clr_csr(1, 1, &csr, &status) != 0)
	    printf("Can't set INV-bit, erreur de SetCsr.");
	}
if (e_code == 0) {
    printf ("\n%s %d\n",Err_mess[e_code], errno);
    exit (0);
    }
if (testing_error == 1) 
    printf ("%s\n",Err_mess[e_code]);
return(0);
} 

int full_buff(data_code)			
int data_code;
{
int i, k;
switch (data_code) {
    case 1:
	for (i = 0; i < 256; i++)
	    buff[i] = 0xaaaa;
	break;
    case 2:
	for (i = 0; i < 256; i++)
	    buff[i] = 0x5555;
	break;
    case 3:
	for (i = 0; i < 256; i++)
	    buff[i] = 0xffff;
	break;
    case 4:
	for (i = 0; i < 256; i++) {
	    buff[i] = 0xff00;
	    }
	break;
    case 5:
	for (i = 0; i < 256; i++) {
	    buff[i] = 0x00ff;
	    }
	break;
    case 6:
	for (i = 0; i <= 256; i = i + 16) {
	    buff[i] = 0xfffe;
	    for (k = i; k < i + 16; k++) 
		buff[k+1] = (buff[k] << 1) + 1;
	    }
	break;
    default:
	break;
    }
}

wait_for_csr(pattern,delai,debug,flg)
register u_short pattern,delai;
{
	unsigned short csr,status;
	do {
		if (debug) printf("Attente du CSR [%04X] :%3d s.\r",pattern,delai);
		if (get_csr(1,1,&csr,&status)==-1)
			return(_errmsg(0,"Probleme [%d] avec get_csr",errno));
		if ((csr & pattern)==pattern) {
			if (debug) printf("\nDetection CSR OK.\n");
			return(1);
		}
		if (go_on==0) return(0);
		if (flg) sleep(1);
	} while (--delai);
	return(0);
}

int r_t_bpr()
{
int resp = 0;
int i, j;
u_short status;
if ((resp = get_tx_buf(1, 1, 64, buff, &status)) != 0) {
    printf ("I can't read transmit_buffer. Error = %d\n", resp);
    return (-1);
    }
if ((resp = get_rx_buf(1, 1, 64, &buff[2000], &status)) != 0) {
    printf ("I can't read transmit_buffer. Error = %d\n", resp);
    return (-1);
    }
printf ("\n\tTx\t\t\t\t\t\tRx\n");
for (i = 0; i < 7; i++) {
    for (j = 0; j <= 3; j++) 
	printf ("%4X  ", buff[i*4 + j]);
    printf ("\t");
    for (j = 0; j <= 3; j++) 
	printf ("%4X  ", buff[i*4 + j + 2000]);
    printf ("\n");
    }
}

test_loc(XILINX,debug)
{
	int res,err=0;

	printf("\tTEST du CSR :");
	res = tst_CSR(1,1,XILINX,stop_after_err, trace_all_tests, testing_error);
	if (res == 0) printf("OK.\n");
	else printf("%d ERREUR(S)\n",res); 
go_on = 1;

	printf("\tTEST du RECEIVE buffer :");
	err+=res=test_buffer(1,1,XILINX,trace_all_tests,0,testing_error);
/*	err+=res=test_buffer(1,1,XILINX,debug,0,testing_error);		*/
	if (res==0) printf("OK.\n");
	else printf("%d ERREUR(S)\n",res); 

	printf("\tTEST du TRANSMIT buffer :");
	err+=res=test_buffer(1,1,XILINX,trace_all_tests,1,testing_error);
/*	err+=res=test_buffer(1,1,XILINX,debug,1,testing_error);		*/
	if (res==0) printf("OK.\n");
	else printf("%d ERREUR(S)\n",res); 

	printf("Total des Erreur(s):%d.\n",err);
	return(err);
}

test_local(XILINX,debug)
int XILINX,debug;
{
	unsigned short csr,status;
	if (wait_for_csr(LOC+LRR,120,debug,1)==0) {
	    printf("\nAttente trop longue. J'arrette!\n");
	    return(1);
	    }
	erreur = test_loc(XILINX,debug);

	/* reset du bit LOCAL du CSR et mise a 1 du bit INV si erreur */

	if (erreur != 0) {
	    csr = INV;
	    if (set_csr(1,1,&csr,&status)==-1) 
		_errmsg(0,"erreur [%d] dans set INV",errno);
	    }
	sleep(2);
	csr=LOC;
	if (clr_csr(1,1,&csr,&status)==-1) 
	    _errmsg(0,"erreur [%d] dans clr LOC",errno);
	}

test_accept(XILINX,debug)
int XILINX,debug;
{
	unsigned short csr,status;
	int res;

	printf("TEST d'acceptance :\n");
	erreur=test_local(XILINX,debug);

	if (go_on) {
		printf("Test REMOTE bounce :");
		erreur=res=test_remote(XILINX,debug,1);
		if (res==0) printf("OK.\n");
		else printf("%d ERREUR(S)\n",res); 
	}

	if (go_on) {
		printf("Test REMOTE collision :\n");
		erreur+=res=test_remote(XILINX,debug,1);
		if (res==0) printf("OK.\n");
		else printf("%d ERREUR(S)\n",res); 
	}

	printf("Total des Erreur(s):%d.\n",erreur);
	return(erreur);
}

int menu()
{
u_short entry_csr, csr, status;
int res = 0;
char choice[20];
char rep[80];
char mess[400];
char full;

    if (key != 9)
	res = read (0, &full,1);
    printf ("%c",CR);

    res=get_csr(1,1,&entry_csr,&status);
    csr=BCR;					 /* set du bit BC */
    if ((res = set_csr(1,1,&csr,&status)) != 0) 
	printf ("Can't define type of RT card, erreur de SetCsr.");
    if ((csr & BCR) != BCR) {
	printf(" La carte est une carte XILINX.");
	XILINX=1;
	} else {
	    printf(" La carte est une ancienne carte.");
	    XILINX=0;
	    csr=BCR; /* reset du bit BC */
	    if (clr_csr(1,1,&csr,&status)==-1)
	        printf ("erreur [%d] dans clr BCR",errno);
	    }
    printf("     CSR = %4x.\n",entry_csr);
    if (stop_after_err == 1)
	sprintf(mess,"%s","  - Stop after error;\n");
	else if (stop_after_err == 0)
	    sprintf (mess,"%s","  - Loop on the error(^E - go on);\n");
		else sprintf (mess,"%s","  - Continue after error;\n");
    if (trace_all_tests == 1)
	strcat (mess,"  - Trace all tests; \n");
	else strcat (mess,"  - Without trace(only OK style); \n");
    if (testing_error == 1)
	strcat (mess,"  - Messages about errors will be print;\n");
	else strcat (mess,"  -  Messages about errors will not be print;\n");

    sprintf (rep, "%s%d", "  - Value of repetition for each tests - ",count);
    strcat (mess, rep);

    printf ("\nCurrent configuration is: {\n");
    printf ("%s",mess);
    printf (";}\nYou can change it - (see menu)\n\n"); 	
    printf("\tChoix du test a effectuer:\n");
    printf("\tConfiguration .........................9\n\n");
    printf("\tTest du RECEIVE buffer ................1\n");
    printf("\tTest du TRANSMIT buffer ...............2\n");
    printf("\tTest du CSR ...........................3\n");
    printf("\tTest remote BOUNCE ....................4\n");
    printf("\tTest remote COLLISION .................5\n");
    printf("\tTest LOCAL ............................6\n");
    printf("\tTest ACCEPT ...........................7\n");

    printf("\t\t\tChoix:");
    scanf("%s",choice);
    return (atoi(choice));
}

main()
{
	int n=0,debug=0,res=0,check_erreur=1;
	u_short csr,status,flag=0;
	char choix[20];
	char full, dir;

	if (lib_init()==-1) exit(_errmsg(errno,"Bad Init Lib"));

	go_on=1;
	intercept(intr);
	setbuf(stdout,0);
	printf ("version      by P.Charrue				03.Aug.94\n");
	printf ("any addition by A.Eline				everyday a l. piece\n");
	printf("Le VME est pret pour executer les tests d'acceptance.\n");
	printf("\tLe BC va m'envoyer une interruption quand se sera a moi de bosser\n");
	printf("\tVous avez la possibilite de prendre la main avec ^E; for abort ^C\n");
	if (wait_irq()==-1) exit(_errmsg(errno,"Erreur de wait_for_irq"));
 
	if (go_on == 1)
	    test_accept(XILINX,debug);
	do  {
	    key = menu();
	    go_on = 1;
	    if (check_erreur==0)
		printf("  WARNING : pas d'affichage des erreurs.\n");
	    n=erreur=0;
	    switch (key) {
/********** Test Rx Buffer *********************************************/
		case 1 :
		erreur = 0;
		while(n<count || count==0) {
		    res = 0;
		    printf("  TEST du RECEIVE buffer :");
		    res = test_buffer(1,1,XILINX,trace_all_tests,0,testing_error);
		    erreur=erreur+res;		
		    if (res==0)
			printf("pass %d; OK.\n", n+1);
			else 
			    printf("pass %d; %d ERROR(S).\n", n+1,res); 
		    if (trace_all_tests == 1)
			printf(" Total des Erreur(s):%d.\n",erreur);
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }
		printf ("\tTOTAL: pass - %d, errors - %d",n,erreur);
		if (erreur == 0) printf (" OK!\n");
		if (go_on == 0) {
		    res = read (0, &full,1);
		    go_on=1;
		    }
		break;
/********** Test Tx Buffer *********************************************/
		case 2 :
		erreur = 0;
		while(n<count || count==0) {
		    res = 0;
		    printf("  TEST du TRANSMIT buffer :");
		    res = test_buffer(1,1,XILINX,trace_all_tests,1,testing_error);
		    erreur=erreur+res;		
		    if (res==0)
			printf("pass %d; OK.\n", n+1);
			else 
			    printf("pass %d; %d ERROR(S).\n", n+1,res); 
		    if (trace_all_tests == 1)
			printf(" Total des Erreur(s):%d.\n",erreur);
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }
		printf ("\tTOTAL: pass - %d, errors - %d",n,erreur);
		if (erreur == 0) printf (" OK!\n");
		if (go_on == 0) {
		    res = read (0, &full,1);
		    go_on=1;
		    }
		break;
/********** Test CSR **************************************************/
		case 3 :
		erreur = 0;
		while (n<count || count==0) {
		    res = 0;
		    printf("  TEST du CSR :");
/*		    res=test_CSR(1,1,XILINX,debug);		*/
		    res = tst_CSR(1,1,XILINX,stop_after_err, trace_all_tests, testing_error);
		    if ((res == 0) && (trace_all_tests == 0))
			printf("OK.");
			else if (res != 0)
			    printf("%d ERREUR(S).",res); 
		    erreur=erreur+res;				 
		    printf (" Total des Erreur(s):%d.\n",erreur);
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }	
		go_on=1;
		break;
/********** Test remote BOUNCE ***************************************/
		case 4:
		erreur = 0;
		while(n<count || count==0) {
		    printf("Test Remote bounce:\n");
		    res=test_remote(XILINX,debug,check_erreur);
		    printf("Pass %d",n+1);
		    if (res==0) printf(" OK.\n");
		    else printf("%d ERREUR(S).\n",res); 
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }	
		go_on=1;
		break;
/********** Test remote COLLISION *************************************/
		case 5 :
		erreur = 0;
		while(n<count || count==0) {
		    printf("    Test Remote collision:\n");
		    res=test_remote(XILINX,debug,check_erreur);
		    printf("Pass %d",n+1);
		    if (res==0) printf(" OK.\n");
		    else printf("%d ERREUR(S).\n",res); 
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }	
		go_on=1;
		break;
/********** Local Test **********************************************/
		case 6 :
		erreur = 0;
		while(n<count || count==0) {
		    printf("  TEST LOCAL	 : pass %d\n", n+1);
		    csr = LOC + LRR;
		    set_csr(1,1,&csr,&status);	
		    test_local(XILINX,debug);
		    if (count!=0) n++;
		    if (go_on==0) break;
		    }	
		go_on=1;
		break;
/********** Acceptance Test ******************************************/
		case 7:
		erreur = 0;
		while(n<count || count==0) {
		    if (go_on==1) test_accept(XILINX,debug);
		    else break;
		    if (count!=0) n++;
		    }	
		go_on=1;
		break;
/********** Configuration *******************************************/
		case 9 :
		gets(choix);
		printf (" Configuration parameters:  (^E - return to menu)\n");
		flag = 0;
		while(flag == 0) {
		    dir = 8;
		    printf(" Stop/Loop/Continue after error (1/0/2)  - [%d]%c%c",stop_after_err,dir,dir);
		    if (gets(choix) != NULL)
			if (strlen(choix) != 0) 
			    stop_after_err = atoi(choix);
		    if (stop_after_err == 0) {
			flag = 1;
			printf ("Loop untill ^E\n");
			} else if (stop_after_err == 1) {
			    printf ("Stop\n");
			    flag = 1;
			    } else if (stop_after_err == 2) {
			        printf ("Continue\n");
			        flag = 1;
			        } 	
		    }
		flag = 0;
		while(flag == 0) {
		    printf(" Trace/No Trace (OK-style) (1/0)         - [%d]%c%c",trace_all_tests,dir,dir);
		    if (gets(choix) != NULL)
			if (strlen(choix) != 0) 
			    trace_all_tests = atoi(choix);
		    if (trace_all_tests == 1) {
			flag = 1;
			printf ("Trace\n");
			} else if (trace_all_tests == 0) {
			    printf ("Without trace\n");
		    	    flag = 1; 
			    }	
		    }
		flag = 0;
		while(flag == 0) {
		    printf(" Print/No Print mess. about error (1/0)  - [%d]%c%c",testing_error,dir,dir);
		    if (gets(choix) != NULL)
			if (strlen(choix) != 0) 
			    testing_error = atoi(choix);
		    if (testing_error == 1) {
			flag = 1;
			printf ("Print\n");
			} else if (testing_error == 0) {
			    printf ("No Print\n");
		    	    flag = 1; 
			    }	
		    }
		printf(" Value of repetition of the each test;\n");
		printf("\t0 - unlimited loop (untill ^e)\t     - [%d]%c%c",count,dir,dir);
		if (gets(choix) != NULL)
		    if (strlen(choix) != 0) 
			count = atoi(choix);
		break;

/********** Unvalid choice ******************************************/
		default :
		printf("  Quel choix bizarre !! \n");
		break;
		}
	} while (ex_code == 0);
	printf("Fin du test\n");
}
