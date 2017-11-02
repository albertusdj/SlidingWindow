#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

#define ACK 0x6

void die(char *s)
{
    perror(s);
    exit(1);
}

int min(int a, int b){
	if(a<b){
		return a;
	}
	else{
		return b;
	}
}

uint8_t checksum(char* segment,int n){
	int i;
	uint8_t ret = 0x0;
	for(i=0; i<n; i++) {
		ret += segment[i];
	}
	return ret;
}

void makeACK(char* ack, uint8_t windowsize, int seq){
	char temp[4];
	memcpy(temp, (char*) &seq, sizeof(int));

	ack[0] = ACK;
	ack[1] = temp[0];
	ack[2] = temp[1];
	ack[3] = temp[2];
	ack[4] = temp[3];
	ack[5] = windowsize;
	ack[6] = checksum(ack,6);
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

int isEnd(char* packet){

}

int main(int argc, char* argv[]){
	char* filename = argv[1];
	int windowsize = atoi(argv[2]);
	int buffersize = atoi(argv[3]);
	int port = atoi(argv[4]);
	int port2;

	if(windowsize > buffersize){
        windowsize = buffersize;
    }	

	FILE *f = fopen(filename,"w");
	fclose(f);

	struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    unsigned char buff[buffersize];
    memset(buff, '\0', buffersize);
    
    int validatedPacket[buffersize];
    memset(validatedPacket, 0, buffersize*4);

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    memset((char*) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    if(port==8888){
    	port2 = 8889;
    }
    else{
    	port2 = 8888;
    }
    si_other.sin_port = htons(port2);

//---------------------------------------------------------------------

 
    int isEndOfSending = 0;

	while(!isEndOfSending){
		int LFR = -1;
    	int LAF = LFR + windowsize;
    	
		while(LFR!=buffersize-1){
			char* tempPacket = (char*)malloc(9);

			recv_len = recvfrom(s, tempPacket, 9, 0, (struct sockaddr *) &si_other, &slen);

			if(checkPacket(tempPacket)){
				int seqnumber;
				memcpy((char*)&seqnumber, &tempPacket[1], sizeof(int));

				printf("seqnumber = %d\n", seqnumber);

				if(seqnumber==-1){
					isEndOfSending = 1;
					char* ack = (char*)malloc(7);

					makeACK(ack, 1, -1);

					if (sendto(s, ack, 7, 0 , (struct sockaddr *) &si_other, slen)==-1){
			        	die("sendto()");
			    	}

					break;
				}
				else{
					buff[seqnumber%buffersize] = tempPacket[6];
					validatedPacket[seqnumber%buffersize] = 1;

					char* ack = (char*)malloc(7);

					makeACK(ack, (uint8_t) LAF - LFR, seqnumber);

					if (sendto(s, ack, 7, 0 , (struct sockaddr *) &si_other, slen)==-1){
			        	die("sendto()");
			    	}

					int k;
					for(k=LFR+1; k<=LAF; k++){
						if(validatedPacket[k]==0){
							break;
						}
					}
					LFR = k-1;
					LAF = min(LFR + (int)windowsize, buffersize-1);
				}	    	
		    }

		}

		FILE * fp;
    	fp = fopen(filename, "ab");

    	if(fp==NULL){
    		die("file open");
    	}

	    fwrite(buff, sizeof(char), LFR+1, fp);
	    fclose(fp);

	    memset(buff, '\0', buffersize);
	    memset(validatedPacket, 0, buffersize*4);
	}
}