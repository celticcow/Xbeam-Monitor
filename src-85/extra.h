#ifndef _EXTRA_H
#define _EXTRA_H

/**
 * Poet        : Gregory Dunlap
 * Email       : gregory.dunlap@fedex.com
 * Version     : 0.3
 * Last Update : 05.28.08
**/

/**
 * Used as helper functions for cbeaml.c
 * Needs to be in same directory for compile
**/

//prototypes
char*  itoa(char s[], int n);
void   reverse(char s[]);
char*  ftoa(char *str, double number);



char* ftoa(char *str, double number)
{
  sprintf(str, "%f", number);
  //need to return value so we can pass result as func param
  return str;
}


//found on the interwebs
/* itoa:  convert n to characters in s */
char* itoa(char s[], int n)
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

    return(s);
} 

//found on the interwebs
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

#endif
