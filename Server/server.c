// server code for UDP socket programming 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define IP_PROTOCOL 0 
#define NET_BUF_SIZE 128
#define cipherKey 'S' 
#define sendrecvflag 0 
#define connectionEstablished "Connection Established"
#define failedConnection "Failed to Connect"
#define yesfile "The File Already Exists!" 
#define searchDir "dir"

// function to clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// function to encrypt 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// function to receive file 
int recvFile(FILE* fp, char* buf, int s) 
{ 
	int i; 
	char ch;
    fseek(fp, 0, SEEK_SET );
	for (i = 0; i < s; i++) {
		ch = buf[i]; 
		ch = Cipher(ch); 
		if (ch == EOF) 
			return 1; 
		else
			printf("%c\t", ch);
	    	char err = fputc(ch, fp);
    		printf ("err=%c\n", err);
	} 
	return 0; 
} 

/* Check if file exists within directory
	Returns 0 if file does not exist. */
int fileExists(char* buf, int size)
{
	int length;
	printf("Inside file exists\n");
	if (access (buf, F_OK) == -1) {			//Returns -1 if file does not exist
		printf("File Doesn't Exist\n");
		return 0;
	} else {
		printf("File already exists\n");
		clearBuf(buf);
		strcpy(buf, yesfile); 
		puts(buf);
		length = strlen(yesfile);
		buf[length] = EOF;
		return 1;
	}
}

// driver code 
int main(int argc, char *argv[]) 
{ 
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(atoi(argv[1])); 

	addr_con.sin_addr.s_addr = INADDR_ANY; 
	char net_buf[NET_BUF_SIZE];
	char dir_buf[NET_BUF_SIZE];
	strcpy(dir_buf, searchDir);
	FILE* fp; 

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 

	// bind() 
	clearBuf(net_buf);
	int length;
	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0) {
		printf("\nConnected!\n");
	} else {
		printf("\nFailed!\n"); 
	}

	// sendto(sockfd, net_buf, NET_BUF_SIZE, 
	// 				sendrecvflag, 
	// 				(struct sockaddr*)&addr_con, addrlen);

	while (1) { 
		printf("\nWaiting for file name...\n"); 

		// receive file name 
		clearBuf(net_buf); 
		nBytes = recvfrom(sockfd, net_buf, 
						NET_BUF_SIZE, sendrecvflag, 
						(struct sockaddr*)&addr_con, &addrlen); 
		printf("\nFile Name Received: %s\n", net_buf);
		if(!strcmp(net_buf, dir_buf)) {
			printf("registed as dir");
		} else {
			// Make file if it doesn't exist
			if(!fileExists(net_buf, NET_BUF_SIZE))
			{
				fp = fopen(net_buf, "w+");
			}

			// Send information back to client
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
						sendrecvflag, 
						(struct sockaddr*)&addr_con, addrlen);

			while (1) { 
				// receive 
				clearBuf(net_buf); 
				nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
								sendrecvflag, (struct sockaddr*)&addr_con, 
								&addrlen); 

				// process 
				if (recvFile(fp, net_buf, NET_BUF_SIZE)) { 
					break; 
				}
				if (fp != NULL) fclose(fp);
			}
		}
	} 
	return 0; 
}