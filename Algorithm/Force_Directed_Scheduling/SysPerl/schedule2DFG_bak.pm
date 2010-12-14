#!/usr/bin/perl

package SysPerl::schedule2DFG;
use Graph;
use Graph::Easy;
use Data::Dumper;
use strict;

sub new {
 my $class = shift;
 my $self = { 
              DFG           => SysPerl::DFG->new(),
              Graph_list    => Graph->new(),
              BakUpGraph    => Graph->new(),
              Vertex_list   => {},
              BakVertex_list=> {},
              Edge_list     => {},
              BakEdge_list  => {},
              Begin_list    => [],
              BakBegin_list => [],
              End_list      => [],
              BakEnd_list   => [],
              ASAP_list     => {},
              Max_list      => [],
              ALAP_list     => {},
              Report_list   => {},
           }; # A directed graph.
 bless $self, $class;
 return $self;
}

#====================
# max_stack
#====================
sub is_max_stack_empty {
    my ($self) = (@_);
    if( !@{$self->{Max_list}} ){ return 0; }

return -1;
}

sub push_max_stack {
    my ($self,$vertex,$a) = (@_);
        push (@{$self->{Max_list}}, {$vertex,$a});
}

sub pop_max_stack {
   my ($self,$vertex) = (@_);
   return pop (@{$self->{Max_list}});
}

#=================
# begin stack
#=================
sub is_begin_stack_empty {
   my ($self,$begin) = (@_);
   if ( !@{$self->{Begin_list}} ){ return 0; }

return -1;
}

sub is_begin_stack_exist {
    my ($self,$begin) = (@_);
    my @arr = @{$self->{Begin_list}};
    for(my $i=0; $i<=$#arr; $i++){
       if( $arr[$i] eq $begin){ return 0; last; }
   }

return -1;
}

sub push_begin_stack {
   my ($self,$begin) = (@_);
   push (@{$self->{Begin_list}},$begin);
}

sub pop_begin_stack {
   my ($self) = (@_);
return pop (@{$self->{Begin_list}});
}

sub set_begin_list2bakup {
   my ($self) = (@_);
      if( !@{$self->{BakBegin_list}} ){
           @{$self->{BakBegin_list}} = @{$self->{Begin_list}};
      }
}

sub get_begin_bakup2list {
   my ($self) = (@_);
      @{$self->{Begin_list}} = @{$self->{BakBegin_list}};
}

sub find_begin_list {
   my ($self) = (@_);
 
   my $pre_stack = $self->get_vertex_pre_stack(); my %pre_hs = %{$pre_stack};
   my $nxt_stack = $self->get_vertex_nxt_stack(); my %nxt_hs = %{$nxt_stack};

   foreach my $nxt ( keys %nxt_hs ){
      if( !defined($pre_hs{$nxt}) ){ $self->push_begin_stack($nxt); }
   }
}

sub dump_begin_stack {
   my ($self) = (@_);
   print Dumper($self->{Begin_list});
}

#====================
# end_stack
#====================
sub is_end_stack_exist {
    my ($self,$end) = (@_);
    my @arr = @{$self->{End_list}};
    for(my $i=0; $i<=$#arr; $i++){
       if( $arr[$i] eq $end){ return 0; last; }
   }

return -1;
}

sub is_end_stack_empty {
   my ($self,$end) = (@_);
   if ( !@{$self->{End_list}} ){ return 0; }

return -1;
}

sub push_end_stack {
   my ($self,$end) = (@_);
   push (@{$self->{End_list}},$end);
}

sub pop_end_stack {
   my ($self) = (@_);
return pop (@{$self->{End_list}});
}

sub set_end_list2bakup {
   my ($self) = (@_);
      if( !@{$self->{BakEnd_list}} ){
           @{$self->{BakEnd_list}} = @{$self->{End_list}};
      }
}

sub get_end_bakup2list {
   my ($self) = (@_);
      @{$self->{End_list}} = @{$self->{BakEnd_list}};
}

sub find_end_list {
   my ($self) = (@_);

   my $pre_stack = $self->get_vertex_pre_stack; my %pre_hs = %{$pre_stack};
   my $nxt_stack = $self->get_vertex_nxt_stack; my %nxt_hs = %{$nxt_stack};

   foreach my $pre ( keys %pre_hs ){
      if( !defined($nxt_hs{$pre}) ){ $self->push_end_stack($pre); }
   }
}

