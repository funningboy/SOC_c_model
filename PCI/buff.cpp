
// how to compile 
// g++ -o buff buff.cpp

//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : 

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
 
#include "buff.h"


template <class T>
int BUFF_CSS<T>::SET2READ(T r){
   if ( index >= BUFFDEP )
      return BUF_RD_OT;

  Buff[index++] = r; 
return BUF_RD_OK;
} 


template <class T>
int BUFF_CSS<T>::SET2WRITE(T &w){
   if ( index <= 0 ) 
	return BUF_WT_OT;
  
  w = Buff[--index]; 
return BUF_WT_OK;
}


template <class T>
void BUFF_CSS<T>::DisPlayBUFF(){
int i;
for(i=0; i<=BUFFDEP; i++){
  cout<<"i:\t"<<i<<"\tVal:\t"<<Buff[i]<<endl;
 }
}


int main(int argc, char* argv[]){

BUFF_CSS<int> *BUF_Pt = new BUFF_CSS<int>(0);


int c;
BUF_Pt->SET2READ(10);
BUF_Pt->SET2WRITE(c);
BUF_Pt->DisPlayBUFF();
cout<<c<<endl;

return 0;

}
