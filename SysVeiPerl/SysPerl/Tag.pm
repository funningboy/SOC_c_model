package SysPerl::Tag;
use SysPerl::CDFG_If;
use Data::Dumper;
use Switch;
use strict;

sub new {
    my $class = shift;
    my $self = {
                 tag             => {},
                 keyword_hstable => {},
                 key_stack       => [],
                 symbol_stack    => [],
                 operator_stack  => [],
                 number_stack    => [],
    };
    bless $self, $class;
    return $self;
}

sub set_keyword_hstable {
    my ($self) = (@_);
        $self->{keyword_hstable} => {
                                      module     => 0,
                                      endmodule  => 1,
                                      input      => 2,
                                      output     => 3,
                                      wire       => 4,
                                      reg        => 5,
                                      parameter  => 6,
                                      begin      => 7,
                                      end        => 8,
                                      assign     => 9,
                                      always     => 10,
                                      posedge    => 11,
                                      negedge    => 12,
                                      if         => 13,
                                      else       => 14,
                                      for        => 15,
                                      case       => 16,
                                      endcase    => 17,
                                      default    => 18,
                                    };
}

sub set_operator_boundary_hstable {
    my ($self) = (@_);
        $self->{operator_boundary} => {
                                      '(' => 0,
                                      ')' => 1,
                                      '[' => 2,
                                      ']' => 3,
                                      ':' => 4,
                                      ';' => 5,
                                      };
}

sub is_key_stack_empty {
   my ($self) = (@_);
   if ( !@{$self->{key_stack}} ){ return 0; }
return -1; 
}

sub push_key_stack {
    my ($self,$st) = (@_);
    push (@{$self->{key_stack}},$st);
}

sub shift_key_stack {
    my ($self) = (@_);
return shift (@{$self->{key_stack}});   
}

sub is_symbol_stack_empty {
    my ($self) = (@_);
        if( !@{$self->{symbol_stack}} ){ return 0; }
return -1;
}

sub push_symbol_stack {
    my ($self,$st) = (@_);
    push (@{$self->{symbol_stack}},$st);
}

sub shift_symbol_stack {
   my ($self) = (@_);
return shift (@{$self->{symbol_stack}});
}

sub is_operator_stack_empty {
   my ($self) = (@_);
      if( !@{$self->{operator_stack}} ){ return 0; }
return -1;
}

sub push_operator_stack {
    my ($self,$st) = (@_);
    push (@{$self->{operator_stack}},$st);
}

sub shift_operator_stack {
    my ($self) = (@_);
return shift (@{$self->{operator_stack}});
}

sub is_number_stack_empty {
    my ($self) = (@_);
    if( !@{$self->{number_stack}} ){ return 0; }
return -1;
}

sub push_number_stack {
    my ($self,$st) = (@_);
    push (@{$self->{number_stack}},$st);
}

sub shift_number_stack {
    my ($self);
return shift (@{$self->{number_stack}});
}


sub get_keyword_hstable {
    my ($self,$name) = (@_);
    if( $self->{keyword_hstable}->{$name} ){ return $self->{keyword_hstable}; }
return -1;
}

sub set_module_name {
    my ($self,$name) = (@_);
        $self->{module_name} = $name;
}

sub get_module_name {
    my ($self) = (@_);
    return $self->{module_name};
}

sub set_module_input_hstable {
    my ($self,$name,$len) = (@_);
       push (@{$self->{module_input}}, { name   => $name,
                                        length => $len,
                                      }); 
}

sub get_module_input_hstable {
    my ($self) = (@_);
    return  $self->{module_input};
}

sub set_module_output_hstable {
    my ($self,$name,$len) = (@_);
       push (@{$self->{module_output}}, { name   => $name,
                                          length => $len,
                                        });
}

sub get_module_output_hstable {
   my ($self) = (@_);
   return $self->{module_output};
}

sub set_module_wire_hstable {
   my ($self,$name,$len) = (@_);
      push (@{$self->{module_wire}}, { name   => $name,
                                       length => $len,
                                     });  
}

sub get_module_wire_hstable {
    my ($self) = (@_);
    return $self->{module_wire};
}

sub set_module_reg_hstable {
   my ($self,$name,$len) = (@_);
      push (@{$self->{module_reg}},  { name   => $name,
                                       length => $len
                                     }); 
}

sub get_module_reg_hstable {
   my ($self) = (@_);
   return $self->{module_reg};
}

sub set_module_sensitive_hstable {

} 

sub set_module_always_hstable {

}

sub set_module_assign_hstable {
    my ($self) = (@_);
}

sub set_module_parameter_hstable {
    my ($self,$smb,$num) = (@_);
        $self->{module_parameter}->{$smb} = $num;
}

sub get_module_parameter_hstable {
    my ($self,$smb) = (@_);
return $self->{module_parameter}->{$smb};
}

sub set_interface_dec {

}

