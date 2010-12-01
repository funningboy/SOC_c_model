#! /usr/bin/perl -w

# author  : sean chen 
# mail    : funningboy@gmail.com
# publish : 2010/12/01


package DFG2Graph;
use strict;
use Data::Dumper;
#use Graph;
use Graph::Easy;

sub new {
  my $class = shift;
  my $self  = { text      => shift || 'd=s+(a+b*c*d-e*s)+(c+s*d);', 
                tmp_txt   => '',
                rem_txt   => '',
                max_lvl   => 0,
                lvl_inx   => {
                              '(' => {},
                              ')' => {},
                             },
                op_stack  => [],
                st_stack  => [],
                Graph     => Graph::Easy->new(),  
                pri_table => {
                               0 => {
                                      '*' => 0,  #mul
                                      '/' => 0,  #div
                                      '%' => 0,  #rem
                                    },
                               1 => {
                                      '+'  => 0, #add
                                      '-'  => 0, #sub
#                                      '>>' => 0, #rsht
#                                      '<<' => 0, #lsht
                                    },
                            },
                ass_table => {
                             '='        => 0,       #ass
                             '('        => 0,       #lsr
                             ')'        => 0,       #rsr
                             ';'        => 0,       #end
                             'rev_key'  => { 
                                             name => 'itmp_', #reverse key
                                             id   =>  0, 
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


sub crt_graph_vertex {
    my ($self,$v) = (@_);
        $self->{Graph}->add_node($v);
}

sub crt_graph_edge {
    my ($self,$s,$d) = (@_);
        $self->{Graph}->add_edge($s,$d);
} 

sub crt_rev_key {
    my ($self,$id) = (@_);
    return $self->{ass_table}->{rev_key}->{name}.$id;
}

sub crt_op_rename {
    my ($self,$st) = (@_);

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
    chomp $self->{text};

      my $str1 = substr($self->{text},0,index($self->{text},'='));
      my $str2 = substr($self->{text},index($self->{text},'=')+1,index($self->{text},';')-2);
      $self->{text} = $str1.'=('.$str2.')'.';';
}

#======================================
# check max_level
#======================================
sub chk_max_level_text {
    my ($self) = (@_);      
    
    my $cur_lvl = 0;
    my $chr;

    for(my $i=0; $i<length($self->{text}); $i++){
       $chr = substr($self->{text},$i,1);
       if( $cur_lvl > $self->{max_lvl} ){ $self->{max_lvl} = $cur_lvl; }

       if( $chr eq '(' ){ $self->push_lvl_inx_lsr(++$cur_lvl, $i); }
       if( $chr eq ')' ){ $self->push_lvl_inx_rsr($cur_lvl--, $i); } 
   }
}

sub upd_regexp_text{
    my ($self) = (@_);
    my $st = $self->{tmp_text};
       $st =~ s/\+/\\\+/g;
       $st =~ s/\-/\\\-/g;
       $st =~ s/\*/\\\*/g;
       $st =~ s/\//\\\//g;
       $st =~ s/\;//g;
    return $st;
}

#===================================
# update levle text
#===================================
sub upd_level_text {
    my ($self) = (@_);
    my  $old = $self->upd_regexp_text($self->{tmp_text});
        $old = '\('.$old.'\)';
    my  $new = $self->get_st_stack();     
        $self->{text} =~ s/$old/$new/g;

        $self->clr_lvl_inx_lsr();
        $self->clr_lvl_inx_rsr();

        $self->{max_lvl} = 0;
}

#====================================
# get_level_text
#====================================
sub get_level_text {
    my ($self,$lvl) = (@_);

    my $bg_inx = $self->shft_lvl_inx_lsr($lvl);     
    my $ed_inx = $self->shft_lvl_inx_rsr($lvl);
    
    $self->{tmp_text} = substr($self->{text},$bg_inx+1,$ed_inx-$bg_inx-1).';';
}


sub chk_prity_text {
    my ($self) = (@_);     

    my $st  =();
    my $chr;
    for(my $i=0; $i<length($self->{tmp_text}); $i++){
       $chr = substr($self->{tmp_text},$i,1);

       if( defined($self->{pri_table}->{0}->{$chr}) || 
           defined($self->{pri_table}->{1}->{$chr}) ||   
           defined($self->{ass_table}->{$chr})        ){
           # check op_stack
               my $top = ($self->is_op_stack_empty()!=0 )? $self->get_top_op_stack() : -1;
               
               while( $self->is_op_stack_empty()!=0             &&
                      $self->is_st_stack_empty()!=0             && 
                      defined($self->{pri_table}->{1}->{$chr})  && 
                      defined($self->{pri_table}->{0}->{$top})     ){

                   my $op   = $self->pop_op_stack();
                      $op   = $self->crt_op_rename($op);
                   my $src1 = $self->pop_st_stack();    
                   my $src2 = $st;
                   my $dst  = $self->crt_rev_key($self->{ass_table}->{rev_key}->{id}++);

                   # call graph
                      $self->crt_graph_vertex($op);
                      $self->crt_graph_vertex($src1); 
                      $self->crt_graph_vertex($src2);
                      $self->crt_graph_vertex($dst);
                      $self->crt_graph_edge($src1,$op);
                      $self->crt_graph_edge($src2,$op);
                      $self->crt_graph_edge($op,$dst);
                  #    print $dst.','.$src1.','.$src2.','.$op."\n"; 
                      $st   = $dst; 
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
    my ($self) = (@_);

 while( $self->is_op_stack_empty()!=0  &&
        $self->is_st_stack_empty()!=0     ){

        my $op   = $self->pop_op_stack();
           $op   = $self->crt_op_rename($op);
 
        my $src2 = $self->pop_st_stack();
        my $src1 = $self->pop_st_stack();
        my $dst  = $self->crt_rev_key($self->{ass_table}->{rev_key}->{id}++);

        # graph create
          $self->crt_graph_vertex($op);
          $self->crt_graph_vertex($src1); 
          $self->crt_graph_vertex($src2);
          $self->crt_graph_vertex($dst);
   
          $self->crt_graph_edge($src1,$op);
          $self->crt_graph_edge($src2,$op);
          $self->crt_graph_edge($op,$dst);
        
          $self->push_st_stack($dst); 
    }

}

sub get_assign_text {
    my ($self) = (@_);
    
    my $st = substr($self->{text},0,index($self->{text},'='));
    $self->push_st_stack($st);
    $self->push_op_stack('=');   
}

sub chk_assign_text {
    my ($self) = (@_);
 
      my $op   = $self->pop_op_stack();
         $op   = $self->crt_as_rename($op);
 
     my $dst = $self->pop_st_stack();
     my $src = $self->pop_st_stack();

     # graph create
       $self->crt_graph_vertex($op);
       $self->crt_graph_vertex($dst);
       $self->crt_graph_vertex($src);

       $self->crt_graph_edge($src,$op);
       $self->crt_graph_edge($op,$dst);
 
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
     $self->chk_prity_text();
     $self->chk_remain_text();
     $self->upd_level_text();
     $self->chk_max_level_text();
     $lvl = $self->{max_lvl};
  }

     $self->get_assign_text();
     $self->chk_assign_text();
}

sub dump_graphviz {
    my ($self,$loc) = (@_);
    print $self->{Graph}->as_graphviz_file()."\n";
}

sub dump_graph {
    my ($self) = (@_);
        print $self->{Graph}->as_ascii()."\n";
}



1;


