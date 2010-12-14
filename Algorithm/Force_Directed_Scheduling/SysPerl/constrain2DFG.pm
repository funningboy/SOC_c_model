#!/usr/bin/perl
 
package SysPerl::constrain2DFG;
use SysPerl::DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self = {  
              DFG            => SysPerl::DFG->new(),
              BakDFG         => SysPerl::DFG->new(),
              NewDFG         => SysPerl::DFG->new(), 
              ALUDFG         => SysPerl::DFG->new(),
              BakNewDFG      => SysPerl::DFG->new(),
              time_weighted  => {},
              power_weighted => {},
              assign_list    => [], # '='
              search_list    => [],
              bakup_list     => {
                                begin_list => [],
                                end_list   => [],
                                },
              max_cycle_time => 0, #max delay tolerance
              cycle          => 1, #initial cycle @ 1
              deep_cycle     => 1, #max deep cycle @ schedule
              cycle_list     => {},#cycle table for each vertex
              cstep_list     => {},
              power_list     => {},
              rev_key        => {
                                read => {
                                   name => 'r::@_',
                                   id   =>  0,
                                        },
                                write => {
                                   name => 'w::@_',
                                   id   => 0,
                                        },
                                },  
             };

  bless $self, $class;
  return $self;
}

sub error {
    my ($self,$st) = (@_);
    die print $st;
}

sub is_search_list_empty {
   my ($self) = (@_);
   if( !@{$self->{search_list}} ){ return 0; }
return -1;
}

sub push_search_list {
   my ($self,$vertex) = (@_);
   push (@{$self->{search_list}},$vertex);
}

sub pop_search_list {
   my ($self,$vertex) = (@_);
return pop (@{$self->{search_list}}); 
}

sub shft_search_list {
   my ($self) = (@_);
return shift (@{$self->{search_list}});
}

sub get_top_search_list {
   my ($self) = (@_);
   if( $self->is_search_list_empty()!=0 ){
   my $vex = $self->pop_search_list(); 
      $self->push_search_list($vex);
      return $vex;
  }
return -1; 
}

sub clr_NewDFG_time_weighted {
   my ($self) = (@_);
  $self->{NewDFG}->clr_time_weighted_edges();
  $self->{NewDFG}->clr_time_weighted_vertices();
}

sub set_NewDFG_begin_time_weighted {
   my ($self) = (@_);
   my  $all_vex = $self->{NewDFG}->get_begin_lists();

  foreach my $vex (@{$all_vex}) {
       $self->{NewDFG}->updt_time_weighted_vertex($vex,$self->{cycle});
  }
}

sub set_NewDFG_end_time_weighted {
   my ($self) = (@_);
   my  $all_vex = $self->{NewDFG}->get_end_lists();

  foreach my $vex (@{$all_vex}) {
       $self->{NewDFG}->updt_time_weighted_vertex($vex,$self->{deep_cycle});
  }
}

sub set_NewDFG_reschedule_time_weighted {
   my ($self,$force) = (@_);

   my $ALU_hs   = $self->{cstep_list}->{ALU};
   my $R_REG_hs = $self->{cstep_list}->{R_REG};
   my $W_REG_hs = $self->{cstep_list}->{W_REG};
   my $ASSIGN_hs= $self->{cstep_list}->{ASSIGN};

   if($force > 0){
     foreach my $alu (keys %{$ALU_hs}){
         if( $ALU_hs->{$alu}->{begin} == $force &&
             $ALU_hs->{$alu}->{begin} != $ALU_hs->{$alu}->{end} ){
             $self->{NewDFG}->updt_time_weighted_vertex($alu,$force+1);
         }
       }
    }

}
 
sub clr_search_list {
   my ($self) = (@_);
       $self->{search_list} = [];
}

sub run_NewDFG_2_BakNewDFG_by_stacks {
   my ($self) = (@_);
   my $arr = $self->{NewDFG}->get_vertex_pre_stacks(); 
       $self->{BakNewDFG}->set_vertex_pre_stacks($arr);

   my $arr = $self->{NewDFG}->get_vertex_nxt_stacks();
       $self->{BakNewDFG}->set_vertex_nxt_stacks($arr);
}

