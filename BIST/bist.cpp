
// how to compile 
// g++ -I/${boost_lib} -o bist bist.cpp

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/07/bist.html

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
#include <iostream>
#include <boost/crc.hpp>
#include <sstream>
#include <string>

#define MEMDEEP 10

bool Debug = false; 

#ifdef  DEBUG
Debug =true;
#endif
 
using namespace std;

struct MEM {
 unsigned int Addr;
 unsigned int Val;
 unsigned int CkSum;
 unsigned int Crc32;
 unsigned int Status;
};

struct ErrTable{
 unsigned int Org_Addr;
 unsigned int Nw_Addr; 
};
 
void  ProWriteMEM(struct MEM *Ptr,unsigned int Addr,unsigned int Val);
void  ProReadMEM(struct MEM *Ptr,unsigned int Addr,unsigned int *Val);
void  ProCheckErrTable();
unsigned ProCheckSum(unsigned int x);

//=====================================
// Num of 1's counts
//=====================================
unsigned int ProCheckSum(unsigned x) {   
        x = x - ((x >> 1) & 0x55555555);    
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);    
        x = (x + (x >> 4)) & 0x0F0F0F0F;    
        x = x + (x >> 8);    
        x = x + (x >> 16);    
        return x & 0x0000003F;    
} 

//=====================================
// CRC32 Check
//=====================================
unsigned int GetCrc32(const string my_string) {
    boost::crc_32_type result;
    result.process_bytes(my_string.data(), my_string.length());
    return result.checksum();
}

void ProWriteMEM(struct MEM *Ptr,unsigned int Addr,unsigned int Val){

  Ptr->Addr  = Addr;
  Ptr->Val   = Val;
  Ptr->CkSum = ProCheckSum(Val);
  string s;
  stringstream out; 
  out << Val;
  s = out.str();
  Ptr->Crc32 = GetCrc32(s);   
}

void ProReadMEM(struct MEM *Ptr,unsigned int Addr,unsigned int *Val){
   // Ptr->Addr
   *Val = Ptr->Val;
}

void ProWriteErrTable(struct ErrTable *Ptr,unsigned int Org,unsigned int Nw){
   Ptr->Org_Addr  = Org;
   Ptr->Nw_Addr   = Nw;   
}

void ProReadErrTable(struct ErrTable *Ptr,unsigned int *Org,unsigned int *Nw){
   *Org = Ptr->Org_Addr;
   *Nw  = Ptr->Nw_Addr;
}

int CheckErrTable(struct ErrTable *Ptr[],unsigned int Org,unsigned int *Nw){
  int i,r;
  unsigned int Org_Addr,Nw_Addr;

  for(i=0; i<MEMDEEP; i++){
      ProReadErrTable(Ptr[i],&Org_Addr,&Nw_Addr);
      if(Debug ==true){
        printf("Table ID:: %d, Org_Addr ::%x, Nw_Addr ::%x\n",i,Org_Addr,Nw_Addr);
     }
     
     if( Org == Org_Addr && Org_Addr != Nw_Addr ){
         *Nw = Nw_Addr;
          r= 0;
          break;
      }else{
          r= -1;
      }
   }
   return r;
}





int main(int argc,char *argv[]){

   struct MEM   *MEMPtr[MEMDEEP];
   struct ErrTable *ErrPtr[MEMDEEP];

   int i;
   for(i=0; i<MEMDEEP; i++){
       MEMPtr[i] = (struct MEM      *) malloc( sizeof(struct MEM     ) );
       ErrPtr[i] = (struct ErrTable *) malloc( sizeof(struct ErrTable) );
  }

  //Initial Set 2 MEM
   ProWriteMEM(MEMPtr[0],0x00000000,0);
   ProWriteMEM(MEMPtr[1],0x00000004,1);
   ProWriteMEM(MEMPtr[2],0x00000008,2);
   ProWriteMEM(MEMPtr[3],0x0000000c,3);
   ProWriteMEM(MEMPtr[4],0x00000010,4);


  //Set the Fault Loc @ ErrTable
   ProWriteErrTable(ErrPtr[0],0x00000008,0x00000018);
   ProWriteMEM(MEMPtr[5],0x00000018,2);
  
   ProWriteErrTable(ErrPtr[1],0x0000000c,0x00000001c);
   ProWriteMEM(MEMPtr[6],0x0000001c,3);
  
   //Check Fault Loc exists or not
   unsigned int Org_Addr,Nw_Addr,rst;
   Org_Addr = 0x00000008;

   int c = CheckErrTable(ErrPtr,Org_Addr,&Nw_Addr);
   cout <<c<<endl;
   if( c != -1 ){   
    for(i=MEMDEEP>>1; i<MEMDEEP; i++){
        if( MEMPtr[i]->Addr == Nw_Addr ){ 
            ProReadMEM(MEMPtr[i],Nw_Addr,&rst); 
            printf("Find Fault @ %x -> %x :: data %d\n",Org_Addr,Nw_Addr,MEMPtr[i]->Val);
        }

    }
  }


   free(*MEMPtr);
   free(*ErrPtr);
return 0;
}

