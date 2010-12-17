#!/usr/bin/perl
 
package SysPerl::arch2DFG;
use SysPerl::DFG;
use SysPerl::schedule2DFG;
use strict;
use Data::Dumper;

sub new {
  my $class = shift;
  my $self  = {
      sched2DFG  => SysPerl::schedule2DFG->new(),
      PE_list    => {}, # proce element for block
      alu_list   => {},
      reg_list   => {},
      SystemC    => {
                   module => 'SAMPLE',
                   define => 0,
                   port   => {
                               length => 32,
                               type   => 'sc_uint',

                               clk    => 'HCLK',
                               rst    => 'HRESTn',
                               en     => 'HEN',
                               done   => 'HDON',
                               cur_st => '_itmp_cur_st',
                               nxt_st => '_itmp_nxt_st',
                              
                               reg_key  => '_itmp_reg_', 
                               idle_key => '_st_idle_',
                               done_key => '_st_done_',
                               cycle_key=> '_st_cycle_',
                           },
                   sensitive => {
                              ini_st  => {
                                  name =>'_iproc_ini_st',
                                  list =>[ 
                                           'HCLK.pos()',
                                           'HRESTn',
                                         ],
                                        },
                              proc_st => {
                                  name =>'_iproc_run_st',
                                  list =>[ 
                                          '_itmp_cur_st',
                                          'HEN',
                                         ],
                                        },
                              done_st => {
                                   name =>'_iproc_done_st',
                                   list =>[ 
                                           '_itmp_cur_st',
                                         ],
                                        },
                              pe_st   => { 
                                   name => '_iproc_pe_st_',
                                   list =>[ 
                                           '_itmp_cur_st',
                                          ],
                                        },
                           },
                    },
  };
  bless $self, $class;
  return $self;
}


sub set_deep_sched2arch {
   my ($self,$sched) = (@_);
       $self->{sched2DFG} = $sched;
}

sub is_alu_cycle_list_empty {
   my ($self,$cycle) = (@_);
   
   if (   !$self->{alu_list}->{$cycle} && 
        !@{$self->{alu_list}->{$cycle}} ){ return 0; }
return -1;
}

sub push_alu_cycle_list {
   my ($self,$cycle,$vertex) = (@_);
   push (@{$self->{alu_list}->{$cycle}},$vertex);
}

sub pop_alu_cycle_list {
   my ($self,$cycle) = (@_);
return pop (@{$self->{alu_list}->{$cycle}});
}

sub shft_alu_cycle_list {
   my ($self,$cycle) = (@_);
return shift (@{$self->{alu_list}->{$cycle}});
}

sub get_re_W_REG_name {
    my ($self,$vex) = (@_);
   return $self->{reg_list}->{W_REG}->{$vex}; 
}



sub get_re_name {
    my ($self,$vex) = (@_);
        if( $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{ASSIGN}->{$vex} ){
            $vex =~ s/\=\:\:[\@\_0-9]*//g;
    }elsif( $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{W_REG}->{$vex}  ){
            $vex =~ s/w\:\:\@\_//g;
            $vex =  $self->{SystemC}->{port}->{reg_key}.$vex;
    }elsif( $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{R_REG}->{$vex} ){
            $vex =~ s/r\:\:\@\_//g;
            $vex =  $self->{SystemC}->{port}->{reg_key}.$vex.'.read()';
    }elsif( $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{IN}->{$vex}    ){
            if( $vex !~ /^[0-9]*$/ ){
               $vex = $vex.'.read()';
            }
    }elsif( $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{ALU}->{$vex} ){
            $vex =~ s/\:\:[0-9]*//g;
    }

return $vex;
}


sub run_ALU_cycle_path {
   my ($self,$cyc,$vex) = (@_);

             $self->{sched2DFG}->{cons2DFG}->{NewDFG}->sort_vertex_pre_stack($vex);

       while($self->{sched2DFG}->{cons2DFG}->{NewDFG}->is_vertex_pre_stack_empty($vex)!=0){
         my $pre_vex = $self->{sched2DFG}->{cons2DFG}->{NewDFG}->pop_vertex_pre_stack($vex);               

              my $re_nm = $self->get_re_name($vex);             
 
                 $self->push_alu_cycle_list($cyc,$re_nm);

               if($pre_vex->[0]=~/[\=]/){
                  $self->push_alu_cycle_list($cyc,'=');
                  $self->run_ALU_cycle_path($cyc,$pre_vex->[0]);
           } elsif($pre_vex->[0]=~/[\+\-\*\/\%\>\>\<\<]/){
               if($pre_vex->[1] eq 'src1'){  $self->pop_alu_cycle_list($cyc); }
                  $self->run_ALU_cycle_path($cyc,$pre_vex->[0]);
           } else {
              if($pre_vex->[1] eq 'src1'){  $self->pop_alu_cycle_list($cyc); }
                 my $re_nm = $self->get_re_name($pre_vex->[0]);
                    $self->push_alu_cycle_list($cyc,$re_nm);
           }       
       }
}


