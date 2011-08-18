/* Test CSR */

#include <stdio.h>
#include "1553def.h"
extern int errno;
extern int go_on;
extern int loop;

#define BAD(st) {printf("Erreur systeme [%d] en %s du csr\n",errno,st);erreur++;continue;}

test(bc,rt,debut,fin,modif,clear,arg,arger,XILINX)
unsigned short bc,rt;
int debut,fin,modif,clear,arg,arger,XILINX;
{
	unsigned short status,csr,mod_csr=0,mod2_csr=0,old_csr;
	unsigned short check,erreur=0,res;
	int i,int_flg;;

	if (XILINX) {	/* With new card, INT bit stays to 1 when LOC is set (no IT) */
	    if (get_csr(bc,rt,&csr,&status) != 0 ) {
		Err_mil (0);
		}
	    int_flg=((csr&LOC)==LOC);
	    } else int_flg=0;

	if (arg == 1) {
	    printf ("\n    BIT SET   des bits %2d a %2d.",debut,fin);
	    printf ("\n\tbits: ");
	    }

	for (i = debut; i <= fin; i++) {
	    res = check = 1<<i;
	    mod_csr=mod_csr|res;

	    if (get_csr(bc,rt,&old_csr,&status) != 0) Err_mil(0);
	    if (set_csr(bc,rt,&check,&status) != 0 ) Err_mil(0);
	    if (get_csr(bc,rt,&csr,&status) != 0 ) Err_mil(0);

	    if (arg == 1) 
		printf("%x ",i);
	    if (modif==1) {
		if ((csr&0x9fff) != (mod_csr|0x8000)) {
		    if (int_flg && ((i==5) || (i==4))) continue;
		    if (arger != 0)
			printf("Erreur BIT SET;bit :%2d le CSR n'est pas egal a:%4x CSR=%4x.\n",i,mod_csr|0x8000,csr);
		    erreur++;
		    continue;
		    }
		} else if (csr != old_csr) {
		    if (int_flg && ((i==5) || (i==4))) continue;
		    if (arger != 0) 
			printf("Erreur BIT SET;bit :%2d le CSR s'est modifie alors qu'il n'aurait pas du:Old_CSR=%4x CSR=%4x.\n",i,old_csr,csr);
		    erreur++;
		    }
	    }

	mod2_csr=csr&mod_csr;
	mod_csr=0;
	if (clear == 0)
	    return(erreur);		 /* fin du test csr */

	if (arg == 1 ) {
	    printf("\n    BIT CLEAR des bits %2d a %2d.",debut,fin);
	    printf ("\n\tbits: ");
	    } 
	for (i = debut; i <= fin; i++) {
		/* 	Lecture du CSR		 */
		/* 	RESET CSR  bit nr:i	 */
	    res=check=1<<i;
	    mod_csr=mod_csr|res;

	    if (get_csr(bc,rt,&old_csr,&status) != 0) Err_mil(0);
	    if (clr_csr(bc,rt,&check,&status) != 0 ) Err_mil(0);
	    if (get_csr(bc,rt,&csr,&status) != 0 ) Err_mil(0);	

	    if (arg == 1) 
		printf("%x ",i);
	    if (modif==1) {
		if ( (csr&0x9fff) != ((mod2_csr^mod_csr)|0x8000) ) {
		    if (int_flg && ((i==5) || (i==4))) continue;
		    if (arger != 0) 
			printf("Erreur BIT CLEAR;bit :%2d le CSR n'est pas egal a:%4x CSR=%4x.\n",i,((mod2_csr^mod_csr)|0x8000),(csr&0x9fff) );
		    erreur++;
		    continue;
		    }
		} else if (csr != old_csr) {
		    if (int_flg && ((i==5) || (i==4))) continue;
		    if (arger != 0)
			printf("Erreur BIT CLR;bit :%2d le CSR s'est modifie alors qu'il n'aurait pas du: Old_CSR=%4x CSR=%4x.\n",i,old_csr,csr);
		    erreur++;
		    }
	    }
	return(erreur);
}


test_CSR(bc,rt,XILINX,arg)
unsigned short bc,rt;
int XILINX,arg;
{
	int erreur=0;
	int loop = 0;
	unsigned short old_csr=0,csr=0,status=0;

	unsigned short test_csr();

	if (get_csr(1,1,&old_csr,&status) != 0) {
		printf("Erreur systeme [%d] de lecture du CSR\n",errno);
		return(1);
	}

	csr=0x1fff;
	if (clr_csr(bc,rt,&csr,&status) != 0) {
		printf("Erreur systeme [%d] de clear du CSR\n",errno);
		return(1);
	}
	if ((csr&0x1fff) != 0x0000) {
		printf("   Erreur Le CSR n'est pas efface :%4x->%4x.\n",old_csr,csr);
		erreur++;
	}
	erreur+=test(bc,rt,0,4,1,1,arg,XILINX);
	erreur+=test(bc,rt,5,5,1,1,arg,XILINX);
	erreur+=test(bc,rt,10,12,1,1,arg,XILINX);
	erreur+=test(bc,rt,4,4,1,0,arg,XILINX);
	erreur+=test(bc,rt,5,5,0,0,arg,XILINX);
	erreur+=test(bc,rt,4,4,1,1,arg,XILINX);

	return(erreur);
}

tst_CSR(bc,rt,XILINX,arg1,arg2,arg3)
unsigned short bc, rt;
int XILINX, arg1, arg2, arg3;

{
	int res, erreur=0;
	unsigned short old_csr=0,csr=0,status=0;

	unsigned short test_csr();

	if (get_csr(1,1,&old_csr,&status) != 0) {
		Err_mil (0);
	}

	csr=0x1fff;
	go_on = 1;
	if (clr_csr(bc,rt,&csr,&status) != 0) {
	    Err_mil(0);
	    }
	if ((csr&0x1fff) != 0x0000) {
	    if (arg3 != 0)
		printf("   Erreur Le CSR n'est pas efface :%4x->%4x.\n",old_csr,csr);
	    erreur++;
	    } else if (arg2 != 0)
		printf ("\n\t\t\tTest clear CSR  - OK\n");
	loop = 0;
	do  {
	    erreur+=test(bc,rt,0,4,1,1,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);	    
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0; 
	do  {
	    erreur+=test(bc,rt,5,5,1,1,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0;
	do  {
	    erreur+=test(bc,rt,10,12,1,1,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0;
	do  {
	    erreur+=test(bc,rt,4,4,1,0,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0;
	do  {
	    erreur+=test(bc,rt,5,5,0,0,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0;
	do  {
	    erreur+=test(bc,rt,4,4,1,1,arg2,arg3,XILINX);
	    if ((erreur != 0) && (arg1 != 0))
		return (1);
	    if (erreur != 0)
		loop = arg1;
	    } while (loop != 0);
	loop = 0;
	return(erreur);
}