#===============================
# parameter dec ex: parameter WIDTH =8; set WIDTH(8)
# return 
# set module_parameter_hstable($symbol,$number);
#===============================
sub set_parameter_dec {
   my ($self) = (@_);
   my $symbol = ( $self->is_symbol_stack_empty()!=0 )? $self->shift_symbol_stack() : 
                                                       error("internal set_parameter_dec error\n");

   my $operator = ( $self->is_operator_stack_empty()!=0 )? $self->shift_operator_stack() :
                                                           error("internal set_parameter_dec error\n");

   if($operator != '='){ error("we olny support = in parameter set\n"); }

   my $number   = ( $self->is_number_stack_empty()!=0 )? $self->shift_number_stack() :
                                                         error("internal set_number_dec error\n");

     $self->set_module_parameter_hstable($symbol,$number);
}

#============================
# alu dec 
# return rst src1 op src2
#============================
sub set_alu_dec {
   my ($self,$src1,$src2,$op) = (@_);
   switch($op){
     case '+' { return $src1 + $src2; }
     case '-' { return $src1 - $src2; }
     case '*' { return $src1 * $src2; }
     case '/' { return $src1 / $src2; }
     case '%' { return $src1 % $src2; }
#     case '>>' { return $src1 >> $src2; }
#     case '<<' { return $src1 << $src2; }
  }
}


#==============================
# port dec ex: input [9:0] ina -> dec the length 9~0 =10'bits
# return 
# length
#==============================
sub set_port_dec {
    my ($self) = (@_);
    my ($dst,$src1,$src2,$operator,$symbol);

    if( $self->is_symbol_stack_empty()  !=0 && 
        $self->is_operator_stack_empty()!=0 &&
        $self->is_number_stack_empty()  !=0  ){ 
       
        $symbol   = $self->shift_symbol_stack();
        $src1     = $self->get_module_parameter_hstable($symbol);
        $operator = $self->shift_operator_stack();
#        $src2     = $self->shift_number_stack();
        $src2     = $self->shift_symbol_stack();

        $src1      = $self->alu_dec($src1,$src2,$operator);
       } else {
        $src1      = $self->shift_number_stack(); 
       }
   
     if( $self->is_operator_stack_empty()!=0 &&
         $self->is_number_stack_empty()  !=0  ){ 
         $operator = $self->shift_operator_stack();
         $src2     = $self->shift_number_stack();
      
         my $len = ($self->alu_dec($src1,$src2,'-')+1);

         if( $operator  != ':' ||
             $len       < 0    ){ error("internal set_port_dec error\n");  }
           
             return $len;
  }
}

sub inc_always_block_id {
    my ($self) = (@_);
        $self->{always_block_id}++;
}

sub get_always_block_id {
   my ($self) = (@_);
return $self->{always_block_is};
}

#===============================
# always block id dec 
# return always_block_id++
#===============================
sub set_always_block_dec {
    my ($self) = (@_);

    my ($key,$operator);
    if( $self->is_key_stack_empty()     !=0 &&
        $self->is_operator_stack_empty()!=0 ){

        $key      = $self->shift_key_stack();
        $operator = $self->shift_operator_stack();

        if( $key eq 'always' && $operator eq '@' ){ $self->inc_always_block_id(); }
    }
}

#==============================
# senstive dec
# ex: always( state or posedge clk )  -> state,clk.pos()
# return 
# set_module_senstive_hstable($symbol)
#==============================
sub set_sensitive_dec {
    my ($self) = (@_);

    my $blk_id = $self->get_always_block_id();

    my ($key,$symbol);

    while( $self->is_key_stack_empty()!=0 ){
           $key = $self->shift_key_stack();

           if( $key eq 'posedge' || $key eq 'negedge' ){
               $symbol = ( $self->is_symbol_stack_empty()!=0 )? $self->shift_symbol_stack() : 
                                                                error("internal set_senstive_dec error\n");
               $symbol .= ($key eq 'posedge')? '.pos()' :
                          ($key eq 'negedge')? '.neg()' :-1;

               $self->push_symbol_stack($symbol);
           }
 
               $symbol = ( $self->is_symbol_stack_empty()!=0 )? $self->shift_symbol_stack() : 
                                                                error("internal set_senstive_dec error\n");
               $self->set_module_senstive_hstable($symbol);           
       }

               $symbol = ( $self->is_symbol_stack_empty()!=0 )? $self->shift_symbol_stack() : 
                                                                error("internal set_senstive_dec error\n");
               $self->set_module_senstive_hstable($symbol);           
}

#
sub set_CDFG_dec {

} 



#sub set_tag2Graph {
#    my ($self,$name) = (@_);
#
##    switch($self->{tag}){
##     case '0'  {  }
##     case '1'  { }
##     
##   }
#}



sub get_token_file {
   my ($self,$path) = (@_);

  open(iPtr,"$path") || die print "open $path error\n";
  while(<iPtr>){
    chomp;
    my ($key,$name) = split(' ',$_);

    switch($key){
      case 'key'      { }
      case 'symbol'   { }
      case 'operator' { }
      case 'number'   { }
    }
#    print  $self->get_keyword_hstable($key);       

  }
 
 close(iPtr);
}


1;
