// how to compile 
// gcc -lpthread -o network network.c

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/08/network-on-chip-c.html

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

#define MAX_FIFO_DEEP		2	
#define	MAX_NET_WORK_EXP	9
#define	TASK_DEBUG		0	// 0 Display / -1 No
#define	NET_DEBUG		-1	// 0 Display / -1 No


pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

enum ADDR_TABLE {
	NET_WORK_0_ST	=0x00000000,
	NET_WORK_0_ED	=0x000000ff,
	NET_WORK_1_ST	=0x00000100,
	NET_WORK_1_ED	=0x000001ff,
	NET_WORK_2_ST	=0x00000200,
	NET_WORK_2_ED	=0x000002ff,
	NET_WORK_3_ST	=0x00000300,
	NET_WORK_3_ED	=0x000003ff,
	NET_WORK_4_ST	=0x00000400,
	NET_WORK_4_ED	=0x000004ff,
	NET_WORK_5_ST	=0x00000500,
	NET_WORK_5_ED	=0x000005ff,
	NET_WORK_6_ST	=0x00000600,
	NET_WORK_6_ED	=0x000006ff,
	NET_WORK_7_ST	=0x00000700,
	NET_WORK_7_ED	=0x000007ff,
	NET_WORK_8_ST	=0x00000800,
	NET_WORK_8_ED	=0x000008ff,
};

enum NET_TYPE_DIRECT {
	NET_DIRECT_UP	=0,
	NET_DIRECT_DOWN	=1,
	NET_DIRECT_RIGHT=2,
	NET_DIRECT_LEFT	=3,
	NET_DIRECT_DUMMY=4,
	NET_TYPE_READ	=5,
	NET_TYPE_WRITE	=6,
};

enum MESSAGE {
	NET_TRUE		=0,
	NET_FALSE		=1,
	NET_OK_NODE_ID		=2,
	NET_ERR_NODE_ID		=3,
	NET_OK_TASK		=4,
	NET_ERR_TASK		=5,
	NET_OK_NODE_DIRECT	=6,
	NET_ERR_NODE_DIRECT	=7,
	NET_OK_NODE_ADDR	=8,
	NET_ERR_NODE_ADDR	=9,
};

struct Addr {
	int	CurAddr;
	int	EndAddr;
	int	RWType;
	int	TId;
} Addr;

struct Data {
	int	CurAddr;
	int	EndAddr;
	int	Data;
	int	TId;
} Data;

struct NetNode {
	// Address Phase //
	struct	Addr AddrFIFO[MAX_FIFO_DEEP];
	int	Addr_INX;
	int	Addr_EMPTY;
	int	Addr_FULL;
	int	Addr_DELAY;
	// Data Phase //
	struct	Data DataFIFO[MAX_FIFO_DEEP];
	int	Data_INX;
	int	Data_EMPTY;
	int	Data_FULL;
	int	Data_DELAY;

	// Connection
	struct  NetNode *Nxt[];
	// Memory Map Location

} NetNode[MAX_NET_WORK_EXP];

// Define Our Task List //
struct Task {
	int	TId;
	int	NId;
	int	FromAddr;
	int	ToAddr;
	int	RWType;
	int	DepTId;
	int	AddrDone;
} TaskList[] = {
	{0,	0,	NET_WORK_0_ST,	NET_WORK_4_ST,	NET_TYPE_READ,	-1,	NET_FALSE},
	{1,	0,	NET_WORK_0_ST,	NET_WORK_4_ST,	NET_TYPE_WRITE,	0,	NET_FALSE},
	{2,	1,	NET_WORK_1_ST,	NET_WORK_8_ST,	NET_TYPE_WRITE,	-1,	NET_FALSE},
	{3,	0,	NET_WORK_0_ST,	NET_WORK_4_ST,	NET_TYPE_READ,	-1,	NET_FALSE},
};


