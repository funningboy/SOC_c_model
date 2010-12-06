
package SysPerl::CDFG_If;
use SysPerl::DFG;
use SysPerl::syntax2DFG;
use Data::Dumper;
use Switch;
use strict;

sub new {
  my $class = shift;
  my $self = {
            CDFG_if_id           => 0,
            CDFG_if_condition    => {},
            CDFG_if_body         => {},
            CDFG_if_end          => {},

            CDFG_else_id         => 0,
            CDFG_else_condition  => {},
            CDFG_else_body       => {},
            CDFG_else_end        => {},

            CDFG_elsif_id        => 0,
            CDFG_elsif_condition => {},
            CDFG_elsif_body      => {}, 
            CDFG_elsif_end       => {},

            CDFG_stack           => [],
            CDFG                 => SysPerl::DFG->new(),
#            CDFG_symbol_stack    => [],
#            CDFG_operator_stack  => [],
   };
   bless $self, $class;
   return $self;
}

sub inc_CDFG_if_id {
   my ($self) = (@_);
       $self->{CDFG_if_id}++;
}

sub dec_CDFG_if_id {
   my ($self) = (@_);
       $self->{CDFG_if_id}--;
}

sub get_CDFG_if_id {
   my ($self) = (@_);
return $self->{CDFG_if_id};
}

sub inc_CDFG_else_id {
   my ($self) = (@_);
       $self->{CDFG_else_id}++;
}

sub dec_CDFG_else_id {
   my ($self) = (@_);
       $self->{CDFG_else_id}--;
}

sub get_CDFG_else_id {
   my ($self) = (@_);
return $self->{CDFG_else_id};
}

sub inc_CDFG_elsif_id {
   my ($self) = (@_);
       $self->{CDFG_elsif_id}++;
}

sub dec_CDFG_elsif_id {
   my ($self) = (@_);
       $self->{CDFG_elis_id}--;
}

sub get_CDFG_elsif_id {
   my ($self) = (@_);
return $self->{CDFG_elsif_id};
}

sub is_CDFG_stack_empty {
   my ($self) = (@_);
   if( !@{$self->{CDFG_stack}} ){ return 0; }
return -1;
}

sub push_CDFG_stack {
   my ($self,$syntax) = (@_);
   push(@{$self->{CDFG_stack}},$syntax);
}

sub pop_CDFG_stack {
   my ($self) = (@_);
return pop(@{$self->{CDFG_stack}});
}


#sub set_CDFG_symbol_stack {
#   my ($self,$symbol_stack) = (@_);
#       @{$self->{CDFG_symbol_stack}} = @{$symbol_stack};
#}
#
#sub clr_CDFG_symbol_stack {
#   my ($self) = (@_);
#      @{$self->{CDFG_symbol_stack}} = [];
#}
#
#sub is_CDFG_symbol_empty {
#    my ($self) = (@_);
#    if( !@{$self->{CDFG_symbol_stack}} ){ return 0; }
#return -1;
#}
#
#sub shift_CDFG_symbol_stack {
#   my ($self) = (@_);
#return shift( @{$self->{CDFG_symbol_stack}} );     
#}
#
#sub pop_CDFG_symbol_stack {
#   my ($self) = (@_);
#return pop( @{$self->{CDFG_symbol_stack}} );
#}
#
#sub set_CDFG_operator_stack {
#   my ($self,$operator_stack) = (@_);
#       @{$self->{CDFG_operator_stack}} = @{$operator_stack};
#}
#
#sub clr_CDFG_operator_stack {
#   my ($self) = (@_);
#      @{$self->{CDFG_operator_stack}} = [];
#}
#
#sub is_CDFG_operator_empty {
#    my ($self) = (@_);
#    if( !@{$self->{CDFG_operator_stack}} ){ return 0; }
#return -1;
#}
#
#sub shift_CDFG_operator_stack {
#   my ($self) = (@_);
#return shift( @{$self->{CDFG_operator_stack}} );     
#}
#
#sub pop_CDFG_operator_stack {
#   my ($self) = (@_);
#return pop( @{$self->{CDFG_operator_stack}} );
#}

