#ifndef __Lynx__

/* status register CSR for RTI */
#define TB_BIT    0x0001  /* 1 transf. buf. full  0 transf. buf. empty */
#define RB_BIT    0x0002  /* 1 recept. buf. full  0 recept. buf. empty */
#define INV_BIT   0x0004  /* 1 invalid message    0 valid message      */
#define RL_BIT    0x0008  /* 1 reset line                              */
#define INE_BIT   0x0010  /* 1 enabled interrupt  0 disabled interrupt */
#define INT_BIT   0x0020  /* 1 interrupt present  0 no interrupt       */
#define RTP_BIT   0x0040  /* 1 reset transmit pointer                  */
#define RRP_BIT   0x0080  /* 1 reset reception pointer                 */
#define BC_BIT    0x0100  /* 1 BC mode            0 RT mode            */
#define BCREQ_BIT 0x0200  /* 1 BC request                              */
#define LRREQ_BIT 0x0400  /* 1 local/remote request                    */
#define NEM_BIT   0x0800  /* 1 non expected message                    */
#define BRDIS_BIT 0x1000  /* 1 broadcast disable                       */
#define LM_BIT    0x2000  /* 1 local mode                              */
#define TM_BIT    0x4000  /* 1 test mode                               */
#define V5_BIT    0x8000  /* 1 5 Volt present     0 5 Volt not present */

#define lib_init                mil1553_init_quickdriver
#define exit_quickdriver        mil1553_exit_quickdriver
#define send_raw_quick_data     mil1553_send_raw_quick_data
#define send_quick_data         mil1553_send_quick_data
#define get_raw_quick_data      mil1553_get_raw_quick_data
#define get_quick_data          mil1553_get_quick_data
#define get_connected_bc        mil1553_get_connected_bc
#define get_connected_rt        mil1553_get_connected_rt
#define reset_rt                mil1553_reset_rt
#define get_tx_buffer           mil1553_get_tx_buffer
#define get_rx_buffer           mil1553_get_rx_buffer
#define get_rt_csr              mil1553_get_rt_csr
#define clr_rt_csr              mil1553_clr_rt_csr
#define set_rt_csr              mil1553_set_rt_csr
#define mdrop                   mil1553_mdrop
#define get_bc_csr              mil1553_get_bc_csr
#define clr_bc_csr              mil1553_clr_bc_csr
#define set_bc_csr              mil1553_set_bc_csr
#define get_bc_tx_buffer        mil1553_get_bc_tx_buffer
#define get_bc_rx_buffer        mil1553_get_bc_rx_buffer

#include <mil1553_lib.h>
#else

#define lib_init init_quickdriver
#include <drvrutil/mil1553quicklib.h>
#include <drvrutil/1553_parq.h>

/* Missing in drvrutil/mil1553quicklib.h */
extern short mdrop (int bc, int rt, int tr, int sa, int wc, unsigned short *statusptr, char *buf);

#endif
