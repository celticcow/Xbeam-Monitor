#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <list>
#include "cpu.h"

/**
 * Name: Greg Dunlap
 * Date: 12.17.2012
**/

/**
 * Crossbeam Monitoring
 * run as a daemon on the CPM, Query with virtual blade name
 * i.e., ACluster_1 passing back with the system commands via rsh
 * back to this program.  This is a re-write in c++ so we can do 
 * variable cpu number checks
**/

using namespace std;

int main()
{
	int debug = 1;
	
	 int server_sockfd;
	 int client_sockfd;
	 socklen_t server_len;
	 socklen_t client_len;
	 struct sockaddr_in server_address;
	 struct sockaddr_in client_address;
	 
	 server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	 
	 server_address.sin_family = AF_INET;
	 server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	 server_address.sin_port = htons(9999);
	 server_len = sizeof(server_address);
	 
	 bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	 
	 listen(server_sockfd, 5);
	 signal(SIGCHLD, SIG_IGN);
	 
	 while(1)
	 {
		 char tag[50];
		 char nagcmd[50];
		 		 
		 client_len = sizeof(client_address);
		 client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
		 
		 if(fork() == 0)
		 {
			 read(client_sockfd, tag, 49); // read from network
			 for(int i = 0; i < 50; i++)
			 {
				 //if not a letter, number or _ mark it end of string ... basically chomp from perl
				 if(!(((tag[i] >= 'a') && (tag[i] <= 'z')) || ((tag[i] >= 'A') && (tag[i] <= 'Z')) || (tag[i] == '_') || ((tag[i] >= 48) && (tag[i] <= 57))))
				 {
					 tag[i] = '\0';
					 break;
				 }
			 }
			 read(client_sockfd, nagcmd, 49); // read from network
			 for(int i = 0; i < 50; i++)
			 {
				 //if not a letter, number or _ mark it end of string ... basically chomp from perl
				 if(!(((nagcmd[i] >= 'a') && (nagcmd[i] <= 'z')) || ((nagcmd[i] >= 'A') && (nagcmd[i] <= 'Z')) || (nagcmd[i] == '_') || ((nagcmd[i] >= 48) && (nagcmd[i] <= 57))))
				 {
					 nagcmd[i] = '\0';
					 break;
				 }
			 }
			 
			 if(debug == 1)
			 {
				 cout << "*** Debug ***" << endl;
				 cout << "*********" << endl;
				 cout << tag << endl;
				 cout << nagcmd << endl;
				 cout << "*********" << endl;
			 }
			 
			 
			 /**
			  * Function calls that do all the work
			  * cause the man likes to hold functions down
			  **/
			  if(strcmp(tag, "vrrp") == 0)
			  {
			  }
			  if(strcmp(tag, "redif") == 0)
			  {
			  }
			  
			  if(strcmp(nagcmd, "all") == 0)
			  {
			  }
			  else if(strcmp(nagcmd, "cpu") == 0)
			  {
				  if(debug == 1)
				  {
					  cout << "---------" << endl;
					  cout << "--cpu--" << endl;
					  cout << "---------" << endl;
				  }
			  }
			  else if(strcmp(nagcmd, "mem") == 0)
			  {
			  }
			  else if(strcmp(nagcmd, "conn") == 0)
			  {
			  }
			  else if(strcmp(nagcmd, "vrrp") == 0)
			  {
				  //do nothing, covered with tag ... just for symetry
			  }
			  else if(strcmp(nagcmd, "redif") == 0)
			  {
				  //do nothing, covered with tag ... just for symetry
			  }
			  else
			  {
				  // if we hit this ... something bad happened
			  }
			 
			 write(client_sockfd, tag, strlen(tag));
			 write(client_sockfd, "\n", 1);
			 
			 close(client_sockfd);
			 exit(0);
			 
		 }// end of if(fork)
		 else
		 {
			 close(client_sockfd);
		 }
		 
	 }// end of while(1)
	
	return(0);
} // end of program
