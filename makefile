CC=gcc

EXTRA_WARNINGS=-Wall

CFLAGS=-ggdb $(EXTRA_WARNINGS)

GST_LIBS=`pkg-config --libs gstreamer-0.10` 

GST_CFLAGS=`pkg-config --cflags gstreamer-0.10`

LIB_LIBS=`pkg-config --libs glib-2.0`



all: server client



server: dccpserver.c server.c
	$(CC) $+ $(CFLAGS)  $(GST_CFLAGS)  -o $@ $(LIB_LIBS) $(GST_LIBS)



client: dccpclient.c
	$(CC) $+ $(CFLAGS)  $(GST_CFLAGS)  -o $@ $(LIB_LIBS) $(GST_LIBS)



clean:
	rm server 
