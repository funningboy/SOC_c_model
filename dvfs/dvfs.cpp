// how to compile 
// g++ -o dvfs dvfs.cpp

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/07/dvfs-emulator.html
//           http://funningboy.blogspot.com/2010/08/smartreflex-power-and-performance.html


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
#include <iostream>
#include <fstream>
#include <string.h>
#include <list>

using namespace std;

#define oFileName	"tmp.f"
#define TimeConstrain    50

enum eSTATUS{
	WORK_1	=1,
	WORK_2	=2,
	WORK_3	=3,
};

template<class T>
class cSTATUS{
public:
	int	cStID;
	string	cStStr;
	T	cStPower;
	T	cStTime;

void   Set_cStID(int a)          { cStID = a;       }
int    Get_cStID()               { return cStID;    }

void   Set_cStStr(string a)      { cStStr = a;      }
string Get_cStStr()              { return cStStr;   }

void   Set_cStPower(T p)         { cStPower = p;    }
T      Get_cStPower()            { return cStPower; }

void   Set_cStTime(T t)          { cStTime = t;     }
T      Get_cStTime()             { return cStTime;  }
};

template<class T>
class cSOLSPC{
public:
	int		cSLSPID;
	T		cSLSPTotPower;
	T		cSLSPTotTime;

void	Set_cSLSPID(int a)	{ cSLSPID = a; }
int	Get_cSLSPID()		{ return cSLSPID;    }

void	Set_cSLSPTotPower(T a)	{ cSLSPTotPower = a;	}
T	Get_cSLSPTotPower()	{ return cSLSPTotPower;	}

void	Set_cSLSPTotTime(T a)	{ cSLSPTotTime = a;	}
T	Get_cSLSPTotTime()	{ return cSLSPTotTime;	}
};

template<class T>
class cTASK{
public:
	int		  cTsID;
	int		  cTsSeq;						//set the sequence in schedule list
	string		  cTsStr;
	cSTATUS<T> 	  cTs_WORK_1;
	cSTATUS<T> 	  cTs_WORK_2;
	cSTATUS<T> 	  cTs_WORK_3;
        list<cSOLSPC<T> > cSOLSPClist; 
	
void   Set_cTsID(int a)        { cTsID = a;    }
int    Get_cTsID()             { return cTsID; }

void   Set_cTsSeq(int a)        { cTsSeq = a;    }
int    Get_cTsSeq()             { return cTsSeq; }

void   Set_cTsStr(string a)    { cTsStr = a;   }
string Get_cTsStr()            { return cTsStr;}

void   Set_cTs_WORK_1(T p,T t)   { cTs_WORK_1.Set_cStID(0);
                                 cTs_WORK_1.Set_cStStr("ST_WORK_1");
                                 cTs_WORK_1.Set_cStPower(p); 
                                 cTs_WORK_1.Set_cStTime(t);  }
cSTATUS<T> Get_cTs_WORK_1()      { return cTs_WORK_1; }

void   Set_cTs_WORK_2(T p,T t)   { cTs_WORK_2.Set_cStID(1);
                                 cTs_WORK_2.Set_cStStr("ST_WORK_2");
                                 cTs_WORK_2.Set_cStPower(p); 
                                 cTs_WORK_2.Set_cStTime(t);  }
cSTATUS<T> Get_cTs_WORK_2()      { return cTs_WORK_2; }

void   Set_cTs_WORK_3(T p,T t)    { cTs_WORK_3.Set_cStID(2);
                                 cTs_WORK_3.Set_cStStr("ST_WORK_3");
                                 cTs_WORK_3.Set_cStPower(p); 
                                 cTs_WORK_3.Set_cStTime(t);  }
cSTATUS<T> Get_cTs_WORK_3()      { return cTs_WORK_3; }


void   Set_cSOLSPCList(cSOLSPC<T> c){  cSOLSPClist.push_back(c);   }
list<cSOLSPC<T> > Get_cSOLSPCList(){  return cSOLSPClist;   }	

};

