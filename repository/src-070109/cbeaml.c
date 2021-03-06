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
 * Poet        : Gregory Dunlap
 * Email       : gregory.dunlap@fedex.com
 * Last Update : 06.20.08
**/

/**
 * Crossbeam APM monitor
 * run as a daemon on CPM. Query with virtual 
 * blade name i.e. ACluster_1 passing back with
 * system commands via rsh back to this program
 * Would be a whole lot simpler if they would let
 * me put my own OID into the snmpd.conf file
 **/

/**
 * Todo:
 * work on memory area to return Meg (open)
 *
**/


//using namespace std;

void  tokencpuline(char *line, unsigned int parts[7]);
void  getcpuinfo(char *command, unsigned int cpudata[7], unsigned int cpudata0[7], unsigned int cpudata1[7]);
void  getcpuaverage(char *command, float cpuavg[7], float core0[7], float core1[7]);
void  getmeminfo(char *command, int meminfo[2]);
int   getfwconn(char *command);
float percent(unsigned int u1, unsigned int u2, unsigned int total1, unsigned int total2);

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
	  
	  // Commands we need to run on APM
	  char *part1 = "/usr/bin/rsh -l root ";
	  // put tag here
	  char *part2 = " \"/bin/cat /proc/stat | /bin/grep -i \'cpu\'\"";
	  char *part3 = " \"/bin/cat /proc/meminfo | /bin/grep -i \'Mem:\'\"";
	  //Part 4 used for R61
	  //char *part4 = " \". /opt/CPshrd-R61/tmp/.CPprofile.sh; /opt/CPsuite-R61/fw1/bin/fw tab -t connections -s\"";
	  //Part 4 used for R65
	  char *part4 = " \". /opt/CPshrd-R65/tmp/.CPprofile.sh; /opt/CPsuite-R65/fw1/bin/fw tab -t connections -s\"";

	  char *command1 = NULL;
	  char *command2 = NULL;
	  char *command3 = NULL;
	  float cpuavg[7];
	  float cpucore0[7];
	  float cpucore1[7];
	  int meminfo[2];
	  int fwconn = 0;

	  //allocate memory for command strings
	  command1 = (char *)calloc(strlen(part1) + strlen(part2) + strlen(tag) + 1, sizeof(char));
	  command2 = (char *)calloc(strlen(part1) + strlen(part3) + strlen(tag) + 1, sizeof(char));
	  command3 = (char *)calloc(strlen(part1) + strlen(part4) + strlen(tag) + 1, sizeof(char));

	  //construct commands
	  strcpy(command1, part1);
	  strcat(command1, tag);
	  strcat(command1, part2);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command1);
	    }
	  strcpy(command2, part1);
	  strcat(command2, tag);
	  strcat(command2, part3);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command2);
	    }
	  strcpy(command3, part1);
	  strcat(command3, tag);
	  strcat(command3, part4);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command3);
	    }
	  
	  /*function calls that do all the work, 
	    cause the man likes to hold functions down
	  **/

	  if(strcmp(nagcmd,"all") == 0)
	    {
	      //printf("do it all\n");
	      getcpuaverage(command1, cpuavg, cpucore0, cpucore1);
	      getmeminfo(command2, meminfo);
	      fwconn = getfwconn(command3);
	    }
	  else if(strcmp(nagcmd,"cpu") == 0)
	    {
	      //printf("Just some CPU\n");
	      getcpuaverage(command1, cpuavg, cpucore0, cpucore1);
	    }
	  else if(strcmp(nagcmd,"mem") == 0)
	    {
	      //printf("Like Memory Only\n");
	      getmeminfo(command2, meminfo);
	      
	    }
	  else if(strcmp(nagcmd,"conn") == 0)
	    {
	      //printf("FW CONN ONLY \n");
	      fwconn = getfwconn(command3);
	    }
	  else
	    {
	      printf("THIS IS MADNESS\n");
	      //something bad happened
	    }
	  	  
	  if(debug == 1)
	    {
	      printf("***********************\n");
	      printf("User     : %f--\n", cpuavg[0]);
	      printf("Nice     : %f--\n", cpuavg[1]);
	      printf("System   : %f--\n", cpuavg[2]);
	      printf("Idle     : %f--\n", cpuavg[3]);
	      printf("IOWait   : %f--\n", cpuavg[4]);
	      printf("IRQ      : %f--\n", cpuavg[5]);
	      printf("Soft IRQ : %f--\n", cpuavg[6]);
	      printf("***********************\n");
	      printf("***********************\n");
	      printf("User     : %f--\n", cpucore0[0]);
	      printf("Nice     : %f--\n", cpucore0[1]);
	      printf("System   : %f--\n", cpucore0[2]);
	      printf("Idle     : %f--\n", cpucore0[3]);
	      printf("IOWait   : %f--\n", cpucore0[4]);
	      printf("IRQ      : %f--\n", cpucore0[5]);
	      printf("Soft IRQ : %f--\n", cpucore0[6]);
	      printf("***********************\n");
	      printf("***********************\n");
	      printf("User     : %f--\n", cpucore1[0]);
	      printf("Nice     : %f--\n", cpucore1[1]);
	      printf("System   : %f--\n", cpucore1[2]);
	      printf("Idle     : %f--\n", cpucore1[3]);
	      printf("IOWait   : %f--\n", cpucore1[4]);
	      printf("IRQ      : %f--\n", cpucore1[5]);
	      printf("Soft IRQ : %f--\n", cpucore1[6]);
	      printf("***********************\n");
	      printf("Mem Total: %i--\n", meminfo[0]);
	      printf("Mem Used : %i--\n", meminfo[1]);
	      printf("***********************\n");
	      printf("FW Conn  : %i--\n", fwconn);
	      printf("***********************\n");
	    }

	  char temp3[100];

	  //write data out to socket
	  int x = 0;
	  /*
	    Not exactly sure about this line.  Get garbage on first output
	    if this is not here.  Need to condition temp3 it seems ... strange
	    stuff.  Something to look at some day ... 
	  **/
	  ftoa(temp3,0);

	  //all  cpu  mem  conn
	  
	  if((strcmp(nagcmd,"all") == 0) || (strcmp(nagcmd,"cpu") == 0))
	    {
	      for(x = 0; x < 7; x++)
		{
		  write(client_sockfd, ftoa(temp3,cpuavg[x]), strlen(temp3));
		  write(client_sockfd, "\n", 1); //flush the buffers
		}
	      for(x = 0; x < 7; x++)
		{
		  write(client_sockfd, ftoa(temp3,cpucore0[x]), strlen(temp3));
		  write(client_sockfd, "\n", 1);
		}
	      for(x = 0; x < 7; x++)
		{
		  write(client_sockfd, ftoa(temp3,cpucore1[x]), strlen(temp3));
		  write(client_sockfd, "\n", 1);
		}
	    }
	  if((strcmp(nagcmd,"all") == 0) || (strcmp(nagcmd,"mem") == 0))
	    {
	      for(x = 0; x < 2; x++)
		{
		  write(client_sockfd, itoa(temp3,meminfo[x]), strlen(temp3));
		  write(client_sockfd, "\n", 1);
		}
	    }
	  if((strcmp(nagcmd,"all") == 0) || (strcmp(nagcmd,"conn") == 0))
	    {
	      write(client_sockfd, ftoa(temp3,fwconn), strlen(temp3));
	      write(client_sockfd, "\n", 1);	  
	    }
	  
  	  /*
	    free our memory we dynamically allocated, 
	    cause seg faults make the computers cry
	  **/
	  free(command1);
	  free(command2);
	  free(command3);
	
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

