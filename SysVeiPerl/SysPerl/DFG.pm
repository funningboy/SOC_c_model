#!/usr/bin/perl

package SysPerl::DFG;
use Graph;
use Graph::Easy;
use Data::Dumper;
use strict;

sub new {
 my $class = shift;
 my $self = { Graph_list => Graph->new(),
              BakUpGraph => Graph->new(),
              Report_list => {},
              Vertex_list => {},
              BakVertex_list=> {},
              Edge_list => {},
              Begin_list => [],
              BakBegin_list => [],
              End_list => [],
              BakEnd_list => [],
              ASAP_list => {},
              Max_list => [],
              ALAP_list => {},
            }; # A directed graph.
 bless $self, $class;
 return $self;
}

sub get_all_edges {
   my ($self) = (@_);
   my @arr = $self->{Graph_list}->edges();
return \@arr;
}

sub get_all_vertices {
   my ($self) = (@_);
   my @arr = $self->{Graph_list}->vertices();
return \@arr;
}

sub get_time_weighted_vertices{
    my ($self,$arr) = (@_);
    my @arr = @{$arr};

    for(my $i=0; $i<=$#arr; $i++){
     my $tm = $self->get_time_weighted_vertex($arr[$i]);
     print $arr[$i].",".$tm."\n";
   }
}

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

sub del_vertex_pre_stack {
    my ($self,$vertex,$pre) = (@_);
    my $i;
    my @arr = @{$self->{Vertex_list}->{pre}->{$vertex}};

    for($i=0; $i<=$#arr; $i++){
      if( $arr[$i] eq $pre ){ last; }
   }
   delete $arr[$i];

   @{${$self}->{Vertex_list}->{pre}->{$vertex}} = @arr;
}

sub get_vertex_pre_stack {
    my ($self) = (@_);
return $self->{Vertex_list}->{pre};
}

sub set_vertex_nxt_stack2bakup {
   my ($self) = (@_);
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

sub set_vertex_pre_stack2bakup {
   my ($self) = (@_);
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

sub del_vertex_nxt_stack {
    my ($self,$vertex,$nxt) = (@_);
    my $i;
    my @arr = @{$self->{Vertex_list}->{nxt}->{$vertex}};

    for($i=0; $i<=$#arr; $i++){
      if( $arr[$i] eq $nxt ){ last; }
   }
   delete $arr[$i];

   @{${$self}->{Vertex_list}->{nxt}->{$vertex}} = @arr;
}

sub get_vertex_nxt_stack {
    my ($self) = (@_);
return $self->{Vertex_list}->{nxt};
}


sub set_time_weighted_edge {
    my ($self,$src,$dst,$weight) = (@_);
    $self->{Graph_list}->add_edge($src,$dst);
    $self->{Graph_list}->add_weighted_edge($src,$dst,$weight);
    $self->push_vertex_pre_stack($dst,$src);
    $self->push_vertex_nxt_stack($src,$dst);
}

sub del_time_weighted_edge {
    my ($self,$src,$dst) = (@_);
    $self->{Graph_list}->delete_edge($src,$dst);
    $self->{Graph_list}->delete_edge_weight($src,$dst);
    $self->del_vertex_pre_stack($dst,$src);
    $self->del_vertex_nxt_stack($src,$dst);
}

sub updt_time_weighted_edge {
    my ($self,$src,$dst) = (@_);
    $self->{Graph_list}->set_edge_weight($src,$dst);
}

sub get_time_weighted_edge {
    my ($self,$src,$dst) = (@_);
    return $self->{Graph_list}->get_edge_weight($src,$dst);
}

sub set_time_weighted_vertex {
    my ($self,$vertex,$weight) = (@_);
    $self->{Graph_list}->add_vertex($vertex);
    $self->{Graph_list}->add_weighted_vertex($vertex,$weight);
}

sub del_time_weighted_vertex {
    my ($self,$vertex) = (@_);
    $self->{Graph_list}->delete_vertex($vertex);
    $self->{Graph_list}->delete_vertex_weight($vertex);
}

sub updt_time_weighted_vertex {
    my ($self,$vertex,$weight) = (@_);
    $self->{Graph_list}->set_vertex_weight($vertex,$weight);
}

sub get_time_weighted_vertex {
    my ($self,$vertex) = (@_);
    return $self->{Graph_list}->get_vertex_weight($vertex);
}

sub dump_graph {
   my ($self) = (@_);
   print $self->{Graph_list}."\n";
}

sub dump_vertex_pre_stack {
   my ($self,$vertex) = (@_);
   print Dumper( $self->{Vertex_list}->{pre}->{$vertex} );
}

sub get_deep_copy_DFG {
   my ($self) = (@_);
return $self;
}

sub get_deep_copy_graph {
   my ($self) = (@_);
return $self->{Graph_list}->deep_copy_graph();
}

sub get_directed_copy_graph {
  my ($self) = (@_);
return $self->{Graph_list}->directed_copy_graph();
}

sub dump_graph_ascii {
   my ($self) = (@_);

   my $all_vet = $self->get_all_vertices();
   my $all_edg = $self->get_all_edges();

   my $tt = Graph::Easy->new();
   foreach (@{$all_vet}){
      $tt->add_node($_);
   }

   foreach (@{$all_edg}){
     my @arr = @{$_};
     $tt->add_edge($arr[0],$arr[1]);
  }

  print $tt->as_ascii();
}

sub dump_graphviz_file {
   my ($self,$path) = (@_);

   my $all_vet = $self->get_all_vertices();
   my $all_edg = $self->get_all_edges();

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

sub free {
   my ($self) = (@_);
# $self->{Begin_list} = [];
# $self->{End_list} = [];
}

1;
