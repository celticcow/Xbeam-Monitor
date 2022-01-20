#!/usr/bin/perl -w

use strict;
use IO::Socket;
use Getopt::Long;

# Nagios specific
use lib "/opt/nagios2/libexec";
use utils qw(%ERRORS $TIMEOUT);

my $version = "0.1";
my $o_host = undef;
my $o_tag = undef;
my $o_maxcpu = undef;
my $o_version = undef;

#functions

sub print_version
{
    print("check_apmcpu_fedex version : $version\n");
}

sub print_usage
{
    print("Usage: $0 -H <host> -T <APM_tag> -m <max_cpu> [-V]\n");
}

sub check_options
{
    Getopt::Long::Configure ("bundling");
    GetOptions(
               'H:s'   => \$o_host,            'hostname:s'    => \$o_host,
               'T:s'   => \$o_tag,             'tag:s'         => \$o_tag,
               'm:i'   => \$o_maxcpu,          'maxcpu:i'      => \$o_maxcpu,
               'V'     => \$o_version,         'version'       => \$o_version
               );

      if(defined($o_version))
      {
          print_version();
          exit $ERRORS{"UNKNOWN"};
      }

      if( !defined($o_host) || !defined($o_tag) || !defined($o_maxcpu)) 
      {
          print("Bad Usage\n");
          print_usage();
          exit $ERRORS{"UNKNOWN"};
      }   
}

### Main ###

check_options();


my $server = gethostbyname($o_host) or die "gethostbyname: cannot locate host:$!";
my $port = "9999";
my $proto = "tcp";
my $sock = "NA";
my $idle = -1;

$sock = new IO::Socket::INET(PeerAddr => inet_ntoa($server), PeerPort => $port, Proto => $proto, Timeout => "5");

if($sock =~ "IO::Socket::INET=GLOB")
{
    my $tag = $o_tag;
    chomp($tag);
    print $sock "$tag\n";
    print $sock "cpu\n";
    
    for(my $i = 0; $i < 7; $i++)
    {
        my $read = <$sock>;
        chomp($read);
        if($i == 3)
        {
            $idle = $read;
        }
    }
    #print("CPU Core0\n");
    for(my $i = 0; $i < 7; $i++)
    {
        my $read = <$sock>;
        chomp($read);
    }
    #print("CPU Core1\n");
    for(my $i = 0; $i < 7; $i++)
    {
        my $read = <$sock>;
        chomp($read);
    }
    
    #print("******************\n");
    #print("$idle\n");
    #print("******************\n");
    $idle = 100 - $idle;
    #print("******************\n");
    #print("$idle\n");
    #print("******************\n");
}

close($sock);

if($idle < $o_maxcpu)
{
    print("OK  ");
    print("$idle FW CPU : MAX is $o_maxcpu\n");
    exit $ERRORS{"OK"};
}

print("CRITICAL  ");
print("$idle FW CPU : MAX is $o_maxcpu\n");
exit $ERRORS{"CRITICAL"};


