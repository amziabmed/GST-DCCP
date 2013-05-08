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




---------------------------------------
*****  Command Line Arguments :  ******
---------------------------------------




---------------------------------------
***********  Use Iperf :  *************
---------------------------------------




---------------------------------------
***********  Plot data :  *************
---------------------------------------