ofstream    oFilePtr;
ifstream    iFilePtr;
int     snum[3] = { WORK_1, WORK_2, WORK_3 };

int     main(int argc,char* argv[]);
char*   itoa(int value, char* result, int base); 
int     SetModel_WorkSolSpaceWt();
int     SetModel_WorkSolSpaceRd();
void    combination(string x, int n, int r);


cTASK<int> *TsCPUPtr = new cTASK<int>;
cTASK<int> *TsDMAPtr = new cTASK<int>;
cTASK<int> *TsBUSPtr = new cTASK<int>;
 

/* =========================================
* C++ version 0.4 char* style "itoa":
* Written by Lukás Chmela
* Released under GPLv3.
===========================================*/
char* itoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }
	
		char* ptr = result, *ptr1 = result, tmp_char;
		int tmp_value;
	
		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
		} while ( value );
	
		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while(ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr--= *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}

/* ======================================= 
* std::out the result spaces for C(n,m)
*=========================================*/
int  SetModel_WorkSolSpaceWt(){

 oFilePtr.open(oFileName);
 if( oFilePtr == NULL ){ cout<<"Open oFile Error"<<endl; return -1; }

 cout<<"Timing Sequence Space Solutions 4 C(m,n) case"<<endl;

 combination("",3,1);
 combination("",3,2);
 combination("",3,3);

 cout<<endl;
 cout<<endl;

 oFilePtr.close();

return 0;
}

void combination(string x, int n, int r)
{
char buffer[10];

if (r==n||r==0) {
 for (int i=r;i>0;i--) x=x+' '+itoa(snum[i-1],buffer,10);
  cout << x <<endl;
  oFilePtr<<x<<endl;
return;
}

combination(x+' '+itoa(snum[n-1],buffer,10),n-1,r-1);
combination(x,n-1,r);
}



void SetModel_TASKPowerTime(){

            TsCPUPtr->Set_cTsID(0);
            TsCPUPtr->Set_cTsSeq(0);
            TsCPUPtr->Set_cTsStr("CPU_TASK");
            TsCPUPtr->Set_cTs_WORK_1(20,10);
            TsCPUPtr->Set_cTs_WORK_2(10,20);
            TsCPUPtr->Set_cTs_WORK_3(5,40);        

            TsDMAPtr->Set_cTsID(1);
            TsDMAPtr->Set_cTsSeq(1);
            TsDMAPtr->Set_cTsStr("DMA_TASK");
            TsDMAPtr->Set_cTs_WORK_1(30,20);
            TsDMAPtr->Set_cTs_WORK_2(15,40);
            TsDMAPtr->Set_cTs_WORK_3(10,60);
             
            TsBUSPtr->Set_cTsID(2);
            TsBUSPtr->Set_cTsSeq(1); 
            TsBUSPtr->Set_cTsStr("BUS_TASK");
            TsBUSPtr->Set_cTs_WORK_1(40,20);
            TsBUSPtr->Set_cTs_WORK_2(20,40);
            TsBUSPtr->Set_cTs_WORK_3(10,80);
}


