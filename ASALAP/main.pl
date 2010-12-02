
#!/usr/bin/perl

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
   $tt->set_time_weighted_edge('#::0','*::3',0);
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

   $tt->run_ASAP();
#   $tt->dump_ASAP(); 
#   $tt->dump_graph();
#   $tt->dump_vertex_pre_stack('c');
#   $tt->del_time_weighted_vertex('c');
#   print $tt->get_time_weighted_vertex('c');