void SetMapTable4NetNode(){
        // Net 0 connect {1,3} //
	NetNode[0].Nxt[0] = &NetNode[1];	NetNode[0].Addr_DELAY =3;	
	NetNode[0].Nxt[1] = &NetNode[2];	NetNode[0].Data_DELAY =3;
	// Net 1 connect {0,2,4} //
	NetNode[1].Nxt[0] = &NetNode[0];	NetNode[1].Addr_DELAY =4;
	NetNode[1].Nxt[1] = &NetNode[2];	NetNode[1].Data_DELAY =4;
	NetNode[1].Nxt[2] = &NetNode[4];
	// Net 2 connect {1,5} //
	NetNode[2].Nxt[0] = &NetNode[1];	NetNode[2].Addr_DELAY =2;
	NetNode[2].Nxt[1] = &NetNode[5];	NetNode[2].Data_DELAY =2;
	// Net 3 connect {0,4,6} //
	NetNode[3].Nxt[0] = &NetNode[0];	NetNode[3].Addr_DELAY =2;
	NetNode[3].Nxt[1] = &NetNode[4];	NetNode[3].Data_DELAY =2;
	NetNode[3].Nxt[2] = &NetNode[6];
	// Net 4 connect {1,3,5,7}
	NetNode[4].Nxt[0] = &NetNode[1];	NetNode[4].Addr_DELAY =1;
	NetNode[4].Nxt[1] = &NetNode[3];	NetNode[4].Data_DELAY =1;
	NetNode[4].Nxt[2] = &NetNode[5];
	NetNode[4].Nxt[3] = &NetNode[7];
	// Net 5 connect {2,4,8}
	NetNode[5].Nxt[0] = &NetNode[2];	NetNode[5].Addr_DELAY =2;
	NetNode[5].Nxt[1] = &NetNode[4];	NetNode[5].Data_DELAY =2;
	NetNode[5].Nxt[2] = &NetNode[8];
	// Net 6 connect {3,7}
	NetNode[6].Nxt[0] = &NetNode[3];	NetNode[6].Addr_DELAY =2;
	NetNode[6].Nxt[1] = &NetNode[7];	NetNode[6].Data_DELAY =2;
	// Net 7 connect {4,6,8}	
	NetNode[7].Nxt[0] = &NetNode[4];	NetNode[7].Addr_DELAY =4;
	NetNode[7].Nxt[1] = &NetNode[6];	NetNode[7].Data_DELAY =4;
	NetNode[7].Nxt[2] = &NetNode[8];
	// Net 8 connect {5,7}
	NetNode[8].Nxt[0] = &NetNode[5];	NetNode[8].Addr_DELAY =3;
	NetNode[8].Nxt[1] = &NetNode[7];	NetNode[8].Data_DELAY =3;
}

void SetInitial4NetNode(){
int i,j;

	for(i=0; i<MAX_NET_WORK_EXP; i++){
		NetNode[i].Addr_INX   = 0;
		NetNode[i].Addr_EMPTY = NET_TRUE;
		NetNode[i].Addr_FULL  = NET_FALSE;

           for(j=0; j<MAX_FIFO_DEEP; j++){
		NetNode[i].AddrFIFO[j].CurAddr =0;
		NetNode[i].AddrFIFO[j].EndAddr =0;
		NetNode[i].AddrFIFO[j].RWType  =0;
		NetNode[i].AddrFIFO[j].TId     =0;
           }
		NetNode[i].Data_INX   = 0;
		NetNode[i].Data_EMPTY = NET_TRUE;
		NetNode[i].Data_FULL  = NET_FALSE;
        }
}

int DecoderAddr2NetNodeId(int Addr, int *Id){
       if( Addr <= NET_WORK_0_ST && Addr <= NET_WORK_0_ED 	) { *Id=0; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_1_ST && Addr <= NET_WORK_1_ED	) { *Id=1; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_2_ST && Addr <= NET_WORK_2_ED	) { *Id=2; return NET_OK_NODE_ID; }	
  else if( Addr <= NET_WORK_3_ST && Addr <= NET_WORK_3_ED	) { *Id=3; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_4_ST && Addr <= NET_WORK_4_ED	) { *Id=4; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_5_ST && Addr <= NET_WORK_5_ED	) { *Id=5; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_6_ST && Addr <= NET_WORK_6_ED 	) { *Id=6; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_7_ST && Addr <= NET_WORK_7_ED	) { *Id=7; return NET_OK_NODE_ID; }
  else if( Addr <= NET_WORK_8_ST && Addr <= NET_WORK_8_ED	) { *Id=8; return NET_OK_NODE_ID; }

return NET_ERR_NODE_ID;
}

