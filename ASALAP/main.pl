
#!/usr/bin/perl
#======================================
# author : sean chen
# mail : funningboy@gmail.com
# publish : 2010/12/03
#
# how to use it?
# perl main.pl
# dot -Tpng cc.dot > cc.png
#
# require : graphviz http://www.graphviz.org/
#======================================


use CDFG;
use strict;
use Data::Dumper;

my $tt = CDFG->new();
   $tt->set_time_weighted_vertex('#::0',0);

   $tt->set_time_weighted_vertex('*::0',0);
   $tt->set_time_weighted_vertex('*::1',0);
   $tt->set_time_weighted_vertex('*::2',0);
   $tt->set_time_weighted_vertex('*::3',0);
   $tt->set_time_weighted_vertex('+::0',0);

   $tt->set_time_weighted_vertex('*::4',0);
   $tt->set_time_weighted_vertex('*::5',0);
   $tt->set_time_weighted_vertex('+::1',0);
   $tt->set_time_weighted_vertex('<::0',0);

   $tt->set_time_weighted_vertex('-::0',0);

   $tt->set_time_weighted_vertex('-::1',0);

   $tt->set_time_weighted_vertex('#::1',0);

   $tt->set_time_weighted_edge('#::0','*::0',0);
   $tt->set_time_weighted_edge('#::0','*::1',0);
   $tt->set_time_weighted_edge('#::0','*::2',0);
   $tt->set_time_weighted_edge('#::0','*::3',2);
   $tt->set_time_weighted_edge('#::0','+::0',0);

   $tt->set_time_weighted_edge('*::0','*::4',2);
   $tt->set_time_weighted_edge('*::1','*::4',2);
   $tt->set_time_weighted_edge('*::2','*::5',2);
   $tt->set_time_weighted_edge('*::3','+::1',2);
   $tt->set_time_weighted_edge('+::0','<::0',1);
 
   $tt->set_time_weighted_edge('*::4','-::0',2);

   $tt->set_time_weighted_edge('-::0','-::1',1);
   $tt->set_time_weighted_edge('*::5','-::1',2);

   $tt->set_time_weighted_edge('-::1','#::1',1);
   $tt->set_time_weighted_edge('+::1','#::1',1);
   $tt->set_time_weighted_edge('<::0','#::1',1);

   # initial set
   $tt->run_time_weighted_ini();

   # ASAP flow
   $tt->run_time_weighted_test();
   $tt->run_time_weighted_ASAP();
   $tt->rst_time_weighted_ASAP();
   # dump or not
   $tt->dump_ASAP_graphviz_file('ASAP.dot');

   # ALAP flow
   $tt->run_time_weighted_test();
   $tt->run_time_weighted_max_path();
   $tt->run_time_weighted_ALAP();
   $tt->rst_time_weighted_ALAP();
   # dump or not
   $tt->dump_ALAP_grpahviz_file('ALAP.dot');
  
   # report
my $rpt = $tt->run_report();
   print Dumper($rpt);

 
