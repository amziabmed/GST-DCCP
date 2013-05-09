---------------------------------------
*********** Description :  ************
---------------------------------------

This application allows the server to adapt the encoding bitrate 
to the measured sender rate given by the DCCP information. 
We use the CCID-3 (RFC 4342) that implements a receiver-based 
congestion control algorithm in which the sender is rate-limited 
by the receiver. The receiver sends feedback packets to the sender 
containing RTT, lost event information and other connection 
statistics that are plugged into the TCP-Friendly Rate Control 
equation (TFRC) (RFC 3448). 

Based on the current server sending rate, we adapt the encoding 
bitrate to make the transmission more suitable for applications 
such as media streaming, for which a relatively smooth sending 
rate is important.

On the other hand, to validate the transmission behavior of our 
application, we use Iperf to generate enough traffic to saturate 
the link and make trouble in the transmission.

In the source package, you will find a python script that uses a 
matplotlib library for plotting dynamically the current 
transmission rate.

---------------------------------------
**********  How it works:  ************
---------------------------------------
********** How it works: ************

== Compile :

-  Run make command to compile sources

== Server :

- Run ./server -p <port>

== Client : 

- Run ./client -p <port> -i <Server IP address>
 

---------------------------------------
*****  Command Line Arguments :  ******
---------------------------------------

- Server :

		 -p : The port to listen to (default: 4000),
		 -c : The Congestion Control IDentified to be used (default: 3).
-Client :

		 -p : The port to receive packets from (default: 4000),
		 -i : The host IP address to receive packets from (default: localhost),
		 -c : The Congestion Control IDentified to be used (default: 3).

==NOTE : the current version doesn't support CCID 2.



---------------------------------------
***********  Plot data :  *************
---------------------------------------

- Run ./plot.py to plot in real time the current sender rate.
(make sure that python-matplotlib is installed)

---------------------------------------
***********  Use Iperf :  *************
---------------------------------------

== Generate trafic :

- Use Iperf to generate trafic and trouble the transmission between server and client.
- Server : iperf -s
- client : iperf -c <Server IP address> -t 60
- Check the reduction of the transmission rate on the plot.

---------------------------------------
***********  Pipelines :  *************
---------------------------------------

== Server :

	gst-launch v4l2src ! videoscale videorate ! video/x-raw-yuv,width=640, height=480,framerate=25/1 ! x264enc bitrate=5000 tune=zerolatency ! video/x-h264,stream-format=byte-stream ! dccpserversink ccid=3 port=XXX

== Client :

	gst-launch dccpclientsrc ccid=3 port=XXX host=YYY ! h264parse ! ffdec_h264 ! ffmpegcolorspace ! ximagesink










