
#include <stdio.h>
#include <stdlib.h>

char* int2bin(int a) {
char *str,*tmp;
 int cnt = 31;
 str = (char *) malloc(33); //32 + 1 , because its a 32 bit bin number
 tmp = str;
 while ( cnt > -1 ){
      str[cnt]= '0';
      cnt --;
 }
 cnt = 31;
 while (a > 0){
       if (a%2==1){
           str[cnt] = '1';
        }
      cnt--;
        a = a/2 ;
 }
 return tmp;
}


