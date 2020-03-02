// client code for UDP socket programming 
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
#define nofile "File Not Found!" 
#define yesfile "The File Already Exists!"
#define searchDir "dir"

// function to clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// function for decryption 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// function sending file 
int sendFile(FILE* fp, char* buf, int s) 
{ 
	char ch, ch2; 
	for (int i = 0; i < s; i++) { 
		ch = fgetc(fp); 
		ch2 = Cipher(ch); 
		buf[i] = ch2; 
		if (ch == EOF) 
			return 1; 
	} 
	return 0; 
} 

/* checks if file should be sent
	returns 0 if it should send file */
int checkSend(char* buf, int s) 
{ 
	int i, length; 
	char ch; 
	char temp_buf[NET_BUF_SIZE];
	strcpy(temp_buf, yesfile);
	length = strlen(yesfile);
	temp_buf[length] = EOF;
	puts(buf);
	if(!strcmp(buf, temp_buf)) {
		printf("Error Message: ");
		for (i = 0; i < s; i++) { 
			ch = buf[i]; 
			if (ch == EOF)
				return 1; 
			else
				printf("%c", ch); 
		} 
	} else {
		printf("File Created\n\n");
		puts(buf);
		return 0;
	}
} 

// driver code 
int main(int argc, char *argv[]) 
{ 
	int sockfd, nBytes, dirFlag; 
	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(atoi(argv[2]));
	addr_con.sin_addr.s_addr = inet_addr(argv[1]);
	char net_buf[NET_BUF_SIZE];
	char dir_buf[NET_BUF_SIZE]; 
	strcpy(dir_buf, searchDir);
	FILE* fp; 

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, 
					IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n\n"); 
	else
		printf("\nfile descriptor %d received\n\n", sockfd); 

	if(connect(sockfd, (struct sockaddr *)&addr_con, sizeof(addr_con)) == 0) 
    {
		printf("Connection Established\n");
		printf("IP:%s Port:%s\n", argv[1], argv[2]);
	} else {
		printf("Failed to connect");
	}
	while (1) { 
		printf("\nPlease enter file name to send to Server:\n"); 
		scanf("%s", net_buf); 

		if (!strcmp(net_buf, dir_buf)) dirFlag = 1;		

		//Send file name
		sendto(sockfd, net_buf, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen); 

		// recv response
		clearBuf(net_buf); 
		nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
						sendrecvflag, (struct sockaddr*)&addr_con, 
						&addrlen); 
						
		puts(net_buf);

		// Check if file should be sent
		int result = checkSend(net_buf, NET_BUF_SIZE);
		printf("%d\n", result);
		if (result) { 
			printf("File will not be sent\n");
			clearBuf(net_buf);
			break; 
		} else if(dirFlag) {
			printf("Registered as dir");
		} else {
			printf("Made it past check send\n");
			puts(net_buf);
			fp = fopen(net_buf, "r"); 

			if (fp == NULL) 
		 		printf("\nFile open failed!\n"); 
		    else
		 		printf("\nFile Successfully opened!\n"); 

			clearBuf(net_buf);
			while (1) { 

				printf("Made it to process and send\n");
				
			// process 
			if (sendFile(fp, net_buf, NET_BUF_SIZE)) { 
				sendto(sockfd, net_buf, NET_BUF_SIZE, 
					sendrecvflag, 
					(struct sockaddr*)&addr_con, addrlen); 
				break; 
			} 

			// send 
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, 
				(struct sockaddr*)&addr_con, addrlen); 
			clearBuf(net_buf); 
			} 
			if (fp != NULL) fclose(fp);
		}
		//printf("\n\n-------------------------------\n"); 
	} 
	return 0; 
} 