int DecoderNetNodeId2Direct(int Id, int *x, int *y){
       if( Id ==0 ){	*x=0;	*y=0;	return NET_OK_NODE_DIRECT; } 
  else if( Id ==1 ){	*x=0;	*y=1;	return NET_OK_NODE_DIRECT; }
  else if( Id ==2 ){	*x=0;	*y=2;	return NET_OK_NODE_DIRECT; }
  else if( Id ==3 ){	*x=1;	*y=0;	return NET_OK_NODE_DIRECT; }
  else if( Id ==4 ){	*x=1;	*y=1;	return NET_OK_NODE_DIRECT; }
  else if( Id ==5 ){	*x=1;	*y=2;	return NET_OK_NODE_DIRECT; }
  else if( Id ==6 ){	*x=2;	*y=0;	return NET_OK_NODE_DIRECT; }
  else if( Id ==7 ){	*x=2;	*y=1;	return NET_OK_NODE_DIRECT; }
  else if( Id ==8 ){	*x=2;	*y=2;	return NET_OK_NODE_DIRECT; }

return NET_ERR_NODE_DIRECT;
}

int DecoderNetNodeId2AddrBase(int Id, int *addr){
      if( Id ==0 ){ *addr = NET_WORK_0_ST; return NET_OK_NODE_ADDR; }
 else if( Id ==1 ){ *addr = NET_WORK_1_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==2 ){ *addr = NET_WORK_2_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==3 ){ *addr = NET_WORK_3_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==4 ){ *addr = NET_WORK_4_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==5 ){ *addr = NET_WORK_5_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==6 ){ *addr = NET_WORK_6_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==7 ){ *addr = NET_WORK_7_ST; return NET_OK_NODE_ADDR; } 
 else if( Id ==8 ){ *addr = NET_WORK_8_ST; return NET_OK_NODE_ADDR; } 

return NET_ERR_NODE_ADDR;
}

int CheckOwnTaskListAddrDone(int tid){
    int i;

    for(i=0; i<sizeof(TaskList)/sizeof(TaskList[0]); i++ ){
        if( /*TaskList[i].TId == tid &&*/ TaskList[i].AddrDone == NET_FALSE ){ return NET_FALSE; break; }
    }
   
    return NET_TRUE;
}

void SetTaskListAddrInf2NetNoe(int nid,int tid){
                int inx = NetNode[nid].Addr_INX;
		NetNode[nid].AddrFIFO[inx].CurAddr = TaskList[tid].FromAddr;
		NetNode[nid].AddrFIFO[inx].EndAddr = TaskList[tid].ToAddr;	
		NetNode[nid].AddrFIFO[inx].RWType  = TaskList[tid].RWType;
		NetNode[nid].AddrFIFO[inx].TId     = TaskList[tid].TId; 
		inx++;
		NetNode[nid].Addr_INX = inx;
		
		NetNode[nid].Addr_EMPTY = ( NetNode[nid].Addr_INX ==0             )? NET_TRUE : NET_FALSE;
		NetNode[nid].Addr_FULL	= ( NetNode[nid].Addr_INX ==MAX_FIFO_DEEP )? NET_TRUE : NET_FALSE;

//		printf("SetTaskListAddrInf2NetNode() @ NId:: %d,\t TId:: %d,\tAddr_INX:: %d,\tAddr_EMPTY:: %d,\tAddr_FULL:: %d\n",
//                        nid,tid,NetNode[nid].Addr_INX,NetNode[nid].Addr_EMPTY,NetNode[nid].Addr_FULL);
}

void SetNetNodeInf2TaskListAddr(int cnid,int fmaddr,int nnid,int tid){
int i;
		TaskList[tid].FromAddr = fmaddr;
		TaskList[tid].NId      = nnid; 
		int inx = NetNode[cnid].Addr_INX;
		inx--;
                NetNode[cnid].Addr_INX = inx;

		NetNode[cnid].Addr_EMPTY = ( NetNode[cnid].Addr_INX ==0             )? NET_TRUE : NET_FALSE;
		NetNode[cnid].Addr_FULL	 = ( NetNode[cnid].Addr_INX ==MAX_FIFO_DEEP )? NET_TRUE : NET_FALSE;

                for(i=0; i<MAX_FIFO_DEEP-1; i++){
                    NetNode[cnid].AddrFIFO[i] = NetNode[cnid].AddrFIFO[i+1];
                }

//		printf("GetNetNodeInf2TaskListAddr() @ NId:: %d,\t TId:: %d,\tAddr_INX:: %d,\tAddr_EMPTY:: %d,\tAddr_FULL:: %d\n",
//                        cnid,tid,NetNode[cnid].Addr_INX,NetNode[cnid].Addr_EMPTY,NetNode[cnid].Addr_FULL);
}

