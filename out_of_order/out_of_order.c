//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/08/amba-40-axi-bus.html
 
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
#include <pthread.h>

#define MAX_BUFF_DEEP	4	
#define MAX_BURST	4	
#define MAX_TEST_COT	8	

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

enum BUS_STATUS {
 	BUS_TRUE       	=0,
	BUS_FALSE	=1,
};

enum	BUS_ADDR_MAP {
	BUS_SLAVE_1_ST	= 0x00000100,
	BUS_SLAVE_1_ED	= 0x000001ff,
	BUS_SLAVE_2_ST	= 0x00000200,
	BUS_SLAVE_2_ED	= 0x000002ff,
	BUS_MASTER_1_ST	= 0x00000300,
	BUS_MASTER_1_ED = 0x000003ff,
	BUS_MASTER_2_ST	= 0x00000400,
	BUS_MASTER_2_ED = 0x000004ff,
};

typedef struct AddrBus {
	int	FmAddr[MAX_BUFF_DEEP];
	int	ToAddr[MAX_BUFF_DEEP];
	int	INX;
	int	FULL;
	int	EMPTY;
} AddrBusBuf;

typedef struct RdData {
	int	Data[MAX_BURST];
} RdDataBuf;


typedef struct RdDataBus {
	RdDataBuf  	Data[MAX_BUFF_DEEP];
	int		FmAddr[MAX_BUFF_DEEP];
	int		INX;
	int		FULL;
	int		EMPTY;	
} RdDataBusBuf;

int TEST_COT=0;

AddrBusBuf	AddrBusBufPtr;
RdDataBusBuf	RdDataBusBufPtr;


void  AddrBusBuf_Initial(){
      AddrBusBufPtr.INX  =0;
      AddrBusBufPtr.FULL = BUS_FALSE;
      AddrBusBufPtr.EMPTY= BUS_TRUE;
}

void  RdDataBusBuf_Initial(){
      RdDataBusBufPtr.INX  =0;
      RdDataBusBufPtr.FULL = BUS_FALSE;
      RdDataBusBufPtr.EMPTY= BUS_TRUE;
}

void SetAddrBusBufStatus(int FmAddr,int ToAddr){
     int inx = AddrBusBufPtr.INX;
     AddrBusBufPtr.FmAddr[inx] =  FmAddr;
     AddrBusBufPtr.ToAddr[inx] =  ToAddr;
     inx++;
     AddrBusBufPtr.INX = inx;
}

void GetAddrBusBufStatus(){
     int inx = AddrBusBufPtr.INX;
     int i;     
    for( i=0; i<inx; i++){
      AddrBusBufPtr.FmAddr[i] = AddrBusBufPtr.FmAddr[i+1];
      AddrBusBufPtr.ToAddr[i] = AddrBusBufPtr.ToAddr[i+1];
   }
      inx--;
      AddrBusBufPtr.INX = inx;
}

void CheckAddrBusBufStatus(){
     AddrBusBufPtr.FULL = ( AddrBusBufPtr.INX==MAX_BUFF_DEEP )? BUS_TRUE : BUS_FALSE;
     AddrBusBufPtr.EMPTY= ( AddrBusBufPtr.INX==0             )? BUS_TRUE : BUS_FALSE;
}


void GetRdDataBusBufStatus(int n){
    int inx = RdDataBusBufPtr.INX;
    int i;
      printf("M%d Receive @ FmAdder: %x\n",n, RdDataBusBufPtr.FmAddr[0] ); 
 
   for( i=0; i<MAX_BURST; i++ ){
      printf("M%d Receive @ %d, %d\n",n, i, RdDataBusBufPtr.Data[0].Data[i] );
    }
   //update RdDataBusBuf 
   for( i=0; i<inx; i++){
       RdDataBusBufPtr.FmAddr[i] = RdDataBusBufPtr.FmAddr[i+1];
       RdDataBusBufPtr.Data[i]   = RdDataBusBufPtr.Data[i+1];
    }   
       inx--;
       RdDataBusBufPtr.INX = inx;
}

void SetRdDataBusBufStatus(int FmAddr,int n){
     int i;
     int inx = RdDataBusBufPtr.INX;
     
      for(i=0; i<MAX_BURST; i++){
             RdDataBusBufPtr.Data[inx].Data[i] = (n==1)? 10+i : 100+i;
       }
             RdDataBusBufPtr.FmAddr[inx] = FmAddr;
      inx++; 
      RdDataBusBufPtr.INX = inx;
}

 
void CheckRdDataBusBufStatus(){
     RdDataBusBufPtr.FULL = ( RdDataBusBufPtr.INX==MAX_BUFF_DEEP )? BUS_TRUE : BUS_FALSE;
     RdDataBusBufPtr.EMPTY= ( RdDataBusBufPtr.INX==0             )? BUS_TRUE : BUS_FALSE;
}


