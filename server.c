#include "server.h"

/* Adapte the encoding bitrate to the current sender rate given by TFRC 
 * and write the bitrate=sender rate to the data.out file to be used  
 * later for plotting.
 * @param pipeline - the pipeline . 
 * @param bit - the mesured sender rate.
 */

void 
change_bitrate(GstElement * pipeline, guint bit)

{
	 fp = fopen("data.out", "a");

     if (fp == NULL) {
         printf("Could not open data.out for writing.\n");
         exit(0);
     }

	 guint bit_rate = bit/(64*1024);

	 GstElement *encoder = gst_bin_get_by_name(GST_BIN(pipeline), "x264enc");

  	 if(!encoder)
    	fprintf(stderr,"No encoder element with name 'encoder' found.\n");

  	  if(encoder){ 
    	  //GstPad *pad = gst_element_get_static_pad(encoder, "src"); 
    	  //gst_pad_set_blocked(pad, TRUE); 
	    if (bit_rate > 150)
	      g_object_set (G_OBJECT (encoder), "bitrate", bit_rate, NULL); 
    	  //gst_pad_set_blocked(pad, FALSE); 

  	  } 

  	 else

         fprintf(stderr,"Adaptation by adjusting bitrate is not be possible.\n");

  	 if(encoder)
    	 gst_object_unref(GST_OBJECT(encoder));
			
	 fprintf(fp,"%d %u\n",
		 t++,
		 bit_rate);
	 fclose(fp);

}

/* Update timer */

void 
tvsub(struct timeval *tdiff, struct timeval *t1, struct timeval *t0)
{
	 tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	 tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	 while (tdiff->tv_usec < 0)
		 tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}

/* Get the current CCID TX information from socket and call 
 * change_bitrate function to reinject the bitrate to the encoder.
 * @param user_data - the structure that contain the socket for getting 
 * information from DCCP socket and the pipeline for changing bitrate.
 * @return TRUE.
 */


int 
gst_dccp_get_socket_info (gpointer user_data)
{

	 //static struct tfrc_tx_info tfrc_last;
	 struct tfrc_tx_info tfrc;
	 socklen_t len = sizeof(tfrc);
	 struct timeval now, onow;
  	 PropertyNotifyData *data = user_data;


//****************************************************************************
	 getsockopt(data->sockt, SOL_DCCP, DCCP_SOCKOPT_CCID_TX_INFO, (char*) &tfrc, &len);

	 gettimeofday(&now, NULL);
	 tvsub(&onow, &now, &time0);
	 //if (memcmp(&tfrc, &tfrc_last, sizeof(tfrc))){

	 printf("%ld.%06ld %lld %lld %u %u %u %u %u\n",
			onow.tv_sec, onow.tv_usec,
			tfrc.tfrctx_x,
			tfrc.tfrctx_x_recv,
			tfrc.tfrctx_x_calc,
			tfrc.tfrctx_rtt,
			tfrc.tfrctx_p,
			tfrc.tfrctx_rto,
			tfrc.tfrctx_ipi);
	 //tfrc_last = tfrc;
	 //}

//****************************************************************************
	 change_bitrate(data->pipe, tfrc.tfrctx_x);
     return tfrc.tfrctx_x;
}


/* Create a new DCCP socket
 * @return the socket file descriptor
 */

gint
gst_dccp_create_new_socket ()
{
     int sock_fd;
     if ((sock_fd = socket (AF_INET, SOCK_DCCP, IPPROTO_DCCP)) < 0) {
     // GST_ELEMENT_ERROR (element, RESOURCE, OPEN_READ, (NULL), GST_ERROR_SYSTEM);
     }

     return sock_fd;
}




/* **********************************************************************/
/* ***************     these functions are optional	    *****************/
/*     Use them to retieve CCID in use, MTU, current Packet size and    */
/*      implement your sending mechanism considering this values.       */  
/* **********************************************************************/



/*
 * Get the current ccid of TX or RX half-connection. Parameter must be
 * DCCP_SOCKOPT_TX_CCID or DCCP_SOCKOPT_RX_CCID. Here we use 
 * DCCP_SOCKOPT_TX_CCID, for RX use DCCP_SOCKOPT_RX_CCID.
 * @param sock - the socket file descriptor.
 * @return ccid or -1 on error or tx_or_rx not the correct option.
 */

/*
int
gst_dccp_get_ccid (gpointer *sock)
{
int ccid;
socklen_t ccidlen =sizeof (ccid);;
int ret;

ret = getsockopt (sock, SOL_DCCP, DCCP_SOCKOPT_TX_CCID, (char *) &ccid, &ccidlen);
  if (ret < 0) {
    return -1;
  }
  return ccid;
}
*/


/*
 * Get the socket MTU.
 * @param sock - the socket file descriptor.
 * @return the MTU if the operation was successful, -1 otherwise.
 */

/*
int
gst_dccp_get_max_packet_size (gpointer *sock)
{
  int size;
  socklen_t sizelen = sizeof (size);
  int ret;
  ret= getsockopt (sock, SOL_DCCP, DCCP_SOCKOPT_GET_CUR_MPS,(char *) &size, &sizelen) ;
  if (ret < 0) {
    return -1;
  }
  return size;
}
*/
/*
 * Get the current packet size.
 * @param sock - the socket file descriptor.
 * @return the packet size if the operation was successful, -1 otherwise.
 */

/*
int
gst_dccp_get_packet_size (gpointer *sock)
{
int mtu_size;
socklen_t mtu_sizelen =sizeof (mtu_size);
int ret;

ret = getsockopt(sock,SOL_DCCP,DCCP_SOCKOPT_PACKET_SIZE, (char*) &mtu_size, &mtu_sizelen);
  if (ret < 0) {
    return -1;
  }
  return mtu_size;
}


*/




