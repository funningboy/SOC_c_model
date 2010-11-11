//===============================
// sample case
//===============================
          if( in_a ==1 ){ 
              out = in+1;
          } else {
              if( in_b == 0 ){
                  out = in-1;
              } else {
                  if( in_c == 1) {
                       out = in*2;
                 }else {
                       out = in;
                 }
             }
          }

//==============================
//  encoder
//==============================

//range 0 ~ 7 
s = in_c << 2 | in_b << 1 | in_a;

switch(s){
  case  0 : out = in-1; break; // in_c(0), in_b(0), in_a(0)
  case  1 : out = in+1; break; // in_c(0), in_b(0), in_a(1)
  case  2 : out = in;   break; // in_c(0), in_b(1), in_a(0)
  case  3 : out = in+1; break; // in_c(0), in_b(1), in_a(1)
  case  4 : out = in-1; break; // in_c(1), in_b(0), in_a(0)
  case  5 : out = in+1; break; // in_c(1), in_b(0), in_a(1)
  case  6 : out = in*2; break; // in_c(1), in_b(1), in_a(0)
  case  7 : out = in+1; break; // in_c(1), in_b(1), in_a(1)
 }
