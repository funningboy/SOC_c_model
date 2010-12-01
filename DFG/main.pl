#! /usr/bin/perl

#====================================== 
# author  : sean chen 
# mail    : funningboy@gmail.com
# publish : 2010/12/01
#
# how to use it?
# perl main.pl > cc.dot
# dot  -Tpng cc.dot > cc.png 
#
# require : graphviz http://www.graphviz.org/
#======================================

use Data::Dumper; 
use DFG2Graph;
use strict;

my $dfg = DFG2Graph->new('c=(a+b)*d;');
   $dfg->run_text();

#   $dfg->dump_graph();
   $dfg->dump_graphviz();

