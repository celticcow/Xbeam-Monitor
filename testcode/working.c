#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>


void itoa(int n, char s[]);
void reverse(char s[]);
char* ftoa(char *str, double number);

int main()
{
  int temp1 = 17;
  float temp2 = 3.14159;
  char out[30];
  char out2[30];

  //sprintf(out2, "%f", temp2);

  //ftoa(out2, temp2);

  itoa(temp1, out);

  printf("%s\n", out);
  //printf("%s\n", out2);
  //printf("%c\n", out2[1]);
 
  printf("%s\n", ftoa(out2, temp2));
  printf("%c\n", out2[1]);

  return(0);
}

char* ftoa(char *str, double number)
{
  sprintf(str, "%f", number);  
  return str;
}


/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
  int i, sign;

  if ((sign = n) < 0)  /* record sign */
    n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}


/* reverse:  reverse string s in place */
void reverse(char s[])
{
  int c, i, j;

  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}
