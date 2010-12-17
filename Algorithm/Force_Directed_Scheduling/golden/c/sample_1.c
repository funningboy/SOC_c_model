#include <stdio.h>
#include <stdlib.h>

int a =10;
int b =10;
int w =10;
int g =10;
int s =10;
int c;
int d;
int e;

void golden (){
 c = a + b>>1;
 d = w * ( a - b );
 e = d -c - g * c;

}

void dump (){ 

//printf ("c :: %d\n",c);
//printf ("d :: %d\n",d);
printf ("e :: %d\n",e);
}

int main(){

golden();
dump();

return 0;
}