void getcpuaverage(char *command, float cpuavg[7], float core0[7], float core1[7])
{
  //call getcpuinfo with 0 set
  //sleep(2);
  //call getcpuinfo with 1 set
  /*
    0 user
    1 nice
    2 system
    3 idle
    4 iowait (think)
    5 irq (think)
    6 soft irq
  **/

  unsigned int cpuavg0[7];
  unsigned int cpu00[7];
  unsigned int cpu01[7];
  unsigned int cpuavg1[7];
  unsigned int cpu10[7];
  unsigned int cpu11[7];

  //                   avg     core0  core1
  getcpuinfo(command, cpuavg0, cpu00, cpu01);
  sleep(2);
  getcpuinfo(command, cpuavg1, cpu10, cpu11);

  /*
    debug stuff, temp only
  
  int j = 0;
  printf("------------------------------\n");
  printf("cpuavg 0 line\n");
  for(j = 0; j < 7; j++)
    {
      printf("%i\n",cpuavg0[j]);
    }
  printf("cpuavg 1 line\n");
  for(j = 0; j < 7; j++)
    {
      printf("%i\n",cpuavg1[j]);
    }
  printf("------------------------------\n");
  **/

  //do average first
  unsigned int totalavg1 = cpuavg0[0] + cpuavg0[1] + cpuavg0[2] + cpuavg0[3] + cpuavg0[4] + cpuavg0[5] + cpuavg0[6]; 
  unsigned int totalavg2 = cpuavg1[0] + cpuavg1[1] + cpuavg1[2] + cpuavg1[3] + cpuavg1[4] + cpuavg1[5] + cpuavg1[6];

  //printf("stuff %i %i %i %i %i %i %i\n",cpuavg0[0],cpuavg0[1],cpuavg0[2],cpuavg0[3],cpuavg0[4],cpuavg0[5],cpuavg0[6]);
  //printf("\ntotal0 = %i\n",totalavg1);
  //printf("\ntotal1 = %i\n",totalavg2);
  
  // core0
  int totalcpu01 = cpu00[0] + cpu00[1] + cpu00[2] + cpu00[3] + cpu00[4] + cpu00[5] + cpu00[6]; 
  int totalcpu02 = cpu10[0] + cpu10[1] + cpu10[2] + cpu10[3] + cpu10[4] + cpu10[5] + cpu10[6];

  // core1
  int totalcpu11 = cpu01[0] + cpu01[1] + cpu01[2] + cpu01[3] + cpu01[4] + cpu01[5] + cpu01[6]; 
  int totalcpu12 = cpu11[0] + cpu11[1] + cpu11[2] + cpu11[3] + cpu11[4] + cpu11[5] + cpu11[6];

  int i = 0;
  for(i = 0; i < 7; i++)
    {
      cpuavg[i] = percent(cpuavg0[i], cpuavg1[i], totalavg1, totalavg2);
    }
  for(i = 0; i < 7; i++)
    {
      core0[i] = percent(cpu00[i], cpu10[i], totalcpu01, totalcpu02);
    }
  for(i = 0; i < 7; i++)
    {
      core1[i] = percent(cpu01[i], cpu11[i], totalcpu11, totalcpu12);
    }
}

