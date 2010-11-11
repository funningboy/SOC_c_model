
#include <stdio.h>
#include <stdlib.h>

#define CACHE_DEEP	12

enum CACHE_TYPE {
	HIT,
	MISS,
	OK,
	FAIL,
	SHARE,
	LOCAL0,
	LOCAL1,
};

enum CACHE_STATUS {
	CACHE_INVALID,
	CACHE_SHARED,
	CACHE_MODIFIED,
	CACHE_EXCLUSIVE,
};

enum MEM_RANGE {
	MEM_SHARE_BEGIN  = 0x00000000,
	MEM_SHARE_END    = 0x90000000,
	MEM_LOCAL0_BEGIN = 0xa0000000,
	MEM_LOCAL0_END   = 0xb0000000,
	MEM_LOCAL1_BEGIN = 0xc0000000,
	MEM_LOCAL1_END   = 0xd0000000,
	MEM_DUMMY	 = 0xffffffff,
};


struct contain {
	unsigned int 	address;
	int		data;
	int		valid;
} Ccontain; 

struct cache {
	char*		name;
	int		prio;	//priority
	int		status;
	unsigned int	iaddress;
	struct contain	ccontain[CACHE_DEEP];
} Ccache[2];



int set_cache_ini(struct cache *ch,int s){

	if(s!=0 && s!=1){ return FAIL; }

	ch->name 	= (s==0)?  "cache0" : "cache1";
	ch->prio 	= (s==0)?        0  :       1 ;
	ch->status= CACHE_INVALID;

        unsigned int i;
	int j=0;
        for( i=MEM_SHARE_BEGIN; i<=MEM_SHARE_END; i+=0x10000000 ){
		ch->ccontain[j].data    = MEM_DUMMY;
		ch->ccontain[j].valid   = FAIL;
      		ch->ccontain[j].address = i; j++;
	}
	
	ch->ccontain[j].data    = MEM_DUMMY;
	ch->ccontain[j].valid   = FAIL;
  	ch->ccontain[j].address = (s==0)? MEM_LOCAL0_BEGIN : MEM_LOCAL1_BEGIN; j++;

	ch->ccontain[j].data    = MEM_DUMMY;
	ch->ccontain[j].valid   = FAIL;
  	ch->ccontain[j].address = (s==0)? MEM_LOCAL0_END   : MEM_LOCAL1_END;

	if( j<= CACHE_DEEP ){ return OK; }

return FAIL;
}

void get_cache_info(struct cache *ch){
     printf("==============================\n");
     printf("cahce name :: %s\n",ch->name);
     printf("cache prio :: %d\n",ch->prio);
     printf("share mem part ...\n");

     unsigned int i;
     int j=0;

     for( i=MEM_SHARE_BEGIN; i<=MEM_SHARE_END; i+=0x10000000 ){
	printf("	address %8x, data %d, valid %d\n",ch->ccontain[j].address, ch->ccontain[j].data, ch->ccontain[j].valid );
        j++;
     }

      printf("local mem part ...\n");
       printf("	address %8x, data %d, valid %d\n",ch->ccontain[j].address, ch->ccontain[j].data, ch->ccontain[j].valid ); j++;
       printf("	address %8x, data %d, valid %d\n",ch->ccontain[j].address, ch->ccontain[j].data, ch->ccontain[j].valid ); 
        
     printf("==============================\n");
     printf("\n");
}


int is_cache_hit(struct cache *ch,unsigned int address,int *data,int *Id){
        int i = address>>28;
        if( i>CACHE_DEEP ){ return FAIL; }

	       if( MEM_SHARE_BEGIN <= address && address <= MEM_SHARE_END && ch->ccontain[i].valid == OK ){
                   *data = ch->ccontain[i].data; *Id = SHARE; 
                    return HIT; 

        } else if( MEM_LOCAL0_BEGIN <= address && address <= MEM_LOCAL0_END && ch->ccontain[i].valid == OK ){
                   *data = ch->ccontain[i].data; *Id = LOCAL0;
                    return HIT;
      
        } else if( MEM_LOCAL1_BEGIN <= address && address <= MEM_LOCAL1_END && ch->ccontain[i].valid == OK ){
                   *data = ch->ccontain[i].data; *Id = LOCAL1;
                    return HIT;
        }  
	return MISS;
}

int update_cache(struct cache *ch,unsigned int address,int data){
	int i = address>>28;
	if( i>CACHE_DEEP ){ return FAIL; }

	       if( MEM_SHARE_BEGIN <= address && address <=MEM_SHARE_END  ){
                   ch->ccontain[i].data = data;  ch->ccontain[i].valid =OK; 
                   return HIT; 

        } else if( MEM_LOCAL0_BEGIN <= address && address <=MEM_LOCAL0_END  ){
                   ch->ccontain[i].data = data;  ch->ccontain[i].valid =OK; 
                   return HIT;
 
        } else if( MEM_LOCAL1_BEGIN <= address && address <=MEM_LOCAL1_END  ){
                   ch->ccontain[i].data = data;  ch->ccontain[i].valid =OK; 
                   return HIT; 
        } 

	return MISS;
}


int mask_cache(){
}



