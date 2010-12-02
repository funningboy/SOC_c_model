
#! /usr/bin/perl 
package SYSVEI::VeiPerl;
use Data::Dumper;
use strict;

sub new {
my $class = shift;
my $self = { module => {
                 name      => {},
                 interface => {
                          input  => {},
                          output => {},
                          reg    => {},
                          wire   => {},
                              },
                 section   => {
                          senstive => [],
#                          func     => (),
                              },
              },
            };
   bless $self, $class;
   return $self;
} 

sub set_module_name {
my  ($self,$name) = (@_);
     $self->{module}->{name} = $name;
}


sub set_ports {
my  ($self,$pths) = (@_);

my  @key0 = keys %{$pths}; 

    foreach my $key (@key0){

  } 
}

sub set_sensitive_block {

}

sub set_block_func {

}

sub set_end_module {

} 

1;