sub run_NewDFG_2_BakNewDFG_by_lists {
   my ($self) = (@_);
 
   my $arr = $self->{NewDFG}->get_begin_lists();
       $self->{BakNewDFG}->set_begin_lists($arr);

   my $arr = $self->{NewDFG}->get_end_lists();
       $self->{BakNewDFG}->set_end_lists($arr);
}

sub run_BakNewDFG_2_NewDFG_by_stacks {
   my ($self) = (@_);
   my $arr = $self->{BakNewDFG}->get_vertex_pre_stacks();
       $self->{NewDFG}->set_vertex_pre_stacks($arr); 
      
   my $arr = $self->{BakNewDFG}->get_vertex_nxt_stacks();
       $self->{NewDFG}->set_vertex_nxt_stacks($arr); 
}

sub run_BakNewDFG_2_NewDFG_by_lists {
    my ($self) = (@_);

   my $arr = $self->{BakNewDFG}->get_begin_lists();
       $self->{NewDFG}->set_begin_lists($arr);

   my $arr = $self->{BakNewDFG}->get_end_lists();
       $self->{NewDFG}->set_end_lists($arr);
}

sub run_syn_assign_list_ini {
   my ($self) = (@_);

   my $all_vex = $self->{DFG}->get_all_vertices();
  
   foreach my $vex (@{$all_vex}){
     if($vex =~ /\=/){
        $self->push_assign_list($vex); 
     }
  }
        $self->sort_assign_list();
}


sub set_deep_DFG {
   my ($self,$DFG) = (@_);
       $self->{DFG} = $DFG;
}

sub set_constrain_time_weighted {
   my ($self,$time_hs) = (@_);
       $self->{time_weighted} = $time_hs;
}

sub get_constrain_time_weighted_vertex {
   my ($self,$vertex) = (@_);
      if( !$self->{time_weighted}->{$vertex} ){ return -1; }
      return $self->{time_weighted}->{$vertex};     
}

sub run_constrain_time_weighted_DFG {
  my ($self) = (@_);
  my  $all_vex = $self->{DFG}->get_all_vertices();

  foreach my $vex (@{$all_vex}) {
      if( $vex=~/^(\S+)\:\:/){ 
         my $tim =$self->get_constrain_time_weighted_vertex($1);
         if($tim!=-1){
            $self->{DFG}->updt_time_weighted_vertex($vex,$tim);
       }
     }
  }
}

sub set_constrain_power_weighted {
   my ($self,$power_hs) = (@_);
       $self->{power_weighted} = $power_hs;
}

sub get_constrain_power_weighted_vertex {
   my ($self,$vertex) = (@_);
      if( !$self->{power_weighted}->{$vertex} ){ return -1; }
      return $self->{power_weighted}->{$vertex};     
}


sub run_con_max_cycle_time {
   my ($self) = (@_);
   my $max;
foreach my $key 
           (sort {$self->{time_weighted}->{$a} <=> $self->{time_weighted}->{$b}} 
           (keys(%{$self->{time_weighted}}))) {
           $max = $self->{time_weighted}->{$key};
           }  
$self->{max_cycle_time} = $max;
}

sub is_cycle_time_boundary {
   my ($self,$time) = (@_);
   if($time > $self->{max_cycle_time}){ return 0; }
return -1;
}


sub rename_new_write_time_weighted_vertex {
   my ($self) = (@_);
   my $id = $self->{rev_key}->{write}->{id}++;
return $self->{rev_key}->{write}->{name}.$id;
}

sub rename_new_read_time_weighted_vertex {
   my ($self) = (@_);  
   my  $id = $self->{rev_key}->{read}->{id}++;
return $self->{rev_key}->{read}->{name}.$id;    
}