void getcpuinfo(char *command, unsigned int cpudata[7], unsigned int cpudata0[7], unsigned int cpudata1[7])
{
  //printf("getcpuinfo\n%s\n",command);

  FILE* stream = popen(command, "r");
  size_t len = 0;
  
  char *cpua = NULL;
  char *cpu0 = NULL;
  char *cpu1 = NULL;

  getline(&cpua, &len, stream);
  getline(&cpu0, &len, stream);
  getline(&cpu1, &len, stream);

  /*
    debug info. temp only
  printf("Inside getcpuinfo running getlinex3\n");
  printf("%s\n",cpua);
  printf("%s\n",cpu0);
  printf("%s\n",cpu1);
  **/

  pclose(stream);
 
  tokencpuline(cpua, cpudata);
  tokencpuline(cpu0, cpudata0);
  tokencpuline(cpu1, cpudata1);
}

int getfwconn(char *command)
{
  FILE* stream = popen(command, "r");
  size_t len = 0;
  char *header = NULL;
  char *data   = NULL;
  int num = 0;

  getline(&header, &len, stream);
  getline(&data, &len, stream);
 
  //printf("^^^^^^^^^^^^^^^\n");
  //printf("%s\n", header);
  //printf("%s\n", data);
  //printf("^^^^^^^^^^^^^^^\n");

  char *temp = NULL;
  temp = strtok(data, " ");
  temp = strtok(NULL, " ");
  temp = strtok(NULL, " ");
  num  = atoi(strtok(NULL, " "));  //tokenize and grab what we need
    
  pclose(stream);

  return(num);
}

void getmeminfo(char *command, int meminfo[2])
{
  /*todo fix return values for meg
  **/

  FILE* stream = popen(command, "r");
  size_t len = 0;
  char *memline = NULL;
  char *temp  = NULL;

  getline(&memline, &len, stream);
 
  temp = strtok(memline, " "); // get title off line
  meminfo[0] = atoi(strtok(NULL, " "))/1024; //convert to kB
  meminfo[1] = atoi(strtok(NULL, " "))/1024;

  pclose(stream);
}


void tokencpuline(char *line, unsigned int parts[7])
{
  char *temp = NULL;
  int i = 0;

  temp = strtok(line, " "); // get title off line
  do {
    temp = strtok(NULL, " ");
    if(temp)
      {
	//printf("%u: %s\n", i,temp); //debug
	parts[i] = strtoul(temp,NULL,0); //parts[i] = atol(temp); //was atoi --gdunlap 07.15.08
	//printf("%u\n",parts[i]); //debug
	i++;
      }
  } while(temp);
}

float percent(unsigned int u1, unsigned int u2, unsigned int total1, unsigned int total2)
{
  float cpu_used;

  if((total2-total1) != 0) 
    {
      cpu_used = (100*(double)(u2-u1))/(double)(total2-total1);
    }
  else 
    {
      cpu_used = 0;
    }
    
  return(cpu_used);
}
