
#include "t_sample_2.h"

void t_SAMPLE::PRO_INI_ST(){

     t_HRESTn.write(true);
     t_H_en.write(false);
     wait();    
     t_HRESTn.write(false);
     wait();
     wait();  
     t_HRESTn.write(true);
     wait();  
     t_H_en.write(true);
     t_H_a.write(10);
     t_H_b.write(10);
     t_H_w.write(10);
     t_H_g.write(10);
  
}

void t_SAMPLE::PRO_RUN_DONE(){
     if( t_H_done.read() == true ){
         cout<<t_H_e.read()<<endl;
    }
}