sub run_ALU_cluster {
   my ($self) = (@_);

   my $cstp_w_reg_all = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{W_REG};
   foreach my $ky (keys %{$cstp_w_reg_all}){
       my $cyc = $cstp_w_reg_all->{$ky}->{begin};     
          $self->run_ALU_cycle_path($cyc,$ky); 
          $self->push_alu_cycle_list($cyc,';');
   }

   my $cstp_out_all = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{OUT};
   foreach my $ky (keys %{$cstp_out_all}){
       my $cyc = $cstp_out_all->{$ky}->{begin};     
          $self->run_ALU_cycle_path($cyc,$ky); 
          $self->push_alu_cycle_list($cyc,';');
   }
}

sub run_SystemC_header_define {
    my ($self) = (@_);

    print hpath "enum{\n";
    print hpath $self->{SystemC}->{port}->{idle_key}.",\n";
    print hpath $self->{SystemC}->{port}->{done_key}.",\n";

    my $pe_list = $self->{sched2DFG}->{distrib_list};
    foreach my $pe (sort keys %{$pe_list}){
     print hpath $self->{SystemC}->{port}->{cycle_key}.$pe.",\n";
    }
     print hpath "};\n";
     print hpath "\n"; 
}

sub run_SystemC_header_port {
     my ($self) = (@_);

     print hpath 'sc_in<bool> '.$self->{SystemC}->{port}->{clk}.";\n";
     print hpath 'sc_in<bool> '.$self->{SystemC}->{port}->{rst}.";\n";
     print hpath 'sc_in<bool> '.$self->{SystemC}->{port}->{en}.";\n";
     print hpath 'sc_out<bool> '.$self->{SystemC}->{port}->{done}.";\n";
     print hpath "\n";
 
     my $in_list = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{IN};
     foreach my $in (keys %{$in_list}){
        if($in!~/^[0-9]*$/){
          print hpath 'sc_in<'.$self->{SystemC}->{port}->{type}.'<'.$self->{SystemC}->{port}->{length}.'> > '.$in.";\n";
        }
     }
     print hpath "\n";
 
     my $out_list = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{OUT};
     foreach my $out (keys %{$out_list}){
        print hpath 'sc_out<'.$self->{SystemC}->{port}->{type}.'<'.$self->{SystemC}->{port}->{length}.'> > '.$out.";\n";
     }
     print hpath "\n";
 
     my $reg_list = $self->{sched2DFG}->{cons2DFG}->{cstep_list}->{W_REG};
     foreach my $reg (keys %{$reg_list}){
        my $re_nm = $self->get_re_name($reg);
        print hpath 'sc_signal<'.$self->{SystemC}->{port}->{type}.'<'.$self->{SystemC}->{port}->{length}.'> > '.$re_nm.";\n";
     }
    print hpath "\n";
 
    print hpath 'sc_signal<'.$self->{SystemC}->{port}->{type}.'<'.$self->{SystemC}->{port}->{length}.'> > '.$self->{SystemC}->{port}->{cur_st}.";\n";
    print hpath 'sc_signal<'.$self->{SystemC}->{port}->{type}.'<'.$self->{SystemC}->{port}->{length}.'> > '.$self->{SystemC}->{port}->{nxt_st}.";\n";

}

