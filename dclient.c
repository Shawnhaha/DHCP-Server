/*
DHCP Client
Author: Shangda Li
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER "129.120.151.96"		 	//server ip address
#define TRANS_MIN 100 				//minimum value of transaction id
#define TRANS_MAX 999 				//maximum value of transaction id

struct dhcp_pkt {   //dhcp packet struct
  char siaddr[32]; // Server IP address
  char ip[32];		  //client IP address
	int tran_id;			//transaction ID
	int lifetime;			//Lease time of the IP address
};


void die(char *s) { //error
    perror(s);
    exit(1);
}


int main(int argc, char *argv[]) {
	struct sockaddr_in si_other;				//sock address structure
	int sockfd, i, slen=sizeof(si_other), PORT;
  struct dhcp_pkt msg;    // dhcp packet struct

  if(argc == 2) {
     PORT = atoi(argv[1]); //assgin port number
  }
  else {
     printf("Please run client with a valid port number.\n");
     exit(1);
  }

  //create a UDP socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		die("socket");
	}

  // zero out the structure
	memset((char *) &si_other, 0, sizeof(si_other));

	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);

	if (inet_aton(SERVER , &si_other.sin_addr) == 0)
  {
		die("inet_aton()");
	}

  //intialize the DHCP structure's variables
  strcpy(msg.siaddr, SERVER);
	strcpy(msg.ip, "0.0.0.0");
	msg.tran_id = rand() % (TRANS_MAX + TRANS_MIN);				//assign a random numver to transaction ID
  msg.lifetime = 0;

	//DHCP discover
	if (sendto(sockfd, &msg, sizeof(msg) , 0, (struct sockaddr *) &si_other, slen) < 0)
	{
		die("DHCP discover");
	}
	printf("DHCP discover\nSent to Server:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
	printf("\n");

  //DHCP offer
	if (recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
	{
		 die("DHCP offer");
	}
	printf("DHCP offer\nRecieved from Server:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
	printf("\n");

  msg.tran_id++;		//old transaction id + 1

	//DHCP request
	if (sendto(sockfd, &msg, sizeof(msg) , 0, (struct sockaddr *) &si_other, slen) < 0)
	{
		die("DHCP request");
	}

	printf("DHCP request\nSent to Server:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
	printf("\n");

	//DHCP ACK
	if (recvfrom(sockfd, &msg, sizeof(msg), 0, (struct sockaddr *) &si_other, &slen) < 0)
	{
		 die("DHCP ACK");
	}

	printf("DHCP ACK\nRecieved from Server:\n\tsiaddr = %s\n\tyiaddr = %s\n\ttransaction id = %d\n\tlifetime = %d secs\n", msg.siaddr, msg.ip, msg.tran_id, msg.lifetime);
	printf("\n");
	close(sockfd);

  return 0;
}
