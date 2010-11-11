
// how to compile 
// gcc -lpthread -o smart_reflex smart_reflex.cpp

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/08/smartreflex-power-and-performance.html

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

#define BATTERY_CONTAIN 5000	//unint power contain
#define TEST_COT        3

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;


enum BATTERY_STATUS{
	BATTERY_FULL	=0,
	BATTERY_EMPTY	=1,
	BATTERY_HALF	=2,
	BATTERY_LOWER	=3,
	BATTERY_OK	=4,
	BATTERY_ERROR	=5,
};


enum TASK_STATUS{
	TASK_VEDIO	=0,
	TASK_GAME	=1,
	TASK_PHONE	=2,
	TASK_WALKMAN	=3,
	TASK_IDLE	=4,
	TASK_OFF	=7,
};


struct WORK_STATUS{
	char          *name;
	unsigned int  on_upw;
	unsigned int  slp_upw;
	unsigned int  off_upw;
} WORKList [] = {
 { "TFT"	,100, 50, 10 },
 { "KEYBOARD"	, 10,  5,  1 },
 { "WIFI"	,500,200, 10 },
 { "GSM"	,700,500, 30 },
};

struct BATTERY{
	char          *name;
	unsigned int  remain;
} BATTERYList [] = {
  { "iBATTERY"	, 100 },
};
 
struct TASK{
	char          *name;
	unsigned int  sel;
} TASKList [] = {
  { "TASK_OFF"    , TASK_OFF    },
  { "TASK_IDLE"   , TASK_IDLE   },
  { "TASK_VEDIO"  , TASK_VEDIO  },
  { "TASK_PHONE"  , TASK_PHONE  },
  { "TASK_IDLE"   , TASK_IDLE   },
  { "TASK_WALKMAN", TASK_WALKMAN},
};


int 			 COT =0;

 
void Set_WORK_STATUS(struct WORK_STATUS *Ptr,char *nm,unsigned int on,unsigned int slp,unsigned int off){
                     Ptr->name   = nm;
                     Ptr->on_upw = on;
                     Ptr->slp_upw= slp;
                     Ptr->off_upw= off;
}

void Set_BATTERY(struct BATTERY *Ptr,char *nm,unsigned int rman){
                     Ptr->name   = nm;
                     Ptr->remain = rman;
}

void Set_TASK(struct TASK *Ptr,char *nm,unsigned int sel){
                     Ptr->name = nm;
                     Ptr->sel  = sel;
}

int Check_BATTERY_STATUS(struct BATTERY *Ptr){
                   if((                      Ptr->remain >     BATTERY_CONTAIN) ||
                      (0.8*BATTERY_CONTAIN < Ptr->remain && Ptr->remain <=    BATTERY_CONTAIN) ){ return BATTERY_FULL;  }
              else if( 0.6*BATTERY_CONTAIN < Ptr->remain && Ptr->remain <= 0.8*BATTERY_CONTAIN ){ return BATTERY_OK;    }
              else if( 0.4*BATTERY_CONTAIN < Ptr->remain && Ptr->remain <= 0.6*BATTERY_CONTAIN ){ return BATTERY_HALF;  }
              else if( 0.2*BATTERY_CONTAIN < Ptr->remain && Ptr->remain <= 0.4*BATTERY_CONTAIN ){ return BATTERY_LOWER; }
              else if(   0*BATTERY_CONTAIN < Ptr->remain && Ptr->remain <= 0.2*BATTERY_CONTAIN ){ return BATTERY_EMPTY; }
              else   {                                                                            return BATTERY_ERROR; }
}

void Charge_BATTERY(struct BATTERY *Ptr){
    if( Check_BATTERY_STATUS(Ptr) != BATTERY_FULL ){
         Ptr->remain += 50;
   }
}


