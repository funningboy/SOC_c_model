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

#define MAX_BURST	4	
#define MAX_TEST_COT	3

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

enum BUS_STATUS {
 	BUS_ON_BUSY	=0,
	BUS_OFF_BUSY	=1,
	BUS_ON_LOCK	=2,
	BUS_OFF_LOCK	=3,
};

enum	BUS_ADDR_MAP {
	BUS_SLAVE_1_ST	= 0x00000100,
	BUS_SLAVE_1_ED	= 0x000001ff,
	BUS_SLAVE_2_ST	= 0x00000200,
	BUS_SLAVE_2_ED	= 0x000002ff,
	BUS_MASTER_1_ST	= 0x00000300,
	BUS_MASTER_1_ED = 0x000003ff,
};

typedef struct Bus {
	int	FmAddr;
	int	ToAddr;
	int	Data[MAX_BURST];
	int	Busy;
	int	Lock;
} Bus_Buf;


Bus_Buf BusPtr;
int TEST_COT=0;

void Bus_Initial(){
     BusPtr.Busy = BUS_OFF_BUSY;
     BusPtr.Lock = BUS_OFF_LOCK;
}

void *MASTER_1_Transmitter(void *t){
     long my_id = (long)t;

while( TEST_COT < MAX_TEST_COT ){   
    int  cot =10;
 
   while( BusPtr.Busy == BUS_ON_BUSY ){
     sleep(1);     
     if( cot==0 ){ printf("Out-of-Time-Wait M1 Transmitter retry...\n"); break; }
     cot--;
  }

 if( cot>0 ){
      printf("M1 Transmitter @ %d\n", TEST_COT);
      pthread_mutex_lock(&count_mutex);
      BusPtr.FmAddr = BUS_MASTER_1_ST;
      BusPtr.ToAddr = ( TEST_COT%2==0 )? BUS_SLAVE_1_ST: BUS_SLAVE_2_ST;
      BusPtr.Busy = BUS_ON_BUSY;
      pthread_mutex_unlock(&count_mutex);
 } else {
   sleep(2);
 }

 sleep(3);
}

pthread_exit(NULL);
}

void *MASTER_1_Receiver(void *t){
    long my_id = (long)t;
    int  cot   =10;

while( TEST_COT < MAX_TEST_COT ){   
 
  while( BusPtr.Busy == BUS_ON_BUSY && BusPtr.Lock == BUS_OFF_LOCK ){
    sleep(1);
    if( cot==0 ){ printf("Out-of-Time wait M1 Receiver retry...\n"); break; }
    cot--; 
  }
  
  int i;
  
 if( cot>0 && BUS_MASTER_1_ST <= BusPtr.FmAddr && BusPtr.FmAddr <= BUS_MASTER_1_ED ){
    pthread_mutex_lock(&count_mutex);
    BusPtr.Busy = BUS_OFF_BUSY;
    BusPtr.Lock = BUS_OFF_LOCK;
    for(i=0; i<MAX_BURST; i++){
      printf("M1 Receive Fm %d,%d,%d\n",BusPtr.ToAddr,i,BusPtr.Data[i]);
    }
    printf("M1 Reveive Done @ %d\n",TEST_COT);
    printf("\n");

    TEST_COT++;
    pthread_mutex_unlock(&count_mutex);

  } else {
    sleep(2);
  }

 sleep(3);
}

pthread_exit(NULL);
}

void *SLAVE_1_DO(void *t){
    long my_id = (long)t;

while( TEST_COT < MAX_TEST_COT ){   
  
 while( BusPtr.Busy == BUS_OFF_BUSY ){
    sleep(1);
 }   
 
 int i;
 if( BUS_SLAVE_1_ST <= BusPtr.ToAddr && BusPtr.ToAddr <= BUS_SLAVE_1_ED ){
     pthread_mutex_lock(&count_mutex);
     BusPtr.Lock = BUS_ON_LOCK;
     for(i=0; i<MAX_BURST; i++){ 
        BusPtr.Data[i] = i+10;
     } 
     pthread_mutex_unlock(&count_mutex);
 } else {
   sleep(3);
 }

sleep(3);
}

 pthread_exit(NULL);
}

void *SLAVE_2_DO(void *t){
    long my_id = (long)t;

while( TEST_COT < MAX_TEST_COT ){   
   
 while( BusPtr.Busy == BUS_OFF_BUSY ){
    sleep(1);
 }   
 
 int i;
 if( BUS_SLAVE_2_ST <= BusPtr.ToAddr && BusPtr.ToAddr <= BUS_SLAVE_2_ED ){
     pthread_mutex_lock(&count_mutex);
     BusPtr.Lock = BUS_ON_LOCK;
     for(i=0; i<MAX_BURST; i++){ 
        BusPtr.Data[i] = i+100;
     } 
     pthread_mutex_unlock(&count_mutex);
 } else {
   sleep(3);
 }

 sleep(3);
}

 pthread_exit(NULL);
}

int main(int argc,char* argv[]){
 Bus_Initial();
 
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


