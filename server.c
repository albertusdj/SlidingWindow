#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define ACK 0x6
 
void die(char *s)
{
    perror(s);
    exit(1);
}

void makeACK(char* ack, int nextSeqNumber, uint8_t ad_windowsize){
	ack[0] = ACK;
	char temp[4];
	memcpy(temp, (char*) &seq, sizeof(int));
	ack[1] = temp[0];
	ack[2] = temp[1];
	ack[3] = temp[2];
	ack[4] = temp[3];

	ack[5] = ad_windowsize;
	ack[6] = 0x0;
}
 
int main(int argc, char* argv[])
{
	char* filename = argv[1];
	int windowsize = atoi(argv[2]);
	int buffersize = atoi(argv[3]);
	int port = atoi(argv[4]);
	
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    unsigned char buff[buffersize];
    memset(buff, 0, buffersize);
     
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
    
    char tempBuff[9];

    int socket;
    if ((socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    struct sockaddr_in address;
    

    while((recv_len) = recvfrom(s, tempBuff, 9, 0, (struct sockaddr *) &si_other, &slen)!=-1){
    	FILE * fp;
    	fp = fopen("text2.txt", "ab");

    	if(fp==NULL){
    		die("file open");
    	}

	    fwrite(tempBuff, 1, recv_len, fp);
	    fclose(fp);


    }

	/*while((recv_len = recvfrom(s, buff, buffersize, 0, (struct sockaddr *) &si_other, &slen))!=-1)
	{
	  	FILE * fp;
    	fp = fopen("text2.txt", "ab");

    	if(fp==NULL){
    		die("file open");
    	}

	    fwrite(buff, 1, recv_len, fp);
	    fclose(fp);
	}*/

	if(recv_len < 0){
	    printf("No data\n");
	}
 
    close(s);
    return 0;
}