sub get_CDFG_name {
   my ($self,$syntax) = (@_);
  
   switch($syntax) {
     case 'CDFG_if_condition'   { return 'CDFG_if_condition_'  .$self->get_CDFG_if_id();   }
     case 'CDFG_if_body'        { return 'CDFG_if_body'        .$self->get_CDFG_if_id();   }
     case 'CDFG_if_end'         { return 'CDFG_if_end'         .$self->get_CDFG_if_id();   }
     case 'CDFG_else_condition' { return 'CDFG_else_condition' .$self->get_CDFG_else_id(); }
     case 'CDFG_else_body'      { return 'CDFG_else_body'      .$self->get_CDFG_else_id(); }
     case 'CDFG_else_end'       { return 'CDFG_else_end'       .$self->get_CDFG_else_id(); }
     case 'CDFG_elsif_condition'{ return 'CDFG_elsif_condition'.$self->get_CDFG_elsif_id();}
     case 'CDFG_elsif_body'     { return 'CDFG_elsif_body'     .$self->get_CDFG_elsif_id();}
     case 'CDFG_elsif_end'      { return 'CDFG_elsif_end'      .$self->get_CDFG_elsif_id();} 
   }
}

#===========================
# if condition
# if( ina == x i || inb == y ) -> ina == x ||
#                                 inb == y
#===========================
sub set_CDFG_if_condition {
   my ($self,$syntax) = (@_);
               
              $self->inc_CDFG_if_id();
   my $name = $self->get_CDFG_name('CDFG_if_condition');

# DFG cerate
#   $self->{CDFG_if_condition}->{$name} = SysPerl::syntax2DFG->new($syntax);
#   $self->{CDFG_if_condition}->{$name}->run_text();
#   #$self->{CDFG_if_body}->{$name}->dump_graph();
#   $self->{CDFG_if_body}->{$name}->dump_graphviz();


# CFG create
       $self->{CDFG}->set_time_weighted_vertex($name,0);

    if($self->is_CDFG_stack_empty()!=0){
       my $src = $self->pop_CDFG_stack();
       $self->{CDFG}->set_time_weighted_edge($src,$name);   
   }   
       $self->push_CDFG_stack($name);
}

sub set_CDFG_if_body {
   my ($self,$syntax) = (@_);

   my $name = $self->get_CDFG_name('CDFG_if_body');
# DFG cerate
   if($syntax){
   $self->{CDFG_if_body}->{$name} = SysPerl::syntax2DFG->new($syntax);
   $self->{CDFG_if_body}->{$name}->run_text();
#   #$self->{CDFG_if_body}->{$name}->dump_graph();
   $self->{CDFG_if_body}->{$name}->dump_graphviz();
##   $self->{CDFG_if_body}->{$name}->get_DFG_graph();
##   $self->{CDFG_if_body}->{$name}->free();
}

# CFG create
      $self->{CDFG}->set_time_weighted_vertex($name,0);

    if($self->is_CDFG_stack_empty()!=0){
       my $src = $self->pop_CDFG_stack();
       $self->{CDFG}->set_time_weighted_edge($src,$name);   
   }   
       $self->push_CDFG_stack($name); 
}

sub set_CDFG_if_end {
   my ($self,$syntax) = (@_);

   my $name = $self->get_CDFG_name('CDFG_if_end');
# DFG cerate
   if($syntax){

   }
# CFG create
      $self->{CDFG}->set_time_weighted_vertex($name,0);

    if($self->is_CDFG_stack_empty()!=0){
       my $src = $self->pop_CDFG_stack();
       $self->{CDFG}->set_time_weighted_edge($src,$name);   
   }   
    $self->push_CDFG_stack($name);
    $self->dec_CDFG_if_id(); 
}


sub get_CDFG_if_condition {
   my ($self,$if_id) = (@_);
#return $self->{CDFG_if_condition}->{$if_id};
}

sub dump_CDFG {
   my ($self) = (@_);
       $self->{CDFG}->dump_graph();
}


1;