void *MASTER_1_Transmitter(void *t){
     long my_id = (long)t;
     int FmAddr,ToAddr;

     while(TEST_COT<MAX_TEST_COT){
      int cot =10;
          
         while( AddrBusBufPtr.FULL == BUS_TRUE ){
             sleep(1);
             if( cot==0 ){ printf("out-of-Time-Wait ... @ M1 Transmitter 4 Addr Phase\n"); break; }
             cot--;
         } 

         if( cot>0 ){
            pthread_mutex_lock(&count_mutex);
            FmAddr = BUS_MASTER_1_ST;
            ToAddr = ( TEST_COT%2==0 )? BUS_SLAVE_1_ST : BUS_SLAVE_2_ST;
            SetAddrBusBufStatus( FmAddr, ToAddr);
            CheckAddrBusBufStatus();
            printf("M1 Transmit @ FmAddr: %x, ToAddr %x\n", FmAddr, ToAddr);    
            pthread_mutex_unlock(&count_mutex);
         }

         sleep(1);
     }

pthread_exit(NULL);
}

void *MASTER_1_Receiver(void *t){
    long my_id = (long)t;
 
    while(TEST_COT<MAX_TEST_COT){
    int  cot   =10;
     
       while( RdDataBusBufPtr.EMPTY == BUS_TRUE ){
          sleep(1);
          if( cot==0 ){ printf("out-of-Time-Wait ... @ M1 Receiver 4 RdData Phase\n"); break; }
          cot--;  
     }
     
     int i,inx;
     if( cot>0 && BUS_MASTER_1_ST <= RdDataBusBufPtr.FmAddr[0] && RdDataBusBufPtr.FmAddr[0] <= BUS_MASTER_1_ED ){
         pthread_mutex_lock(&count_mutex);
         GetRdDataBusBufStatus(1);
         CheckRdDataBusBufStatus();
         TEST_COT++;
         pthread_mutex_unlock(&count_mutex);                
     }

     sleep(1);
   }
 
pthread_exit(NULL);
}


void *SLAVE_1_DO(void *t){
    long my_id = (long)t;
    int  FmAddr;
  
    while(TEST_COT<MAX_TEST_COT){
     int cot =10;

       while( RdDataBusBufPtr.FULL == BUS_TRUE && AddrBusBufPtr.EMPTY == BUS_TRUE ){
          sleep(2);
          if( cot==0 ){ printf("out-ot-Time-Wait ... @ S1 Receive 4 RdData/Addr Phase\n"); break; }
          cot --;
       }

       if( cot>0  && BUS_SLAVE_1_ST <= AddrBusBufPtr.ToAddr[0] && AddrBusBufPtr.ToAddr[0] <= BUS_SLAVE_1_ED ){
          pthread_mutex_lock(&count_mutex);
          FmAddr = AddrBusBufPtr.FmAddr[0]; 
          printf("S1 Receive Req @ FmAddr: %x\n", FmAddr);
          SetRdDataBusBufStatus( FmAddr, 1);
          GetAddrBusBufStatus();
          CheckAddrBusBufStatus();
          CheckRdDataBusBufStatus();
          pthread_mutex_unlock(&count_mutex);
      }

     sleep(3);  
   }
 pthread_exit(NULL);
}

void *SLAVE_2_DO(void *t){
    long my_id = (long)t;
    int  FmAddr;
  
    while(TEST_COT<MAX_TEST_COT){
     int cot =10;

       while( RdDataBusBufPtr.FULL == BUS_TRUE && AddrBusBufPtr.EMPTY == BUS_TRUE ){
          sleep(2);
          if( cot==0 ){ printf("out-ot-Time-Wait ... @ S2 Receive 4 RdData/Addr Phase\n"); break; }
          cot --;
       }

       if( cot>0  && BUS_SLAVE_2_ST <= AddrBusBufPtr.ToAddr[0] && AddrBusBufPtr.ToAddr[0] <= BUS_SLAVE_2_ED ){
          pthread_mutex_lock(&count_mutex);
          FmAddr = AddrBusBufPtr.FmAddr[0]; 
          printf("S2 Receive Req @ FmAddr: %x\n", FmAddr);
          SetRdDataBusBufStatus( FmAddr, 2);
          GetAddrBusBufStatus();
          CheckAddrBusBufStatus();
          CheckRdDataBusBufStatus();
          pthread_mutex_unlock(&count_mutex);
      }

     sleep(5);  
   }
 pthread_exit(NULL);
}

int main(int argc,char* argv[]){
 AddrBusBuf_Initial();
 RdDataBusBuf_Initial();
 
 pthread_t thread[4];

 pthread_create( &thread[0],NULL, MASTER_1_Transmitter, NULL);
 pthread_create( &thread[1],NULL, MASTER_1_Receiver, NULL);
 pthread_create( &thread[2],NULL, SLAVE_1_DO, NULL);
 pthread_create( &thread[3],NULL, SLAVE_2_DO, NULL);
                      
 pthread_join( thread[0],NULL);
 pthread_join( thread[1],NULL);
 pthread_join( thread[2],NULL);
 pthread_join( thread[3],NULL);

 pthread_exit(NULL);

return 0;
}


