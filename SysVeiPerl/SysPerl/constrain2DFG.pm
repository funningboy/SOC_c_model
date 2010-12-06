#!/usr/bin/perl
 
package SysPerl::constrain2DFG;
use SysPerl::DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self = {  
              DFG            => SysPerl::DFG->new(),
              time_weighted  => {},
              power_weighted => {},
             };

  bless $self, $class;
}

sub error {
    my ($self,$st) = (@_);
    die print $st;
}

sub set_deep_DFG {
   my ($self,$DFG) = (@_);
   $self->{DFG} = $DFG;
   $self->{DFG}->dump_graph_ascii();
   
}

sub set_time_weighted_vertices {
   my ($self,$time_hs) = (@_);
       $self->{time_weighted} = $time_hs;
   print Dumper($self->{time_weighted});
}

sub get_time_weighted_vertex {
   my ($self,$vertex) = (@_);
      if( !defined($self->{time_weighted}->{$vertex}) ){ return 0; }
      return $self->{time_weighted}->{$vertex};     
}

sub run_constrain {
  my ($self) = (@_);

  my $all_vetx = $self->{DFG}->get_all_vertices();

   foreach my $name (@{$all_vetx}){ 
    my ($vertex,$id,$lvl) = split("::",$name);
   
    my $weight = $self->get_time_weighted_vertex($vertex);
    my $name = $vertex."::".$id."::".$lvl;
       $self->{DFG}->updt_time_weighted_vertex($name,$weight);
   }

  $self->{DFG}->get_time_weighted_vertices($all_vetx);
}

sub get_deep_copy_DFG {
  my ($self) = (@_);
return $self->{DFG};
}

1;
 
