#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main()
{

  FILE* stream=popen("/bin/cat /home/gdunlap/Office/Xbeam-Monitor/rawdata.txt","r");
  char* buffer=NULL;
  size_t len=0;
  int value=0;
  
  char *cpu  = NULL;
  char *cpu0 = NULL;
  char *cpu1 = NULL;


  getline(&cpu, &len, stream);
  getline(&cpu0, &len, stream);
  getline(&cpu1, &len, stream);
  
  /*
  if(stream && getline(&buffer, &len, stream))
    {
      printf("%s\n", buffer);
    }
  else 
    {
      perror("popen");
      exit(0);
    }
  **/

  printf("%s\n", cpu);
  printf("%s\n", cpu0);
  printf("%s\n", cpu1);

  pclose(stream);
  free(buffer);
  
  int cpuavg[7]; 

  char *temp = NULL;
  int i = 0;

  temp = strtok(cpu, " ");
  printf("x: %s\n",temp);

  do
    {
      temp = strtok(NULL, " ");
      if(temp)
	{
	  printf("%i: %s\n", i,temp);
	  cpuavg[i] = atoi(temp);
	  i++;
	}
    }
  while(temp);
  
  int total = 0;

  total = cpuavg[0] + cpuavg[1] + cpuavg[2] + cpuavg[3] + cpuavg[4] + cpuavg[5] + cpuavg[6];

  printf("%i\n", total);

  return(0);
}