int CheckTaskListAndSetAddrInf2NetNode(int nid){
    int i,j;

    for(i=0; i<sizeof(TaskList)/sizeof(TaskList[0]); i++ ){
      
       if( TaskList[i].NId == nid && TaskList[i].AddrDone == NET_FALSE ){
         if( TaskList[i].DepTId == -1 ){ 

 		SetTaskListAddrInf2NetNoe( nid, i);
                return NET_OK_TASK; break; 
                }
        
         else {
             for( j=0; j<sizeof(TaskList)/sizeof(TaskList[0]); j++){
                  if( TaskList[i].DepTId == TaskList[j].TId && TaskList[j].AddrDone == NET_TRUE ){

 		SetTaskListAddrInf2NetNoe( nid, i);
                return NET_OK_TASK; break;  
                }   
             }
        }
     }   
  }

return NET_ERR_TASK;
}	

int CheckAddrInfNetNode2NetNode(int cnid,int remx, int remy,int *nnid){
   int up=-1,dwn=-1,lf=-1,rig=-1;
   int direct;
   int newnid,newaddr;

   for(;;){ 
         if( remx>0 && dwn==-1 ){ newnid = cnid + 3; direct =NET_DIRECT_DOWN; }
    else if( remx<0 && up ==-1 ){ newnid = cnid - 3; direct =NET_DIRECT_UP;   }
    else if( remy>0 && rig==-1 ){ newnid = cnid + 1; direct =NET_DIRECT_RIGHT;}
    else if( remy<0 && up ==-1 ){ newnid = cnid - 1; direct =NET_DIRECT_LEFT; } 
    else if( remx==0 && remy==0){ *nnid  = cnid; return NET_OK_NODE_DIRECT; break;  }
    else                        {                return NET_ERR_NODE_DIRECT; break; }
 
    if( DecoderNetNodeId2AddrBase(newnid, &newaddr) == NET_ERR_NODE_ADDR ){ return NET_ERR_NODE_DIRECT; break; }
    
   int Addr_INX = NetNode[newnid].Addr_INX; 
     
   if( NetNode[newnid].Addr_FULL == NET_TRUE ){     if( direct ==NET_DIRECT_DOWN ){ dwn=0; }
                                               else if( direct ==NET_DIRECT_UP   ){ up=0;  }
                                               else if( direct ==NET_DIRECT_RIGHT){ rig=0; }
                                               else if( direct ==NET_DIRECT_LEFT ){ lf=0;  }
   } else {
     *nnid = newnid;
     return NET_OK_NODE_DIRECT;
     break;
   }
  }

}

int CheckAddrInfNetNode2TaskList(int nid){

    int newCurNid,newCurX,newCurY,curAddrBase;
    int newEndNid,newEndX,newEndY,newTmpNid;

    int curAddr = NetNode[nid].AddrFIFO[0].CurAddr;
    int endAddr = NetNode[nid].AddrFIFO[0].EndAddr;    
    int TId     = NetNode[nid].AddrFIFO[0].TId;
  
    if( DecoderAddr2NetNodeId(curAddr,&newCurNid) == NET_ERR_NODE_ID ){ printf("Out-Of-CurAddr 2 NodeId Phase @ %d \n",nid); return  NET_ERR_TASK; }
    if( DecoderAddr2NetNodeId(endAddr,&newEndNid) == NET_ERR_NODE_ID ){ printf("Out-Of-EndAddr 2 NodeId Phase @ %d \n",nid); return  NET_ERR_TASK; }
 
    if( DecoderNetNodeId2Direct(newCurNid, &newCurX, &newCurY) == NET_ERR_NODE_DIRECT ){ printf("Out-Of-CurNid 2 Direct Phase @ %d\n",nid); 
                                                                                         return NET_ERR_TASK; }
    if( DecoderNetNodeId2Direct(newEndNid, &newEndX, &newEndY) == NET_ERR_NODE_DIRECT ){ printf("Out-Of-EndNid 2 Direct Phase @ %d\n",nid); 
                                                                                         return NET_ERR_TASK; }
    int remainX = newEndX - newCurX;
    int remainY = newEndY - newCurY;
   
    if( DecoderNetNodeId2AddrBase(newCurNid, &curAddrBase) == NET_ERR_NODE_ADDR ){ printf("Out-Of-CurNid 2 CurAddrBase Phase @ %d\n",nid);
                                                                                   return NET_ERR_TASK; }
     
    if( CheckAddrInfNetNode2NetNode(newCurNid, remainX, remainY, &newTmpNid) == NET_ERR_NODE_DIRECT ){
                               printf("Out-Of-CurNid 2 NxtNid Phase @ %d\n",nid);  return NET_ERR_TASK;
    } else {
        int offset =  curAddr - curAddrBase;
        
        if( DecoderNetNodeId2AddrBase(newTmpNid, &curAddrBase) == NET_ERR_NODE_ADDR ){ printf("Out-Of-CurNid 2 CurAddrBase Phase @ %d\n",nid);
                                                                                   return NET_ERR_TASK; }
        
         curAddr = offset + curAddrBase;
         SetNetNodeInf2TaskListAddr(nid, curAddr, newTmpNid, TId);
	  // Done
        if( newCurNid == newTmpNid ){ TaskList[TId].AddrDone = NET_TRUE;  }
        return NET_OK_TASK;
    }
}