int SetModel_WorkSolSpaceRd(){

int    c;
string s;
char   *cstr;

int    CPUTotTime=0, CPUTotPower=0, CPUID=0;
int    DMATotTime=0, DMATotPower=0, DMAID=0;
int    BUSTotTime=0, BUSTotPower=0, BUSID=0;

iFilePtr.open(oFileName);

if( iFilePtr == NULL ){ cout<<"Open iFile Error"<<endl; return -1; }

do {
     c = iFilePtr.get();
     if (c != ' ' && c!= '\n' ){ 
         s += c; 
     }else if( !s.empty() ) {
        strcpy (cstr, s.c_str());
        
      switch( atoi(cstr) ){
           case 1 :  CPUTotTime  += TsCPUPtr->Get_cTs_WORK_1().Get_cStTime(); 
                     CPUTotPower += TsCPUPtr->Get_cTs_WORK_1().Get_cStPower(); 
                  
                     DMATotTime  += TsDMAPtr->Get_cTs_WORK_1().Get_cStTime(); 
                     DMATotPower += TsDMAPtr->Get_cTs_WORK_1().Get_cStPower(); 

                     BUSTotTime  += TsBUSPtr->Get_cTs_WORK_1().Get_cStTime(); 
                     BUSTotPower += TsBUSPtr->Get_cTs_WORK_1().Get_cStPower(); 
                     
                     break;

           case 2 :  CPUTotTime  += TsCPUPtr->Get_cTs_WORK_2().Get_cStTime(); 
                     CPUTotPower += TsCPUPtr->Get_cTs_WORK_2().Get_cStPower(); 
                  
                     DMATotTime  += TsDMAPtr->Get_cTs_WORK_2().Get_cStTime(); 
                     DMATotPower += TsDMAPtr->Get_cTs_WORK_2().Get_cStPower(); 

                     BUSTotTime  += TsBUSPtr->Get_cTs_WORK_2().Get_cStTime(); 
                     BUSTotPower += TsBUSPtr->Get_cTs_WORK_2().Get_cStPower(); 

                     break;

           case 3 :  CPUTotTime  += TsCPUPtr->Get_cTs_WORK_3().Get_cStTime(); 
                     CPUTotPower += TsCPUPtr->Get_cTs_WORK_3().Get_cStPower(); 
                  
                     DMATotTime  += TsDMAPtr->Get_cTs_WORK_3().Get_cStTime(); 
                     DMATotPower += TsDMAPtr->Get_cTs_WORK_3().Get_cStPower(); 

                     BUSTotTime  += TsBUSPtr->Get_cTs_WORK_3().Get_cStTime(); 
                     BUSTotPower += TsBUSPtr->Get_cTs_WORK_3().Get_cStPower(); 

                    break;
           default: break; 
         }         
                     s = "";
     }


         if (c == '\n'){
                cSOLSPC<int> *cCPUCPtr = new cSOLSPC<int>;
                cSOLSPC<int> *cDMACPtr = new cSOLSPC<int>;
                cSOLSPC<int> *cBUSCPtr = new cSOLSPC<int>;

                          cCPUCPtr->Set_cSLSPID(CPUID++);
                          cCPUCPtr->Set_cSLSPTotTime(CPUTotTime);
                          cCPUCPtr->Set_cSLSPTotPower(CPUTotPower);

                          cDMACPtr->Set_cSLSPID(DMAID++);
                          cDMACPtr->Set_cSLSPTotTime(DMATotTime);
                          cDMACPtr->Set_cSLSPTotPower(DMATotPower);

                          cBUSCPtr->Set_cSLSPID(BUSID++);
                          cBUSCPtr->Set_cSLSPTotTime(BUSTotTime);
                          cBUSCPtr->Set_cSLSPTotPower(BUSTotPower);

                          TsCPUPtr->Set_cSOLSPCList(*cCPUCPtr);
                          TsDMAPtr->Set_cSOLSPCList(*cDMACPtr);
                          TsBUSPtr->Set_cSOLSPCList(*cBUSCPtr);
    
                          CPUTotTime =0;  CPUTotPower =0;
                          DMATotTime =0;  DMATotPower =0;
                          BUSTotTime =0;  BUSTotPower =0;
                          
                          delete cCPUCPtr;
                          delete cDMACPtr;
                          delete cBUSCPtr;
                         
            }
 
    } while (c != EOF);

return 0;
}


