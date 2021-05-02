This program is to simulate the working of DHCP server using a client-server architecture.

Usage:
Put all files to one directory on cse machine.

Type "make" to run makefile.

After makefile runs, the TCP server and client will be compiled.

The executable files called "dserver" and "dclient".

Run the DHCP server on cse03.cse.unt.edu machine with "./dserver <port_number>".
It will ask you network address and subnet part as follows:
network address:
subnet_part:
The network address is the IP address of the gateway or the router. The subnet part
indicates the subnet part of the subnet mask. The program only accpects a subnet
part that is greater than 23 bits and less than 31 bits which means the DHCP server
can only support a max of 254 hosts. For example: A subnet part input of 24 tells
that the subnet mask is 255.255.255.0 and the lower 8 bits is the host part which
changes according to each host.

Run the DHCP client on any other cse machine with "./dclient <port_number>".

Run the server and client with a same valid port number.

After using the program, you can use "make clean" to remove all executable files.