sub run_SystemC_header_sensitive {
    my ($self) = (@_);

    print hpath 'SC_METHOD('.$self->{SystemC}->{sensitive}->{ini_st}->{name}.");\n";
    print hpath "dont_initialize();\n";
    my $sen_list = $self->{SystemC}->{sensitive}->{ini_st}->{list};     
    foreach my $sen (@{$sen_list}){
      print hpath 'sensitive << '.$sen.";\n";
    }
    print hpath "\n";

    print hpath 'SC_METHOD('.$self->{SystemC}->{sensitive}->{proc_st}->{name}.");\n";
    print hpath "dont_initialize();\n";
    my $sen_list = $self->{SystemC}->{sensitive}->{proc_st}->{list};     
    foreach my $sen (@{$sen_list}){
      print hpath 'sensitive << '.$sen.";\n";
    }
    print hpath "\n";

    print hpath 'SC_METHOD('.$self->{SystemC}->{sensitive}->{done_st}->{name}.");\n";
    print hpath "dont_initialize();\n";
    my $sen_list = $self->{SystemC}->{sensitive}->{done_st}->{list};     
    foreach my $sen (@{$sen_list}){
      print hpath 'sensitive << '.$sen.";\n";
    }
    print hpath "\n";


    my $pe_list = $self->{sched2DFG}->{distrib_list};
    foreach my $pe (sort keys %{$pe_list}){
     print hpath 'SC_METHOD('.$self->{SystemC}->{sensitive}->{pe_st}->{name}.$pe.");\n";
     print hpath "dont_initialize();\n";
     my $sen_list = $self->{SystemC}->{sensitive}->{pe_st}->{list};     
     foreach my $sen (@{$sen_list}){
      print hpath 'sensitive << '.$sen.";\n";
    }
    print hpath "\n";
    }
}

sub run_SystemC_heaser_function {
    my ($self) = (@_);

    print hpath 'void '.$self->{SystemC}->{sensitive}->{ini_st}->{name}."();\n";
    print hpath 'void '.$self->{SystemC}->{sensitive}->{proc_st}->{name}."();\n";
    print hpath 'void '.$self->{SystemC}->{sensitive}->{done_st}->{name}."();\n";
    my $pe_list = $self->{sched2DFG}->{distrib_list};
    foreach my $pe (sort keys %{$pe_list}){
      print hpath 'void '.$self->{SystemC}->{sensitive}->{pe_st}->{name}.$pe."();\n";
    }
}

sub run_SystemC_body_ini_st {
   my ($self) = (@_);

   print bpath 'void '.$self->{SystemC}->{module}.'::'.$self->{SystemC}->{sensitive}->{ini_st}->{name}."(){\n";
   my $rd_pt_1 = $self->{SystemC}->{port}->{rst}.'.read()';
   my $rd_pt_2 = $self->{SystemC}->{port}->{nxt_st}.'.read()';
   my $wt_pt_1 = $self->{SystemC}->{port}->{cur_st};
   
   print bpath $wt_pt_1.' = ( '.$rd_pt_1.'== false'.' )? '.$self->{SystemC}->{port}->{idle_key}.' : '.$rd_pt_2.";\n";
   print bpath "}\n";
   print bpath "\n";
}

sub run_SystemC_body_proc_st {
  my ($self) = (@_);

  print bpath 'void '.$self->{SystemC}->{module}.'::'.$self->{SystemC}->{sensitive}->{proc_st}->{name}."(){\n";
  my $rd_pt_1 = $self->{SystemC}->{port}->{cur_st}.'.read()';
  my $rd_pt_2 = $self->{SystemC}->{port}->{en}.'.read()';
  my $wt_pt_1 = $self->{SystemC}->{port}->{nxt_st};

  print bpath 'switch( '.$rd_pt_1." ){\n";
  my $rd_pt_3 = ( $self->{sched2DFG}->{distrib_list}->{1} )? $self->{SystemC}->{port}->{cycle_key}.'1' : $self->{SystemC}->{port}->{don_key};
  print bpath 'case '.$self->{SystemC}->{port}->{idle_key}.' : '.$wt_pt_1.' = ( '.$rd_pt_2.' == true ) ? '.$rd_pt_3.' : '.
                      $self->{SystemC}->{port}->{idle_key}."; break;\n";
  print bpath 'case '.$self->{SystemC}->{port}->{done_key}.' : '.$wt_pt_1.' = '. $self->{SystemC}->{port}->{idle_key}."; break;\n";
  

  my $cyc_list = $self->{sched2DFG}->{distrib_list};
  foreach my $cyc (sort keys %{$cyc_list}){
  my $rd_pt_3 = ( $self->{sched2DFG}->{distrib_list}->{$cyc+1} )? $self->{SystemC}->{port}->{cycle_key}.($cyc+1) : $self->{SystemC}->{port}->{done_key};
  print bpath 'case '.$self->{SystemC}->{port}->{cycle_key}.$cyc.' : '.$wt_pt_1.' = '.$rd_pt_3."; break;\n";
  }

  print bpath "}\n";
  print bpath "}\n";
  print bpath "\n";
}

