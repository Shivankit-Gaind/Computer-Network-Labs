/*
		Simple udp client pktloss
*/

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h> // close() function
#include <signal.h>
#include <stdbool.h>
#define BUFLEN 512 //Max length of buffer
# define PORT 8883 //The port on which to listen for incoming data


volatile sig_atomic_t received_flag = false; // A boolean variable required to stop re-transmitting once ACK is received

typedef struct packet1 {
	int sq_no;
}
ACK_PKT;

typedef struct packet2 {
	int sq_no;
	char data[BUFLEN];
}
DATA_PKT;

struct sockaddr_in si_other;
int s, i, slen = sizeof(si_other);

char message[BUFLEN];
DATA_PKT send_pkt, rcv_ack;

void die(char * s) {
	perror(s);
	exit(1);
}

/* Interrupt handler to re-transmit the packet */
void handle_alarm(int sig) {

	if (received_flag == false) // don't re-transmit of ACK received
	{

		if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr * ) & si_other, slen) == -1) {
			die("sendto()");
		}

		alarm(1); // Re schedule the timer
	}
}

int main(void) {

	signal(SIGALRM, handle_alarm); // Re-transmission Handler

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		die("socket");
	}

	memset((char * ) & si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

	int state = 0;
	while (1) {

		switch (state) {
		case 0:
			printf("Enter message 0: "); //wait for sending packet with seq. no. 0
			fgets(send_pkt.data, sizeof(send_pkt), stdin);
			send_pkt.sq_no = 0;
			if (sendto(s, & send_pkt, sizeof(send_pkt), 0, (struct sockaddr * ) & si_other, slen) == -1) {
				die("sendto()");
			}
			state = 1;
			received_flag = false; 
			alarm(1); // Run the timer
			break;

		case 1: //wait for ACK 0
			if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr * ) & si_other, &slen) == -1) {
				die("recvfrom()");
			}

			if (rcv_ack.sq_no == 0) {
				printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
				state = 2;
				received_flag = true; // Message received, stop re-transmiting
				break;
			}

		case 2:
			printf("Enter message 1: "); //wait for sending packet with seq. no. 1
			fgets(send_pkt.data, sizeof(send_pkt), stdin);
			send_pkt.sq_no = 1;
			if (sendto(s, & send_pkt, sizeof(send_pkt), 0, (struct sockaddr * ) & si_other, slen) == -1) {
				die("sendto()");
			}
			state = 3;
			received_flag = false;
			alarm(1); // Run the timer

			break;

		case 3: //waiting for ACK 1
			if (recvfrom(s, & rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr * ) & si_other, & slen) == -1) {
				die("recvfrom()");
			}
			if (rcv_ack.sq_no == 1) {
				printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
				state = 0;
				received_flag = true; //message received, stop re-transmitting
				break;
			}

		}

	}
	close(s);
	return 0;
}
