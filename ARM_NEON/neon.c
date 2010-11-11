
// how to compile 
// gcc ilpthread -o neon neon.c

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/08/arm-cortex-ax-with-neon-simd.html

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
#include <inttypes.h>

#define BUF_SIZE	4

struct ABC {
	uint8_t	A;	
	uint8_t	B;
	uint8_t	C;
	
} ABC_BUF[BUF_SIZE];

struct XYZ {
	uint8_t	X;
	uint8_t	Y;
	uint8_t	Z;
} XYZ_BUF[BUF_SIZE]= {
	{       0,      0,      0},
	{      25,     25,     25},
	{       3,      5,      9},
	{       7,      5,      3},
};

void C_XYZ2ABC(){
int i;
     for(i=0; i<BUF_SIZE; i++){
	ABC_BUF[i].A = XYZ_BUF[i].X + XYZ_BUF[i].Y + XYZ_BUF[i].Z;
	ABC_BUF[i].B = XYZ_BUF[i].X + XYZ_BUF[i].Y - XYZ_BUF[i].Z;
	ABC_BUF[i].C = XYZ_BUF[i].X - XYZ_BUF[i].Y - XYZ_BUF[i].Z;
     }
}

void ASSEMBLY_XYZ2ABC(){
int i;
uint8_t tA,tB,tC;

   for( i=0; i<BUF_SIZE; i++ ){
                    tA = (XYZ_BUF[i].X + XYZ_BUF[i].Y);
          ABC_BUF[i].A = (tA           + XYZ_BUF[i].Z);

                    tB = (XYZ_BUF[i].X + XYZ_BUF[i].Y);
	  ABC_BUF[i].B = (tB           - XYZ_BUF[i].Z);

                    tC = (XYZ_BUF[i].X - XYZ_BUF[i].Y);
          ABC_BUF[i].C = (tC           - XYZ_BUF[i].Z);
  }
}


uint32_t Exp4x8uint_8(uint8_t i24,uint8_t i16,uint8_t i8,uint8_t i0){
uint32_t r;
         r  = i24<<24;
         r += i16<<16;
         r += i8<<8;
         r += i0;
return r;
}

uint32_t Exe4x8uint_32(uint32_t a32,uint32_t b32,uint8_t op){
uint32_t r; 
uint8_t  ra24 = a32>>24;
uint8_t  ra16 = a32>>16;
uint8_t  ra8  = a32>>8;
uint8_t  ra0  = a32;

uint8_t  rb24 = b32>>24;
uint8_t  rb16 = b32>>16;
uint8_t  rb8  = b32>>8;
uint8_t  rb0  = b32;

switch(op){
   case 0: ra24 += rb24; ra16 += rb16; ra8 += rb8; ra0 += rb0; break; 
   case 1: ra24 -= rb24; ra16 -= rb16; ra8 -= rb8; ra0 -= rb0; break;
 }
 
 r  = ra24<<24;
 r += ra16<<16;
 r += ra8<<8;
 r += ra0;
 
 return r;
}

void NEON_XYZ2ABC(){
int i;
uint32_t X,Y,Z;
uint32_t A,B,C;

  for( i=0; i<BUF_SIZE; i=i+4 ){
	X = Exp4x8uint_8(XYZ_BUF[i+3].X, XYZ_BUF[i+2].X, XYZ_BUF[i+1].X, XYZ_BUF[i ].X);
	Y = Exp4x8uint_8(XYZ_BUF[i+3].Y, XYZ_BUF[i+2].Y, XYZ_BUF[i+1].Y, XYZ_BUF[i ].Y);
	Z = Exp4x8uint_8(XYZ_BUF[i+3].Z, XYZ_BUF[i+2].Z, XYZ_BUF[i+1].Z, XYZ_BUF[i ].Z);

        A  = Exe4x8uint_32(X,Y,0);
        A  = Exe4x8uint_32(A,Z,0);

        B  = Exe4x8uint_32(X,Y,0);
        B  = Exe4x8uint_32(B,Z,1);

        C  = Exe4x8uint_32(X,Y,1);
        C  = Exe4x8uint_32(C,Z,1);

        ABC_BUF[i+3].A = A>>24; ABC_BUF[i+2].A = A>>16;  ABC_BUF[i+1].A = A>>8; ABC_BUF[i].A = A; 
        ABC_BUF[i+3].B = B>>24; ABC_BUF[i+2].B = B>>16;  ABC_BUF[i+1].B = B>>8; ABC_BUF[i].B = B; 
        ABC_BUF[i+3].C = C>>24; ABC_BUF[i+2].C = C>>16;  ABC_BUF[i+1].C = C>>8; ABC_BUF[i].C = C; 
   }
}


void Display_ABC(){
int i; 
   for( i=0; i<BUF_SIZE; i++ ){
    printf("I     :: %3x,",i);
    printf("A[%2d]:: %3x,",i,ABC_BUF[i].A);
    printf("B[%2d]:: %3x,",i,ABC_BUF[i].B);
    printf("C[%2d]:: %3x,\n",i,ABC_BUF[i].C);
  }
  printf("\n");  
}

int main(int argc,char *argv[]){
 
 printf("C Code Result...\n");
 C_XYZ2ABC();
 Display_ABC();

 printf("NEON Result...\n");
 NEON_XYZ2ABC();
 Display_ABC();


return 0;
}
