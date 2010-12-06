#!/usr/bin/perl

use Data::Dumper;
#use SysPerl::Token;
#use SysPerl::Tag;
#use SysPerl::CDFG_If;
use SysPerl::syntax2DFG;
use SysPerl::constrain2DFG;
use sysPerl::schedule2DFG;
use strict;

#my $syspl = SysPerl::Token->new();
#   $syspl->parse_file('FSM.v');


#my $syspl = SysPerl::Tag->new();
#   $syspl->get_token_file('flow2');
 
#my $syst   = SysPerl::CDFG_If->new();
#   $syst->set_CDFG_if_condition('(a==1)||(x==1)');
#   $syst->set_CDFG_if_body();
#   $syst->set_CDFG_if_condition('b==1');
#   $syst->set_CDFG_if_body('c = a + b * 1 ;');
#   
#   $syst->set_CDFG_if_end();
#   $syst->set_CDFG_if_end();
#   $syst->dump_CDFG();

my $tt = ['c','=','a','+','b','>>','1',';'];
my $cc = ['d','=','w','*','(','a','-','b',')',';'];

#===================================
# gen simple DFG graph
#===================================
my $syn  = SysPerl::syntax2DFG->new();
   $syn->read_text($tt);
   $syn->run_text();
   $syn->free();

   $syn->read_text($cc);
   $syn->run_text();
   $syn->free();
 
#   my @vertices = $syn->get_vertices();
#   my @edges    = $syn->get_edges();
#   my $graph    = $syn->get_deep_copy_graph();   
    my $DFG      = $syn->get_deep_copy_DFG();
    
  $syn->dump_graphviz_file('syn2DFG.dot');
#  $syn->free();

my $constrain_time_weighted_vertices = { 
     '+'  => 1,
     '-'  => 1,
     '*'  => 5,
     '/'  => 8,
     '%'  => 10,
     '>>' => 0,
     '<<' => 0,
};

#=================================
# insert constrain 2 simple DFG graph
#=================================
my $con = SysPerl::constrain2DFG->new();
#   $con->set_vertices(\@vertices);
#   $con->set_edges(\@edges);
   $con->set_deep_DFG($DFG);

#   $con->set_ALU_number();
#   $con->set_support_AMBA_3x_bus();
#   $con->set_support_Cache_MESI();
 
   $con->set_time_weighted_vertices($constrain_time_weighted_vertices);
#   $con->set_time_weighted_edges();

#   $con->set_power_weighted_vertices();
#   $con->set_power_weighted_edges();
   $con->run_constrain();

#my $graph    = $con->get_deep_copy_graph();   
 my $DFG      = $con->get_deep_copy_DFG();
#    $con->free();

##================================
## run scheduling ASAP/ALAP HW bound...
##================================  
my $sch = SysPerl::schedule2DFG->new(); 
 #  $sch->run_HW_bound();
 # initial set
   $sch->set_deep_copy_DFG($DFG);
   $sch->run_time_weighted_ini();

   # ASAP flow
   $sch->run_time_weighted_test();
   $sch->run_time_weighted_ASAP();
   $sch->rst_time_weighted_ASAP();
   # dump or not
   $sch->dump_ASAP_graphviz_file('ASAP.dot');

   # ALAP flow
   $sch->run_time_weighted_test();
   $sch->run_time_weighted_max_path();
   $sch->run_time_weighted_ALAP();
   $sch->rst_time_weighted_ALAP();
   # dump or not
   $sch->dump_ALAP_grpahviz_file('ALAP.dot');
  
   # report
my $rpt = $sch->run_report();
   print Dumper($rpt);

   
