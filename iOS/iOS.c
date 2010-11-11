
// how to compile 
// gcc -lpthread -o iOS iOS.cpp

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/11/orzmicrokernel-case-stusy-emulator.html

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

int SYS_STATUS =-1;       //the system status
int SYS_TIME   =1;        //the system time trigger nxt edge @ 1s 

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

enum OS_QUEUE{
     OS_QUEUE_FAIL,
     OS_EQUEUE_FAIL,
     OS_QUEUE_EMPTY,
     OS_QUEUE_FULL,
     OS_QUEUE_EXIST,
};

struct TASK {
    int id;
    int time_out;
    int priority;
    int work_time;
    int sleep;

} TASK[] = { 
         {0, 10,  0, 3, 5},
         {1,  3,  1, 3, 3},
         {2,  5,  2, 3, 4},
};

struct QUEUE *qptr        =NULL;
struct QUEUE *pop_qptr    =NULL;
struct QUEUE *emgcy_qptr  =NULL; //@ emergency list

char* encode(int a){
      if(a == (OS_QUEUE_FULL|OS_QUEUE_EXIST) ){ return "the os queue is full or the task already exist\n"; }
 else if(a ==  OS_QUEUE_FAIL){                  return "the os queue create fail\n";                       } 
 else if(a ==  OS_EQUEUE_FAIL){                 return "the os emergency queue create fail\n";             }
 else if(a ==  OS_QUEUE_EMPTY){                 return "the os queue is empty\n";                          }
}

void *iTASK_0(void *t){
      for(;;){
        if( SYS_STATUS == OS_QUEUE_FAIL){ pthread_exit(NULL); }

        pthread_mutex_lock(&count_mutex);
        if( is_queue_full(&qptr)             == QUEUE_OK ||
            is_queue_exist(&qptr,TASK[0].id) == QUEUE_OK ){ 
            printf("%s,%s","@iTask_0->",encode(OS_QUEUE_FULL | OS_QUEUE_EXIST));
        } else {
            if( push_queue_head(&qptr,TASK[0].id,TASK[0].time_out,TASK[0].priority,TASK[0].work_time) != QUEUE_OK ){ 
                printf("%s,%s","@iTASK_0->",encode(OS_QUEUE_FAIL));    
                SYS_STATUS = OS_QUEUE_FAIL;
                pthread_exit(NULL); 
             }
        }
         pthread_mutex_unlock(&count_mutex);

      sleep(TASK[0].sleep);
     }
}

void *iTASK_1(void *t){
      for(;;){
        if( SYS_STATUS == OS_QUEUE_FAIL){ pthread_exit(NULL); }

        pthread_mutex_lock(&count_mutex);
        if( is_queue_full(&qptr)             == QUEUE_OK ||
            is_queue_exist(&qptr,TASK[1].id) == QUEUE_OK ){ 
            printf("%s,%s","@iTask_1->",encode(OS_QUEUE_FULL | OS_QUEUE_EXIST));
        } else {
            if( push_queue_head(&qptr,TASK[1].id,TASK[1].time_out,TASK[1].priority,TASK[1].work_time) != QUEUE_OK ){ 
                printf("%s,%s","@iTASK_1->",encode(OS_QUEUE_FAIL));   
                SYS_STATUS = OS_QUEUE_FAIL; 
                pthread_exit(NULL); 
             }
        }
         pthread_mutex_unlock(&count_mutex);

      sleep(TASK[1].sleep);
     }
}

void *iTASK_2(void *t){
      for(;;){
        if( SYS_STATUS == OS_QUEUE_FAIL){ pthread_exit(NULL); }

        pthread_mutex_lock(&count_mutex);
        if( is_queue_full(&qptr)             == QUEUE_OK ||
            is_queue_exist(&qptr,TASK[2].id) == QUEUE_OK ){ 
            printf("%s,%s","@iTask_2->",encode(OS_QUEUE_FULL | OS_QUEUE_EXIST));
        } else {
            if( push_queue_head(&qptr,TASK[2].id,TASK[2].time_out,TASK[2].priority,TASK[2].work_time) != QUEUE_OK){ 
                printf("%s,%s","@iTASK_2->",encode(OS_QUEUE_FAIL));  
                SYS_STATUS = OS_QUEUE_FAIL; 
                pthread_exit(NULL); 
             }
        }
         pthread_mutex_unlock(&count_mutex);

      sleep(TASK[2].sleep);
     }
}


