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
 * Last Update : 10.25.10
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

void  tokencpuline(char *line, unsigned long long int parts[7]);
void  getcpuinfo(char *command, unsigned long long int cpudata[7], unsigned long long int cpudata0[7], unsigned long long int cpudata1[7]);
void  getcpuaverage(char *command, float cpuavg[7], float core0[7], float core1[7]);
void  getmeminfo(char *command1, char *command2, int meminfo[2]);
int   getfwconn(char *command);
float percent(unsigned long long int u1, unsigned long long int u2, unsigned long long int total1, unsigned long long int total2);
void  getvrrp(char *command, int status[2]);
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
	  
	  // Commands we need to run on APM
	  char *part1 = "/usr/bin/rsh -l root ";
	  // put tag here
	  char *part2 = " \"/bin/cat /proc/stat | /bin/grep -i \'cpu\'\"";
	  char *part31 = " \"/bin/cat /proc/meminfo | /bin/grep -i \'MemTotal:\' | /bin/grep -v \'RawMemTotal:\'\"";
	  char *part32 = " \"/bin/cat /proc/meminfo | /bin/grep -i \'MemFree:\'\"";
	  //Part 4 used for R70
	  char *part4 = " \". /opt/CPshrd-R70/tmp/.CPprofile.sh; /opt/CPsuite-R70/fw1/bin/fw tab -t connections -s\"";
	  //command runs on CPM, not APM like others
	  ////for 8.x and later
	  char *cmdvrrp = "/crossbeam/bin/cli -i \"show vrrp detail-status\""; 
	  ////for XOS 8.0.1
	  //char *cmdvrrp = "/crossbeam/bin/cli -i \"show vrrp status\""; 
	  char *cmdint  = "/crossbeam/bin/cli -i \"show redundancy-interface\"";

	  char *command1 = NULL;
	  char *command21 = NULL;
	  char *command22 = NULL;
	  char *command3 = NULL;
	  char *command4 = NULL;
	  char *command5 = NULL;

	  float cpuavg[7];
	  float cpucore0[7];
	  float cpucore1[7];
	  
	  int meminfo[2];
	  int vrrp[2];       // active and passive
	  int reduniface[2]; // active and backup
	  int fwconn = 0;

	  //allocate memory for command strings
	  command1 = (char *)calloc(strlen(part1) + strlen(part2) + strlen(tag) + 1, sizeof(char));
	  command21 = (char *)calloc(strlen(part1) + strlen(part31) + strlen(tag) + 1, sizeof(char));
	  command22 = (char *)calloc(strlen(part1) + strlen(part32) + strlen(tag) + 1, sizeof(char));
	  command3 = (char *)calloc(strlen(part1) + strlen(part4) + strlen(tag) + 1, sizeof(char));
	  command4 = (char *)calloc(strlen(cmdvrrp), sizeof(char));
	  command5 = (char *)calloc(strlen(cmdint), sizeof(char));

	  //construct commands
	  strcpy(command1, part1);
	  strcat(command1, tag);
	  strcat(command1, part2);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command1);
	    }
	  strcpy(command21, part1);
	  strcat(command21, tag);
	  strcat(command21, part31);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command21);
	    }
	  strcpy(command22, part1);
	  strcat(command22, tag);
	  strcat(command22, part32);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command22);
	    }
	  strcpy(command3, part1);
	  strcat(command3, tag);
	  strcat(command3, part4);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command3);
	    }
	  strcpy(command4, cmdvrrp);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command4);
	    }
	  strcpy(command5, cmdint);
	  if(debug == 1)
	    {
	      printf("***%s***\n", command5);
	    }

	  /*function calls that do all the work, 
	    cause the man likes to hold functions down
	  **/

	  if(strcmp(tag, "vrrp") ==0)
	    {
	      if(debug == 1)
		{
		  printf("VRRP ONLY\n");
		}
	      getvrrp(command4, vrrp);
	    }
	  if(strcmp(tag, "redif") == 0)
	    {
	      if(debug == 1)
		{
		  printf("Checking for redundent iface\n");
		}
	      getreinterface(command5, reduniface);
	    }

	  if(strcmp(nagcmd,"all") == 0)
	    {
	      if(debug == 1)
		{
		  printf("do it all\n");
		}
	      getcpuaverage(command1, cpuavg, cpucore0, cpucore1);
	      getmeminfo(command21, command22, meminfo);
	      fwconn = getfwconn(command3);
	      //getvrrp(command4, vrrp);
	    }
	  else if(strcmp(nagcmd,"cpu") == 0)
	    {
	      if(debug == 1)
		{
		  printf("Just some CPU\n");
		}
	      getcpuaverage(command1, cpuavg, cpucore0, cpucore1);
	    }
	  else if(strcmp(nagcmd,"mem") == 0)
	    {
	      if(debug == 1)
		{
		  printf("Like Memory Only\n");
		}
	      getmeminfo(command21, command22, meminfo);
	      
	    }
	  else if(strcmp(nagcmd,"conn") == 0)
	    {
	      if(debug == 1)
		{
		  printf("FW CONN ONLY \n");
		}
	      fwconn = getfwconn(command3);
	    }
	  else if(strcmp(nagcmd,"vrrp") == 0)
	    {
	      //do nothing, covered with tag, this is just for symetry
	      //printf("VRRP ONLY\n");
	      //getvrrp(command4, vrrp);
	    }
	  else if(strcmp(nagcmd, "redif") == 0)
	    {
	      //do nothing, covered with tag, this is a cpm command
	    }
	  else
	    {
	      printf("THIS IS MADNESS\n");
	      //something bad happened
	    }
	  	  
	  if(debug == 1)
	    {
	      printf("*******AVG****************\n");
	      printf("User     : %f--\n", cpuavg[0]);
	      printf("Nice     : %f--\n", cpuavg[1]);
	      printf("System   : %f--\n", cpuavg[2]);
	      printf("Idle     : %f--\n", cpuavg[3]);
	      printf("IOWait   : %f--\n", cpuavg[4]);
	      printf("IRQ      : %f--\n", cpuavg[5]);
	      printf("Soft IRQ : %f--\n", cpuavg[6]);
	      printf("***********************\n");
	      printf("*******CORE 0*************\n");
	      printf("User     : %f--\n", cpucore0[0]);
	      printf("Nice     : %f--\n", cpucore0[1]);
	      printf("System   : %f--\n", cpucore0[2]);
	      printf("Idle     : %f--\n", cpucore0[3]);
	      printf("IOWait   : %f--\n", cpucore0[4]);
	      printf("IRQ      : %f--\n", cpucore0[5]);
	      printf("Soft IRQ : %f--\n", cpucore0[6]);
	      printf("*******CORE 1*************\n");
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
	      printf("VRRP Status: %i Active %i Passive", vrrp[0], vrrp[1]);
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
	  if((strcmp(nagcmd,"vrrp") == 0))
	    {
	      write(client_sockfd, itoa(temp3, vrrp[0]), vrrp[0]>9 ?strlen(temp3)+1:strlen(temp3));
	      write(client_sockfd, "\n", 1);
	      write(client_sockfd, itoa(temp3, vrrp[1]), vrrp[1]>9 ?strlen(temp3)+1:strlen(temp3));
	      write(client_sockfd, "\n", 1);
	    }
	  else if((strcmp(nagcmd,"redif") == 0))
	    {
	      write(client_sockfd, itoa(temp3, reduniface[0]), reduniface[0]>9 ?strlen(temp3)+1:strlen(temp3));
	      //printf("--%s--\n", temp3);
	      write(client_sockfd, "\n", 1);
	      write(client_sockfd, itoa(temp3, reduniface[1]), reduniface[1]>9 ?strlen(temp3)+1:strlen(temp3));
	      write(client_sockfd, "\n", 1);
	    }
	  // APM command returns

	  ftoa(temp3,0); //conditioning for float to ascii

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
	    http://xkcd.com/371/
	  **/
	  free(command1);
	  free(command21);
	  free(command22);
	  free(command3);
	  free(command4);
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
  //changed from unsigned int to unsigned long long 
  
  unsigned long long int cpuavg0[7];
  unsigned long long int cpu00[7];
  unsigned long long int cpu01[7];
  
  unsigned long long int cpuavg1[7];
  unsigned long long int cpu10[7];
  unsigned long long int cpu11[7];
  
  
  //                   avg     core0  core1  core2  core3  core4  core5  core6  core7
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
      printf("%i: %lld\n",j, cpuavg0[j]);
    }
  printf("\n\ncpuavg 1 line\n");
  for(j = 0; j < 7; j++)
    {
      printf("%i: %lld\n",j, cpuavg1[j]);
    }
  printf("------------------------------\n");
  **/

  //do average first
  unsigned long long int totalavg1 = cpuavg0[0] + cpuavg0[1] + cpuavg0[2] + cpuavg0[3] + cpuavg0[4] + cpuavg0[5] + cpuavg0[6]; 
  unsigned long long int totalavg2 = cpuavg1[0] + cpuavg1[1] + cpuavg1[2] + cpuavg1[3] + cpuavg1[4] + cpuavg1[5] + cpuavg1[6];

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

