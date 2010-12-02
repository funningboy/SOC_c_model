#!/usr/bin/perl

package CDFG;
use Graph;
use Data::Dumper;
use strict;

sub new {
 my $class = shift;
 my $self = { Graph_list  => Graph->new(),
              Vertex_list => {},
              Edge_list   => {},
              Begin_list  => [],
              End_list    => [],
            };             # A directed graph.
 bless $self, $class;
 return $self;
}

sub is_begin_stack_exist {
    my ($self,$begin) = (@_);
    my @arr =  @{$slef->{Begin_list}};
    for(my $i=0; $i<$#arr; $i++){
       if( $arr[$i] eq $)
   }
}

sub is_begin_stack_empty {
   my ($self,$begin) = (@_);
   if ( !@{$self->{Begin_list}} ){ return 0; }

return -1;
}

sub push_begin_stack {
   my ($self,$begin) = (@_);
   push (@{$self->{Begin}},$begin);
}

sub pop_begin_stack {
   my ($self) = (@_);
return pop (@{$self->{Begin}});
}

sub is_end_stack_empty {
   my ($self,$end) = (@_);
   if ( !@{$self->{End_list}} ){ return 0; }

return -1;
}

sub push_end_stack {
   my ($self,$end) = (@_);
   push (@{$self->{End}},$end);
}

sub pop_end_stack {
   my ($self) = (@_);
return pop (@{$self->{End}});
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

    for($i=0; $i<$#arr; $i++){
      if(  $arr[$i] eq $pre ){ last; }
   }
   delete $arr[$i];

   @{${$self}->{Vertex_list}->{pre}->{$vertex}} = @arr;
}

sub get_vertex_pre_stack {
    my ($self) = (@_);
return $self->{Vertex_list}->{pre};
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

sub pop_vertex_pre_stack {
    my ($self,$vertex) = (@_);
return pop (@{$self->{Vertex_list}->{nxt}->{$vertex}});
}

sub del_vertex_nxt_stack {
    my ($self,$vertex,$nxt) = (@_);
    my $i;
    my @arr = @{$self->{Vertex_list}->{nxt}->{$vertex}}; 

    for($i=0; $i<$#arr; $i++){
      if(  $arr[$i] eq $nxt ){ last; }
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

sub updt_time_weighred_edge {
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
   print  $self->{Graph_list}."\n";
}

sub dump_vertex_pre_stack {
   my ($self,$vertex) = (@_);
   print Dumper( $self->{Vertex_list}->{pre}->{$vertex} );
}


sub find_begin_list {
   my ($self) = (@_);
 
   my $pre_stack = $self->get_vertex_pre_stack;  my %pre_hs = %{$pre_stack};
   my $nxt_stack = $self->get_vertex_nxt_stack;  my %nxt_hs = %{$nxt_stack};

   foreach my $nxt ( keys %nxt_hs ){
      if( !defined($pre_hs{$nxt}) ){ $self->push_begin_stack($nxt); }
   }
}

sub find_end_list {
   my ($self) = (@_);

   my $pre_stack = $self->get_vertex_pre_stack;  my %pre_hs = %{$pre_stack};
   my $nxt_stack = $self->get_vertex_nxt_stack;  my %nxt_hs = %{$nxt_stack};

   foreach my $pre ( keys %pre_hs ){
      if( !defined($nxt_hs{$pre}) ){ $self->push_end_stack($pre); }
   }
}

sub run_ASAP {
   my ($self) = (@_);

               $self->find_begin_list();
               $self->find_end_list();
  my $bakup = $self->get_vertex_nxt_stack();
 
  my ($src,$nxt,$tm_svex,$tm_dvex,$tm_edg);
    
        while( $self->is_begin_stack_empty()!=0 ){
               $src = $self->pop_begin_stack();

            while( $self->is_vertex_nxt_stack_empty($src)!=0 ){
                   $nxt     = $self->pop_vertex_nxt_stack($src);
                   $tm_svex = $self->get_time_weighted_vertex($src);
                   $tm_edg  = $self->get_time_weighted_edge($src,$nxt);
                   $tm_dvex = $self->get_time_weighted_vertex($nxt);
                  
                   $tm_dvex = (($tm_svex + $tm_edg) > $tm_dvex )? ($tm_svex + $tm_edg) : $tm_dvex;
                             $self->updt_time_weighted_vertex($nxt,$tm_dvex);
 
                   $self->push_begin_list($nxt);
            }

        }
}

sub run_ALAP {
   my ($self) = (@_);

   $self->find_end_list();
}

1;
