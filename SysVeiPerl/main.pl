#!/usr/bin/perl

use SYSVEI::VeiPerl;

# create module 
my $mm = SYSVEI::VeiPerl->new();
   $mm->set_module_name('FIR');

my %mm_port = {
   input => {
        in_a => 32,
        in_b => 32,
        clk  =>  1,
        rst  =>  1,
            },
   output => {
        ot_c => 32,
   },
};

my %mm_block = { 
   senstive => [ 'clk.pos', 
                 'rst',
               ],
   func     => ' 
               if( rst==1 ){
                   ot_c = in_b + ina;
               }
              ' 
 };

   $mm->set_ports(\%mm_inport);

#   $mm->set_reg();
#    $mm->set_wire();

my $blk0 = $mm->set_sensitive_block();
#         $blk0->set_block_func();

my $blk1 = $mm->set_sensitive_block();
#         $blk1->set_block_func();

   $mm->set_end_module();

#$sys->run();

