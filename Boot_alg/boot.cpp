//# author : sean chen 
//# email  : funningboy@gmail.com
//# post @ : http://funningboy.blogspot.com/2010/08/booth-algorithm.html

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
 
#include <iostream>
#include <string>


#define	BITSIZE 5	

using namespace std;

class Booth{
private:
	int	a;
	string	a_str;
	int	b;
	string	b_str;
	int	c;
	string	c_str;
	int	bit;
public :
int	CheckRange(int a){int i=( -32768<=a && a<=32767 )? 0:-1; return i; }

void	SetBooth_a(int i){ a=i;      }
int	GetBooth_a(){      return a; }

void	SetBooth_a_str(int a){ a_str = Int2Bin(a); }
string  GetBooth_a_str(){      return a_str;       }

void	SetBooth_b(int i){ b=i;      }
int	GetBooth_b(){      return b; }

void	SetBooth_b_str(int b){ b_str = Int2Bin(b); }
string  GetBooth_b_str(){      return b_str;       }

void	SetBooth_c(int i){ c=i;      }
int	GetBooth_c(){      return c; }

void	SetBooth_c_str(int c,int b){ c_str = Int2Bin(c)+Int2Bin(b); }
string	GetBooth_c_str(){      return c_str;       }

int	SetBooth_Bit(int b){ bit=b; }

string	GetBegin_str(string s){ string::iterator it;
                                string t;
                                it = s.begin();
                                t = *it;
                                return t;
                              }

string	GetEnd_str(string s){ string::iterator it; 
			       string t;
                               it = s.end()-1;
                               t = *it;
                               return t;
                             }

string Int2Bin(int number)
{
    string bins;
    int mask = 0x01;

    bins.resize(BITSIZE);

    for( int i = 0; i < BITSIZE; i++ )
         bins[BITSIZE-1-i] = ((number >> i) & mask )? '1' : '0';

    return bins;
}

void Pro_SHIFT_c_str(){
     string::iterator it;
     string str;
 
       str += *c_str.begin();
     for(it= c_str.begin(); it<c_str.end()-1; it++){
       str += *it; 
    }

     c_str=str;
     c = c >>1;
}

void Pro_ADD_A_c_str(){
   int r = c = c + a; 
   string  r_str = Int2Bin(r);

   string::iterator it;
   string s_str;
   for( it=c_str.begin()+BITSIZE; it<c_str.end(); it++ ){ 
        s_str += *it;
   }
   r_str += s_str;
   c_str = r_str;
}

void Pro_SUB_A_c_str(){
   int r = c = c -a; 
   string  r_str = Int2Bin(r);

   string::iterator it;
   string s_str;
   for( it=c_str.begin()+BITSIZE; it<c_str.end(); it++ ){ 
        s_str += *it;
   }
   r_str += s_str;
   c_str = r_str;
}

};



int main(int argc,char *argv[]){

Booth *BoothPtr = new Booth;

int a =2;
int b =-5;

if( BoothPtr->CheckRange(a) != 0 ){ 

cout<<"Out of Data Range in '-2^15 ~ 2^15-1' @a"<<endl;
return -1;
}
BoothPtr->SetBooth_a(a);
BoothPtr->SetBooth_a_str(a);


if( BoothPtr->CheckRange(b) != 0 ){ 
cout<<"Out of Data Range in '-2^15 ~ 2^15-1' @b"<<endl;
return -1;
}

BoothPtr->SetBooth_b(b);
BoothPtr->SetBooth_b_str(b);

BoothPtr->SetBooth_c(0);
BoothPtr->SetBooth_c_str(0,b);

string b_str = BoothPtr->GetBegin_str( BoothPtr->GetBooth_a_str() );

cout << "============================================"<<endl;
cout << "Step0.\tInitial a,b..." <<endl;  
cout << " a::\t" << BoothPtr->GetBooth_a() <<"\t"<< BoothPtr->GetBooth_a_str()<<endl;
cout << "xb::\t" << BoothPtr->GetBooth_b() <<"\t"<< BoothPtr->GetBooth_b_str()<<endl;
cout << "--------------------------------------------"<<endl;
cout << " c::\t" << BoothPtr->GetBooth_c_str() <<" "<< b_str<<endl;
cout << "============================================"<<endl;
cout << endl;

string r_str;

//cout << BoothPtr->Pro_SHIFT_c_str("1101");

for(int i=0; i<BITSIZE; i++){
      
      r_str = BoothPtr->GetEnd_str( BoothPtr->GetBooth_c_str() );  
      r_str += b_str;

          b_str = BoothPtr->GetEnd_str( BoothPtr->GetBooth_c_str() );

         if( r_str == "00" ){ cout << "Step"<< i+1 <<".\tSHIFT"<<endl; BoothPtr->Pro_SHIFT_c_str();                           }
    else if( r_str == "01" ){ cout << "Step"<< i+1 <<".\tADD_A"<<endl; BoothPtr->Pro_ADD_A_c_str();  BoothPtr->Pro_SHIFT_c_str(); } 
    else if( r_str == "10" ){ cout << "Step"<< i+1 <<".\tSUB_A"<<endl; BoothPtr->Pro_SUB_A_c_str();  BoothPtr->Pro_SHIFT_c_str(); }
    else if( r_str == "11" ){ cout << "Step"<< i+1 <<".\tSHIFT"<<endl;BoothPtr->Pro_SHIFT_c_str();                               }
    else{ cout <<" Booth Internal Error ..." << endl; return -1; } 
   
cout << " a::\t" << BoothPtr->GetBooth_a() <<"\t"<< BoothPtr->GetBooth_a_str()<<endl;
cout << " b::\t" << BoothPtr->GetBooth_b() <<"\t"<< BoothPtr->GetBooth_b_str()<<endl;
cout << " c::\t" << BoothPtr->GetBooth_c() <<"\t"<< BoothPtr->GetBooth_c_str()<<endl; 
cout << "--------------------------------------------"<<endl;
cout << " c::\t" << BoothPtr->GetBooth_c_str() <<" "<< b_str<<endl;
cout << "============================================"<<endl;
cout << endl;


}

cout << " c::\t" << "Bin::"<<BoothPtr->GetBooth_c_str() <<" "<< "Dec::"<<a*b<<endl;

return 0;
}
