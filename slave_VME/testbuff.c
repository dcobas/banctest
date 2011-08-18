/* Test Buffers */
#include <stdio.h>
#include <errno.h>
#include <types.h>
#include "1553def.h"

/*extern int erreur;*/
extern int go_on;
extern int stop_after_err;
int curr_err = 0;
/*unsigned short erreur = 0;*/
int sflag, contr;

int convbin (number, string)
short number;
char *string;

{
int i;
    sprintf (string,"%s","b");
    for (i = 0; i < 16; i++) {
	if (number < 0)
	    strcat(string, "1");
	    else strcat(string, "0");
	number = number <<= 1;
	}
    string [i+1] = 0;   
    return (0);
    }

int reset_p (bc, rt, XILINX, arg, n_buff)
unsigned short bc, rt;
int XILINX, arg, n_buff;
{
	unsigned short csr,status,res;

	if (n_buff == 0) { 
	    if (arg == 1 ) printf("\nTest du bit RESET RECEIVE BUFFER  ");
	    res = 0x0080;
	    } else { 
		if (arg == 1) printf("\nTest du bit RESET TRANSMIT BUFFER  ");
		res = 0x0040;
		}
	if (arg == 1) printf("(avec bit SET ->%4x.)",res);
	do  {
	    if (set_csr(bc,rt,&res,&status) != 0) {
		curr_err = 1;
		if (arg != 0) printf("Test error RESET POINTER");
		}
	    if (curr_err == 0) {
		if (arg == 1) printf (" OK\n");
		return (0);
		} 
	    } while ((curr_err != 0) && (stop_after_err == 0));
	return (1);
	}

tst_buffer(bc,rt,XILINX,arg,n_buff,test_erreur,fun_wr,fun_rd)
unsigned short bc,rt;
int XILINX,arg,n_buff,test_erreur;

