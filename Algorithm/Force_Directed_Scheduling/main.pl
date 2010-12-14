#!/usr/bin/perl

#======================================
# author : sean chen
# mail : funningboy@gmail.com
# publish : 2010/12/14
#
# how to use it?
# perl main.pl
# dot -Tpng cc.dot > cc.png
#
# require : graphviz http://www.graphviz.org/
#======================================
use Data::Dumper;
use SysPerl::syntax2DFG;
use SysPerl::constrain2DFG;
use SysPerl::schedule2DFG;
use SysPerl::arch2DFG;
use strict;

#===================================
# @step 1 : gen simple DFG graph
# return  : DFG graph
#         : vertex_pre_stack 
#         : vertex_nxt_stack
#===================================
# 
#  c = (a+b)>>1;
#  d = w*(a-b);
#  e = d-c-g*c;

my $tt = ['c','=','(','a','+','b',')','>>','1',';'];
my $cc = ['d','=','w','*','(','a','-','b',')',';'];
my $gg = ['e','=','d','-','c','-','g','*','c',';'];
#my $gg = ['e', '=', 'e', '+', '1', ';'];

my $syn  = SysPerl::syntax2DFG->new();
   $syn->read_text($tt);
   $syn->run_text();
   $syn->free();

   $syn->read_text($cc);
   $syn->run_text();
   $syn->free();

   $syn->read_text($gg);
   $syn->run_text();
   $syn->free();

#   my $graph = $syn->get_deep_copy_graph();  
 
# remove tmp_reg && feedback assign 
# ex : d=c;
#      e=d+1;
    $syn->run_updt_DFG();

# get all graph && DFG flow 2 schedule 
my $DFG = $syn->get_deep_copy_DFG();

# dump graph as dot file    
  $syn->dump_DFG_graphviz_file('syn2DFG.dot');
  $syn->free();

#=====================================
# @step2. : 
# flow 1  : insert time weighted constrain 2 simple DFG graph and
#           gen Cstep graph(cycle step grpah)
# flow 2  : insert average power weighted constrain 2 Cstep and gen the force vale 
#           @ force directed scheduling
# return :  cycle graph
#=====================================
# set unit time wait delay
my $constrain_time_weighted_vertices = { 
     '+'  => 1,   # add delay 1 unit s
     '-'  => 1,   # sub delay 1 unit s
     '*'  => 5,   # mul
     '/'  => 8,   # div
     '%'  => 8,   # rem
     '>>' => 1,   # rsht
     '<<' => 1,   # lsht
};

#set unit average power consumed
my $constrain_power_weighted_vertices = {
     '+'  => 4,
     '-'  => 4,
     '*'  => 8,
     '/'  => 10,
     '%'  => 10,
     '>>' => 1,
     '<<' => 1,
};

my $con = SysPerl::constrain2DFG->new();
   $con->set_deep_DFG($DFG);

   $con->set_constrain_time_weighted($constrain_time_weighted_vertices);
   $con->set_constrain_power_weighted($constrain_power_weighted_vertices);

   $con->run_constrain_time_weighted_DFG();
   $con->run_constrain_NewDFG();
 
#   $con->dump_ALUDFG_graphviz_file('alu.dot');
   $con->dump_NewDFG_graphviz_file('con.dot');

#=============================
# schedule && cluster
#=============================
my $sch = SysPerl::schedule2DFG->new();
   $sch->set_deep_cons2DFG($con);

   $sch->run_forece_directed_scheduling();
   $sch->report();

# not yet finish
#=============================
# explore hardware
#=============================  
#my $arc = SysPerl::arch2DFG->new();
#   $arc->set_deep_sched2arch($sch);
#   $arc->run_REG_cluster();
#   $arc->run_ALU_cluster();
#   $arc->run_expc();

