package SysPerl::Token;
use Verilog::Parser;
use Verilog::Netlist;
use Data::Dumper;
@ISA = qw(Verilog::Parser);

# parse, parse_file, etc are inherited from Verilog::Parser
sub new {
    my $class = shift;
    #print "Class $class\n";
    my $self = $class->SUPER::new();
    bless $self, $class;
    return $self;
}

sub sysfunc {
    my $self  = shift;
    my $token = shift;

    print 'sysfunc'.' '.$token."\n";
}

sub attribute {
    my $self  = shift;
    my $token = shift;

    print 'attribute'.' '.$token."\n";
}

sub operator {
    my $self  = shift;
    my $token = shift;
   
    print 'operator'.' '.$token."\n";  
}

sub number {
    my $self  = shift;
    my $token = shift;
   
    print 'number'.' '.$token."\n";  
}

sub symbol {
    my $self  = shift;
    my $token = shift;
    
    print 'symbol'.' '.$token."\n";
}

sub keyword {
    my $self  = shift;
    my $token = shift;
   
    print 'key'.' '.$token."\n";
}

sub string {
    my $self  = shift;
    my $token = shift;
   
    print 'string'.' '.$token."\n";
}

 
