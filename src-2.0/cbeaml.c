#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "extra.h"

/**
 * Name        : Gregory Dunlap
 * Email       : gregory.dunlap@fedex.com
 * Last Update : 06.25.13
**/

/**
 * Version 2.0 takes advantage of the XOS 9.6.x tree 
 * that will trigger and alarm for vrrp failover and
 * that now we can hit the Checkpoint SNMP daemon
 * to get true multi-core CPU values as well as the
 * firewall stats (memory/conns)
 **/

/**
 * Todo:
 *
**/


//using namespace std;

void  getreinterface(char *command, int status[2]);

int main()
{
  int debug = 0;

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
	  //chomd the string we passed in
	  read(client_sockfd, tag, 49);
	 
	  int i;
	  for(i = 0; i < 50; i++)
	    {
	      //printf("%i+%c+\n", i, tag[i]);
	      //if not a letter,number or _ mark it end of string
	      if(!(((tag[i] >= 'a') && (tag[i] <= 'z')) || ((tag[i] >= 'A') && (tag[i] <= 'Z')) || (tag[i] == '_') || ((tag[i] >= 48) && (tag[i] <= 57))))
		{
		  tag[i] = '\0';
		  break;
		}
	    }//end of chmod
	  
	  
	  read(client_sockfd, nagcmd, 49);
	  for(i = 0; i < 50; i++)
	    {
	      //printf("%i+%c+\n", i, tag[i]);
	      //if not a letter,number or _ mark it end of string
	      if(!(((nagcmd[i] >= 'a') && (nagcmd[i] <= 'z')) || ((nagcmd[i] >= 'A') && (nagcmd[i] <= 'Z')) || (nagcmd[i] == '_') || ((nagcmd[i] >= 48) && (nagcmd[i] <= 57))))
		{
		  nagcmd[i] = '\0';
		  break;
		}
	    }//end of chmod
	   
	  if(debug == 1)
	    {
	      printf("%s--\n", nagcmd);
	    }
	  
	  char *cmdint  = "/crossbeam/bin/cli -i \"show redundancy-interface\"";

	  char *command5 = NULL;
	  
	  int reduniface[2]; // active and backup
	  
	  //allocate memory for command strings
	  command5 = (char *)calloc(strlen(cmdint), sizeof(char));

	  //construct commands
	  
	  strcpy(command5, cmdint);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command5);
	    }

	  /*function calls that do all the work, 
	    cause the man likes to hold functions down
	  **/
	  
	  if(strcmp(tag, "redif") == 0)
	    {
	      if(debug == 1)
		{
		  printf("Checking for redundent iface\n");
		}
	      getreinterface(command5, reduniface);
	    }
	  else
	    {
	      printf("THIS IS MADNESS\n");
	      //something bad happened
	    }
	  	  
	  if(debug == 1)
	    {
	      printf("***********************\n");
	      printf("Interface Status: %i Active %i Passive", reduniface[0], reduniface[1]);
	    }

	  char temp3[100];

	  //write data out to socket
	  int x = 0;
	  /*
	    Not exactly sure about this line.  Get garbage on first output
	    if this is not here.  Need to condition temp3 it seems ... strange
	    stuff.  Something to look at some day ... 
	  **/
	  //
	  itoa(temp3,0); // used for the int's for cpm commands

	  // CPM Command Returns
	  // if vrrp[x] is larger than 9 we need additional byte of space to send
	  if((strcmp(nagcmd,"redif") == 0))
	    {
	      write(client_sockfd, itoa(temp3, reduniface[0]), reduniface[0]>9 ?strlen(temp3)+1:strlen(temp3));
	      //printf("--%s--\n", temp3);
	      write(client_sockfd, "\n", 1);
	      write(client_sockfd, itoa(temp3, reduniface[1]), reduniface[1]>9 ?strlen(temp3)+1:strlen(temp3));
	      write(client_sockfd, "\n", 1);
	    }
	  
	  
  	  /*
	    free our memory we dynamically allocated, 
	    cause seg faults make the computers cry
	    http://xkcd.com/371/
	  **/
	  
	  free(command5);
	
          close(client_sockfd);
          exit(0);
        }
      else
        {
          close(client_sockfd);
        }
    }
  return 0;
} //end of main

void  getreinterface(char *command, int status[2])
{
  FILE* stream = popen(command, "r");
  size_t len = 0;
  char *temp = NULL;
  int master  = 0;
  int passive = 0;

  char *subm = "Master";
  char *subp = "Backup";
  char *act  = "Active"; //garbage line
  
  while(!feof(stream))
    {
      getline(&temp, &len, stream);

      char *result1 = strstr(temp, subm);
      char *result2 = strstr(temp, subp);
      char *resultg = strstr(temp, act);

      if(resultg != NULL)
	{
	  //garbage line header, ignore or counts get messed up
	}
      else if(result1 != NULL)
	{
	  master++;
	}
      else if(result2 != NULL)
	{
	  passive++;
	}

      //printf("---%s**", temp);
    }

  //printf("end of redundancy-interface\n");
  status[0] = master;
  status[1] = passive;
  pclose(stream);
}