sub run_NewDFG_by_time_weighted_constrain {
   my ($self,$vertex) = (@_);

      $self->{NewDFG}->set_time_weighted_vertex($vertex,0);

   while($self->{DFG}->is_vertex_pre_stack_empty($vertex)!=0){
      my $pre_vex = $self->{DFG}->shft_vertex_pre_stack($vertex);
      my $cur_tim = $self->{DFG}->get_time_weighted_vertex($vertex);
      my $pre_tim = $self->{DFG}->get_time_weighted_vertex($pre_vex->[0]);

         $self->{NewDFG}->set_time_weighted_edge($pre_vex->[0],$vertex,0,$pre_vex->[1]);

        #print $pre_vex->[0].','.$vertex.','.$pre_tim.','.$cur_tim."\n";
 
         if( $self->is_cycle_time_boundary($pre_tim+$cur_tim) ==0 ){
                   my $rd_vex = $self->rename_new_read_time_weighted_vertex();
                   my $wt_vex = $self->rename_new_write_time_weighted_vertex();
                
                       $self->{NewDFG}->set_time_weighted_vertex($rd_vex,0);
                       $self->{NewDFG}->set_time_weighted_vertex($wt_vex,0);
                
                       $self->{NewDFG}->set_time_weighted_edge($pre_vex->[0],$wt_vex,0,$pre_vex->[1]);
                       $self->{NewDFG}->set_time_weighted_edge($wt_vex,$rd_vex,0,$pre_vex->[1]);
                       $self->{NewDFG}->set_time_weighted_edge($rd_vex,$vertex,0,$pre_vex->[1]);
  
                       $self->{NewDFG}->del_time_weighted_edge($pre_vex->[0],$vertex);
         } else {
                    if($pre_vex->[0]=~/\=\:\:/||
                       $pre_vex->[0]=~/r\:\:\@/ ){ $self->{DFG}->updt_time_weighted_vertex($pre_vex->[0],0); }
                    else{                   $self->{DFG}->updt_time_weighted_vertex($pre_vex->[0],$cur_tim+$pre_tim); }
         }
     
     $self->run_NewDFG_by_time_weighted_constrain($pre_vex->[0]);
  }

     $self->{DFG}->updt_time_weighted_vertex($vertex,0);
}

sub run_NewDFG_by_time {
    my ($self) = (@_);
    
    my $end_list =  $self->{NewDFG}->{end_list};
    foreach my $end (@{$end_list}){
               $self->run_NewDFG_by_time_weighted_constrain($end);
    }
}

sub run_DFG_begin_list {
    my ($self) = (@_);

    my $all_vex = $self->{DFG}->get_all_vertices();

    foreach my $vex (@{$all_vex}){
    if( $self->{DFG}->is_vertex_pre_stack_empty($vex) ==0 ){
        $self->{DFG}->push_begin_list($vex);
        $self->{NewDFG}->push_begin_list($vex);
      }
    }
}

sub run_DFG_end_list {
   my ($self) = (@_);

   my $all_vex = $self->{DFG}->get_all_vertices();
 
   foreach my $vex (@{$all_vex}){
   if( $self->{DFG}->is_vertex_nxt_stack_empty($vex) ==0 ){
       $self->{DFG}->push_end_list($vex);
       $self->{NewDFG}->push_end_list($vex);
  }
 }
}

#==================================
# in here time weighted = cycle
#==================================
sub run_NewDFG_ASAP_cycle {
   my ($self) = (@_);

   my ($nxt,$cur_cyc,$nxt_cyc,$cyc);

   while( $self->{NewDFG}->is_begin_list_empty()!= 0 ){
      my  $src = $self->{NewDFG}->pop_begin_list();

             $self->{cycle_list}->{ASAP}->{$src} = $self->{NewDFG}->get_time_weighted_vertex($src);

           if( $self->{BakNewDFG}->is_end_list_exist($src)==0 ){ 
               $self->run_BakNewDFG_2_NewDFG_by_stacks();
              }

       while( $self->{NewDFG}->is_vertex_nxt_stack_empty($src)!=0 ){
              $nxt     = $self->{NewDFG}->pop_vertex_nxt_stack($src);
              $cur_cyc = $self->{NewDFG}->get_time_weighted_vertex($src); 
              $nxt_cyc = $self->{NewDFG}->get_time_weighted_vertex($nxt->[0]);

             if( $nxt->[0]=~/r\:\:\@/ ){
                $cur_cyc = $cur_cyc + 1;
                $self->{deep_cycle} = $cur_cyc;
             }
               
             $cyc = ( $nxt_cyc > $cur_cyc )? $nxt_cyc : $cur_cyc;
             $self->{NewDFG}->updt_time_weighted_vertex($nxt->[0],$cyc);
             $self->{NewDFG}->push_begin_list($nxt->[0]);                            
       }
#       print $src.','.$self->{NewDFG}->get_time_weighted_vertex($src)."\n";
      $self->run_NewDFG_ASAP_cycle();
   }
}


