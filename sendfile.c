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

// -------------------------------------------------------------------------

// FUnction to be called when error occured

void die(char *s)
{
    perror(s);
    exit(1);
}

// --------------------------------------------------------------------------

// Function to check if the data is corrupted

uint8_t checksum(char* segment,int n){
	int i;
	uint8_t ret = 0x0;
	for(i=0; i<n; i++) {
		ret += segment[i];
	}
	return ret;
}

// --------------------------------------------------------------------------

// Function to makea segment from data to be sent over network

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


// --------------------------------------------------------------------------

// Function to check ACK

int checkACK(char* ack){
	uint8_t temp;
	memcpy((char*) &temp, &ack[6], sizeof(char));

	if(checksum(ack,6)==temp){
		return 1;
	} 
	else{
		return 0;
		printf("corrupt\n");
	}
}

//----------------------------------------------------------------------------

// Function to take minium of two number

int min(int a, int b){
	if(a<b){
		return a;
	}
	else{
		return b;
	}
}


//----------------------------------------------------------------------------

// Main Function

int main(int argc, char* argv[]){
	char* filename = argv[1];
	uint8_t windowsize = atoi(argv[2]);
	int buffersize = atoi(argv[3]);
	char* dest = argv[4];
	int port = atoi(argv[5]);
	int port2;


	uint8_t receiver_windowsize = windowsize;

	unsigned char buff[buffersize];
	memset(buff,'\0', buffersize);

	int validatedACK[buffersize];
	memset(validatedACK, 0, buffersize*4);

	// -----------------------------------------------------------------------

	//Preparation to send and receive
	struct sockaddr_in si_other, si_me;
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

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    memset((char*) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    if(port == 8888){
    	port2 = 8889;
    }
    else{
    	port2 = 8888;
    }
    si_me.sin_port = htons(port2);

    // set timeout
	if(setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0){
	    die("setsockopt failed");
	}

	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }


    //-------------------------------------------------------------------------

    //Preparation open file
    FILE *fp = fopen(filename, "rb");

    if(fp==NULL){
    	die("open file");
    }

    //--------------------------------------------------------------------------

    int index_last_file_segment_sent = -1;
    
    while(1){
    	int nread = fread(buff, 1, buffersize, fp);

    	int LAR = -1;
    	int LFS = LAR + (int) windowsize;
    	printf("cek1\n");
    	if(nread > 0){
    		printf("cek2\n");
    		while(LAR<nread-1){  //while we have not finished sending all data in buff
    			printf("cek3\n");
    			printf("nread = %d\n", nread);
    			printf("LAR = %d\n", LAR);

    			for(int l=0; l<buffersize; l++){
    				printf("%d ", validatedACK[l]);
    			}
    			printf("\n");

    			int j = 0;
    			for(int i=LAR+1; i<=min(nread-1, min(LFS, LAR + (int) receiver_windowsize)); i++){
    				if(!validatedACK[i]){
	    				char* segment = (char*)malloc(9);
	    
	        			makeSegment(segment, buff[i], index_last_file_segment_sent+j+1);
	        			if (sendto(s, segment, 9, 0 , (struct sockaddr *) &si_other, slen)==-1)
		        		{
		            		die("sendto()");
		        		}
		        		
		        		j++;
		        		free(segment);
		        	}
    			}

    			/*
    			char* ack = (char*)malloc(7);
    			recv_len = recvfrom(s2, ack, 7, 0, (struct sockaddr *) &si_other, &slen);
    			if(recv_len > 0){
    				if(checkACK(ack)){
    					memcpy((char*)&index_last_file_segment_sent, &ack[1], sizeof(int));
    					
    					LAR = index_last_file_segment_sent%buffersize;
    					LFS = min(buffersize-1, LAR+(int) windowsize);

    					memcpy((char*)&receiver_windowsize, &ack[5], sizeof(char));
    				}
    			}
    			free(ack);*/

    			
    			for(int i=LAR+1; i<=min(nread-1, min(LFS, LAR + (int) receiver_windowsize)); i++){
    				if(!validatedACK[i]){ //only accept packet which have not been validated
    					char* ack = (char*)malloc(7);
    				
    					int recv_len = recvfrom(s, ack, 7, 0, (struct sockaddr *) &si_other, &slen);
    					
    				
    					if(recv_len > 0){
    						if(checkACK(ack)){
    							int tempIndex;
    							memcpy((char*)&tempIndex, &ack[1], sizeof(int));
    							memcpy((char*)&receiver_windowsize, &ack[5], sizeof(char));
    							
    							//LAR = index_last_file_segment_sent%buffersize;
    							//LFS = min(buffersize-1, LAR+(int) windowsize);
    							printf("ack=%d\n", tempIndex);
    							validatedACK[tempIndex%buffersize] = 1;
    						}
    					}
    					else{
    						printf("timeout\n");
    					}
    					free(ack);
    				}
    			}


    			int k;
    			for(k=LAR+1; k<=LFS; k++){
    				if(validatedACK[k]==0){
    					break;
    				}
    			}
    			index_last_file_segment_sent += k - LAR - 1;
    			LAR = k-1;
    			LFS = min(buffersize-1, LAR+windowsize);
    			
    		}

    		memset(buff, '\0', buffersize);
    		memset(validatedACK, 0, buffersize*4);
    	}

    	if(nread<buffersize){
    		printf("akhirnya\n");
    		if(feof(fp)){
    			int endIsConfirmed = 0;

    			while(!endIsConfirmed){
    				char* segment = (char*)malloc(9);
    				char endC = 'a';
    				makeSegment(segment, endC, -1);
    				if (sendto(s, segment, 9, 0 , (struct sockaddr *) &si_other, slen)==-1)
		        	{
		            	die("sendto()");
		        	}
		        	free(segment);
    				
		        	char* confirmation = (char*)malloc(7);
		        	int recv_len = recvfrom(s, confirmation, 7, 0, (struct sockaddr *) &si_other, &slen);

		        	free(confirmation);

		        	if(recv_len > 0){
		        		endIsConfirmed = 1;
		        	}
    			}

    			printf("Finished sending file.\n");
    		}

    		if(ferror(fp)){
    			printf("Error in reading file\n");
    		}

    		break;
    	}
    }

    fclose(fp);
    close(s);

    return 0;
}