void *iOS(void *t){
     for(;;){
         if( SYS_STATUS == OS_QUEUE_FAIL){ pthread_exit(NULL); }

             pthread_mutex_lock(&count_mutex);
 
           if( update_queue_time_out(&qptr,SYS_TIME)     != QUEUE_OK ||
               sort_queue_by_time_out(&qptr)             != QUEUE_OK  ){ printf("%s",encode(OS_QUEUE_EMPTY)); } 
          else{ 
              if(check_queue_time_out(&qptr,&emgcy_qptr)!= QUEUE_OK ||
                  mv_queue_to_queue(&qptr,&emgcy_qptr)   != QUEUE_OK ){ printf("%s,%s","iOS->",encode(OS_EQUEUE_FAIL));
                                                                          SYS_STATUS = OS_QUEUE_FAIL; 
                                                                          pthread_exit(NULL);  }
             // emergency queue list exist             
             if(is_queue_empty(&emgcy_qptr)!= QUEUE_OK){
                if( sort_queue_by_priority(&emgcy_qptr)    != QUEUE_OK ||
                    get_queue_head(&emgcy_qptr,&pop_qptr)  != QUEUE_OK ){ printf("%s,%s","iOS->",encode(OS_EQUEUE_FAIL));
                                                                          SYS_STATUS = OS_QUEUE_FAIL; 
                                                                          pthread_exit(NULL);  }
               
                       printf("\t%s,%d\n","@iOS work on emergency queue -> id @",pop_qptr->id);
                       update_queue_work_time(&pop_qptr,SYS_TIME);
                       if( check_queue_work_time(&pop_qptr) == QUEUE_OK ){
                           pop_queue_head(&emgcy_qptr,&pop_qptr);
                       }
                       printf("@NORMAL queue\n");
                       dump_queue(qptr);
                       printf("@EMERGENCY queue\n");
                       dump_queue(emgcy_qptr);
                
          // normal queue list
              }else {
                 if( sort_queue_by_priority(&qptr)    != QUEUE_OK ||
                     get_queue_head(&qptr,&pop_qptr)  != QUEUE_OK ){ printf("%s,%s","iOS->",encode(OS_QUEUE_FAIL));
                                                                          SYS_STATUS = OS_QUEUE_FAIL; 
                                                                          pthread_exit(NULL);  }
  
                       printf("\t%s,%d\n","@iOS work on normal queue -> id @",pop_qptr->id);   
                       update_queue_work_time(&pop_qptr,SYS_TIME);
                       if( check_queue_work_time(&pop_qptr) == QUEUE_OK ){
                           pop_queue_head(&qptr,&pop_qptr);
                       }
                       printf("@NORMAL queue\n");
                       dump_queue(qptr);
                       printf("@EMERGENCY queue\n");
                       dump_queue(emgcy_qptr);
               }
            }
            pthread_mutex_unlock(&count_mutex);
       
   sleep(SYS_TIME); 
   }
}

int main(int argc,char* argv[]){

pthread_t thread[4];

pthread_create( &thread[0],NULL, iTASK_0, NULL);
pthread_create( &thread[1],NULL, iTASK_1, NULL);
pthread_create( &thread[2],NULL, iTASK_2, NULL);
pthread_create( &thread[3],NULL, iOS, NULL);

for(;;){

if(SYS_STATUS==OS_QUEUE_FAIL){ 
pthread_mutex_destroy(&count_mutex);
pthread_join(thread[0], NULL);
pthread_join(thread[1], NULL);
pthread_join(thread[2], NULL);
pthread_join(thread[3], NULL);

break; }

sleep(1);
}


return 0;
}