//void  getcpuinfo(char *command, unsigned int cpudata[7], unsigned int cpudata0[7], unsigned int cpudata1[7], unsigned int cpudata2[7], unsigned int cpudata3[7], unsigned int cpudata4[7], unsigned int cpudata5[7], unsigned int cpudata6[7], unsigned int cpudata7[7])
void  getcpuinfo(char *command, unsigned long long int cpudata[7], unsigned long long int cpudata0[7], unsigned long long int cpudata1[7])
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
  printf("%s\n",cpu2);
  printf("%s\n",cpu3);
  printf("%s\n",cpu4);
  printf("%s\n",cpu5);
  printf("%s\n",cpu6);
  printf("%s\n",cpu7);
  printf("***********************\n");
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

void getmeminfo(char *command1, char *command2, int meminfo[2])
{
  /*todo fix return values for meg
  **/

  FILE* stream1 = popen(command1, "r");
  FILE* stream2 = popen(command2, "r");
  size_t len = 0;
  char *memline = NULL;
  char *temp  = NULL;

  getline(&memline, &len, stream1);
 
  temp = strtok(memline, " "); // get title off line
  meminfo[0] = atoi(strtok(NULL, " "));///1024; //convert to kB
  
  getline(&memline, &len, stream2);
 
  temp = strtok(memline, " "); // get title off line
  meminfo[1] = atoi(strtok(NULL, " "));

  pclose(stream1);
  pclose(stream2);
}