int Check_TASK_STATUS(struct BATTERY *Ptr,struct TASK *Ttr){

    int battery_status = Check_BATTERY_STATUS(Ptr);
    unsigned int consume;
 
      switch(Ttr->sel){
                          // VEDIO  : TFT(ON)    KEYBOARD(SLEEP)  WIFI(ON)      GSM(SLEEP)
                          // GAME   : TFT(ON)    KEYBOARD(ON)     WIIF(SLEEP)   GSM(SLEEP)
                          // PHONE  : TFT(SLEEP) KEYBOARD(SLEEP)  WIFI(SLEEP)   GSM(ON)
                          // WALKMAN: TFT(SLEEP) KWYBOARD(ON)     WIFI(SLEEP)   GSM(SLEEP)
                          // IDLE   : TFT(SLEEP) KEYBOARD(SLEEP)  WIFI(SLEEP)   GSM(SLEEP)
                          // OFF    : TFT(OFF)   KEYBOARD(OFF)    WIFI(OFF)     GSM(OFF) 
	case TASK_VEDIO   : consume = WORKList[0].on_upw  + WORKList[1].slp_upw + WORKList[2].on_upw  + WORKList[3].slp_upw; break; 
	case TASK_GAME	  : consume = WORKList[0].on_upw  + WORKList[1].on_upw  + WORKList[2].slp_upw + WORKList[3].slp_upw; break;
	case TASK_PHONE	  : consume = WORKList[0].slp_upw + WORKList[1].slp_upw + WORKList[2].slp_upw + WORKList[3].on_upw;  break;
	case TASK_WALKMAN : consume = WORKList[0].slp_upw + WORKList[1].on_upw  + WORKList[2].slp_upw + WORKList[3].slp_upw; break;
	case TASK_IDLE	  : consume = WORKList[0].slp_upw + WORKList[1].slp_upw + WORKList[2].slp_upw + WORKList[3].slp_upw; break;
	case TASK_OFF	  : consume = WORKList[0].off_upw + WORKList[1].off_upw + WORKList[2].off_upw + WORKList[3].off_upw; break;
        default           : return -1; break;
    }

   if( battery_status != BATTERY_EMPTY || battery_status != BATTERY_ERROR ){
             if( 0.9*Ptr->remain > consume ){ Ptr->remain -= consume; return 0; }
             else{                                                    return -1;}
      } 
}

void *Emulator_BATTERY(void *t){
     int i;
     struct   BATTERY *BATTERYPtr     = &BATTERYList[0];
 
      while(COT <= TEST_COT){
         for(i=0; i<30; i++){
           pthread_mutex_lock(&count_mutex);
          if( Check_BATTERY_STATUS(BATTERYPtr) == BATTERY_ERROR ){ pthread_exit(NULL); }
           
           Charge_BATTERY(BATTERYPtr);
           printf("0::%d\n", BATTERYPtr->remain);
 
   //        pthread_cond_signal(&condition_cond);
           pthread_mutex_unlock(&count_mutex);
           sleep(1); 
          }
        sleep(20);
      }

    if( COT == TEST_COT ){ pthread_exit(NULL); }

}

void *Emulator_TASK(void *t){
     int      i,j,rst;
     struct   BATTERY     *BATTERYPtr     = &BATTERYList[0];
     struct   TASK        *TASKPtr;
	    
        if( Check_BATTERY_STATUS(BATTERYPtr) == BATTERY_ERROR ){ pthread_exit(NULL); }  
        
       if(COT <= TEST_COT ){ 
          for(i=0; i<sizeof(TASKList)/sizeof(*TASKList); i++){
            TASKPtr = &TASKList[i];
            j=0;

            while(j<3){
             pthread_mutex_lock(&count_mutex);
 //            pthread_cond_wait(&condition_cond,&count_mutex); 
             rst = Check_TASK_STATUS(BATTERYPtr,TASKPtr);
             if( rst ==0 ){  printf("1::%d,%d\n", i,BATTERYPtr->remain );  }
             pthread_mutex_unlock(&count_mutex);
             
              if( rst==0 ){
                  j++;
                  sleep(0.5);
               } else {
                 printf("Out of Battery contain...Please turn off App\n");
                 j =0;
                 sleep(50);
               }
            }
            printf("App done ...\n");       
            sleep(1); 
         }
         COT++;
        }

    if( COT == TEST_COT ){ pthread_exit(NULL); }
}
 
int main(int argc, char *argv[]){

 pthread_t thread_1,thread_2;

 pthread_create( &thread_1,NULL, Emulator_BATTERY, NULL);
 pthread_create( &thread_2,NULL, Emulator_TASK, NULL);
                      
 pthread_join( thread_1,NULL);
 pthread_join( thread_2,NULL);

return 0;
}

