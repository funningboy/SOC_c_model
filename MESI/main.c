
//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/10/mesi-emulator-cache-protocol.html

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
 
#include "cache.h"
#include "cpu.h"

#define gldata 10

enum INTERFACE_TYPE {
	ON,
	OFF,
};

struct interface {
	char*		neame;
	int		req;
	int		ack;
	unsigned 	address;
	int		data;
	int		rw;
	int		hit_or_not;
} Cinterface[2] = {
      { "cpu0_cache0"  , OFF ,OFF, 0xffffffff, 0xffffffff, MISS },
      { "cpu1_cache1"  , OFF ,OFF, 0xffffffff, 0xffffffff, MISS },
};

void run_mem_read(unsigned int address,int Id){
     
     if( Id == SHARE ){ 
        printf("	Read from Share memory 2 cache @ share mem part...\n");
     } else {
        printf("	Read from Share memory 2 cache @ local mem part...\n");
     }
     update_cache(&Ccache[0], address, gldata);
     update_cache(&Ccache[1], address, gldata);
	
}

void run_mem_write(){

}

void *run_cpu0(void *t){
  int inx;
  unsigned int address;
  int rw;

     for(;;){
           inx =  Ccpu[0].inx;

       if( inx == TASK_DEEP ){ pthread_exit(NULL); }

          address = Ccpu[0].ctask[inx].address;
               rw = Ccpu[0].ctask[inx].rw;

              Cinterface[0].req     = ON;
              Cinterface[0].address = address;
              Cinterface[0].rw      = rw;
  
          do{ sleep(3); } while( Cinterface[0].ack == ON );

             Cinterface[0].req = OFF;

             inx = ( Cinterface[0].hit_or_not == MISS )? inx : inx+1;
             Ccpu[0].inx = inx;

              sleep(3);
    }
}

void *run_cache0(void *t){
  int inx, status;
  unsigned int address;
  int rw;
  int data, Id;
  int hit_or_not;
  int snoop_rw,snoop_hit_or_not;

     for(;;){
         do{ sleep(3); } while( Cinterface[0].req == OFF || Cinterface[1].req == OFF ); 	
             
            address = Cinterface[0].address; 
                 rw = Cinterface[0].rw;
               data = Cinterface[0].data;
 
                status = Ccache[0].status;
 
                snoop_rw         = Cinterface[1].rw;
                snoop_hit_or_not = Cinterface[1].hit_or_not;
                
                hit_or_not = is_cache_hit(&Ccache[0], address, &data, &Id);
                if( hit_or_not == FAIL ){ /*pthread_kill_other_threads_np(NULL);*/ pthread_exit(NULL); } 
                
                switch(status){
                  case CACHE_INVALID   :     if( rw == READ && hit_or_not == MISS && Id == SHARE  ){ 
                                                 status = CACHE_SHARED;    
                                                 run_mem_read(address, SHARE); 
                                             } 
                                        else if( rw == READ && hit_or_not == MISS && Id == LOCAL0 ){ 
                                                 status = CACHE_EXCLUSIVE; 
                                                 run_mem_read(address, LOCAL0); 
                                             } 
                                        else if( rw == WRIT && hit_or_not == MISS                 ){ 
                                                 status = CACHE_MODIFIED;  
                                                 run_mem_write(address);
                                             } 
                                        break;
                  case CACHE_SHARED    :     if( (rw       == READ &&       hit_or_not == HIT) ||
                                                 (snoop_rw == READ && snoop_hit_or_not == HIT) ){    
                                                  status = CACHE_SHARED;    
                                                  run_mem_read(address, SHARE|LOCAL0 ); 
                                                 }
                                        else if(  snoop_rw == WRIT && snoop_hit_or_not == HIT  ){    status = CACHE_INVALID;   }
                                        else if(  rw       == WRIT &&       hit_or_not == HIT  ){    status = CACHE_MODIFIED;  } 
                                        break;
                  case CACHE_MODIFIED  : break;
                  case CACHE_EXCLUSIVE : break;

                }
                             
   }
}

int main(int argc,char *argv[]){

pthread_t thread[4];

// initial cache[0] && cache[1]
if( set_cache_ini(&Ccache[0],0) == FAIL || set_cache_ini(&Ccache[1],1) == FAIL ){
    printf("initial cache[0] && cache[1] fail ...\n");
    printf("	please check the \"CACHE_DEEP\" is >= cache deep\n");
    printf("	please check the prio is \"0 or 1\"\n");return -1;
}

// initial CPU task
 get_cpu_info(&Ccpu[0]);
 get_cache_info(&Ccache[0]);

 pthread_create( &thread[0],NULL, run_cpu0,   NULL); 
 pthread_create( &thread[1],NULL, run_cache0, NULL); 
 
 pthread_exit(NULL);
return 0;
}
