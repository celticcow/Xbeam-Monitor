#!/usr/bin/perl -w

#################################
#Crossbeam APM system monitor
#
#Poet: Gregory Dunlap (464480)
#
#Last Update: 04.24.08
#
#################################

use strict;

my $debug = 0;

my @cpulines = `/usr/bin/rsh -l root $ARGV[0] \"/bin/cat /proc/stat | /bin/grep -i \'cpu\'\"`;
my @meminfo  = `/usr/bin/rsh -l root $ARGV[0] \"/bin/cat /proc/meminfo | /bin/egrep -i \'Mem:|Swap:\'\"`;

chomp($cpulines[0]);
chomp($cpulines[1]);
chomp($cpulines[2]);
chomp($meminfo[0]);
chomp($meminfo[1]);

my @cpu0 = split(" ", $cpulines[0]);
my @cpu1 = split(" ", $cpulines[1]);
my @cpu2 = split(" ", $cpulines[2]);
my @mem  = split(" ", $meminfo[0]);
my @swap = split(" ", $meminfo[1]);


if($debug == 1)
{
    print("------------------------\n");
    print("CPU0\n");
    print("$cpu0[0]\n");
    print("$cpu0[1]\n");
    print("$cpu0[2]\n");
    print("$cpu0[3]\n");
    print("$cpu0[4]\n");
    print("$cpu0[5]\n");
    print("$cpu0[6]\n");
    print("$cpu0[7]\n");
    print("------------------------\n");
}

my $total00 = $cpu0[1] + $cpu0[2] + $cpu0[3] + $cpu0[4] + $cpu0[5] + $cpu0[6] + $cpu0[7];
my $total01 = $cpu1[1] + $cpu1[2] + $cpu1[3] + $cpu1[4] + $cpu1[5] + $cpu1[6] + $cpu1[7];
my $total02 = $cpu2[1] + $cpu2[2] + $cpu2[3] + $cpu2[4] + $cpu2[5] + $cpu2[6] + $cpu2[7];

my $used00 = $cpu0[1] + $cpu0[2] + $cpu0[3] + $cpu0[5] + $cpu0[6] + $cpu0[7];
my $used01 = $cpu1[1] + $cpu1[2] + $cpu1[3] + $cpu1[5] + $cpu1[6] + $cpu1[7];
my $used02 = $cpu2[1] + $cpu2[2] + $cpu2[3] + $cpu2[5] + $cpu2[6] + $cpu2[7];

if($debug == 1)
{
    print("++++++++++++++++++++++++\n");
    print("Total $total00\n");
    print("Used  $used00\n");
    print("+++++++++++++++++++++++++\n");
}


#-----------------------------------------
sleep(1);
#-----------------------------------------

my @cpulines2 = `/usr/bin/rsh -l root $ARGV[0] \"/bin/cat /proc/stat | /bin/grep -i \'cpu\'\"`;

chomp($cpulines2[0]);
chomp($cpulines2[1]);
chomp($cpulines2[2]);

my @cpu20  = split(" ", $cpulines2[0]);
my @cpu21  = split(" ", $cpulines2[1]);
my @cpu22  = split(" ", $cpulines2[2]);

if($debug == 1)
{
    print("------------------------\n");
    print("CPU0\n");
    print("$cpu20[0]\n");
    print("$cpu20[1]\n");
    print("$cpu20[2]\n");
    print("$cpu20[3]\n");
    print("$cpu20[4]\n");
    print("$cpu20[5]\n");
    print("$cpu20[6]\n");
    print("$cpu20[7]\n");
    print("------------------------\n");
}

my $total20 = $cpu20[1] + $cpu20[2] + $cpu20[3] + $cpu20[4] + $cpu20[5] + $cpu20[6] + $cpu20[7];
my $total21 = $cpu21[1] + $cpu21[2] + $cpu21[3] + $cpu21[4] + $cpu21[5] + $cpu21[6] + $cpu21[7];
my $total22 = $cpu22[1] + $cpu22[2] + $cpu22[3] + $cpu22[4] + $cpu22[5] + $cpu22[6] + $cpu22[7];

my $used20 = $cpu20[1] + $cpu20[2] + $cpu20[3] + $cpu20[5] + $cpu20[6] + $cpu20[7];
my $used21 = $cpu21[1] + $cpu21[2] + $cpu21[3] + $cpu21[5] + $cpu21[6] + $cpu21[7];
my $used22 = $cpu22[1] + $cpu22[2] + $cpu22[3] + $cpu22[5] + $cpu22[6] + $cpu22[7];

