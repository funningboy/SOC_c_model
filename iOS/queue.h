
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define QUEUE_MAX 3

enum QUEUE_STATUS {
	QUEUE_OK,
	QUEUE_ER_PUT,
	QUEUE_ER_DEL,
	QUEUE_ER_ARB,
	QUEUE_ER_POP,
	QUEUE_ER_SORT,
	QUEUE_ER_CHK,
	QUEUE_ER_FREE,
};

struct QUEUE {
	int	id;
        int     time_out;
	int	priority;
        int     work_time;
	struct QUEUE *nxt;
	struct QUEUE *pre;
};

int free_queue(struct QUEUE **qulist){
    struct QUEUE *hqu = *qulist;
    struct QUEUE  *qu = NULL;
 
    if( is_queue_empty(&hqu) == QUEUE_OK )
       return QUEUE_ER_FREE;

    while(hqu!=NULL){
         qu = hqu;
         hqu = hqu->nxt;
         free(qu);
   }

*qulist = NULL;
return QUEUE_OK;
}

int get_queue_head(struct QUEUE **qulist,struct QUEUE **pop_qu){
    struct QUEUE *hqu = *qulist;

   if( is_queue_empty(&hqu) == QUEUE_OK )
       return QUEUE_ER_POP; 
     
   *pop_qu = hqu;
   *qulist = hqu;

return QUEUE_OK;
}

int pop_queue_head(struct QUEUE **qulist,struct QUEUE **pop_qu){
    struct QUEUE *hqu = *qulist;
    
   if( is_queue_empty(&hqu) == QUEUE_OK )
       return QUEUE_ER_POP; 
     
   *pop_qu = hqu;
   *qulist = hqu->nxt;

free(hqu);
return QUEUE_OK;
}

int push_queue_head(struct QUEUE **qulist, int id, int time_out, int priority, int work_time){
	struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
	struct QUEUE *qul = *qulist;

        if( qu == NULL )
            return QUEUE_ER_PUT;

        qu->id        = id;
        qu->time_out  = time_out;
        qu->priority  = priority;
        qu->work_time = work_time;       
        // initial head
        if( is_queue_empty(&qul) == QUEUE_OK ){ 
            qu->pre  = NULL;
            qu->nxt  = NULL;           
            *qulist  = qu;
        // insert tail 
        } else { 
            qul->pre      = qu;
            qu->pre       = NULL;
            qu->nxt       = qul;
            *qulist       = qu;
        }
 
return QUEUE_OK;
}

int dump_queue(struct QUEUE *qulist){
    struct QUEUE *hqu = qulist;
       
    while( hqu != NULL ){
        printf("queue :: id %2d,time_out : %5d,priority : %5d,work_time : %5d\n",
                hqu->id,
                hqu->time_out,
                hqu->priority,
                hqu->work_time);
        hqu = hqu->nxt;
   }
        printf("\n");

return QUEUE_OK;
}

int del_queue(struct QUEUE **qulist,int id){
    struct QUEUE *qu = *qulist;

if(is_queue_empty(&qu) == QUEUE_OK )
  return QUEUE_ER_DEL;
 
    while( qu != NULL ){
        if( qu->id == id ){
            // only one 
            if( qu->pre == NULL && qu->nxt == NULL ){
                 *qulist = NULL;
            // delete head
            } else if( qu->pre == NULL && qu->nxt != NULL ){ 
                qu->nxt->pre = NULL;
                *qulist      = qu->nxt;
            // delete tail
            } else if( qu->nxt == NULL && qu->pre != NULL ){
                qu->pre->nxt = NULL;
            // delete middle
            } else {
                qu->pre->nxt = qu->nxt;
                qu->nxt->pre = qu->pre;
            }

            free(qu);
            break;
         }
        qu = qu->nxt;
   }

return QUEUE_OK;
}

int is_queue_empty(struct QUEUE **qulist){
   struct QUEUE *hqu  = *qulist; 
    if( hqu == NULL )
        return QUEUE_OK;

return QUEUE_ER_ARB;
}

int is_queue_full(struct QUEUE **qulist){
    struct QUEUE *hqu  = *qulist;
    int j=0;

      while( hqu != NULL ){
             j++;
             hqu = hqu->nxt;
     }            

     if(j>=QUEUE_MAX)
        return QUEUE_OK;

return QUEUE_ER_ARB;
}

int is_queue_exist(struct QUEUE **qulist,int id){
    struct QUEUE *hqu  = *qulist;

      while( hqu != NULL ){
             if( hqu->id == id ){
                 return  QUEUE_OK;
              }
             hqu = hqu->nxt;
     }            
     
return QUEUE_ER_ARB;
}

