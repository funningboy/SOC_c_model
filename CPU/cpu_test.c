
// how to compile 
// gcc cpu_test.c

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/06/cpu-test.html

//# This program is free software; you can redistribute it and/or
//# modify it under the terms of the GNU General Public License as
//# published by the Free Software Foundatio; either version 2 of
//# the License, or (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//# MA 02111-1307 USA
 
#include <stdio.h>
#include <stdlib.h>
#include "util.h"


#ifdef	DEBUG
int 	VIA_DEBUG	=1;
#else
int	VIA_DEBUG	=0;
#endif
//==================================
//Hash Definition
//==================================
enum TSTLIST {
	TST_VECTOR_1	=0,
	TST_VECTOR_2	=1,
	TST_VECTOR_3	=2,
};


enum ERRORLIST {
	OUT_POWER	=0,
	OUT_FREQ	=1,
	OUT_THEM	=2,	
	OUT_BOOL	=3,
};

//==================================
//THEM[31:20],	FREQ[19:8],	POWER[7:0]
//==================================
enum CONFIGREG {
	POWER_TAG	=8,
	FREQ_TAG	=12,
	THEM_TAG	=12,
};

//===================================
//	Range Definition
//===================================
#define MAX_POWER	5	
#define MIN_POWER	1
#define	MAX_FREQ	1000
#define	MIN_FREQ	20
#define	MAX_THEM	50
#define	MIN_THEM	2


struct Supply {
	char*	Name_Supply;		//Name		Test_Vector Name;
	int	Power_Supply;		//Power 	Supply unit in voltage(.v)
	int	Freq_Supply;		//Frequency	Supply unit in Mhz
	int	Them_Supply;		//Thermal 	Supply unit in .c
} SPLY_TSTLIST[] = {
	{"TST_VECTOR_1",	2,	500,	30	},
	{"TST_VECTOR_2",	3,	500,	30	},
	{"TST_VECTOR_3",	3,	400,	28	},
};

//===================================
// TEST Vector Support
//===================================

struct Vector {
	char* 	Name_Vector;
	char*	v;	
} VECT_TSTLIST[] = {
	{"TST_VECTOR_1",	"0101 0100 0010 0110 0101 0100 0010 0110"	},
	{"TST_VECTOR_2",	"0101 0111 0010 0010 0101 0111 0010 0010"	},
	{"TST_VECTOR_2",	"0101 0110 1010 0110 0101 0110 1010 0110"	},
};

#define	ON	"1"	
#define	OFF	"0"
#define HZ	"Z"
#define	HX	"X"
//===================================
//TESTER Definition
//===================================
struct TESTER {
	char* 	Name_TESTER;
	char*	CLK;
	char*	RST;
	char*	VAL;
	char*	TX;
	char*	RX;

} OUR_TESTER = { "VIA_TST",	OFF,	OFF,	OFF,	OFF,	OFF	}; 

//===================================
//
//===================================
struct Detect {
	char* 	Name_Detect;
	int	Power_Detect;
	
};


int ERROR_HANDLE(int a, int i){
 switch(a){
   case 0 : printf("Please Check Power_Supply at V-%d\n", i);	return -1;	break;
   case 1 : printf("Please Check Freq_Supply  at V-%d\n", i);	return -1;	break;
   case 2 : printf("Please Check Them_Supply  at V-%d\n", i);	return -1;	break;
   case 3 : printf("Please Check Test Vector  at V-%d\n", i);	return -1;	break;
  default : return 0;	break; 
 }
return 0;
}



int SETREG(int p, int f, int t ){

     if( p < MIN_POWER || p > MAX_POWER) { return OUT_POWER; }
     if( f < MIN_FREQ  || f > MAX_FREQ ) { return OUT_FREQ;  }
     if( t < MIN_THEM  || t > MAX_THEM ) { return OUT_THEM;  }
      
     return  t << (POWER_TAG + FREQ_TAG) + f << (FREQ_TAG) + p;
  }


void Set2TESTER(char* c, char* r, char* v,char* t){
OUR_TESTER.CLK = c;
OUR_TESTER.RST = r;
OUR_TESTER.TX  = t;
OUR_TESTER.VAL = v;
}

char* Put2TESTER() {
char* c = OUR_TESTER.CLK;
char* s = OUR_TESTER.RST;
char* v = OUR_TESTER.VAL;
char* t = OUR_TESTER.TX;

OUR_TESTER.RX = "1";

char* r = OUR_TESTER.RX;
printf ("CLK:%2s, RST:%2s,VAL: %2s, TX:%2s, RX:%2s\n",c,s,v,t,r);
return r;
}


void Set2TSTVector(int a,int i){
char* v = VECT_TSTLIST[i].v;

	Set2TESTER(ON, OFF, OFF, OFF);
	Put2TESTER();

while( (*v++)!='\0'){
       ((*v)==' ')?  : 
	((*v)=='0')? Set2TESTER(OFF, OFF, ON, OFF) : Set2TESTER(OFF, OFF, ON, ON);
	Put2TESTER();

	((*v)==' ')?  : 
	((*v)=='0')? Set2TESTER(ON, OFF, ON, OFF) : Set2TESTER(ON, OFF, ON, ON);
	Put2TESTER();

 }
	Set2TESTER(OFF,	OFF, OFF, OFF);
	Put2TESTER();

}

void Set2Config(int a,int i){
    char* t = int2bin(a);

    if(VIA_DEBUG)	printf("Config Vector (%d) :: %s\n",i,t);

	Set2TESTER(ON, OFF, OFF, OFF);
	Put2TESTER();

while( (*t++)!= '\0'){
	((*t)=='0')? Set2TESTER(OFF, OFF, ON, OFF) : Set2TESTER(OFF, OFF, ON, ON);
	Put2TESTER();
   	
	((*t)=='0')? Set2TESTER(ON, OFF, ON, OFF) : Set2TESTER(ON, OFF, ON, ON);
 	Put2TESTER();
    }


	Set2TESTER(OFF,	OFF, OFF, OFF);
	Put2TESTER();
}

void Set2Ini(int a,int i){
int j;
	
	Set2TESTER(OFF,	ON, OFF, OFF);
	Put2TESTER();
 
   for( j=0; j<=3; j++){
	Set2TESTER(ON,	ON, OFF, OFF);
	Put2TESTER();

	(j==3)? Set2TESTER(OFF,	OFF, OFF, OFF) : Set2TESTER(OFF, ON, OFF, OFF);

	Put2TESTER();
   }
}

int main() {
int i,a;


for(i=0; i<sizeof(SPLY_TSTLIST)/sizeof(SPLY_TSTLIST[0]); i++){
//Step.1 initial 
Set2Ini(a,i);

//Step.2 Set Config REG
a = SETREG(SPLY_TSTLIST[i].Power_Supply, SPLY_TSTLIST[i].Freq_Supply, SPLY_TSTLIST[i].Them_Supply);
if (ERROR_HANDLE(a,i)== -1) return -1;

Set2Config(a,i);

//Step.3 Set Test Vector
Set2TSTVector(a,i);


//printf("%d\n",a);
}

return 0;
}



