
#include <stdio.h>


int  add (int a, int b);


int add(int a, int b ){ return a+b; }

int main (){

int a =0;
int b=2;

if(a==b) printf("eq\n"); 
else printf("ne\n"); 

printf("%d\n", add(2,4));

return 0;
}
