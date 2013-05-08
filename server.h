#include <gst/gst.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>

/* DCCP socket specific options */

#ifndef SOL_DCCP
#define SOL_DCCP 269
#endif


#ifndef DCCP_SOCKOPT_PACKET_SIZE
#define DCCP_SOCKOPT_PACKET_SIZE 	1	/* Set/get the dccp packet size */
#endif


#ifndef DCCP_SOCKOPT_CCID_RX_INFO
#define DCCP_SOCKOPT_CCID_RX_INFO 	128	/* Returns a `struct tfrc_rx_info' */
#endif

#ifndef DCCP_SOCKOPT_CCID_TX_INFO
#define DCCP_SOCKOPT_CCID_TX_INFO 	192	/* Returns a `struct tfrc_tx_info' */
#endif

#ifndef DCCP_SOCKOPT_GET_CUR_MPS
#define DCCP_SOCKOPT_GET_CUR_MPS  	5 	/* Max packet size, RFC 4340. */
#endif
#ifndef DCCP_SOCKOPT_TX_CCID
#define DCCP_SOCKOPT_TX_CCID		14	/* Set/get the TX CCID     */
#endif
#ifndef DCCP_SOCKOPT_RX_CCID
#define DCCP_SOCKOPT_RX_CCID		15	/* Set/get the RX CCID     */
#endif
#ifndef IPPROTO_DCCP
#define IPPROTO_DCCP 33
#endif

FILE *fp;
int sock,t;
struct timeval time0;


typedef struct
{
     int sockt;
     GstElement *pipe;
} PropertyNotifyData;

 
 /**     tfrc_rx_info    -    TFRC Receiver Data Structure
  *
  *      @tfrcrx_x_recv: receiver estimate of sending rate (3.2.2)
  *      @tfrcrx_rtt:    round-trip-time (communicated by sender)
  *      @tfrcrx_p:      current estimate of loss event rate (3.2.2)
  */
struct tfrc_rx_info {
         __u32 tfrcrx_x_recv;
         __u32 tfrcrx_rtt;
         __u32 tfrcrx_p;
};
 
 /**     tfrc_tx_info    -    TFRC Sender Data Structure
  *
  *      @tfrctx_x:      computed transmit rate (4.3 (4))
  *      @tfrctx_x_recv: receiver estimate of send rate (4.3)
  *      @tfrctx_x_calc: return value of throughput equation (3.1)
  *      @tfrctx_rtt:    (moving average) estimate of RTT (4.3)
  *      @tfrctx_p:      current loss event rate (5.4)
  *      @tfrctx_rto:    estimate of RTO, equals 4*RTT (4.3)
  *      @tfrctx_ipi:    inter-packet interval (4.6)
  *
  */
struct tfrc_tx_info {
         __u64 tfrctx_x;
         __u64 tfrctx_x_recv;
         __u32 tfrctx_x_calc;
         __u32 tfrctx_rtt;
         __u32 tfrctx_p;
         __u32 tfrctx_rto;
         __u32 tfrctx_ipi;
};

void change_bitrate(GstElement * pipeline, guint bit);

void tvsub(struct timeval *tdiff, struct timeval *t1, struct timeval *t0);

int gst_dccp_get_socket_info (gpointer user_data);

gint gst_dccp_create_new_socket ();