sub dump_end_stack {
   my ($self) = (@_);
  print Dumper($self->{End_list});
}

# the max path result for ASAP 
sub copy_time_weighted_end_stack {
    my ($self) = (@_);

    while( $self->is_end_stack_empty()!=0 ){
        my $vex = $self->pop_end_stack();
        my $tm = $self->get_time_weighted_vertex($vex);
              $self->push_max_stack($vex,$tm);
   }
}

#==================
# vertex_pre_stack
#==================
sub is_vertex_pre_stack_empty {
    my ($self,$vertex) = (@_);
    if ( !@{$self->{Vertex_list}->{pre}->{$vertex}} ){ return 0; }

return -1;
}

sub push_vertex_pre_stack {
    my ($self,$vertex,$pre) = (@_);
        push (@{$self->{Vertex_list}->{pre}->{$vertex}},$pre);
}

sub pop_vertex_pre_stack {
    my ($self,$vertex) = (@_);
return pop (@{$self->{Vertex_list}->{pre}->{$vertex}});
}
sub get_vertex_pre_stack {
    my ($self) = (@_);
return $self->{Vertex_list}->{pre};
}

sub set_vertex_pre_stack2bakup {
   my ($self) = (@_);
      if ( !$self->{Vertex_list}->{pre} ){
            $self->{Vertex_list}->{pre} = $self->{DFG}->get_vertex_pre_stack();
      }

      if( !$self->{BakVertex_list}->{pre} ){
           foreach my $ky (keys %{$self->{Vertex_list}->{pre}}){
              @{$self->{BakVertex_list}->{pre}->{$ky}} = @{$self->{Vertex_list}->{pre}->{$ky}};
           }
      }
}

sub get_vertex_pre_bakup2stack {
   my ($self) = (@_);
      foreach my $ky (keys %{$self->{BakVertex_list}->{pre}}){
              @{$self->{Vertex_list}->{pre}->{$ky}} = @{$self->{BakVertex_list}->{pre}->{$ky}};
           }
}

#===================
# vertex_nxt_stack
#===================
sub is_vertex_nxt_stack_empty {
    my ($self,$vertex) = (@_);
    if ( !@{$self->{Vertex_list}->{nxt}->{$vertex}} ){ return 0; }

return -1;
}

sub push_vertex_nxt_stack {
    my ($self,$vertex,$nxt) = (@_);
        push (@{$self->{Vertex_list}->{nxt}->{$vertex}},$nxt);
}

sub pop_vertex_nxt_stack {
    my ($self,$vertex) = (@_);
return pop (@{$self->{Vertex_list}->{nxt}->{$vertex}});
}

sub get_vertex_nxt_stack {
    my ($self) = (@_);
return $self->{Vertex_list}->{nxt};
}

sub set_vertex_nxt_stack2bakup {
   my ($self) = (@_);
      if( !$self->{Vertex_list}->{nxt} ){
           $self->{Vertex_list}->{nxt} = $self->{DFG}->get_vertex_nxt_stack();
      }

      if( !$self->{BakVertex_list}->{nxt} ){
           foreach my $ky (keys %{$self->{Vertex_list}->{nxt}}){
              @{$self->{BakVertex_list}->{nxt}->{$ky}} = @{$self->{Vertex_list}->{nxt}->{$ky}};
           }
      }
}

sub get_vertex_nxt_bakup2stack {
   my ($self) = (@_);
      foreach my $ky (keys %{$self->{BakVertex_list}->{nxt}}){
              @{$self->{Vertex_list}->{nxt}->{$ky}} = @{$self->{BakVertex_list}->{nxt}->{$ky}};
           }
}

#=================
# ASAP 
#=================
sub get_ASAP_time_weighted_vertices{
    my ($self,$arr) = (@_);
    my @arr = @{$arr};
    
    for(my $i=0; $i<=$#arr; $i++){
      my $tm = $self->get_time_weighted_vertex($arr[$i]);
               $self->set_time_weighted_ASAP_list($arr[$i],$tm);
   }
}

sub set_time_weighted_ASAP_list {
   my ($self,$vertex,$weight) = (@_);
       $self->{ASAP_list}->{$vertex} = $weight;
}