sub run_NewDFG_ALAP_cycle {
  my ($self) = (@_);

  my ($nxt,$cur_cyc,$nxt_cyc,$cyc);

   while( $self->{NewDFG}->is_end_list_empty()!= 0 ){
      my  $src = $self->{NewDFG}->pop_end_list();

             $self->{cycle_list}->{ALAP}->{$src} = $self->{NewDFG}->get_time_weighted_vertex($src);

           if( $self->{BakNewDFG}->is_begin_list_exist($src)==0 ){ 
               $self->run_BakNewDFG_2_NewDFG_by_stacks();
              }

       while( $self->{NewDFG}->is_vertex_pre_stack_empty($src)!=0 ){
              $nxt     = $self->{NewDFG}->pop_vertex_pre_stack($src);
              $cur_cyc = $self->{NewDFG}->get_time_weighted_vertex($src); 
              $nxt_cyc = $self->{NewDFG}->get_time_weighted_vertex($nxt->[0]);

             if( $nxt->[0]=~/w\:\:\@/ ){
                $cur_cyc = $cur_cyc - 1;
             }
   
             $cyc = ( $cur_cyc < $nxt_cyc || $nxt_cyc==0 )? $cur_cyc : $nxt_cyc;
             $self->{NewDFG}->updt_time_weighted_vertex($nxt->[0],$cyc);
             $self->{NewDFG}->push_end_list($nxt->[0]);                            
       }
#       print $src.','.$self->{NewDFG}->get_time_weighted_vertex($src)."\n";
      $self->run_NewDFG_ALAP_cycle();
   }
}

sub run_CSTEP_constrain_by_power {
  my ($self) = (@_);
  my  $all_vex = $self->{NewDFG}->get_all_vertices();

  foreach my $vex (@{$all_vex}) {
      if( $vex=~/^(\S+)\:\:/){ 
         my $pow =$self->get_constrain_power_weighted_vertex($1);
         if($pow!=-1){
            $self->{power_list}->{$vex} = $pow;
       }
     }
   }
}


sub run_CSTEP_by_NewDFG {
   my ($self) = (@_);
   my $ASAPhs = $self->{cycle_list}->{ASAP};
   my $ALAPhs = $self->{cycle_list}->{ALAP};

   foreach my $ky (keys %{$self->{cycle_list}->{ASAP}} ){
       my $asap_cyc = $self->{cycle_list}->{ASAP}->{$ky}; 
       my $alap_cyc = $self->{cycle_list}->{ALAP}->{$ky};
       my $nm;

           if($ky=~/[\+\-\*\/\%\>\>\<\<]/){ $nm = 'ALU';   }
        elsif($ky=~/w\:\:\@/)             { $nm = 'W_REG'; }
        elsif($ky=~/r\:\:\@/)             { $nm = 'R_REG'; }
        elsif($ky=~/\=\:\:/)              { $nm = 'ASSIGN';}
        elsif($self->{NewDFG}->is_vertex_pre_stack_empty($ky)==0){ $nm = 'IN';  }
        elsif($self->{NewDFG}->is_vertex_nxt_stack_empty($ky)==0){ $nm = 'OUT'; }

           $self->{cstep_list}->{$nm}->{$ky} = { 
                                                 begin => $asap_cyc,
                                                 end   => $alap_cyc,
                                               };
  }
}

