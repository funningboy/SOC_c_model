
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
                                      '>>' => 0, #rsht
                                      '<<' => 0, #lsht
                                    },
                               1 => {
                                      '+' => 0, #add
                                      '-' => 0, #sub
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


sub crt_DFG_vertex {
    my ($self,$v) = (@_);
        $self->{DFG}->set_time_weighted_vertex($v,0);
}

sub crt_DFG_edge {
    my ($self,$s,$d) = (@_);
        $self->{DFG}->set_time_weighted_edge($s,$d,0);
}

sub crt_rev_key {
    my ($self,$id) = (@_);
    return $self->{ass_table}->{rev_key}->{name}.$id;
}

sub crt_op_rename {
    my ($self,$st,$lvl) = (@_);

    if( defined($self->{pri_table}->{0}->{$st} )){
        my $id = $self->{pri_table}->{0}->{$st}++;
        return $st."::".$id."::".$lvl;
    }

    if( defined($self->{pri_table}->{1}->{$st} )){
        my $id = $self->{pri_table}->{1}->{$st}++;
        return $st."::".$id."::".$lvl;
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
#   die;
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
                      $self->crt_DFG_vertex($op);
                      $self->crt_DFG_vertex($src1);
                      $self->crt_DFG_vertex($src2);
                      $self->crt_DFG_vertex($dst);
                      $self->crt_DFG_edge($src1,$op);
                      $self->crt_DFG_edge($src2,$op);
                      $self->crt_DFG_edge($op,$dst);
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
          $self->crt_DFG_vertex($op);
          $self->crt_DFG_vertex($src1);
          $self->crt_DFG_vertex($src2);
          $self->crt_DFG_vertex($dst);
   
          $self->crt_DFG_edge($src1,$op);
          $self->crt_DFG_edge($src2,$op);
          $self->crt_DFG_edge($op,$dst);
        
          $self->push_st_stack($dst);
    }

}

sub get_assign_text {
    my ($self) = (@_);
    
    my @arr = @{$self->{text}};
 
    if( $arr[1] ne '=' && $arr[3] eq '=' ){ error("we only support a = b;, not suport a = b = c;\n"); }

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
       $self->crt_DFG_vertex($op);
       $self->crt_DFG_vertex($dst);
       $self->crt_DFG_vertex($src);

       $self->crt_DFG_edge($src,$op);
       $self->crt_DFG_edge($op,$dst);
 
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

sub dump_graphviz_file {
    my ($self,$path) = (@_);
        $self->{DFG}->dump_graphviz_file($path);
}

sub dump_graph_ascii {
    my ($self) = (@_);
        $self->{DFG}->dump_graph_ascii();
}

sub get_deep_copy_DFG {
    my ($self) = (@_);
return $self->{DFG};
}

sub get_deep_copy_graph {
   my ($self) = (@_);
return $self->{DFG}->get_deep_copy_graph();
}

sub get_vertices {
   my ($self) = (@_);
return $self->{DFG}->get_all_vertices();
}

sub get_edges {
  my ($self) = (@_);
return $self->{DFG}->get_all_edges();
}

sub free {
   my ($self) = (@_);
       $self = {};
}

1;