sub dump_time_weighted_ASAP_list {
   my ($self) = (@_);
    print Dumper($self->{ASAP_list});
}

sub dump_ASAP_graphviz_file {
   my ($self,$path) = (@_);

   my $all_vet = $self->get_all_vertices();
   my $all_tm = $self->get_ASAP_time_weighted_vertices($all_vet);
   my $all_edg = $self->get_all_edges();
                 $self->dump_time_weighted_ASAP_list();

   my $tt = Graph::Easy->new();
   foreach (@{$all_vet}){
      $tt->add_node($_);
   }

   foreach (@{$all_edg}){
     my @arr = @{$_};
     $tt->add_edge($arr[0],$arr[1]);
  }
      open (optr,">$path") || die "open $path error\n";
      print optr $tt->as_graphviz_file();

close(optr);
}

sub rst_time_weighted_ASAP {
   my ($self) = (@_);
   my $all_vet = $self->get_all_vertices();
   my $all_tm = $self->get_ASAP_time_weighted_vertices($all_vet);
   my $all_edg = $self->get_all_edges();
}

sub run_time_weighted_ASAP {
   my ($self) = (@_);

   my ($src,$nxt,$tm_svex,$tm_dvex,$tm_edg);
    
        while( $self->is_begin_stack_empty()!=0 ){
               $src = $self->pop_begin_stack();

            while( $self->is_vertex_nxt_stack_empty($src)!=0 ){
                   $nxt = $self->pop_vertex_nxt_stack($src);
                   $tm_svex = $self->get_time_weighted_vertex($src);
                   $tm_edg = $self->get_time_weighted_edge($src,$nxt);
                   $tm_dvex = $self->get_time_weighted_vertex($nxt);
    
                   $tm_dvex = (($tm_svex + $tm_edg) > $tm_dvex )? ($tm_svex + $tm_edg) : $tm_dvex;
                              $self->updt_time_weighted_vertex($nxt,$tm_dvex);

                   if( $self->is_end_stack_exist($nxt)!=0 ){ $self->push_begin_stack($nxt); }
            }
           
           $self->run_time_weighted_ASAP();
        }
}

#================
# ALAP
#================
sub get_ALAP_time_weighted_vertices{
    my ($self,$arr) = (@_);
    my @arr = @{$arr};
    
    for(my $i=0; $i<=$#arr; $i++){
      my $tm = $self->get_time_weighted_vertex($arr[$i]);
               $self->set_time_weighted_ALAP_list($arr[$i],$tm);
   }
}

sub set_time_weighted_ALAP_list {
   my ($self,$vertex,$weight) = (@_);
       $self->{ALAP_list}->{$vertex} = $weight;
}

sub dump_time_weighted_ALAP_list {
   my ($self) = (@_);
    print Dumper($self->{ALAP_list});
}

sub rst_time_weighted_ALAP {
   my ($self) = (@_);
   my $all_vet = $self->get_all_vertices();
   my $all_tm = $self->get_ALAP_time_weighted_vertices($all_vet);
   my $all_edg = $self->get_all_edges();
}

sub dump_ALAP_grpahviz_file {
   my ($self,$path) = (@_);

   my $all_vet = $self->get_all_vertices();
   my $all_tm = $self->get_ALAP_time_weighted_vertices($all_vet);
   my $all_edg = $self->get_all_edges();
                 $self->dump_time_weighted_ALAP_list();

   my $tt = Graph::Easy->new();
   foreach (@{$all_vet}){
      $tt->add_node($_);
   }

   foreach (@{$all_edg}){
     my @arr = @{$_};
     $tt->add_edge($arr[0],$arr[1]);
  }
      open (optr,">$path") || die "open $path error\n";
      print optr $tt->as_graphviz_file();

close(optr);
}

sub run_time_weighted_ALAP {
   my ($self) = (@_);

   my ($src,$nxt,$tm_svex,$tm_dvex,$tm_edg);
    
        while( $self->is_end_stack_empty()!=0 ){
               $src = $self->pop_end_stack();

            while( $self->is_vertex_pre_stack_empty($src)!=0 ){
                   $nxt = $self->pop_vertex_pre_stack($src);
                   $tm_svex = $self->get_time_weighted_vertex($src);
                   $tm_edg = $self->get_time_weighted_edge($nxt,$src);
                   $tm_dvex = $self->get_time_weighted_vertex($nxt);
              
                   $tm_dvex = (($tm_svex - $tm_edg) > $tm_dvex )? ($tm_svex - $tm_edg) : $tm_dvex;
                              $self->updt_time_weighted_vertex($nxt,$tm_dvex);
            
                   if( $self->is_begin_stack_exist($nxt)!=0 ){ $self->push_end_stack($nxt); }
            }
          
           $self->run_time_weighted_ALAP();
        }
}

