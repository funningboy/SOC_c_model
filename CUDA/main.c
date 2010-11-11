//#ifdef  _ALU_H
//#define _ALU_H

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/10/cuda-nvdia-hardware-driver.html

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
#include <time.h>
#include "queue.h"

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;


enum ALU_MESSAGE {
	ALU_OK,
	ALU_ER_COMMAND,
	ALU_ER_STATUS,
	ALU_ER_DELAY,
	ALU_ER_DONE,
};

enum ALU_STATUS {
	ALU_WORK,
	ALU_IDLE,
};

enum ALU_COMMAND {
	ALU_ADD,
	ALU_SUB,
	ALU_MUL,
	ALU_DIV,
	ALU_DUMMY,
};

enum ALU_DELAY {
	ALU_DEF_DELAY = 1,
	ALU_ADD_DELAY = 3,
	ALU_SUB_DELAY = 7,
	ALU_MUL_DELAY = 10,
	ALU_DIV_DELAY = 15,
};


struct QUEUE *qulist = NULL;
int    DONE          = ALU_ER_DONE;
int    gid =0;  
time_t rawtime;
struct tm    *timeinfo;

struct iALU {
	int	id;
	int	status;
	int	command;
	int	in_a;
	int	in_b;
	int	out_c;

} ALU[3] = {
	{ 0, ALU_IDLE, ALU_DUMMY, 0, 0, 0 },
	{ 1, ALU_IDLE, ALU_DUMMY, 0, 0, 0 },
	{ 2, ALU_IDLE, ALU_DUMMY, 0, 0, 0 },
};

struct iCMD {
	int	command;
	int	in_a;
	int	in_b;
} CMD[7] = { { ALU_ADD, 3, 4 },
             { ALU_SUB, 3, 4 },
             { ALU_MUL, 3, 4 },
             { ALU_SUB, 3, 4 },
             { ALU_MUL, 3, 4 },
             { ALU_DIV, 3, 4 },
             { ALU_ADD, 3, 4 },
};

int iALU_delay(int command, int *delay){

    switch(command){
       case ALU_ADD : *delay = ALU_ADD_DELAY; return ALU_OK; break;
       case ALU_SUB : *delay = ALU_SUB_DELAY; return ALU_OK; break;
       case ALU_MUL : *delay = ALU_MUL_DELAY; return ALU_OK; break;
       case ALU_DIV : *delay = ALU_DIV_DELAY; return ALU_OK; break;
    }

return ALU_ER_DELAY;
}

int iALU_decoder(int command, int in_a, int in_b, int *out_c){
     
     switch(command){
        case ALU_ADD : *out_c = in_a + in_b; return ALU_OK; break;
        case ALU_SUB : *out_c = in_a - in_b; return ALU_OK; break;
        case ALU_MUL : *out_c = in_a * in_b; return ALU_OK; break;
        case ALU_DIV : *out_c = in_a / in_b; return ALU_OK; break;
     }

return ALU_ER_COMMAND;
}


void *iALU_do_0(void *t){
int delay;

      for(;;){
             if( ALU[0].command != ALU_DUMMY && ALU[0].status != ALU_WORK ){
             pthread_mutex_lock(&count_mutex);
             if( iALU_delay(ALU[0].command,&delay) == ALU_ER_DELAY ){
                 printf("ALU_ER_DELAY @ iALU_0\n");   DONE=ALU_OK; pthread_exit(NULL); }

             if( iALU_decoder(ALU[0].command,ALU[0].in_a,ALU[0].in_b,&ALU[0].out_c) == ALU_ER_COMMAND ){
                 printf("ALU_ER_COMMAND @ iALU_0\n"); DONE=ALU_OK; pthread_exit(NULL); }

             ALU[0].status = ALU_WORK;
             if( put_work_queue(&qulist,0,ALU[0].command,ALU[0].in_a,ALU[0].in_b,ALU[0].out_c) == QUEUE_ER_PUT ) {
                 printf("QUEUE_ER_PUT @ iALU_0\n"); DONE=ALU_OK; pthread_exit(NULL); }
             pthread_mutex_unlock(&count_mutex);
 
             sleep(delay);

             pthread_mutex_lock(&count_mutex);
             ALU[0].status = ALU_IDLE;
             if( del_work_queue(&qulist,0) == QUEUE_ER_DEL ){
                 printf("QUEUE_ER_DEL @ iALU_0\n"); DONE=ALU_OK; pthread_exit(NULL); }
             pthread_mutex_unlock(&count_mutex);

           }

             sleep(ALU_DEF_DELAY);

      if( DONE == ALU_OK )
          pthread_exit(NULL); 

      }
}

