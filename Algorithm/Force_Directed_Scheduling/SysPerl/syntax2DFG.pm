
#! /usr/bin/perl -w

# author : sean chen
# mail : funningboy@gmail.com
# publish : 2010/12/01


package SysPerl::syntax2DFG;
use SysPerl::DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self = {  text     => [],
                tmp_txt  => [],
                rem_txt  => [],
                bg_inx   => 0,
                ed_inx   => 0,
                max_lvl  => 0,
                lvl_inx  => {
                              '(' => {},
                              ')' => {},
                             },
                op_stack => [],
                st_stack => [],
                DFG => SysPerl::DFG->new(),
                pri_table => {
                               0 => {
                                      '*'  => 0, #mul
                                      '/'  => 0, #div
                                      '%'  => 0, #rem
                                   },
                               1 => {
                                      '+' => 0, #add
                                      '-' => 0, #sub
                                      '>>' => 0, #rsht
                                      '<<' => 0, #lsht
 
                                   },
                            },
                ass_table => {
                             '=' => 0, #ass
                             '(' => 0, #lsr
                             ')' => 0, #rsr
                             ';' => 0, #end
                             'rev_key' => {
                                             name => 'itmp_', #reserve key
                                             id => 0,
                                           },
                             'slt_key' => {
                                         read => {
                                             name => 'r::@_',
                                             id   => 0,
                                                 },
                                         write => {
                                             name => 'w::@_',
                                             id   => 0,
                                                 },
                                           },
                              },
   
             };
  bless $self, $class;
  return $self;
}
    
sub error {
    my ($st) = (@_);
    die print $st;
}

sub read_text {
    my ($self,$text) = (@_);
    $self->{text} = $text;
}

sub push_op_stack {
    my ($self,$a) = (@_);
    push (@{$self->{op_stack}},$a);
}

sub pop_op_stack {
    my ($self) = (@_);
    return pop(@{$self->{op_stack}});
}

sub get_top_op_stack {
    my ($self) = (@_);

    my $a = $self->pop_op_stack();
            $self->push_op_stack($a);
    return $a;
}

sub is_op_stack_empty {
   my ($self) = (@_);
   if( !@{$self->{op_stack}} ){ return 0 }

return -1;
}

sub push_st_stack {
    my ($self,$a) = (@_);
    push(@{$self->{st_stack}},$a);
}

sub pop_st_stack {
    my ($self) = (@_);
    return pop(@{$self->{st_stack}});
}

sub get_st_stack {
    my ($self) = (@_);
    my $a = $self->pop_st_stack();
            $self->push_st_stack($a);
    return $a;
}

sub is_st_stack_empty {
    my ($self) = (@_);
    if( !@{$self->{st_stack}} ){ return 0; }

return -1;
}


sub crt_rev_key {
    my ($self,$id) = (@_);
    return $self->{ass_table}->{rev_key}->{name}.$id;
}

sub crt_split_read_rename {
   my ($self,$name) = (@_);
return $self->{ass_table}->{slt_key}->{read}->{name}.$name;
}

sub crt_split_write_rename {
    my ($self,$name) = (@_);
return $self->{ass_table}->{slt_key}->{write}->{name}.$name;
}

sub crt_op_rename {
    my ($self,$st,$lvl) = (@_);

    if( defined($self->{pri_table}->{0}->{$st} )){
        my $id = $self->{pri_table}->{0}->{$st}++;
        return $st."::".$id;
    }

    if( defined($self->{pri_table}->{1}->{$st} )){
        my $id = $self->{pri_table}->{1}->{$st}++;
        return $st."::".$id;
    }
}

sub crt_as_rename {
    my ($self,$st) = (@_);

    if( defined($self->{ass_table}->{$st}) ){
        my $id = $self->{ass_table}->{$st}++;
        return $st."::".$id;
    }
}


sub is_lvl_inx_lsr_empty {
    my ($self,$lvl) = (@_);
       if( !@{$self->{lvl_inx}->{'('}->{$lvl}} ){ return 0; }

return -1;
}

sub shft_lvl_inx_lsr {
    my ($self,$lvl) = (@_);
return shift(@{$self->{lvl_inx}->{'('}->{$lvl}});
}