void SetModel_SolSpaceAns(){
 list<cSOLSPC<int> > cCPUlistPtr;
 list<cSOLSPC<int> > cDMAlistPtr;
 list<cSOLSPC<int> > cBUSlistPtr;

 list<cSOLSPC<int> >::iterator cCPUPtr;
 list<cSOLSPC<int> >::iterator cDMAPtr;
 list<cSOLSPC<int> >::iterator cBUSPtr;

  cCPUlistPtr  =  TsCPUPtr->Get_cSOLSPCList();
  cDMAlistPtr  =  TsDMAPtr->Get_cSOLSPCList();
  cBUSlistPtr  =  TsBUSPtr->Get_cSOLSPCList();

  cSOLSPC<int> *cCPUCPtr = new cSOLSPC<int>;
  cSOLSPC<int> *cDMACPtr = new cSOLSPC<int>;
  cSOLSPC<int> *cBUSCPtr = new cSOLSPC<int>;

int    CPUTotTime, CPUTotPower, CPUID;
int    DMATotTime, DMATotPower, DMAID;
int    BUSTotTime, BUSTotPower, BUSID;

cout<<"Final Answers"<<endl;

  for( cCPUPtr = cCPUlistPtr.begin(); cCPUPtr != cCPUlistPtr.end(); cCPUPtr++ ){
        CPUID      = cCPUPtr->Get_cSLSPID();
        CPUTotTime = cCPUPtr->Get_cSLSPTotTime();
        CPUTotPower= cCPUPtr->Get_cSLSPTotPower();

      for( cDMAPtr = cDMAlistPtr.begin(); cDMAPtr != cDMAlistPtr.end(); cDMAPtr++ ){
         DMAID      = cDMAPtr->Get_cSLSPID();
         DMATotTime = cDMAPtr->Get_cSLSPTotTime();
         DMATotPower= cDMAPtr->Get_cSLSPTotPower();

         if( (CPUTotTime+DMATotTime)<= TimeConstrain ){
           cout<<"Sol CPU @ Work Time Seq ::"<<CPUID<<endl;
           cout<<"Sol DMA @ Work Time Seq ::"<<DMAID<<endl;
           cout<<"=========================="<<endl; 
        }
     }
  }

   for( cCPUPtr = cCPUlistPtr.begin(); cCPUPtr != cCPUlistPtr.end(); cCPUPtr++ ){
        CPUID      = cCPUPtr->Get_cSLSPID();
        CPUTotTime = cCPUPtr->Get_cSLSPTotTime();
        CPUTotPower= cCPUPtr->Get_cSLSPTotPower();
    
      for( cBUSPtr = cBUSlistPtr.begin(); cBUSPtr != cBUSlistPtr.end(); cBUSPtr++ ){
         BUSID      = cBUSPtr->Get_cSLSPID();
         BUSTotTime = cBUSPtr->Get_cSLSPTotTime();
         BUSTotPower= cBUSPtr->Get_cSLSPTotPower();

         if( (CPUTotTime+BUSTotTime)<= TimeConstrain ){
            cout<<"Sol CPU @ Work Time Seq ::"<<CPUID<<endl;
            cout<<"Sol BUS @ Work Time Seq ::"<<BUSID<<endl;
            cout<<"========================="<<endl;
         }

     }
  }

   


}


int main(int argc,char* argv[]){

cout<<"This Model only support @ CPU to DMA time windos"<<endl;
cout<<"                          CPU to BUS            "<<endl;
cout<<"if you want another time windows, Ref \"SetModel_SolSpaceAns()\""<<endl;
cout<<endl;
cout<<endl;
cout<<"Power And Time Set              , Ref \"SetModel_TASKPowerTime()\""<<endl;
cout<<"Timing Sequence Space solutions , Ref \"SetModel_WorkSolSpaceWt()\""<<endl;
cout<<endl;
cout<<endl;

SetModel_TASKPowerTime();
SetModel_WorkSolSpaceWt();
SetModel_WorkSolSpaceRd();
SetModel_SolSpaceAns();

return 0;
}
	
