Simple UDP Example
==================

About
~~~~~

This is a dead simple example of how to use the UDP networking features
in XINU 2.0.

Usage
~~~~~

-  Add the both of the files to the shell directory as xsh\_udpclient.c
   and xsh\_udpserver.c
-  Modify shell.c and shell.h to include the udpclient and udpserver
   commands.
-  Modify the Makefile in the shell directory to include
   xsh\_udpclient.c and xsh\_udpserver.c and then make XINU from the
   compile directory
-  Boot two separate XINU consoles
-  Run netup command from the shell on both consoles
-  Run udpserver from the shell on the first console
-  Run udpclient from the shell on the second console with the arguments
   of the udpserver's ip and the message to be sent in quotes

   -  eg: udpclient 192.168.6.102 "Hello XINU World!"

Source
~~~~~~

**UDP Client** #include <stddef.h> #include <stdio.h> #include
<device.h> #include <ether.h> #include <udp.h> #include <string.h>
#define MSG\_MAX\_LEN 64 #define ECHO\_PORT 9989 /\*\* \* Shell command
(udpclient) runs a client that sends an ASCII message over the network
to a server using UDP. \* Expects arg0 to be echoclient, args1 to be the
destination IP address, args2 to be the message in quotes \* @param
nargs number of arguments in args array \* @param args array of
arguments \* @return non-zero value on error \*/ shellcmd
xsh\_udpclient(int nargs, char \*args[]) { ushort dev = 0; char
buf[MSG\_MAX\_LEN]; char \*dest = args[1]; struct netaddr dst; struct
netaddr \*localhost; struct netif \*interface; /\* Allocate a new UDP
device \*/ if ((ushort)SYSERR == (dev = udpAlloc())) { fprintf(stderr,
"Client: Failed to allocate a UDP device."); return SYSERR; } /\* Look
up local ip info \*/ interface = netLookup((ethertab[0].dev)->num); if
(NULL == interface) { fprintf(stderr, "Client: No network interface
found\\r\\n"); return SYSERR; } localhost = &(interface->ip); /\* Change
the destination to ipv4 \*/ if (SYSERR == dot2ipv4(dest, &dst)) {
fprintf(stderr, "Client: Failed to convert ip address."); return SYSERR;
} /\* Open the UDP device with the destination and echo port\*/ if
(SYSERR == open(dev, localhost, &dst, NULL, ECHO\_PORT)) {
fprintf(stderr, "Client: Could not open the UDP device\\r\\n"); return
SYSERR; } /\* Send the message to the destination\*/ memcpy(buf,
args[2], MSG\_MAX\_LEN); if(SYSERR == write(dev, buf, MSG\_MAX\_LEN)) {
close(dev); return SYSERR; } /\* Closee the device when done \*/
close(dev); return 0; }

**UDP Server** #include <stddef.h> #include <stdio.h> #include
<device.h> #include <udp.h> #include <stdlib.h> #include <ether.h>
#include <string.h> #define ECHO\_PORT 9989 /\*\* \* Shell command
(udpserver) runs a UDP server that waits for an incoming message, and
then prints it out. Does not expect any arguments. \* @param nargs
number of arguments in args array \* @param args array of arguments \*
@return non-zero value on error \*/ shellcmd xsh\_echoserver(int nargs,
char \*args[]) { ushort dev = 0; int len = 0; char
buffer[UDP\_MAX\_DATALEN]; struct netaddr \*localhost; struct netif
\*interface; struct udpPseudoHdr \*pseudo; struct udpPkt \*udp; /\*
Allocate a new UDP device \*/ if ((ushort)SYSERR == (dev = udpAlloc()))
{ fprintf(stderr, "Server: Failed to allocate a UDP device.\\r\\n");
return SYSERR; } /\* Look up local ip info \*/ interface =
netLookup((ethertab[0].dev)->num); if (NULL == interface) {
fprintf(stderr, "Server: No network interface found\\r\\n"); return
SYSERR; } /\* Open the UDP device using localhost and the echo port to
listen to\*/ localhost = &(interface->ip); if (SYSERR == open(dev,
localhost, NULL, ECHO\_PORT, NULL)) { fprintf(stderr, "Server: Could not
open the UDP device\\r\\n"); return SYSERR; } /\* Set the UDP device to
passive mode \*/ if (SYSERR == control(dev, UDP\_CTRL\_SETFLAG,
UDP\_FLAG\_PASSIVE, NULL)) { kprintf("Server: Could not set UDP device
to passive mode\\r\\n"); close(dev); return SYSERR; } /\* Read lop, wait
for a new request \*/ printf("Server: Waiting for message\\r\\n"); while
(SYSERR != (len = read(dev, buffer, UDP\_MAX\_DATALEN))) { pseudo =
(struct udpPseudoHdr \*)buffer; udp = (struct udpPkt \*)(pseudo + 1);
printf("Server: Received Message - %s\\r\\n", udp->data); } close(dev);
return 0; }
