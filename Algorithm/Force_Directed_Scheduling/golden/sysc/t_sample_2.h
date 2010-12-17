#include <systemc.h>
#include <iostream>

SC_MODULE(t_SAMPLE){
        sc_in<bool>              t_HCLK;   
        sc_out<bool>             t_HRESTn;
        sc_out<bool>             t_H_en; 
        sc_out<sc_uint<32> >     t_H_a;
        sc_out<sc_uint<32> >     t_H_b;
        sc_out<sc_uint<32> >     t_H_w;
        sc_out<sc_uint<32> >     t_H_g;
        sc_in<sc_uint<32> >      t_H_e;
        sc_in<bool>              t_H_done;

SC_CTOR(t_SAMPLE){
        SC_THREAD(PRO_INI_ST);
        dont_initialize();
        sensitive << t_HCLK.pos();

        SC_METHOD(PRO_RUN_DONE);
        dont_initialize();
        sensitive << t_HCLK.pos();

     };

     void PRO_INI_ST();
     void PRO_RUN_DONE();
};
