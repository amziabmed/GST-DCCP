#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <math.h> 
#include <sys/time.h>
#include <gst/gst.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "server.h"   

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



int main (int argc, char *argv[]){

	 GMainLoop *loop;
	 GstBus *bus;
	 GstElement *pipeline, *v4l2src, *videoscale, *videorate, *capsfilter, *x264enc,  *caps4enc, *dccpserversink;
	 GstCaps *caps , *capsenc;
	 PropertyNotifyData data;

	 gint port = 4000;
	 gint ccid = 3;
	 GOptionContext *context = NULL;
	 GError *err;


	 GOptionEntry entries[] = 
	 {
		 {"Port", 'p', 0, G_OPTION_ARG_INT, &port, "The port to listen to (default: 4000)", NULL}, 
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


  	 if ((sock = gst_dccp_create_new_socket ()) < 0) {
      	 return FALSE;
     }

	 /* create elements */
	 pipeline = gst_pipeline_new ("video-sender");


	 v4l2src = gst_element_factory_make ("v4l2src", "v4l2src");

	 g_object_set (G_OBJECT (v4l2src), "device", "/dev/video1", NULL);

	 videoscale = gst_element_factory_make ("videoscale", "videoscale");

	 videorate = gst_element_factory_make ("videorate", "videorate");

	 capsfilter = gst_element_factory_make ("capsfilter", "capsfilter");

	 x264enc = gst_element_factory_make ("x264enc", "x264enc");
	 g_object_set (G_OBJECT (x264enc), 
		       //"speed-preset", 1,
		       "tune", (guint)4, 
		       "bitrate", 5000,
		       "threads", 4,
		       "intra-refresh",TRUE,
		       NULL);

	 caps4enc = gst_element_factory_make ("capsfilter", "caps4enc");

	 dccpserversink = gst_element_factory_make ("dccpserversink", "server-sink");
	 g_object_set (G_OBJECT (dccpserversink), "port", port, NULL);
	 g_object_set (G_OBJECT (dccpserversink), "ccid", ccid, NULL);

	 if (!pipeline || !v4l2src || !dccpserversink || !videoscale || !videorate || !capsfilter || !caps4enc || !x264enc ) {
		 g_print ("One element could not be created\n");
		 return -1;
	 }
	
	
	 caps = gst_caps_from_string("video/x-raw-yuv,width=640,height=480,framerate=25/1");
	 g_object_set (G_OBJECT (capsfilter), "caps", caps, NULL);
	 //gst_object_unref (caps);
	 capsenc = gst_caps_from_string("video/x-h264,stream-format=byte-stream");
	 g_object_set (G_OBJECT (caps4enc), "caps", capsenc, NULL);
	 //gst_object_unref (caps);


	 bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	
	 gst_bus_add_watch (bus, bus_call, loop);
	
	 gst_object_unref (bus);

	 /* put all elements in a bin */
	 gst_bin_add_many (GST_BIN (pipeline), v4l2src, videoscale, videorate, capsfilter, x264enc, caps4enc,  dccpserversink, NULL);

	 gst_element_link_many (v4l2src, videoscale, videorate, capsfilter, x264enc, caps4enc, dccpserversink, NULL);



	 /* Now set to playing and iterate. */
	 g_print ("Setting to PLAYING\n");
	
	 gst_element_set_state (pipeline, GST_STATE_PLAYING);
	
	 g_object_get(G_OBJECT(dccpserversink), "sockfd", &sock,NULL);

  	 data.sockt = sock;
	 data.pipe=pipeline;
	
  	 g_timeout_add_seconds(1,gst_dccp_get_socket_info, &data);

	 g_main_loop_run (loop);
	
	 g_print ("Running\n");

	 /* clean up nicely */
	 g_print ("Returned, stopping playback\n");
	 gst_element_set_state (pipeline, GST_STATE_NULL);
	 g_print ("Deleting pipeline\n");
	 gst_object_unref (GST_OBJECT (pipeline));


	 return 0;


}
