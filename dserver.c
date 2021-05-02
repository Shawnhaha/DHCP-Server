/*
DHCP Server
Author: Shangda Li
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct dhcp_pkt {			//dhcp packet struct
	char siaddr[32]; // Server IP address
	char ip[32];		//client IP address
	int tran_id;  	//transaction ID
	int lifetime;		//Lease time of the IP address
};

struct ip_address {			//ip structure
	char ip[32];
	int available; 		// ip availability
};


void die(char *s) {			//error
    perror(s);
    exit(1);
}

//check for available IP address
int avail_ip(struct ip_address arr[], int IP_COUNT){	//pass ip address pool to the function
	int i;
	for(i = 0; i < IP_COUNT; i++)
	{
		if(arr[i].available == 1)
		return i;		// if available, return the ip index
	}
return -1; // if no ip available, return -1
}

int main(int argc, char *argv[]) {
	struct sockaddr_in si_me, si_other;
	int sockfd, slen = sizeof(si_other), PORT;
	struct dhcp_pkt msg;			// dhcp packet struct
	char gateway[32];
	int gatebytes[4];
	int subnet;
	int i, index;

	if(argc == 2) {
     PORT = atoi(argv[1]); //assgin port number
    }
  else {
     printf("Please run DHCP server with a valid port number\n");
     exit(1);
  }

	//ask network address
	printf("network address: ");
	scanf("%s", gateway);
	sscanf(gateway, "%d.%d.%d.%d", &gatebytes[3], &gatebytes[2], &gatebytes[1], &gatebytes[0]);

	//ask subnet part
	printf("subnet part:");
	scanf("%d", &subnet);
	while (subnet < 24 || subnet > 30) {
		printf("Please input a subnet part greater than 23 bits and less than 31 bits.\n");
		printf("subnet part:");
		scanf("%d", &subnet);
	}

	int IP_COUNT; // number of IP addresses per subnet
	switch(subnet) {
         case 24:
					 IP_COUNT = 254;
					 break;
         case 25:
					 IP_COUNT = 126;
					 break;
         case 26:
					 IP_COUNT = 62;
					 break;
				 case 27:
					 IP_COUNT = 30;
					 break;
	 		 	 case 28:
					 IP_COUNT = 14;
					 break;
	 			 case 29:
					 IP_COUNT = 6;
					 break;
				 case 30:
					 IP_COUNT = 2;
					 break;
         default:
           break;
  }


	struct ip_address addr_arr[IP_COUNT];		   //creat a ip address pool

	int j = gatebytes[0] + 1;
	for (i = 0; i < IP_COUNT; i++) {
			sprintf(addr_arr[i].ip, "%d.%d.%d.%d", gatebytes[3], gatebytes[2], gatebytes[1], j);
			addr_arr[i].available = 1;
			j++;
	}

	//create a UDP socket
	if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		die("socket");
	}

	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));


	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if(bind(sockfd, (struct sockaddr*) &si_me, sizeof(si_me)) < 0)
	{
		die("bind");
	}


	while(1)    	//keep listening for new data
	{
		printf("Waiting for a client's message...\n\n");
		fflush(stdout);

		//DHCP discover
		if (recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
		{
			die("DHCP discover\n");
		}
		printf("DHCP discover\nRecieved from Client:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
		printf("\n");

		if(strcmp(msg.ip, "0.0.0.0") == 0) {
	    		index = avail_ip(addr_arr, IP_COUNT);
 		}
		strcpy(msg.ip, addr_arr[index].ip);
		msg.lifetime = 3600;  // assgin 3600 seconds to lease time


		//DHCP offer
		if (sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, slen) < 0)
		{
			die("DHCP offer");
		}

		printf("DHCP offer\nSent to Client:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
		printf("\n");

		//DHCP request
		if (recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
		{
			die("DHCP request");
		}

		printf("DHCP request\nRecieved from Client:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
		printf("\n");

		//The DHCP server registers that the IP address is taken
		if(strcmp(msg.ip, addr_arr[index].ip) == 0) {
			addr_arr[index].available = 0;
    }

		//DHCP ACK
		if (sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, slen) < 0)
		{
			die("DHCP ACK");
		}
		printf("DHCP ACK\nSent to Client:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
		printf("\n");
	}

	close(sockfd);

return 0;
}
