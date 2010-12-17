
#include <systemc.h>
#include "SAMPLE.h"
#include "t_sample_2.h"

int main(int argc , char *argv[]){

  sc_clock s_HCLK("S_HCLK", 10, SC_NS, 0.5, 0.0, SC_NS);

   sc_signal<bool>             s_HRESTn;
   sc_signal<bool>             s_H_en;
   sc_signal<sc_uint<32> >     s_H_a;
   sc_signal<sc_uint<32> >     s_H_b;
   sc_signal<sc_uint<32> >     s_H_w;
   sc_signal<sc_uint<32> >     s_H_g;
   sc_signal<sc_uint<32> >     s_H_e;
   sc_signal<bool>             s_H_done;

   SAMPLE SAMPLE_ptr("SAMPLE");
   SAMPLE_ptr.HCLK(s_HCLK);
   SAMPLE_ptr.HRESTn(s_HRESTn);
   SAMPLE_ptr.HEN(s_H_en);
   SAMPLE_ptr.a(s_H_a);
   SAMPLE_ptr.b(s_H_b);
   SAMPLE_ptr.w(s_H_w);
   SAMPLE_ptr.g(s_H_g);
   SAMPLE_ptr.e(s_H_e);
   SAMPLE_ptr.HDON(s_H_done);

   t_SAMPLE t_SAMPLE_ptr("t_SAMPLE");
   t_SAMPLE_ptr.t_HCLK(s_HCLK);
   t_SAMPLE_ptr.t_HRESTn(s_HRESTn);
   t_SAMPLE_ptr.t_H_en(s_H_en);
   t_SAMPLE_ptr.t_H_a(s_H_a);
   t_SAMPLE_ptr.t_H_b(s_H_b);
   t_SAMPLE_ptr.t_H_w(s_H_w);
   t_SAMPLE_ptr.t_H_g(s_H_g);
   t_SAMPLE_ptr.t_H_e(s_H_e);
   t_SAMPLE_ptr.t_H_done(s_H_done);


sc_start(100, SC_NS);

return 0;

}




