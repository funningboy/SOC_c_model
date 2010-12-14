#!/usr/bin/perl
 
package SysPerl::arch2DFG;
use SysPerl::DFG;
use SysPerl::schedule2DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self  = {
      sched2DFG => SysPerl::schedule2DFG->new(),
      cycle_list=> {},
      reg_list  => {},
      deep_reg  => 0,
      rev_key   => '_itmp_reg_',
      alu_list  => [],
  };
  bless $self, $class;
  return $self;
}


sub set_deep_sched2arch {
   my ($self,$sched) = (@_);
       $self->{sched2DFG} = $sched;
}

sub run_ALU_cluster {
   my ($self) = (@_);

   my $cstp_alu_all = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{ALU};
   my $brk =0;

#   foreach my $ky (keys %{$cstp_alu_all}){
#           my $cyc = $cstp_alu_all->{$ky}->{begin};
#
#           while($self->{sched2DFG}->{cons2DFG}->{NewDFG}->is_vertex_pre_stack_empty($ky)!=0){
#              my $pre_vex = $self->{sched2DFG}->{cons2DFG}->{NewDFG}->pop_vertex_pre_stack($ky);               
#              if($pre_vex->[0]=~/[\+\-\*\/\%\>\>\<\<]/){
#                 $brk =1;
#                 last;
#              }
#            }
#          
#           if($brk==0){
#             push(@t);
#             
#             while($self->{sched2DFG}->{cons2DFG}->{NewDFG}->is_vertex_nxt_stack_empty($ky)!=0){   
#
#           }
#    }
}

sub run_REG_cluster {
   my ($self) = (@_);

   my $cstp_w_all = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{W_REG};

  foreach my $ky (keys %{$cstp_w_all}){
          my $cyc = $cstp_w_all->{$ky}->{begin};
          if( !$self->{reg_list}->{$cyc} ){ 
               $self->{reg_list}->{$cyc} = 0;
          }
               $self->{reg_list}->{$cyc}++;   

               $self->{reg_list}->{W_REG}->{$ky} = $self->{rev_key}.$self->{reg_list}->{$cyc};                                        
        } 

#print Dumper($self->{reg_list});
}

1;