sub run_ALUDFG_by_NewDFG_constrain {
   my ($self,$src) = (@_);

        while( $self->{NewDFG}->is_vertex_nxt_stack_empty($src)!=0 ){
               my $nxt  = $self->{NewDFG}->pop_vertex_nxt_stack($src);
              
               #print $src.','.$nxt."\n";

               if( $nxt->[0]=~/[\+\-\*\/\%\>\>\<\<]/ ){
                my $srch = $self->get_top_search_list();
                   $self->{ALUDFG}->set_time_weighted_vertex($nxt->[0],0);
                   $self->{ALUDFG}->set_time_weighted_edge($srch,$nxt->[0],0,$nxt->[1]);
                   $self->push_search_list($nxt->[0]);

              }elsif( $self->{NewDFG}->is_vertex_nxt_stack_empty($nxt->[0])==0 ){
                 my $srch = $self->get_top_search_list();
                    $self->{ALUDFG}->set_time_weighted_edge($srch,'@',0);
                    $self->{ALUDFG}->set_time_weighted_vertex('@',0);
              } 
                 
               $self->run_ALUDFG_by_NewDFG_constrain($nxt->[0]);
        }


        if( $src=~/[\+\-\*\/\%\>\>\<\<\#]/ ){
            $self->pop_search_list();
        }
}

sub run_ALUDFG_by_NewDFG {
   my ($self) = (@_);
 
   while( $self->is_begin_list_empty()!= 0 ){
      my  $src = $self->pop_begin_list();
          $self->push_search_list('#',0);
          $self->run_ALUDFG_by_NewDFG_constrain($src);
          $self->clr_search_list();
   } 
}


sub run_constrain_NewDFG {
  my ($self) = (@_);
   #find the max time delay @ each alu
   $self->run_con_max_cycle_time();

   #find the begin && end boundary @ DFG graph = NewDFG graph  
   $self->run_DFG_begin_list();
   $self->run_DFG_end_list();

   #gen NewDFG grpah @ time constrain
   $self->run_NewDFG_by_time();

#$self->dump_NewDFG_vertex_pre_stack();
#$self->dump_NewDFG_vertex_nxt_stack();
}

sub get_deep_copy_NewDFG {
   my ($self) = (@_);
return $self->{NewDFG};
}

sub get_deep_copy_ALUDFG {
   my ($self) = (@_);
return $self->{ALUDFG};
}

sub dump_NewDFG_time_weighted_vertices {
   my ($self) = (@_);
       $self->{NewDFG}->dump_time_weighted_vertices();
}

sub dump_NewDFG_time_weighted_edges {
   my ($self) = (@_);
       $self->{NewDFG}->dump_time_weighted_edges();
}

sub dump_NewDFG_vertex_pre_stack {
   my ($self) = (@_);
   my $all_vex = $self->{NewDFG}->get_all_vertices();
   print "dump_NewDFG_vertex_pre_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{NewDFG}->get_vertex_pre_stack($vex));
   }
}

sub dump_NewDFG_vertex_nxt_stack {
   my ($self) = (@_);
   my $all_vex = $self->{NewDFG}->get_all_vertices();
   print "dump_NewDFG_vertex_nxt_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{NewDFG}->get_vertex_nxt_stack($vex));
   }
}
sub dump_NewDFG_graphviz_file {
   my ($self,$path) = (@_);
       $self->{NewDFG}->dump_graphviz_file($path);
}


sub dump_ALUDFG_time_weighted_vertices {
   my ($self) = (@_);
       $self->{ALUDFG}->dump_time_weighted_vertices();
}

sub dump_ALUDFG_time_weighted_edges {
   my ($self) = (@_);
       $self->{ALUDFG}->dump_time_weighted_edges();
}

sub dump_ALUDFG_vertex_pre_stack {
   my ($self) = (@_);
   my $all_vex = $self->{ALUDFG}->get_all_vertices();
   print "dump_ALUDFG_vertex_pre_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{ALUDFG}->get_vertex_pre_stack($vex));
   }
}

sub dump_ALUDFG_vertex_nxt_stack {
   my ($self) = (@_);
   my $all_vex = $self->{ALUDFG}->get_all_vertices();
   print "dump_ALUDFG_vertex_nxt_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{ALUDFG}->get_vertex_nxt_stack($vex));
  }
}

sub dump_ALUDFG_time_vertices {
   my ($self) = (@_);
   $self->{ALUDFG}->dump_time_weighted_edges();
}

sub dump_ALUDFG_time_edges {
  my ($self) = (@_);
   $self->{ALUDFG}->dump_time_weighted_edges();
}

sub dump_ALUDFG_graphviz_file {
   my ($self,$path) = (@_);
       $self->{ALUDFG}->dump_graphviz_file($path);
}

sub free {
   my ($self) = (@_);
}

1;
 