sub push_lvl_inx_lsr {
   my ($self,$lvl,$a) = (@_);
      push(@{$self->{lvl_inx}->{'('}->{$lvl}},$a);
}

sub clr_lvl_inx_lsr {
   my ($self) = (@_);
   $self->{lvl_inx}->{'('} = {};
}

sub is_lvl_inx_rsr_empty {
    my ($self,$lvl) = (@_);
       if( !@{$self->{lvl_inx}->{')'}->{$lvl}} ){ return 0; }

return -1;
}

sub shft_lvl_inx_rsr {
    my ($self,$lvl) = (@_);
return shift(@{$self->{lvl_inx}->{')'}->{$lvl}});
}

sub push_lvl_inx_rsr {
   my ($self,$lvl,$a) = (@_);
      push(@{$self->{lvl_inx}->{')'}->{$lvl}},$a);
}

sub clr_lvl_inx_rsr {
   my ($self) = (@_);
       $self->{lvl_inx}->{')'} = {};
}

#=====================================
# check ini_level
#=====================================
sub chk_ini_level_text {
    my ($self) = (@_);

    my @tmp_arr;
    foreach my $st (@{$self->{text}}) {
       push (@tmp_arr,$st);
      
         if( $st eq '=' ){ push (@tmp_arr,'('); }
      elsif( $st eq ';' ){ pop  (@tmp_arr);
                           push (@tmp_arr,')');
                           push (@tmp_arr,';'); last; }
    }
     
      @{$self->{text}} = @tmp_arr;
 
#   print Dumper($self->{text});
}

#======================================
# check max_level
#======================================
sub chk_max_level_text {
    my ($self) = (@_);
    
    my $cur_lvl = 0;
    my @arr     = @{$self->{text}};

    for(my $i=0; $i<=$#arr; $i++){
       my $chr = $arr[$i];

       if( $cur_lvl > $self->{max_lvl} ){ $self->{max_lvl} = $cur_lvl; }

       if( $chr eq '(' ){ $self->push_lvl_inx_lsr(++$cur_lvl, $i); }
       if( $chr eq ')' ){ $self->push_lvl_inx_rsr($cur_lvl--, $i); }
   }

# print Dumper($self->{text});
}

#===================================
# update levle text
#===================================
sub upd_level_text {
    my ($self,$lvl) = (@_);

  my $new    = $self->get_st_stack();  
  my @arr    = @{$self->{text}};
  my @tmp_arr;

  for(my $i=0; $i<$self->{bg_inx}; $i++){
     push (@tmp_arr,$arr[$i]);
  }
    push(@tmp_arr,$new);
  
  for(my $i=$self->{ed_inx}+1; $i<=$#arr; $i++){
    push (@tmp_arr,$arr[$i]);
  }

  @{$self->{text}} = @tmp_arr;

  $self->clr_lvl_inx_lsr();
  $self->clr_lvl_inx_rsr();
  $self->{max_lvl} = 0;

#  print Dumper (\@tmp_arr);
}

#====================================
# get_level_text
#====================================
sub get_level_text {
    my ($self,$lvl) = (@_);

    my $bg_inx = $self->shft_lvl_inx_lsr($lvl);
    my $ed_inx = $self->shft_lvl_inx_rsr($lvl);

       $self->{bg_inx} = $bg_inx;
       $self->{ed_inx} = $ed_inx;

    my @arr = @{$self->{text}};
    my @tmp_arr;

    for(my $i=0; $i<=$#arr; $i++){
       if( $bg_inx < $i && $i < $ed_inx ){
           push (@tmp_arr,$arr[$i]);
       }
    }    
    push (@tmp_arr,';');

    @{$self->{tmp_text}} = @tmp_arr;

#   print Dumper (\@tmp_arr);
}


