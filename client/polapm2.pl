#!/usr/bin/perl -w

# poet     : Greg Dunlap
# last mod : 06.20.08

use strict;
use IO::Socket;

my $pathtorrd = "/usr/bin";
my $rrdfiles = "/opt/httpd/htdocs/infocon2";

my $currentdate = time;

my $server = gethostbyname($ARGV[0]) or die "gethostbyname: cannot locate host:$!";
my $port = "9999";
my $proto = "tcp";
my $sock = "NA";

$sock = new IO::Socket::INET(PeerAddr => inet_ntoa($server), PeerPort => $port, Proto => $proto, Timeout => "5");



#print($sock);
#print("\n");
               
my $idle;
my @cpucore0;
my @cpucore1;
my $memtotal;
my $memused;

my $beg = 0;
if($sock =~ "IO::Socket::INET=GLOB")
{
    #write tag to system
    my $tag = $ARGV[1];
    chomp($tag);
    print $sock "$tag\n";
    print $sock "all\n";

    #get data back
    #print("CPU Average\n");
    for(my $i = 0; $i < 7; $i++)
    {
	my $read = <$sock>;
	chomp($read);
	if($i == 3)
	{
	    $idle = $read;
	}
	#print($read);
	#print("\n");
    }

    #print("CPU Core0\n");
    for(my $i = 0; $i < 7; $i++)
    {
	my $read = <$sock>;
	chomp($read);
	$cpucore0[$i] = $read;
	#print($read);
	#print("\n");
    }
    #print("CPU Core1\n");
    for(my $i = 0; $i < 7; $i++)
    {
	my $read = <$sock>;
	chomp($read);
	$cpucore1[$i] = $read;
	#print($read);
	#print("\n");
    }

    #print("Memory Total\n");
    my $read = <$sock>;
    chomp($read);
    $memtotal = $read;

    #print($read);
    #print("\n");
    #print("Memory Used\n");
    $read = <$sock>;
    chomp($read);
    $memused = $read;
    #print($read);
    #print("\n");

    my $fwconn = <$sock>;
    chomp($fwconn);

    close($sock);

    $idle = substr($idle,0,length($idle)-1);
    $cpucore0[0] = substr($cpucore0[0], 0, length($cpucore0[0])-1);
    $cpucore0[1] = substr($cpucore0[1], 0, length($cpucore0[1])-1);
    $cpucore0[2] = substr($cpucore0[2], 0, length($cpucore0[2])-1);
    $cpucore0[4] = substr($cpucore0[4], 0, length($cpucore0[4])-1);
    $cpucore0[5] = substr($cpucore0[5], 0, length($cpucore0[5])-1);
    $cpucore0[6] = substr($cpucore0[6], 0, length($cpucore0[6])-1);
    $cpucore0[3] = substr($cpucore0[3], 0, length($cpucore0[3])-1);
    
    $cpucore1[0] = substr($cpucore1[0], 0, length($cpucore1[0])-1);
    $cpucore1[1] = substr($cpucore1[1], 0, length($cpucore1[1])-1);
    $cpucore1[2] = substr($cpucore1[2], 0, length($cpucore1[2])-1);
    $cpucore1[4] = substr($cpucore1[4], 0, length($cpucore1[4])-1);
    $cpucore1[5] = substr($cpucore1[5], 0, length($cpucore1[5])-1);
    $cpucore1[6] = substr($cpucore1[6], 0, length($cpucore1[6])-1);
    $cpucore1[3] = substr($cpucore1[3], 0, length($cpucore1[3])-1);
    $memtotal = substr($memtotal,0,length($memtotal)-1);
    $memused = substr($memused,0,length($memused)-1);
    
    print("*************************\n");
    print("CPU Idle: $idle\n");
    print("*************************\n");
    print("CPU Core0\n");
    print("$cpucore0[0] $cpucore0[1] $cpucore0[2] $cpucore0[4] $cpucore0[5] $cpucore0[6] $cpucore0[3]\n");
    print("*************************\n");
    print("CPU Core1\n");
    print("$cpucore1[0] $cpucore1[1] $cpucore1[2] $cpucore1[4] $cpucore1[5] $cpucore1[6] $cpucore1[3]\n");
    print("*************************\n");
    print("Memory Total\n");
    print("$memtotal--\n");
    print("Memory Used\n");
    print("$memused--\n");
    print("*************************\n");

    my $temp3 = 100-$idle;
 

    print("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1].rrd $currentdate:$temp3:$fwconn:0:0:0");
    print("\n");
    print("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-linux-1.rrd $currentdate:$cpucore0[0]:$cpucore0[1]:$cpucore0[2]:$cpucore0[4]:$cpucore0[5]:$cpucore0[6]:$cpucore0[3]");
    print("\n");
    print("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-linux-2.rrd $currentdate:$cpucore1[0]:$cpucore1[1]:$cpucore1[2]:$cpucore1[4]:$cpucore1[5]:$cpucore1[6]:$cpucore1[3]");
    print("\n");
    print("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-mem.rrd $currentdate:$memtotal:$memused:0:0");
    print("\n");

    system("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1].rrd $currentdate:$temp3:$fwconn:0:0:0");
    system("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-linux-1.rrd $currentdate:$cpucore0[0]:$cpucore0[1]:$cpucore0[2]:$cpucore0[4]:$cpucore0[5]:$cpucore0[6]:$cpucore0[3]");
    system("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-linux-2.rrd $currentdate:$cpucore1[0]:$cpucore1[1]:$cpucore1[2]:$cpucore1[4]:$cpucore1[5]:$cpucore1[6]:$cpucore1[3]");
    system("$pathtorrd/rrdtool update $rrdfiles/$ARGV[0]_$ARGV[1]-mem.rrd $currentdate:$memtotal:$memused:0:0");


}
else
{
    print("No connection\n");
}
