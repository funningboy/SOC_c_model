#include"SAMPLE.h"

void SAMPLE::_iproc_ini_st(){
_itmp_cur_st = ( HRESTn.read()== false )? _st_idle_ : _itmp_nxt_st.read();
}

void SAMPLE::_iproc_run_st(){
switch( _itmp_cur_st.read() ){
case _st_idle_ : _itmp_nxt_st = ( HEN.read() == true ) ? _st_cycle_1 : _st_idle_; break;
case _st_done_ : _itmp_nxt_st = _st_idle_; break;
case _st_cycle_1 : _itmp_nxt_st = _st_cycle_2; break;
case _st_cycle_2 : _itmp_nxt_st = _st_cycle_3; break;
case _st_cycle_3 : _itmp_nxt_st = _st_done_; break;
}
}

void SAMPLE::_iproc_pe_st_1(){
//@cycle 1
//@power 9
if( _itmp_cur_st.read() == _st_cycle_1 ){
_itmp_reg_c=a.read()+b.read()>>1;
_itmp_reg_1a.read()-b.read();
}
}

void SAMPLE::_iproc_pe_st_2(){
//@cycle 2
//@power 16
if( _itmp_cur_st.read() == _st_cycle_2 ){
_itmp_reg_d=w.read()*_itmp_reg_1.read();
_itmp_reg_0g.read()*_itmp_reg_c.read();
}
}

void SAMPLE::_iproc_pe_st_3(){
//@cycle 3
//@power 8
if( _itmp_cur_st.read() == _st_cycle_3 ){
e=_itmp_reg_d.read()-_itmp_reg_c.read()-_itmp_reg_0.read();
}
}

void SAMPLE::_iproc_done_st(){
HDON=false;
if( _itmp_cur_st.read() == 1 ){
HDON=true;
}
}

