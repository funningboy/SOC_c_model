#include <systemc.h>
#include <iostream>

enum{
_st_idle_,
_st_done_,
_st_cycle_1,
_st_cycle_2,
_st_cycle_3,
};

SC_MODULE(SAMPLE){

sc_in<bool> HCLK;
sc_in<bool> HRESTn;
sc_in<bool> HEN;
sc_out<bool> HDON;

sc_in<sc_uint<32> > w;
sc_in<sc_uint<32> > a;
sc_in<sc_uint<32> > b;
sc_in<sc_uint<32> > g;

sc_out<sc_uint<32> > e;

sc_signal<sc_uint<32> > _itmp_reg_c;
sc_signal<sc_uint<32> > _itmp_reg_1;
sc_signal<sc_uint<32> > _itmp_reg_d;
sc_signal<sc_uint<32> > _itmp_reg_0;

sc_signal<sc_uint<32> > _itmp_cur_st;
sc_signal<sc_uint<32> > _itmp_nxt_st;

SC_CTOR(SAMPLE){

SC_METHOD(_iproc_ini_st);
dont_initialize();
sensitive << HCLK.pos();
sensitive << HRESTn;

SC_METHOD(_iproc_run_st);
dont_initialize();
sensitive << _itmp_cur_st;
sensitive << HEN;

SC_METHOD(_iproc_done_st);
dont_initialize();
sensitive << _itmp_cur_st;

SC_METHOD(_iproc_pe_st_1);
dont_initialize();
sensitive << _itmp_cur_st;

SC_METHOD(_iproc_pe_st_2);
dont_initialize();
sensitive << _itmp_cur_st;

SC_METHOD(_iproc_pe_st_3);
dont_initialize();
sensitive << _itmp_cur_st;

};

void _iproc_ini_st();
void _iproc_run_st();
void _iproc_done_st();
void _iproc_pe_st_1();
void _iproc_pe_st_2();
void _iproc_pe_st_3();
};