sub chk_prity_text {
    my ($self,$lvl) = (@_);

    my $st;
    my $chr;

    my @arr = @{$self->{tmp_text}};

    for(my $i=0; $i<=$#arr; $i++){
       $chr = $arr[$i];

       if( defined($self->{pri_table}->{0}->{$chr}) ||
           defined($self->{pri_table}->{1}->{$chr}) ||
           defined($self->{ass_table}->{$chr}) ){
           # check op_stack
               my $top = ($self->is_op_stack_empty()!=0 )? $self->get_top_op_stack() : -1;
               
               while( $self->is_op_stack_empty()!=0 &&
                      $self->is_st_stack_empty()!=0 &&
                      defined($self->{pri_table}->{1}->{$chr}) &&
                      defined($self->{pri_table}->{0}->{$top}) ){

                   my $op = $self->pop_op_stack();
                      $op = $self->crt_op_rename($op,$lvl);
                   my $src1 = $self->pop_st_stack();
                   my $src2 = $st;
                   my $dst = $self->crt_rev_key($self->{ass_table}->{rev_key}->{id}++);

                   # call DFG graph
                      $self->{DFG}->set_time_weighted_vertex($op,0);
                      $self->{DFG}->set_time_weighted_vertex($src1,0);
                      $self->{DFG}->set_time_weighted_vertex($src2,0);
                      $self->{DFG}->set_time_weighted_vertex($dst,0);
                      $self->{DFG}->set_time_weighted_edge($src1,$op,0,'src1');
                      $self->{DFG}->set_time_weighted_edge($src2,$op,0,'src2');
                      $self->{DFG}->set_time_weighted_edge($op,$dst,,0,'dst');
                      $st = $dst;
            }
           #push
               if($chr ne ';'){ $self->push_op_stack($chr); }
               $self->push_st_stack($st);
               $st =();

      } else {
              $st.=$chr;
             }
     }
}

sub chk_remain_text {
    my ($self,$lvl) = (@_);

 while( $self->is_op_stack_empty()!=0 &&
        $self->is_st_stack_empty()!=0 ){

        my $op = $self->pop_op_stack();
           $op = $self->crt_op_rename($op,$lvl);
 
        my $src2 = $self->pop_st_stack();
        my $src1 = $self->pop_st_stack();
        my $dst = $self->crt_rev_key($self->{ass_table}->{rev_key}->{id}++);

        # DFG graph create
          $self->{DFG}->set_time_weighted_vertex($op,0);
          $self->{DFG}->set_time_weighted_vertex($src1,0);
          $self->{DFG}->set_time_weighted_vertex($src2,0);
          $self->{DFG}->set_time_weighted_vertex($dst,0);
   
          $self->{DFG}->set_time_weighted_edge($src1,$op,0,'src1');
          $self->{DFG}->set_time_weighted_edge($src2,$op,0,'src2');
          $self->{DFG}->set_time_weighted_edge($op,$dst,0,'dst');
        
          $self->push_st_stack($dst);
    }

}

sub get_assign_text {
    my ($self) = (@_);
    
    my @arr = @{$self->{text}};
 
    if( $arr[1] ne '=' && $arr[3] eq '=' ){ $self->error("we only support a = b;, not suport a = b = c;\n"); }

    $self->push_st_stack($arr[0]);
    $self->push_op_stack($arr[1]);
}

sub chk_assign_text {
    my ($self) = (@_);
 
      my $op = $self->pop_op_stack();
         $op = $self->crt_as_rename($op);
 
     my $dst = $self->pop_st_stack();
     my $src = $self->pop_st_stack();

     # DFG graph create
       $self->{DFG}->set_time_weighted_vertex($op,0);
       $self->{DFG}->set_time_weighted_vertex($dst,0);
       $self->{DFG}->set_time_weighted_vertex($src,0);

       $self->{DFG}->set_time_weighted_edge($src,$op,0,'src1');
       $self->{DFG}->set_time_weighted_edge($op,$dst,0,'dst');
 
}


#====================================
# run text && create graph
#====================================
sub run_text {
    my ($self) = (@_);
    my $lvl;

     $self->chk_ini_level_text();
     $self->chk_max_level_text();
     $lvl = $self->{max_lvl};

   while($lvl>0){
     $self->get_level_text($lvl);
     $self->chk_prity_text($lvl);
     $self->chk_remain_text($lvl);
     $self->upd_level_text($lvl);
     $self->chk_max_level_text();
     $lvl = $self->{max_lvl};
  }

     $self->get_assign_text();
     $self->chk_assign_text();
}

sub dump_DFG_graphviz_file {
    my ($self,$path) = (@_);
        $self->{DFG}->dump_graphviz_file($path);
}

sub dump_DFG_graph_ascii {
    my ($self) = (@_);
        $self->{DFG}->dump_graph_ascii();
}