#==============================
# her from DFG.pm 
#==============================
sub get_all_vertices {
   my ($self) = (@_);
   my @arr = $self->{Graph_list}->vertices();
return \@arr;
}

sub get_all_edges {
   my ($self) = (@_);
   my @arr = $self->{Graph_list}->edges();
return \@arr;
}

sub get_time_weighted_vertex {
    my ($self,$vertex) = (@_);
    return $self->{Graph_list}->get_vertex_weight($vertex);
}

sub get_time_weighted_edge {
    my ($self,$src,$dst) = (@_);
    return $self->{Graph_list}->get_edge_weight($src,$dst);
}

sub updt_time_weighted_vertex {
    my ($self,$vertex,$weight) = (@_);
    $self->{Graph_list}->set_vertex_weight($vertex,$weight);
}

sub updt_time_weighted_edge {
    my ($self,$src,$dst) = (@_);
    $self->{Graph_list}->set_edge_weight($src,$dst);
}

sub set_time_weighted_vertex {
    my ($self,$vertex,$weight) = (@_);
    $self->{Graph_list}->add_vertex($vertex);
    $self->{Graph_list}->add_weighted_vertex($vertex,$weight);
}

#============================
# schedule2DFG
#============================
sub set_deep_copy_DFG {
   my ($self,$DFG) = (@_);
       $self->{DFG} = $DFG;
}

sub set_Graph2BakUpGraph {
   my ($self) = (@_);
      if( !$self->{BakUpGraph} ){ $self->{BakUpGraph} = $self->{DFG}->get_deep_copy_graph(); }
}

sub get_BakUpGraph2Graph {
   my ($self) = (@_);
       $self->{Graph_list} = $self->{BakUpGraph}->deep_copy_graph();
}

sub updt_time_weighted_Graph {
    my ($self) = (@_);
   
    while( $self->is_max_stack_empty()!=0 ){
       my $hs = $self->pop_max_stack(); my %hs = %{$hs};
          foreach my $vex ( keys %hs) {
               my $tm = $hs{$vex};
               $self->set_time_weighted_vertex($vex,$tm);
          }
   }
}

sub run_time_weighted_max_path {
    my ($self) = (@_);
        $self->run_time_weighted_ASAP();
        $self->copy_time_weighted_end_stack();
        $self->run_time_weighted_test();
        $self->updt_time_weighted_Graph();
#
# $self->dump_begin_stack();
# $self->dump_end_stack();
}

sub run_time_weighted_ini {
   my ($self) = (@_);

      $self->set_Graph2BakUpGraph();

      $self->set_vertex_nxt_stack2bakup();
      $self->set_vertex_pre_stack2bakup();

      $self->find_begin_list();
      $self->find_end_list();

      $self->set_begin_list2bakup();
      $self->set_end_list2bakup();
}

sub run_time_weighted_test {
   my ($self) = (@_);

     $self->get_BakUpGraph2Graph();

     $self->get_vertex_nxt_bakup2stack();
     $self->get_vertex_pre_bakup2stack();

     $self->get_begin_bakup2list();
     $self->get_end_bakup2list();
   
}

sub run_report {
    my ($self) = (@_);

    my %ALAP_hs = %{$self->{ALAP_list}};
    my %ASAP_hs = %{$self->{ASAP_list}};

    foreach my $ky ( keys %ASAP_hs ){

           if( $ALAP_hs{$ky} == $ASAP_hs{$ky} ){
               $self->{Report_list}->{critical}->{$ky} = { BG_TM => $ASAP_hs{$ky},
                                                            ED_TM => $ALAP_hs{$ky},
                                                           };
        }else{
               $self->{Report_list}->{tolerance}->{$ky} = { BG_TM => $ASAP_hs{$ky},
                                                            ED_TM => $ALAP_hs{$ky},
                                                           };
        }
   }

return $self->{Report_list};
}

1;