if($debug == 1)
{
    print("++++++++++++++++++++++++\n");
    print("Total $total20\n");
    print("Used  $used20\n");
    print("+++++++++++++++++++++++++\n");
}

# average cpu of the 2 cores
#my $used = percent($used20, $used00, $total20, $total00);
my $var1 = percent($cpu20[1], $cpu0[1], $total20, $total00);
my $var2 = percent($cpu20[2], $cpu0[2], $total20, $total00);
my $var3 = percent($cpu20[3], $cpu0[3], $total20, $total00);
my $var4 = percent($cpu20[4], $cpu0[4], $total20, $total00);
my $var5 = percent($cpu20[5], $cpu0[5], $total20, $total00);
my $var6 = percent($cpu20[6], $cpu0[6], $total20, $total00);
my $var7 = percent($cpu20[7], $cpu0[7], $total20, $total00);

#core 1
my $var01 = percent($cpu21[1], $cpu1[1], $total21, $total01);
my $var02 = percent($cpu21[2], $cpu1[2], $total21, $total01);
my $var03 = percent($cpu21[3], $cpu1[3], $total21, $total01);
my $var04 = percent($cpu21[4], $cpu1[4], $total21, $total01);
my $var05 = percent($cpu21[5], $cpu1[5], $total21, $total01);
my $var06 = percent($cpu21[6], $cpu1[6], $total21, $total01);
my $var07 = percent($cpu21[7], $cpu1[7], $total21, $total01);

#core 2
my $var11 = percent($cpu22[1], $cpu2[1], $total22, $total02);
my $var12 = percent($cpu22[2], $cpu2[2], $total22, $total02);
my $var13 = percent($cpu22[3], $cpu2[3], $total22, $total02);
my $var14 = percent($cpu22[4], $cpu2[4], $total22, $total02);
my $var15 = percent($cpu22[5], $cpu2[5], $total22, $total02);
my $var16 = percent($cpu22[6], $cpu2[6], $total22, $total02);
my $var17 = percent($cpu22[7], $cpu2[7], $total22, $total02);

# Memory Checks
if($debug == 1)
{
    print("MEMORY INFO  total  used  free \n");
    print("$mem[0]\n$mem[1]\n$mem[2]\n$mem[3]\n\n");
    print("$swap[0]\n$swap[1]\n$swap[2]\n$swap[3]\n");
}



if($debug == 1)
{
    print($cpulines[0]);
    print("\n");
    print($cpulines[1]);
    print("\n");
    print($cpulines[2]);
    print("\n");
    #print("$total00\n");
    #print("$total01\n");
    #print("$total02\n");
    print("\n\n\n");
    print($cpulines2[0]);
    print("\n");
    print($cpulines2[1]);
    print("\n");
    print($cpulines2[2]);
    print("\n");
    #print("$total20\n");
    #print("$total21\n");
    #print("$total22\n");
}

#print("****************************\n");
print("$var1 ");
print("$var2 ");
print("$var3 ");
print("$var4 ");
print("$var5 ");
print("$var6 ");
print("$var7 ");
#print("****************************\n");
#print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
print("$var01 ");
print("$var02 ");
print("$var03 ");
print("$var04 ");
print("$var05 ");
print("$var06 ");
print("$var07 ");
#print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
#print("----------------------------\n");
print("$var11 ");
print("$var12 ");
print("$var13 ");
print("$var14 ");
print("$var15 ");
print("$var16 ");
print("$var17 ");
#print("----------------------------\n");
#print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
print("$mem[1] ");
print("$mem[2] ");
print("$mem[3] ");
print("$swap[1] ");
print("$swap[2] ");
print("$swap[3]");
#print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
print("\n");
#---------------------------------------------

sub percent
{
    my $used = 0;
    
    my($u1, $u2, $t1, $t2) = @_;
    
    if(($t2 - $t1) != 0)
    {
	$used = (100*($u2-$u1))/($t2-$t1);
    }
    else
    {
	$used = 0;
    }
    
    if($used < 0 || $used == -0)
    {
	$used = $used * -1;
    }
    
    return($used);
    #return(int($used));
}