sub run_SystemC_body_pe_st {
    my ($self,$path) = (@_);

    my $alu_list = $self->{alu_list};
    foreach my $ky (sort keys %{$alu_list}){
     
      print bpath 'void '.$self->{SystemC}->{module}.'::'.$self->{SystemC}->{sensitive}->{pe_st}->{name}.$ky."(){\n";

      my $rd_pt_1 = $self->{SystemC}->{port}->{cur_st}.".read()";
      my $rd_pt_2 = $self->{SystemC}->{port}->{cycle_key}.$ky;

      print bpath '//@cycle '.$ky."\n";
      print bpath '//@power '.$self->{sched2DFG}->{distrib_list}->{$ky}."\n";
      print bpath 'if( '.$rd_pt_1.' == '.$rd_pt_2." ){\n";

      my $st  = join('',@{$self->{alu_list}->{$ky}} );
      my @arr = split(';',$st);  

      foreach my $st (@arr){
        print bpath $st.";\n";
      } 
      print bpath "}\n"; 
      print bpath "}\n";
      print bpath "\n";
   }
}

sub run_SystemC_body_done_st {
    my ($self) = (@_); 
   
  my $cyc_list = $self->{sched2DFG}->{distrib_list};
  my @cyc = (sort reverse keys %{$cyc_list});

  print bpath 'void '.$self->{SystemC}->{module}.'::'.$self->{SystemC}->{sensitive}->{done_st}->{name}."(){\n";
  my $rd_pt_1 = $self->{SystemC}->{port}->{cur_st}.".read()";
  my $rd_pt_2 = $self->{port}->{cycle_key}.$cyc[0];
  my $wt_pt_1 = $self->{SystemC}->{port}->{done};
  print bpath $wt_pt_1."=false;\n";
  print bpath 'if( '.$rd_pt_1.' == '.$rd_pt_2." ){\n";
  print bpath $wt_pt_1."=true;\n";
  print bpath "}\n";
  print bpath "}\n";
  print bpath "\n";
}

sub run_explore_SystemC_header{
   my ($self,$path) = (@_);

   open (hpath,">$path") || die "open $path error\n";
   print hpath "#include <systemc.h>\n";   
   print hpath "#include <iostream>\n";
   print hpath "\n";

   $self->run_SystemC_header_define();

   print hpath "SC_MODULE(".$self->{SystemC}->{module}."){\n";
   print hpath "\n";
 
   $self->run_SystemC_header_port();

   print hpath "\n";
   print hpath "SC_CTOR\(".$self->{SystemC}->{module}."){\n";
   print hpath "\n";

   $self->run_SystemC_header_sensitive();
   print hpath "};\n";
   print hpath "\n";
  
   $self->run_SystemC_heaser_function();
   print hpath "};\n";
   close(hpath);
}

sub run_explore_SystemC_body{
   my ($self,$header,$path) = (@_);

   open (bpath,">$path") || die "open $path error\n";
  
   print bpath '#include"'.$header."\"\n";
   print bpath "\n";
   $self->run_SystemC_body_ini_st();
   $self->run_SystemC_body_proc_st();
   $self->run_SystemC_body_pe_st();
   $self->run_SystemC_body_done_st();

   close(bpath); 
}

sub run_explore_SystemC {
   my ($self) = (@_);
   my  $header= $self->{SystemC}->{module}.'.h';
   my  $body  = $self->{SystemC}->{module}.'.cpp';

   $self->run_explore_SystemC_header($header);
   $self->run_explore_SystemC_body($header,$body);
}

sub run_explore_cycle{
   my ($self) = (@_);

    my $alu_list = $self->{alu_list};
    foreach my $ky (sort keys %{$alu_list}){
    print '//@cycle '.$ky."\n";
    print '//@power '.$self->{sched2DFG}->{distrib_list}->{$ky}."\n";

    my $st  = join('',@{$self->{alu_list}->{$ky}} );
    my @arr = split(';',$st);  

    foreach my $st (@arr){
      print $st.";\n";
    } 
     print "\n";
     print "\n";
  }
}

1;