void DisplayTaskList(){
int i;

for(i=0; i<sizeof(TaskList)/sizeof(TaskList[0]); i++){
	printf("TId      :: %5d,",TaskList[i].TId);
	printf("NId      :: %5d,",TaskList[i].NId);
	printf("FromAddr :: %5x,",TaskList[i].FromAddr);
	printf("ToAddr   :: %5x,",TaskList[i].ToAddr);
	printf("RWType   :: %5d,",TaskList[i].RWType);
	printf("DepTId   :: %5d,",TaskList[i].DepTId);
	printf("AddrDone :: %5d\n",TaskList[i].AddrDone);
 }
	printf("---------------------------------------\n");
	printf("\n");
}


void DisplayMapTable4NetNode(){
     int i,j;

 for(i=0; i<MAX_NET_WORK_EXP; i++){
    printf("NId        :: %5d,",i);
    printf("Addr_INX   :: %5d,",NetNode[i].Addr_INX);
    printf("Addr_EMPTY :: %5d,",NetNode[i].Addr_EMPTY);
    printf("Addr_FULL  :: %5d,",NetNode[i].Addr_FULL);
    printf("Addr_DELAY :: %5d\n",NetNode[i].Addr_DELAY);
    printf("@\n");   
   for(j=0; j<MAX_FIFO_DEEP; j++ ){ 
       
    printf("CurAddr    :: %5x,",NetNode[i].AddrFIFO[j].CurAddr);
    printf("EndAddr    :: %5x,",NetNode[i].AddrFIFO[j].EndAddr);
    printf("RWType     :: %5d,",NetNode[i].AddrFIFO[j].RWType);
    printf("TId        :: %5d,\n",NetNode[i].AddrFIFO[j].TId);
   }
    printf("@\n");
    printf("\n");
 }
	printf("---------------------------------------\n");
	printf("\n");

}
void *SetAddrInf2NetNodeId_0(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
                if(NET_DEBUG==0){ DisplayMapTable4NetNode();}
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_0(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
               if(NET_DEBUG==0 ){ DisplayMapTable4NetNode(); }
               if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_1(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
		if(NET_DEBUG==0){ DisplayMapTable4NetNode(); }
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_1(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
	       if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }     
	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_2(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
 		if(NET_DEBUG==0){ DisplayMapTable4NetNode(); }       
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_2(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
 	       if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }        
	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_3(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
 		if(NET_DEBUG==0){ DisplayMapTable4NetNode(); }         
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_3(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
  	       if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
               if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_4(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
    	        if(NET_DEBUG==0){ DisplayMapTable4NetNode(); }        
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_4(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
     	       if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
       	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_5(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
                if(NET_DEBUG==0){ DisplayMapTable4NetNode(); } 
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_5(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
     	       if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
       	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_6(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
                if(NET_DEBUG==0){ DisplayMapTable4NetNode(); } 
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_6(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
               if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
       	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_7(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
	        if(NET_DEBUG==0){ DisplayMapTable4NetNode(); } 
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_7(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
               if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
       	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}

void *SetAddrInf2NetNodeId_8(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
     cot =3;
     while( NetNode[NodeId].Addr_FULL == NET_TRUE ){
            sleep( NetNode[NodeId].Addr_DELAY );
            if( cot== 0 ){ printf("Out-of-Time Wait 4 NetNode Set Addr Phase @ %d \n",NodeId); break; }
            cot--;
     }

     if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if ( CheckTaskListAndSetAddrInf2NetNode(NodeId) == NET_OK_TASK ){
		printf("Set TaskList 2 NetNode Ok @ %d \n",NodeId);
	        if(NET_DEBUG==0){ DisplayMapTable4NetNode(); } 
	}
	pthread_mutex_unlock(&count_mutex);
     } else {
           sleep( NetNode[NodeId].Addr_DELAY );
    }

    sleep( NetNode[NodeId].Addr_DELAY );
 }
 pthread_exit(NULL); 
}

void *GetAddrInf2NetNodeId_8(void *t){
     int NodeId = (int)t;
     int cot;

while( CheckOwnTaskListAddrDone(NodeId) == NET_FALSE ){
    cot =3;
    while( NetNode[NodeId].Addr_EMPTY == NET_TRUE ){
           sleep( NetNode[NodeId].Addr_DELAY );
           if( cot== 0){ printf("Out-of-Time Wait 4 NetNode Get Addr Phase @ %d \n",NodeId); break; }
           cot--;
   }

   if( cot >0 ){
	pthread_mutex_lock(&count_mutex);
	if (CheckAddrInfNetNode2TaskList(NodeId) == NET_OK_TASK ){
               printf("Get NetNode 2 TaskList Ok @ %d\n",NodeId);
               if(NET_DEBUG==0) { DisplayMapTable4NetNode(); }         
       	       if(TASK_DEBUG==0){ DisplayTaskList();         }
        }
	pthread_mutex_unlock(&count_mutex);
	
  } else {
      sleep( NetNode[NodeId].Addr_DELAY );
  }

   sleep( NetNode[NodeId].Addr_DELAY );
 }	 

 pthread_exit(NULL);
}








int main(int argc,char* argv[]){

//1. Set The Map Table 
SetMapTable4NetNode();

//2. Initial the NetNode 2 Reset Mode
SetInitial4NetNode();

DisplayMapTable4NetNode();
DisplayTaskList();


 pthread_t thread[18];
 
 // Addr Phase 
 pthread_create( &thread[ 0],NULL, SetAddrInf2NetNodeId_0, (void*)0);
 pthread_create( &thread[ 1],NULL, GetAddrInf2NetNodeId_0, (void*)0);
 pthread_create( &thread[ 2],NULL, SetAddrInf2NetNodeId_1, (void*)1);
 pthread_create( &thread[ 3],NULL, GetAddrInf2NetNodeId_1, (void*)1);
 pthread_create( &thread[ 4],NULL, SetAddrInf2NetNodeId_2, (void*)2);
 pthread_create( &thread[ 5],NULL, GetAddrInf2NetNodeId_2, (void*)2);
 pthread_create( &thread[ 6],NULL, SetAddrInf2NetNodeId_3, (void*)3);
 pthread_create( &thread[ 7],NULL, GetAddrInf2NetNodeId_3, (void*)3);
 pthread_create( &thread[ 8],NULL, SetAddrInf2NetNodeId_4, (void*)4);
 pthread_create( &thread[ 9],NULL, GetAddrInf2NetNodeId_4, (void*)4);
 pthread_create( &thread[10],NULL, SetAddrInf2NetNodeId_5, (void*)5);
 pthread_create( &thread[11],NULL, GetAddrInf2NetNodeId_5, (void*)5);
 pthread_create( &thread[12],NULL, SetAddrInf2NetNodeId_6, (void*)6);
 pthread_create( &thread[13],NULL, GetAddrInf2NetNodeId_6, (void*)6);
 pthread_create( &thread[14],NULL, SetAddrInf2NetNodeId_7, (void*)7);
 pthread_create( &thread[15],NULL, GetAddrInf2NetNodeId_7, (void*)7);
 pthread_create( &thread[16],NULL, SetAddrInf2NetNodeId_8, (void*)8);
 pthread_create( &thread[17],NULL, GetAddrInf2NetNodeId_8, (void*)8);
                     
 pthread_join( thread[ 0],NULL);
 pthread_join( thread[ 1],NULL);
 pthread_join( thread[ 2],NULL);
 pthread_join( thread[ 3],NULL);
 pthread_join( thread[ 4],NULL);
 pthread_join( thread[ 5],NULL);
 pthread_join( thread[ 6],NULL);
 pthread_join( thread[ 7],NULL);
 pthread_join( thread[ 8],NULL);
 pthread_join( thread[ 9],NULL);
 pthread_join( thread[10],NULL);
 pthread_join( thread[11],NULL);
 pthread_join( thread[12],NULL);
 pthread_join( thread[13],NULL);
 pthread_join( thread[14],NULL);
 pthread_join( thread[15],NULL);
 pthread_join( thread[16],NULL);
 pthread_join( thread[17],NULL);


 pthread_exit(NULL); 
 
return 0;
}
