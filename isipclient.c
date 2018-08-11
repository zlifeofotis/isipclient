/*
 * Copyright (C) Chesley Dio
 */

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "isipclient.h"

 void do_data(int,char*);
 void do_pipe(int);

 int main(int argc,char** argv){
 	int sockfd;
 	int i;
 	struct sockaddr_in srvaddr;
 	char* str;
 	struct sigaction act;
 	memset(&act,0,sizeof(act));
 	act.sa_handler = do_pipe;
 	sigemptyset(&act.sa_mask);
 	sigaction(SIGPIPE,&act,NULL);

 	if(argc != 3){
 		puts(" usage:\n    isipclient <srv ip> <srv port>\n");
 		exit(1);
 	}
 	memset(&srvaddr,0,sizeof(struct sockaddr_in));
 	srvaddr.sin_family = AF_INET;
 	srvaddr.sin_addr.s_addr = inet_addr(argv[1]);
 	srvaddr.sin_port	= htons((uint16_t)atoi(argv[2]));
	//printf("%s\n",argv[1]);
	//printf("%s\n",argv[2]);
	//printf("%ld\n",srvaddr.sin_addr.s_addr);
	//printf("%d\n",srvaddr.sin_port);

 	sockfd = socket(PF_INET,SOCK_STREAM,0);
	//printf("socket result: %d\n",sockfd);
 	if(sockfd == -1){
 		perror("socket error!");
 		exit(1);
 	}
 	i = connect(sockfd,(struct sockaddr*)&srvaddr,sizeof(struct sockaddr_in));
	//printf("connect result: %d\n",i);
 	if(i == -1){
 		perror("connect error!");
 		exit(1);
 	}
 	puts("connecting ok!");

 	str = malloc(1536);
 	if(!str){
 		perror("malloc(1536)");
 		exit(1);
 	}

 	while(1)
 		do_data(sockfd,str);
 	return(0);
 }

 void do_data(int sockfd,char* str){
 	isip_msg_t msg;
 	char mbody[1024];
 	size_t len;
 	char* line = NULL;
 	size_t len1 = 0;
 	char* str1;
	//isip_msg_t* msg1;

 	len = sizeof(msg);
 	memset(str,0,1536);
 	memset(&msg,0,len);
 	str1 = str;

 	puts("isipclient>[3 characters] [>0 and <1024]");
 	fputs("isipclient>",stdout);

	/*istyle hold 3 characters,len is length of message and it is less than 1024*/
 	while(scanf("%s %d",msg.istyle,&msg.len) != 2){
		/*clear buffer residual data*/
 		getline(&line,&len1,stdin);
 		fflush(stdin);
 		fputs("isipclient>",stdout);
 	}
 	fflush(stdin);
 	printf("message header=>%s %d\n",msg.istyle,msg.len);

	/*clear buffer residual data*/
 	getline(&line,&len1,stdin);
 	fflush(stdin);

 	puts("isipclient>[Less than 1024 characters]");
 	fputs("isipclient>",stdout);
 	memset(mbody,0,1024);
 	fgets(mbody,1024,stdin);
	/*clear end charater,which is '\n'*/
 	mbody[strlen(mbody)-1] = '\0';
	//gets(mbody);
	//mbody[1023] = '\0';
	//printf("message body=>%d %02x%02x%02x%02x%02x\n",strlen(mbody),mbody[0],mbody[1],mbody[2],mbody[3],mbody[4]);
 	printf("message body=>%s\n",mbody);

	//printf("%d\n",len);
 	
 	memcpy(str,(char*)&msg,len);
 	str += len;
 	strcpy(str,mbody);
 	len += strlen(mbody);
 	
	//msg1 = (isip_msg_t*)str1;
	//printf("%s,%d\n",msg1->istyle,msg1->len);
	//printf("%s\n",str);
 	if(write(sockfd,str1,len) == -1)
 		perror("write(sockfd,str1,len)");

 }

 void do_pipe(int signum){
 	printf("%d happen\n",signum);

 }
