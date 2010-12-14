#!/usr/bin/perl

package SysPerl::DFG;
use Graph;
use Graph::Easy;
use Data::Dumper;
use strict;

sub new {
 my $class = shift;
 my $self = { Graph_list  => Graph->new(),
              Vertex_list => {},
              Edge_list   => {},
              begin_list  => [],
              end_list    => [],
              assign_list => [],
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

sub dump_time_weighted_vertices {
    my ($self) = (@_);

    my $arr = $self->get_all_vertices();
    my @arr = @{$arr};

    foreach my $vex (@arr){
     my $tm = $self->get_time_weighted_vertex($vex);
     print $vex." , ".$tm."\n";
   }
}

sub dump_time_weighted_edges {
   my ($self) = (@_);

  my $arr = $self->get_all_edges();
  my @arr = @{$arr};

  foreach my $edg (@arr){
   my $tm = $self->get_time_weighted_edge($edg->[0],$edg->[1]);
   print $edg->[0]." , ".$edg->[1].' , '.$tm."\n";
 }
}

sub is_begin_list_empty {
   my ($self) = (@_);
   if( !@{$self->{begin_list}}){ return 0; }
return -1;
}

sub push_begin_list {
   my ($self,$vertex) = (@_);
       push (@{$self->{begin_list}},$vertex);
}

sub pop_begin_list {
   my ($self) = (@_);
return pop (@{$self->{begin_list}});
}

sub set_begin_lists {
   my ($self,$arr) = (@_);
       @{$self->{begin_list}} = @{$arr};
}

sub get_begin_lists {
   my ($self) = (@_);
return $self->{begin_list};
}

sub is_begin_list_exist {
    my ($self,$begin) = (@_);
    my @arr = @{$self->{begin_list}};
    for(my $i=0; $i<=$#arr; $i++){
       if( $arr[$i] eq $begin){ return 0; last; }
   }

return -1;
}

sub is_end_list_empty {
   my ($self) = (@_);
   if( !@{$self->{end_list}}){ return 0; }
return -1;
}


sub push_end_list {
   my ($self,$vertex) = (@_);
       push (@{$self->{end_list}},$vertex);
}

sub pop_end_list {
   my ($self) = (@_);
return pop (@{$self->{end_list}});
}

sub set_end_lists {
   my ($self,$arr) = (@_);
       @{$self->{end_list}} = @{$arr};
}

sub get_end_lists {
   my ($self) = (@_);
return $self->{end_list};
}

sub is_end_list_exist {
    my ($self,$end) = (@_);
    my @arr = @{$self->{end_list}};
    for(my $i=0; $i<=$#arr; $i++){
       if( $arr[$i] eq $end){ return 0; last; }
   }

return -1;
}

sub is_assign_list_empty {
   my ($self) = (@_);
   if( !@{$self->{assign_list}} ){ return 0; }
return -1;
}

sub push_assign_list {
   my ($self,$assign) = (@_);
   push (@{$self->{assign_list}},$assign);
}

sub pop_assign_list {
   my ($self) = (@_);
return pop (@{$self->{assign_list}});
}

sub shft_assign_list {
   my ($self) = (@_);
return shift (@{$self->{assign_list}});
}

sub sort_assign_list {
   my ($self) = (@_);
 
  my @arr = @{$self->{assign_list}};
  @{$self->{assign_list}} = sort (@arr);
}

sub is_vertex_pre_stack_empty {
    my ($self,$vertex) = (@_);
    if (   !$self->{Vertex_list}->{pre}->{$vertex} ||
         !@{$self->{Vertex_list}->{pre}->{$vertex}} ){ return 0; }

return -1;
}

sub push_vertex_pre_stack {
    my ($self,$vertex,$pre,$typ) = (@_);
        push (@{$self->{Vertex_list}->{pre}->{$vertex}},[$pre,$typ]);
}

sub pop_vertex_pre_stack {
    my ($self,$vertex) = (@_);
return pop (@{$self->{Vertex_list}->{pre}->{$vertex}});
}

sub shft_vertex_pre_stack {
    my ($self,$vertex) = (@_);
return shift (@{$self->{Vertex_list}->{pre}->{$vertex}}); 
}

sub del_vertex_pre_stack {
    my ($self,$vertex,$pre) = (@_);
    my @tmp;
    my @arr;
 
   if ( $self->is_vertex_pre_stack_empty($vertex)!=0 ){
        @arr = @{$self->{Vertex_list}->{pre}->{$vertex}};
 
     for(my $i=0; $i<=$#arr; $i++){
        if( $arr[$i]->[0] eq $pre ){
            delete $self->{Vertex_list}->{pre}->{$vertex}->[$i];
        }
      }

     #remove undef
     foreach my $ky (@{$self->{Vertex_list}->{pre}->{$vertex}}){
       if($ky){ 
         push (@tmp,$ky);
      }
    }
   
   @{$self->{Vertex_list}->{pre}->{$vertex}} = @tmp;
  }
 
}

sub get_vertex_pre_stack {
    my ($self,$vertex) = (@_);
return $self->{Vertex_list}->{pre}->{$vertex};
}

sub get_vertex_pre_stacks {
   my ($self) = (@_);
return $self->{Vertex_list}->{pre};
}

sub set_vertex_pre_stacks {
   my ($self,$arr) = (@_);
      foreach my $ky (keys %{$arr}){ 
        @{$self->{Vertex_list}->{pre}->{$ky}} = @{$arr->{$ky}};
  }
}

sub is_vertex_nxt_stack_empty {
    my ($self,$vertex) = (@_);
    if (   !$self->{Vertex_list}->{nxt}->{$vertex}  ||
         !@{$self->{Vertex_list}->{nxt}->{$vertex}} ){ return 0; }

return -1;
}

sub push_vertex_nxt_stack {
    my ($self,$vertex,$nxt,$typ) = (@_);
        push (@{$self->{Vertex_list}->{nxt}->{$vertex}},[$nxt,$typ]);
}

sub pop_vertex_nxt_stack {
    my ($self,$vertex) = (@_);
return pop (@{$self->{Vertex_list}->{nxt}->{$vertex}});
}

sub shft_vertex_nxt_stack {
    my ($self,$vertex) = (@_);
return shift (@{$self->{Vertex_list}->{nxt}->{$vertex}});
}

sub del_vertex_nxt_stack {
    my ($self,$vertex,$nxt) = (@_);
    my @tmp;
    my @arr;
 
   if ( $self->is_vertex_nxt_stack_empty($vertex)!=0 ){
        @arr = @{$self->{Vertex_list}->{nxt}->{$vertex}};
 
     for(my $i=0; $i<=$#arr; $i++){
        if( $arr[$i]->[0] eq $nxt ){
            delete $self->{Vertex_list}->{nxt}->{$vertex}->[$i];
        }
      }

     foreach my $ky (@{$self->{Vertex_list}->{nxt}->{$vertex}}){
       if($ky){ 
         push (@tmp,$ky);
      }
    }
   
   @{$self->{Vertex_list}->{nxt}->{$vertex}} = @tmp;
  }
}

sub get_vertex_nxt_stack {
    my ($self,$vertex) = (@_);
return $self->{Vertex_list}->{nxt}->{$vertex};
}

sub set_vertex_nxt_stacks {
   my ($self,$arr) = (@_);
   foreach my $ky (keys %{$arr}){
      @{$self->{Vertex_list}->{nxt}->{$ky}} = @{$arr->{$ky}}; 
  }
}

sub get_vertex_nxt_stacks {
   my ($self) = (@_);
return $self->{Vertex_list}->{nxt}; 
}

sub set_time_weighted_edge {
    my ($self,$src,$dst,$weight,$typ) = (@_);
    $self->{Graph_list}->add_edge($src,$dst);
    $self->{Graph_list}->add_weighted_edge($src,$dst,$weight);
    $self->push_vertex_pre_stack($dst,$src,$typ);
    $self->push_vertex_nxt_stack($src,$dst,$typ);

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

sub clr_time_weighted_edges {
    my ($self) = (@_);

  my $arr = $self->get_all_edges();
  my @arr = @{$arr};

  foreach my $edg (@arr){
   my $tm = $self->set_time_weighted_edge($edg->[0],$edg->[1],0);
 }

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

sub clr_time_weighted_vertices { 
    my ($self) = (@_);

    my $arr = $self->get_all_vertices();
    my @arr = @{$arr};

    foreach my $vex (@arr){
     my $tm = $self->set_time_weighted_vertex($vex,0);
   }
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