sub dump_DFG_vertex_pre_stack {
   my ($self) = (@_);
   my $all_vex = $self->{DFG}->get_all_vertices();
   print "dump_DFG_vertex_pre_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{DFG}->get_vertex_pre_stack($vex));
   }
}

sub dump_DFG_vertex_nxt_stack {
   my ($self) = (@_);
   my $all_vex = $self->{DFG}->get_all_vertices();
   print "dump_DFG_vertex_nxt_stack\n";
   foreach my $vex (@{$all_vex}){
      print $vex."\n";
      print Dumper($self->{DFG}->get_vertex_nxt_stack($vex));
  }
}

sub get_deep_copy_DFG {
    my ($self) = (@_);
return $self->{DFG};
}

sub get_deep_copy_DFG_graph {
   my ($self) = (@_);
return $self->{DFG}->get_deep_copy_graph();
}





#================================
# delete itmp vertex
#================================
sub del_itmp_time_weighted_vertex {
    my ($self,$vertex) = (@_);

    if($vertex!~ /itmp_/ ){ $self->error("del_time_weighted_vertex not support, please check the vertex named 'itmp_'\n"); }

    if($self->{DFG}->is_vertex_pre_stack_empty($vertex)!=0 &&
       $self->{DFG}->is_vertex_nxt_stack_empty($vertex)!=0 ){ 
    
    my $pre_arr = $self->{DFG}->get_vertex_pre_stack($vertex);  
    my $nxt_arr = $self->{DFG}->get_vertex_nxt_stack($vertex); 

      $self->{DFG}->del_time_weighted_vertex($vertex);

    foreach my $pre (@{$pre_arr}){
     foreach my $nxt (@{$nxt_arr}){
         $self->{DFG}->del_time_weighted_edge($pre->[0],$vertex);
         $self->{DFG}->del_time_weighted_edge($vertex,$nxt->[0]);
         $self->{DFG}->set_time_weighted_edge($pre->[0],$nxt->[0],0,$nxt->[1]);
    }
   }
 }
}

sub updt_remove_rev_key_DFG {
  my ($self) = (@_);

  for(my $i=0; $i<$self->{ass_table}->{rev_key}->{id}; $i++){
     my $vertex = $self->crt_rev_key($i);
        $self->del_itmp_time_weighted_vertex($vertex);
   }
}

sub updt_split_feedback_DFG {
   my ($self) = (@_);

   my $all_vex = $self->{DFG}->get_all_vertices();

   my @arr;
   foreach my $vex (@{$all_vex}){
      if($vex=~ /\=\:\:/){
        push (@arr,$vex); 
      }
  }

  foreach my $ass (@arr){
        if( $self->{DFG}->is_vertex_nxt_stack_empty($ass)!=0 ){
         my $nxt_arr = $self->{DFG}->get_vertex_nxt_stack($ass);

        foreach my $src (@{$nxt_arr}){
          if( $self->{DFG}->is_vertex_nxt_stack_empty($src->[0])!=0 ){
                  my $rst_arr = $self->{DFG}->get_vertex_nxt_stack($src->[0]);
                     $self->{DFG}->del_time_weighted_edge($ass,$src->[0]);
                     $self->{DFG}->del_time_weighted_vertex($src->[0]);
                  my $wt_name = $self->crt_split_write_rename($src->[0]);
                     $self->{DFG}->set_time_weighted_vertex($wt_name,0);
                     $self->{DFG}->set_time_weighted_edge($ass,$wt_name,0,'dst');
                  my @rst_arr = @{$rst_arr};

            foreach my $nxt (@rst_arr){
                  my $rd_name = $self->crt_split_read_rename($src->[0]);
                     $self->{DFG}->set_time_weighted_vertex($rd_name,0);
                     $self->{DFG}->set_time_weighted_edge($wt_name,$rd_name,0,$nxt->[1]);     
                     $self->{DFG}->set_time_weighted_edge($rd_name,$nxt->[0],0,$nxt->[1]);     
                     $self->{DFG}->del_time_weighted_edge($src->[0],$nxt->[0]);
                 }
              }           
           }
       }
   }
}

sub run_updt_DFG {
   my ($self) = (@_); 
 
 $self->updt_remove_rev_key_DFG();
 $self->updt_split_feedback_DFG();
}

sub free {
   my ($self) = (@_);
       $self = {};
}

1;