int swap_queue_by_time_out(struct QUEUE **qulist){
     struct QUEUE *hqu  = *qulist;
     struct QUEUE *qu_1 = hqu;
     struct QUEUE *tqu  = NULL;
	
    if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB;

      while( qu_1 != NULL && qu_1->nxt !=NULL ){
         if( qu_1->time_out > qu_1->nxt->time_out ){
              if( qu_1->pre != NULL ){
                  qu_1->nxt->pre = qu_1->pre;
                  qu_1->pre->nxt = qu_1->nxt; 
              } else{
                  qu_1->nxt->pre = NULL;
                  hqu = qu_1->nxt;
              }

              if( qu_1->nxt->nxt !=NULL ){      
                  qu_1->nxt->nxt->pre = qu_1;
              } 
                  tqu = qu_1->nxt;
                  qu_1->nxt = qu_1->nxt->nxt;
                  qu_1->pre = tqu;
                  tqu->nxt = qu_1;
         } else {
                  qu_1 = qu_1->nxt; 
         }	 
      }

*qulist = hqu;  
return QUEUE_OK;
}

int sort_queue_by_time_out(struct QUEUE **qulist){
struct QUEUE *hqu  = *qulist;
int i;

for(i=0;i<QUEUE_MAX;i++){
    if( swap_queue_by_time_out(&hqu) != QUEUE_OK ){ return QUEUE_ER_SORT; }
   }

*qulist = hqu;
return QUEUE_OK; 
}

int swap_queue_by_priority(struct QUEUE **qulist){
     struct QUEUE *hqu  = *qulist;
     struct QUEUE *qu_1 = hqu;
     struct QUEUE *tqu  = NULL;
	
    if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB ;

      while( qu_1 != NULL && qu_1->nxt !=NULL ){
         if( qu_1->priority > qu_1->nxt->priority ){
              if( qu_1->pre != NULL ){
                  qu_1->nxt->pre = qu_1->pre;
                  qu_1->pre->nxt = qu_1->nxt; 
              } else{
                  qu_1->nxt->pre = NULL;
                  hqu = qu_1->nxt;
              }

              if( qu_1->nxt->nxt !=NULL ){      
                  qu_1->nxt->nxt->pre = qu_1;
              } 
                  tqu = qu_1->nxt;
                  qu_1->nxt = qu_1->nxt->nxt;
                  qu_1->pre = tqu;
                  tqu->nxt = qu_1;
         } else {
                  qu_1 = qu_1->nxt; 
         }	 
      }

*qulist = hqu;  
return QUEUE_OK;
}

int sort_queue_by_priority(struct QUEUE **qulist){
struct QUEUE *hqu  = *qulist;
int i;

for(i=0;i<QUEUE_MAX;i++){
    if( swap_queue_by_priority(&hqu) != QUEUE_OK ){ return QUEUE_ER_SORT; }
 }

*qulist = hqu;
return QUEUE_OK; 
}

int update_queue_time_out(struct QUEUE **qulist,int idec){
struct QUEUE *hqu  = *qulist;
struct QUEUE *qu   =  hqu;
int    remain;

   if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB;

   while(qu!=NULL){
         remain = qu->time_out - idec;
         qu->time_out = (remain>=0)?  remain : 0;
      
         qu = qu->nxt;
    }

*qulist = hqu;
return QUEUE_OK;
}

int mv_queue_to_queue(struct QUEUE **qulist,struct QUEUE **emgcylist){
struct QUEUE *hqu     = *qulist;
struct QUEUE *heme    = *emgcylist;
struct QUEUE *eme     = heme;

if(is_queue_empty(&hqu) == QUEUE_OK)
  return QUEUE_ER_ARB;

while(eme!=NULL){
  if( del_queue(&hqu,eme->id) != QUEUE_OK ){ return QUEUE_ER_DEL; }
  eme = eme->nxt;
}

*qulist    = hqu;
*emgcylist = heme;
return QUEUE_OK;  
}


int check_queue_time_out(struct QUEUE **qulist,struct QUEUE **emgcylist){
struct QUEUE *hqu     = *qulist;
struct QUEUE *qu      =  hqu;
struct QUEUE *emg_hqu = *emgcylist;
int    remain;

   if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB;

   while(qu!=NULL){
         if( qu->time_out ==0 ){
            if( push_queue_head(&emg_hqu, 
                                 qu->id, 
                                 qu->time_out,
                                 qu->priority,
                                 qu->work_time) != QUEUE_OK ){ return QUEUE_ER_CHK;}
         }
       qu = qu->nxt;
    }

*qulist = hqu;
*emgcylist = emg_hqu;
return QUEUE_OK;
}



int update_queue_work_time(struct QUEUE **qu,int idec){
struct QUEUE *hqu  = *qu;
int    remain;

   if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB;

         remain = hqu->work_time - idec;
         hqu->work_time = (remain>=0)?  remain : 0;
      
*qu = hqu;
return QUEUE_OK;
}

int check_queue_work_time(struct QUEUE **qu){
struct QUEUE *hqu  = *qu;
int    remain;

   if(is_queue_empty(&hqu) == QUEUE_OK)
          return QUEUE_ER_ARB;

  if(hqu->work_time == 0)
     return QUEUE_OK;
      
return QUEUE_ER_CHK;  
}
