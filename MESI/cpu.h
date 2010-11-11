

#include <stdio.h>
#include <stdlib.h>

#define TASK_DEEP	10	

enum CPU_TYPE {
	READ,
	WRIT,
};

struct task {
	unsigned int	address;
	int		rw;
} Ctask;

struct cpu {
	char*		name;
	int		prio;
	int		inx;
	struct	task	ctask[TASK_DEEP];	
} Ccpu[2] = { {"cpu0", 0, 0, 
               { { 0x00000000, READ },
                 { 0x10000000, READ },
                 { 0x20000000, READ },
                 { 0x30000000, WRIT },
                 { 0x40000000, WRIT },
                 { 0xa0000000, READ },
                 { 0xb0000000, READ },
                 { 0xa0000000, WRIT },
                 { 0x00000000, WRIT },
                 { 0x10000000, WRIT },
               },
              },
	     
             { "cpu1", 1, 0, 
               { { 0x00000000, READ },
                 { 0x10000000, READ },
                 { 0x20000000, READ },
                 { 0x30000000, WRIT },
                 { 0x40000000, WRIT },
                 { 0xa0000000, READ },
                 { 0xb0000000, READ },
                 { 0xa0000000, WRIT },
                 { 0x00000000, WRIT },
                 { 0x10000000, WRIT },
               },
              },
};

void get_cpu_info(struct cpu *cp){
     printf("===========================\n");
     printf("cpu name :: %s\n", cp->name);
     printf("cpu prio :: %d\n", cp->prio);
     int i;
     for( i=0; i<TASK_DEEP; i++ ){
          printf("address :: %8x, rw :: %d\n", cp->ctask[i].address, cp->ctask[i].rw); 
     }
     printf("===========================\n");
     printf("\n"); 
}
