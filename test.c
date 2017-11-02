#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

#define SOH 0x1
#define STX 0x2
#define ETX 0x3
#define ACK 0x6

uint8_t checksum(char* segment,int n){
	int i;
	uint8_t ret = 0x0;
	for(i=0; i<n; i++) {
		ret += segment[i];
	}
	return ret;
}

void makeSegment(char* segment,char c, int seq) {
	char temp[4];
	memcpy(temp, (char*) &seq, sizeof(int));
	
	segment[0] = SOH;
	segment[1] = temp[0];
	segment[2] = temp[1];
	segment[3] = temp[2];
	segment[4] = temp[3];
	segment[5] = STX;
	segment[6] = c;
	segment[7] = ETX;
	segment[8] = checksum(segment,8);
}

int checkPacket(char* packet){
	uint8_t temp;
	memcpy((char*) &temp, &packet[8], sizeof(char));

	if(checksum(packet,8)==temp){
		return 1;
	}
	else{
		return 0;
		printf("corrupt\n");
	}
}

int main(){
	char* segment = (char*)malloc(9);

	makeSegment(segment, 'p', 1);

	printf("%d\n", checkPacket(segment));


}