int (*fun_wr)();	/* fonction pour ecrire dans le buffer (set_tx_buf ou set_rx_buf) */
int (*fun_rd)();	/* fonction pour lire dans le buffer (get_tx_buf ou get_rx_buf) */
{
	unsigned short csr,status,old_status,old_csr;
	unsigned short size_of_portion;
	char str1[20], str2[20];
	unsigned short erreur = 0;
	unsigned short check[128],res,res2;
	u_short i,j,k,l,m,curseur = 0,pas = 0,flag = 0,sycle_pass = 0;
	u_short seuil = 0, run = 1;
	extern unsigned short buff[], data_code;
#define BAD(str) {if (test_erreur != 0) {printf("Hardware error");prerr(0,str);}erreur++;csr = INV;set_csr(1,1,&csr,&status);} 

    seuil = 128;
    data_code = 1;
    do  {
	full_buff(data_code);
	for (size_of_portion = 2; size_of_portion <= 256; size_of_portion += 2) {

		/****	 Reset pointer 			****/
	    if ((reset_p (bc,rt,XILINX,arg,n_buff) != 0) && (stop_after_err == 1))
		return (1);
	    curr_err = 0;

		/**** 	clear buffer 		****/
	    for (i = 0; i < seuil; i++) check [i] = 0;
	    curr_err = 0;
	    do  {	  
		if (fun_wr(bc,rt,256,check,&status) != 0 ) {
		    curr_err = 1;
		    if ((test_erreur != 0) && (n_buff == 0)) BAD("Erreur de clear du RX buffer")
		    if ((test_erreur != 0) && (n_buff == 1)) BAD("Erreur de clear du TX buffer")
		    }
		if ((arg == 1) && (curr_err == 0)) printf("Clear buffer OK.\n");
		} while ((curr_err != 0) && (stop_after_err == 0));
	    curr_err = 0;

		/****	 Reset pointer 			****/
	    if ((reset_p (bc, rt, XILINX, arg, n_buff) != 0) && (stop_after_err == 1))
		return (1);
	    curr_err = 0;

		/****   writing size_of_portion/2 words	 ****/
	    if (arg == 1) printf("Writing %d words/",size_of_portion/2);
	    do  {
		if (fun_wr(bc,rt,size_of_portion,&buff[sycle_pass],&status) != 0 ) {
		    curr_err = 1;
		    if ((test_erreur != 0) && (n_buff == 0))
			BAD("Erreur de remplissage du RX buffer")
		    if ((test_erreur != 0) && (n_buff == 1))
			BAD("Erreur de remplissage du TX buffer")
		    }
		if ((stop_after_err == 2) || (curr_err == 0)) break;
		if (stop_after_err == 1) return(1);
		} while (stop_after_err == 0);		

		/*** Reset pointer			 ****/
	    if ((reset_p (bc, rt, XILINX, arg, n_buff) != 0) && (stop_after_err == 1))
		return (1);

		/*** reading size_of_portion/2 words	 ****/
	    curr_err = 0;
	    if (arg == 1) printf("Reading %d words/",size_of_portion/2);
	    do  {
		if (fun_rd(bc,rt,size_of_portion,check,&status) != 0 ) {
		    curr_err = 1;
		    if ((test_erreur != 0) && (n_buff == 0))
			BAD("Erreur de lecture du RX buffer")
		    if ((test_erreur != 0) && (n_buff == 1))
			BAD("Erreur de lecture du TX buffer")
		    }
		if ((stop_after_err == 2) || (curr_err == 0)) break;
		if (stop_after_err == 1) return(1);
		} while (stop_after_err == 0);

		/**** 	comparaison		 	****/
	    if (arg == 1) printf("Comparaison :");
	    flag = 0;
	    for (i=0; i < size_of_portion/2; i++) 
		if(check[i] != buff[i+sycle_pass] ) {
		    erreur ++;
		    flag++;
		    if (test_erreur == 1) {	
			printf ("\n!!! Data Error in the test of ");
			if (n_buff == 0)  
			    printf("RECEIVE BUFFER  ");
			    else  
				printf("TRANSMIT BUFFER  ");
			printf (":Pointer %d\n", i);
			printf (" Writing ");
			j = convbin (buff[i + sycle_pass], str2);
			printf ("%s\n",str2);
			printf (" Reading ");
			j = convbin (check[i], str2);
			printf ("%s\n",str2);
			if (stop_after_err == 1) return (1);
			}
		    if (go_on == 0) break;
		    }
	    if ((arg == 1) && (flag == 0)) {
		i = convbin (buff[sycle_pass], str1);
		printf ("\tdata: = %s",str1);
		}
	    if (go_on == 0) break;
	    }
	sflag += flag;
	if (data_code == 6) {
	    contr++;
	    data_code--;
	    if (++sycle_pass == 16)
		data_code++;
	    } 
	} while ((++data_code < 7) && (go_on != 0));
    if ((arg == 1) && (sflag == 0))
	printf(" OK.\n");
    if (go_on == 0) {
	printf (" Interrupted by your request !(^e)\n");
	i = getchar();
	}	
    return(erreur);
}
/******************************** !!!!! **********************************/

int set_rx_buf(),get_rx_buf(),set_tx_buf(),get_tx_buf(),set_csr(),clr_csr();

test_buffer(bc,rt,XILINX,arg,n_buff,test_erreur)
unsigned short bc,rt;
int XILINX,arg,n_buff,test_erreur;
{

unsigned short csr, status, ret_e;

    sflag = 0;
    contr = 0;
    ret_e = 0;
    if (n_buff == 0)
	if ((ret_e = tst_buffer(bc,rt,XILINX,arg,n_buff,test_erreur,set_rx_buf,get_rx_buf)) != 0) {
	    printf ("\tErrors = %d\n",ret_e);
	    csr = INV;
	    set_csr(1,1,&csr,&status); 
	    return(ret_e);							
	    }
    if (n_buff == 1)
	if ((ret_e= tst_buffer(bc,rt,XILINX,arg,n_buff,test_erreur,set_tx_buf,get_tx_buf)) != 0) {
	    printf ("\tErrors = %d\n",ret_e);
	    csr = INV;
	    set_csr(1,1,&csr,&status); 
	    return(ret_e);							
	    }
    return(ret_e);
}