void *iALU_do_1(void *t){
int delay;

      for(;;){
         if( ALU[1].command != ALU_DUMMY && ALU[1].status != ALU_WORK ){
             pthread_mutex_lock(&count_mutex);
             if( iALU_delay(ALU[1].command,&delay) == ALU_ER_DELAY ){
                 printf("ALU_ER_DELAY @ iALU_1\n");   DONE=ALU_OK; pthread_exit(NULL); }

             if( iALU_decoder(ALU[1].command,ALU[1].in_a,ALU[1].in_b,&ALU[1].out_c) == ALU_ER_COMMAND ){
                 printf("ALU_ER_COMMAND @ iALU_1\n"); DONE=ALU_OK; pthread_exit(NULL); }

             ALU[1].status = ALU_WORK;
             if( put_work_queue(&qulist,1,ALU[1].command,ALU[1].in_a,ALU[1].in_b,ALU[1].out_c) == QUEUE_ER_PUT ){
                 printf("QUEUE_ER_PUT @ iALU_1\n"); DONE=ALU_OK; pthread_exit(NULL); }
       
             pthread_mutex_unlock(&count_mutex);
             
             sleep(delay);

             pthread_mutex_lock(&count_mutex);
             ALU[1].status = ALU_IDLE;
          //   dump_work_queue(qulist);

     
       if( del_work_queue(&qulist,1) == QUEUE_ER_DEL ){
                 printf("QUEUE_ER_DEL @ iALU_1\n"); DONE=ALU_OK; pthread_exit(NULL); }
             pthread_mutex_unlock(&count_mutex);


           }

             sleep(ALU_DEF_DELAY);

      if( DONE == ALU_OK )
          pthread_exit(NULL); 

      }
}

void *iALU_do_2(void *t){
int delay;

      for(;;){
         if( ALU[2].command != ALU_DUMMY && ALU[2].status != ALU_WORK ){
             pthread_mutex_lock(&count_mutex);
             if( iALU_delay(ALU[2].command,&delay) == ALU_ER_DELAY ){
                 printf("ALU_ER_DELAY @ iALU_2\n");   DONE=ALU_OK; pthread_exit(NULL); }

             if( iALU_decoder(ALU[2].command,ALU[2].in_a,ALU[2].in_b,&ALU[2].out_c) == ALU_ER_COMMAND ){
                 printf("ALU_ER_COMMAND @ iALU_2\n"); DONE=ALU_OK; pthread_exit(NULL); }

             ALU[2].status = ALU_WORK;
             if( put_work_queue(&qulist,2,ALU[2].command,ALU[2].in_a,ALU[2].in_b,ALU[2].out_c) == QUEUE_ER_PUT ){
                 printf("QUEUE_ER_PUT @ iALU_2\n"); DONE=ALU_OK; pthread_exit(NULL); }
             pthread_mutex_unlock(&count_mutex);

             sleep(delay);

             pthread_mutex_lock(&count_mutex);
             ALU[2].status = ALU_IDLE;
             if( del_work_queue(&qulist,2) == QUEUE_ER_DEL ){
                 printf("QUEUE_ER_DEL @ iALU_2\n"); DONE=ALU_OK; pthread_exit(NULL); }
             pthread_mutex_unlock(&count_mutex);
            
           }

             sleep(ALU_DEF_DELAY);

      if( DONE == ALU_OK )
          pthread_exit(NULL); 

      }
}

void *check_work_queue(void *t){
int i;

  for(;;){
   //check 4 each ALU in work_queue
   pthread_mutex_lock(&count_mutex);
   gid = -1;
   for(i=0; i<3; i++){
       if( is_work_queue_exist(&qulist,i) == QUEUE_ER_ARB ){
            gid = i;
            break;
        }
    }

 //   dump_work_queue(qulist);
   pthread_mutex_unlock(&count_mutex);
    
   sleep(ALU_DEF_DELAY);
 
     if( DONE == ALU_OK)
         pthread_exit(NULL);
  }
}



int iALU_assign(int id,int command,int in_a,int in_b){
    ALU[id].command = command;
    ALU[id].in_a    = in_a;     
    ALU[id].in_b    = in_b;
   
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf("time @ %s", asctime(timeinfo));
    dump_work_queue(qulist);
 
    printf("assign ALU %d\n",id);
    printf("	command : %5d,in_a : %5d,in_b : %5d\n",command,in_a,in_b);
    printf("\n");
    
}

void *icommand(void *t){
   int i=0;
 
   for(;;){
   pthread_mutex_lock(&count_mutex);
    if( 0 <= gid && gid <= 2){
   
     if( i== sizeof(CMD)/sizeof(CMD[0]) ){ 
            DONE=ALU_OK; 
            pthread_exit(NULL); 
            break; 
        }

        iALU_assign(gid,CMD[i].command,CMD[i].in_a,CMD[i].in_b); i++; 
   }
   pthread_mutex_unlock(&count_mutex);

   sleep(2*ALU_DEF_DELAY); 
   }
}

int main(int argc,char *argv[]){

pthread_t thread[5];

pthread_create( &thread[0],NULL, iALU_do_0, NULL);
pthread_create( &thread[1],NULL, iALU_do_1, NULL);
pthread_create( &thread[2],NULL, iALU_do_2, NULL);
pthread_create( &thread[3],NULL, check_work_queue, NULL);
pthread_create( &thread[4],NULL, icommand, NULL);

for(;;){
if( DONE == ALU_OK)
   break;
}

return 0;
}
//#endif
