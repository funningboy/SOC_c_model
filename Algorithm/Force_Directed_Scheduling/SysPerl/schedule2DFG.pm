#!/usr/bin/perl
 
package SysPerl::schedule2DFG;
use SysPerl::DFG;
use SysPerl::constrain2DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self  = {
      cons2DFG     => SysPerl::constrain2DFG->new(),
      reg_list     => {},
      distrib_list => {},
  };
  bless $self, $class;
  return $self;
}

#===============================
# method :force directed scheduling
# flow1  . ASAP,ALAP,scheduling time graph
# flow2  . distribution graph
# flow3  . calculate the force
#===============================
sub run_force_direct_alg_ASAP {
   my ($self,$force) = (@_);

       $self->{cons2DFG}->run_NewDFG_2_BakNewDFG_by_stacks();
       $self->{cons2DFG}->run_NewDFG_2_BakNewDFG_by_lists();
       $self->{cons2DFG}->clr_NewDFG_time_weighted();
       $self->{cons2DFG}->set_NewDFG_begin_time_weighted();
       $self->{cons2DFG}->set_NewDFG_reschedule_time_weighted($force);
       $self->{cons2DFG}->run_NewDFG_ASAP_cycle();
       $self->{cons2DFG}->run_BakNewDFG_2_NewDFG_by_stacks();
       $self->{cons2DFG}->run_BakNewDFG_2_NewDFG_by_lists();

#   $self->{cons2DFG}->dump_NewDFG_time_weighted_edges();
#   $self->{cons2DFG}->dump_NewDFG_time_weighted_vertices();
#die;
}

sub run_force_direct_alg_ALAP {
    my ($self) = (@_);

       $self->{cons2DFG}->run_NewDFG_2_BakNewDFG_by_stacks();
       $self->{cons2DFG}->run_NewDFG_2_BakNewDFG_by_lists();
       $self->{cons2DFG}->clr_NewDFG_time_weighted();
       $self->{cons2DFG}->set_NewDFG_end_time_weighted();
       $self->{cons2DFG}->run_NewDFG_ALAP_cycle();
       $self->{cons2DFG}->run_BakNewDFG_2_NewDFG_by_stacks();
       $self->{cons2DFG}->run_BakNewDFG_2_NewDFG_by_lists();

#   $self->{cons2DFG}->dump_NewDFG_time_weighted_edges();
#   $self->{cons2DFG}->dump_NewDFG_time_weighted_vertices();
#die;
}

sub run_force_direct_alg_CSTEP {
    my ($self) = (@_);
        $self->{cons2DFG}->run_CSTEP_by_NewDFG();
        $self->{cons2DFG}->run_CSTEP_constrain_by_power();

# print Dumper ($self->{cons2DFG}->{cstep_list});
}

sub run_force_direct_alg_distributed {
   my ($self) = (@_);

   my $cst_alu = $self->{cons2DFG}->{cstep_list}->{ALU};
      $self->{distrib_list} = {}; 

   foreach my $alu ( keys %{$cst_alu} ){
      my $bg = $cst_alu->{$alu}->{begin};
      my $ed = $cst_alu->{$alu}->{end};     
      my $dif = $ed-$bg+1;
    
       for(my $i=$bg; $i<=$ed; $i++){
           my $pw = $self->{cons2DFG}->{power_list}->{$alu};
              $self->{distrib_list}->{$i} += $pw/$dif;
     }
  }

#print Dumper($self->{cons2DFG}->{power_list});
#print Dumper($self->{distrib_list});
}


sub run_force_direct_alg_calculated {
   my ($self) = (@_);

   my $dis_list = $self->{distrib_list};
   my @ky_arr   = sort {$a<=>$b} keys (%{$dis_list});
  
   for(my $i=$ky_arr[0]; $i<=$ky_arr[$#ky_arr]; $i++){
      my $pre = $i-1;
      my $nxt = $i+1;
      my $rst;

            if( $pre < $ky_arr[0] ){
        $rst =  $self->{distrib_list}->{$i} - ($self->{distrib_list}->{$i}+$self->{distrib_list}->{$nxt})/2;
       } elsif( $nxt > $ky_arr[$#ky_arr] ){
        $rst =  $self->{distrib_list}->{$i} - ($self->{distrib_list}->{$pre}+$self->{distrib_list}->{$i})/2;
       } else {
        $rst =  $self->{distrib_list}->{$i}   - ($self->{distrib_list}->{$pre}+$self->{distrib_list}->{$i}  )/2  +
                $self->{distrib_list}->{$nxt} - ($self->{distrib_list}->{$i}  +$self->{distrib_list}->{$nxt})/2;
      }

#print Dumper($self->{distrib_list});

      if($rst <0 ){
         return $i; 
    }
  } 

return -1;
}

sub run_forece_directed_scheduling {
   my ($self) = (@_);
   my $force    = 0;
   my $deep_cot = $self->{cons2DFG}->{deep_cycle}*2;
   my $cot      = 0;

   while($cot<=$deep_cot){
                $self->run_force_direct_alg_ASAP($force);
                $self->run_force_direct_alg_ALAP($force);
                $self->run_force_direct_alg_CSTEP();
                $self->run_force_direct_alg_distributed();
       $force = $self->run_force_direct_alg_calculated();

       if( $force ==-1 ){ last; }
        #   print Dumper ($self->{cons2DFG}->{cstep_list});
     $cot++;  
   }
}


sub set_deep_cons2DFG {
   my ($self,$DFG) = (@_);
       $self->{cons2DFG} = $DFG;
}

sub report {
    my ($self) = (@_);
    print Dumper ($self->{cons2DFG}->{cstep_list});
    print Dumper($self->{distrib_list});
}


1;
