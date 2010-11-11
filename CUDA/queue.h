
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

enum QUEUE_STATUS {
	QUEUE_OK,
	QUEUE_ER_PUT,
	QUEUE_ER_DEL,
	QUEUE_ER_ARB,
};

struct QUEUE {
	int	id;
        int     command;
	int	in_a;
	int	in_b;
	int	out_c;	
	struct QUEUE *nxt;
	struct QUEUE *pre;
} WORK_QUEUE;


int put_work_queue(struct QUEUE **qulist, int id, int command, int in_a, int in_b, int out_c){
	struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
	struct QUEUE *qul = (struct QUEUE *) malloc(sizeof(struct QUEUE));

        qul = *qulist;

        if( qu == NULL )
            return QUEUE_ER_PUT;

        qu->id      = id;
        qu->command = command;
        qu->in_a    = in_a;
        qu->in_b    = in_b;
        qu->out_c   = out_c;
       
        // initial head
        if( qul == NULL ){ 
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

int dump_work_queue(struct QUEUE *qulist){
    struct QUEUE *qu = (struct QUEUE *) malloc(sizeof(struct QUEUE)); 
    qu = qulist;
       
    while( qu != NULL ){
        printf("queue :: id %2d,cmd : %5d,in_a : %5d,in_b : %5d, (pre)out_c : %5d\n",
                qu->id,qu->command,qu->in_a,qu->in_b,qu->out_c);
        qu = qu->nxt;
   }
        printf("\n");

free(qu);
return QUEUE_OK;
}

int del_work_queue(struct QUEUE **qulist,int id){
    struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
    struct QUEUE *qut = (struct QUEUE *) malloc(sizeof(struct QUEUE));
    qu = *qulist;

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

            return QUEUE_OK;
            break;
         }
        qu = qu->nxt;
   }

free(qu);
return QUEUE_ER_DEL; 
}

int is_work_queue_empty(struct QUEUE **qulist){
   struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
          qu = *qulist; 
    if( qu == NULL )
        return QUEUE_OK;

free(qu);
return QUEUE_ER_ARB;
}

int is_work_queue_full(struct QUEUE **qulist){
    struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
           qu = *qulist;
    int j;

      while( qu != NULL ){
             j++;
             qu = qu->nxt;
     }            
      
     if(j>=3)
        return QUEUE_OK;
     
return QUEUE_ER_ARB;
}

int is_work_queue_exist(struct QUEUE **qulist,int id){
    struct QUEUE *qu  = (struct QUEUE *) malloc(sizeof(struct QUEUE));
           qu = *qulist;

      while( qu != NULL ){
             if( qu->id == id ){
                 return  QUEUE_OK;
                 break;
              }
             qu = qu->nxt;
     }            
     
return QUEUE_ER_ARB;
}
