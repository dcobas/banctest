/* test remote des buffers */
#include <stdio.h>
#include <errno.h>
#include <types.h>
#include "1553def.h"

#define BAD(st) return(_errmsg(errno,"erreur [%d] dans %s",errno,st));
test_bounce(XILINX,debug)
int XILINX,debug;
{
	int erreur=0;
	register int count=2,seuil;
	u_short csr=0,sta=0;
	extern buff[];
	extern go_on;
	extern int trace_all_tests,testing_error;

	seuil=XILINX==1?8192:256;

	if (debug >= 1) printf("Taille du buffer :%d.\n",seuil);
	go_on=1;

	while(count<=seuil && go_on != 0) {
		if (debug >= 1) printf("  Attente de l'IRQ pour %d bytes.\n",count);
		if (wait_for_csr(RB,40000,debug,0)==0) {
			printf("Synchro erreur. Je ne trouve pas le bit RB a 1\n");
			return(++erreur);
		}
		if (go_on==0) return(erreur);

		/* reset receive pointer */
		csr=RRP; 
		if (set_csr(1,1,&csr,&sta)==-1) BAD("set RRP");

		/* lecture du receive buffer */
		if (get_rx_buf(1,1,count,buff,&sta) == -1 ) BAD("get_rx_buf");

		/* clear bit RB */
		csr=RB;
		if (clr_csr(1,1,&csr,&sta) == -1) BAD("clr RB");

		/* reset transmit pointer */
		csr=RTP;
		if (set_csr(1,1,&csr,&sta) == -1) BAD("set RTP");

		/* write transmit buffer */
		if (set_tx_buf(1,1,count,buff,&sta) == -1 ) BAD("set_tx_buf");

		/* set bit TB */
		csr=TB;
		if (set_csr(1,1,&csr,&sta) == -1 ) BAD("set TB");

		if (count<256) count+=2; else count+=256;
	}
	/* Clear bounce test pattern */
	csr=LRR+NEM;
	if (clr_csr(1,1,&csr,&sta) == -1) BAD("clr LRR+NEM");
	return(erreur);
}

test_collision(XILINX,debug,check_erreur)
{
	u_short csr,sta;
	int res;

	/* G64 attends RB, puis mets TB et teste le Transmit buffer et clear TB*/
	/* Puis, il attends TB, mets RB et teste le receive buffer et clear RB */

	if (wait_for_csr(RB,60,debug,1)==0) {
		printf("Erreur de synchro. Le BC n'a pas mis mon bit RB a 1\n");
		return(1);
	}
	/* set bit TB */
	csr=TB;
	if (set_csr(1,1,&csr,&sta) == -1 ) BAD("set TB");
	if (debug==0) printf("Test du transmit buffer ");
	res = test_buffer(1,1,XILINX,trace_all_tests,1,testing_error);
	if (res==0) printf("OK ");
	res += test_buffer(1,1,XILINX,trace_all_tests,1,testing_error);
	if (res==0) printf("OK ");
	res += test_buffer(1,1,XILINX,trace_all_tests,1,testing_error);
	if (res==0) printf("OK\n");
	/* clr bit TB */
	csr=TB;
	if (clr_csr(1,1,&csr,&sta) == -1 ) BAD("clr TB");

	if (wait_for_csr(TB,60,debug,1)==0) {
		printf("Erreur de synchro. Le BC n'a pas mis mon bit TB a 1\n");
		return(1);
	}
	/* set bit RB */
	csr=RB;
	if (set_csr(1,1,&csr,&sta) == -1 ) BAD("set RB");
	if (debug==0) printf("Test du receive buffer ");
	res = test_buffer(1,1,XILINX,trace_all_tests,0,testing_error);
	if (res==0) printf("OK ");
	res += test_buffer(1,1,XILINX,trace_all_tests,0,testing_error);
	if (res==0) printf("OK ");
	res += test_buffer(1,1,XILINX,trace_all_tests,0,testing_error);
	if (res==0) printf("OK\n");

	/* set INV bit in case of error */
/*	if (res) {
		csr=INV;
		if (set_csr(1,1,&csr,&sta)==-1) BAD("set INV");
	}							*/
	/* clr bit RB	*/
	csr=RB;
	if (clr_csr(1,1,&csr,&sta) == -1 ) BAD("clr RB");
	/* Clear bounce test pattern */
	csr=LRR+BRD;
	if (clr_csr(1,1,&csr,&sta) == -1) BAD("clr LRR+NEM");
	return(res);
}

test_remote(XILINX,debug,check_erreur)
{
	u_short csr,sta;

	if (wait_for_csr(LRR,60,debug,1)==0) {
		printf("Erreur de synchro. Le BC n'a pas mis mon bit LRR a 1\n");
		return(1);
	}
	if (get_csr(1,1,&csr,&sta) == -1) BAD("get csr");
	if ((csr&NEM)==NEM) return(test_bounce(XILINX,debug));
	if ((csr&BRD)==BRD) return(test_collision(XILINX,debug,check_erreur));
	printf("Test REMOTE sans NEM (bounce) ni BRD (collision)\n");
	return(1);
}
