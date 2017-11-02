#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#define SOH 0x1
#define STX 0x2
#define ETX 0x3
#define ACK 0x6

int seqNum = 0;
 
void die(char *s)
{
    perror(s);
    exit(1);
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
	segment[8] = 0x0;
}

int isAckAccepted(char * ackbuff){
	if(ackbuff[0]==ACK){
		int ackseqnumber;

		memcpy((char*) &ackseqnumber, &ackbuff[1], sizeof(int));

		if(ackseqnumber!=seqNum){
			return 1;
		}
	}
	else{
		return 0;
	}
}



int main(int argc, char* argv[])
{	
	char* filename = argv[1];
	int windowsize = atoi(argv[2]);
	int buffersize = atoi(argv[3]);
	char* dest = argv[4];
	int port = atoi(argv[5]);
	
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
     
    if (inet_aton(dest , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    FILE *fp = fopen("test.txt", "rb");

    if(fp==NULL){
    	die("open file");
    }

    int sock;
    struct sockaddr_in myaddress;
    int myaddrlen = sizeof(myaddress);
    char ackbuff[7];
    memset((char*) &myaddress, 0, sizeof(myaddress));
    myaddress.sin_family = AF_INET;
    if(port==8889){
        myaddress.sin_port = htons(8888);
    }
    else{
        myaddress.sin_port = htons(8889);
    }
    myaddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
	    die("socket for receive ack");
	}

	struct timeval timeout;
    timeout.tv_sec = 0.1;
    timeout.tv_usec = 0;


	if(setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
	    die("setsockopt failed");
	}

	memset(ackbuff, '\0', 7);

	if( bind(sock , (struct sockaddr*)&myaddress, sizeof(myaddress) ) == -1)
    {
        die("bind");
    }

    while(1)
    {
    	unsigned char buff[buffersize];
    	memset(buff,'\0', buffersize);
    	int nread = fread(buff, 1, buffersize, fp);
        
        if(nread > 0){
        	int index = 0;

        	while(index<256){
        		int ackAccepted = 0;
        		while(!ackAccepted){
        			char* segment = (char*)malloc(9);
        			makeSegment(segment, buff[index], seqNum);
        			if (sendto(s, segment, 9, 0 , (struct sockaddr *) &si_other, slen)==-1)
	        		{
	            		die("sendto()");
	        		}

	        		int recv_len;
	        		recv_len = recvfrom(sock, ackbuff, 7, 0, (struct sockaddr *) &myaddress, &myaddrlen)==-1;
	        		
	        		if(isAckAccepted(ackbuff)==1){
	        			ackAccepted = 1;
	        			if(seqNum==0){
	        				seqNum = 1;
	        			}
	        			else{
	        				seqNum = 0;
	        			}
	        		}

        		}
        		index++;
        	}
	        
	    }

        if(nread < 256){
        	if(feof(fp)){
        		printf("End of file\n");
        	}

        	if(ferror(fp)){
        		printf("Error reading\n");
        	}
        	break;
        }
    }
 
    close(s);
    return 0;
}