void getvrrp(char *command, int status[2])
{
  //captain place holder
  FILE* stream = popen(command, "r");
  size_t len = 0;
  char *temp = NULL;
  int master  = 0;
  int passive = 0;

  char *subm = "Master";
  char *subp = "Backup";
  
  while(!feof(stream))
    {
      getline(&temp, &len, stream);

      char *result1 = strstr(temp, subm);
      char *result2 = strstr(temp, subp);

      if(result1 != NULL)
	{
	  master++;
	}
      if(result2 != NULL)
	{
	  passive++;
	}

      //printf("---%s**", temp);
    }

  //printf("end of vrrp\n");
  status[0] = master;
  status[1] = passive;
  pclose(stream);
}

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
//was an unsigned int
void tokencpuline(char *line, unsigned long long int parts[7])
{
  char *temp = NULL;
  int i = 0;

  temp = strtok(line, " "); // get title off line
  do {
    temp = strtok(NULL, " ");
    if(temp)
      {
	//printf("in token this is the index and temp\n");
	//printf("%u: %s\n", i,temp); //debug
	//parts[i] = 4294967296ULL;
	parts[i] = strtoull(temp,NULL,0); //parts[i] = atol(temp); //was atoi --gdunlap 07.15.08
	//printf("in token this is parts long long int\n");
	//printf("%llu\n",parts[i]); //debug
	i++;
      }
  } while(temp);
}

float percent(unsigned long long int u1, unsigned long long int u2, unsigned long long int total1, unsigned long long int total2)
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
