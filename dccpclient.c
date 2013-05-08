#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <gst/gst.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "client.h"


int sock;


//static struct timeval time0;

static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data){
	
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE (msg)) {
	case GST_MESSAGE_EOS:
		g_print ("End-of-stream\n");
		g_main_loop_quit (loop);
		break;
	case GST_MESSAGE_ERROR: {
		gchar *debug;
		GError *err;

		gst_message_parse_error (msg, &err, &debug);
		g_free (debug);

		g_print ("Error: %s\n", err->message);
		g_error_free (err);

		g_main_loop_quit (loop);
		break;
	}
	default:
		break;
	}

	return TRUE;
}



/*
static void tvsub(struct timeval *tdiff, struct timeval *t1, struct timeval *t0)
{
	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	while (tdiff->tv_usec < 0)
		tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}
static int Nread(GstElement * dccpclientsrc)
{
	sock=socket (AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	g_object_get(G_OBJECT(dccpclientsrc), "sockfd", &sock, NULL);



	struct tfrc_rx_info tfrc;
	socklen_t len = sizeof(tfrc);
	struct timeval now, onow;


	getsockopt(sock, SOL_DCCP, DCCP_SOCKOPT_CCID_RX_INFO,(char*)&tfrc, &len);

	gettimeofday(&now, NULL);
	tvsub(&onow, &now, &time0);
	printf("%ld.%06ld %u %u %u\n",
			onow.tv_sec, onow.tv_usec,
			tfrc.tfrcrx_x_recv,
			tfrc.tfrcrx_rtt,
			tfrc.tfrcrx_p);
	//if (tfrc.tfrcrx_p>0)
		//tfrc.tfrcrx_p=0;
		//setsockopt(sock, SOL_DCCP, DCCP_SOCKOPT_CCID_TX_INFO,&tfrc, sizeof(tfrc));




	return 1;
}
*/
int main (int argc, char *argv[]){

	GMainLoop *loop;
	GstBus *bus;
	GstElement *pipeline, *ffmpegcolorspace, *ximagesink, *h264parse, *ffdec_h264, *dccpclientsrc;


	gchar *host="127.0.0.1";
	gint port = 4000;
	gint ccid = 3;
	GOptionContext *context = NULL;
	GError *err;

	/* Option Entry */

	GOptionEntry entries[] = 
	{
		{"Port", 'p', 0, G_OPTION_ARG_INT, &port, "The port to receive packets from (default: 4000)", NULL},
		{"Host", 'i', 0, G_OPTION_ARG_STRING, &host, "The host IP address to receive packets from (default: localhost)", NULL}, 
		{"ccid", 'c', 0, G_OPTION_ARG_INT, &ccid, "The Congestion Control IDentified to be used (default: 3)", NULL}, 
		{NULL}
	};

	if (!g_thread_supported())
	  g_thread_init (NULL);

	context = g_option_context_new("- TFRC over DCCP");

	g_option_context_add_main_entries(context, entries, NULL);

	g_option_context_add_group(context, gst_init_get_option_group());

	if (!g_option_context_parse(context, &argc, &argv, &err)) {
		g_error("%s\n", err->message);
		return FALSE;
	}

	/* initialize GStreamer */

	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);



	/* create elements */

	pipeline = gst_pipeline_new ("video-sender");

	ximagesink = gst_element_factory_make ("ximagesink", "ximagesink");

	ffmpegcolorspace = gst_element_factory_make ("ffmpegcolorspace", "ffmpegcolorspace");

	h264parse = gst_element_factory_make ("h264parse", "h264parse");

	ffdec_h264 = gst_element_factory_make ("ffdec_h264", "ffdec_h264");

	dccpclientsrc = gst_element_factory_make ("dccpclientsrc", "client-source");

	if (!pipeline || !ximagesink || !ffmpegcolorspace || !h264parse || !ffdec_h264 || !dccpclientsrc) {
		g_print ("One element could not be created\n");
		return -1;
	}


	
	g_object_set (G_OBJECT (dccpclientsrc), "host", host, NULL);
	g_object_set (G_OBJECT (dccpclientsrc), "port", port, NULL);
	g_object_set (G_OBJECT (dccpclientsrc), "ccid", ccid, NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));

	gst_bus_add_watch (bus, bus_call, loop);

	gst_object_unref (bus);

	/* put all elements in a bin */

	gst_bin_add_many (GST_BIN (pipeline), dccpclientsrc, h264parse, ffdec_h264,ffmpegcolorspace, ximagesink, NULL);

	gst_element_link_many (dccpclientsrc, h264parse, ffdec_h264,ffmpegcolorspace,  ximagesink, NULL);



	/* Now set to playing and iterate. */

	g_print ("Setting to PLAYING\n");
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_print ("Running\n");


  	//g_timeout_add(1000,(GSourceFunc)Nread, dccpclientsrc);
	g_main_loop_run (loop);

	/* clean up nicely */

	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);
	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));

	return 0;
}
