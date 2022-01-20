#!/usr/bin/perl -w

###################### check_blades_fedex ## ##############
# Version : 0.1
# Date : July 09 2009
# Author  : Gregory Dunlap
###########################################################
#
# Check the number of active and standby blades in a X series box
#

use strict;
use Net::SNMP;
use Getopt::Long;

# Nagios specific
use lib "/opt/nagios2/libexec";
use utils qw(%ERRORS $TIMEOUT);

my $version     = "0.1";
my $bladetable  = "1.3.6.1.4.1.6848.2.1.6.1.1";
my $o_host      = undef;
my $o_community = undef;
my $o_active    = undef;
my $o_standby   = undef;
my $o_version   = undef;


#functions

sub print_version
{
    print("check_blades_fedex version : $version\n");
}

sub print_usage
{
    print("Usage: $0 -H <host> -C <snmp_community> -a <active blades> -s <standby blades> [-V]\n");
}


sub check_options
{
    Getopt::Long::Configure ("bundling");
    GetOptions(
	'H:s'   => \$o_host,            'hostname:s'    => \$o_host,
	'C:s'   => \$o_community,       'community:s'   => \$o_community,
	'a:i'   => \$o_active,          'active:i'      => \$o_active,
	's:i'   => \$o_standby,         'standby:s'     => \$o_standby,
	'V'     => \$o_version,         'version'       => \$o_version
	);
    
    if(defined($o_version))
    {
	print_version();
	exit $ERRORS{"UNKNOWN"};
    }
    
    if( !defined($o_host) || !defined($o_community) || !defined($o_active) || !defined($o_standby))
    {
	print("Bad Usage\n");
	print_usage();
	exit $ERRORS{"UNKNOWN"};
    }   
}

#### Main code ####

check_options();

my ($session, $error) = Net::SNMP->session(
    -hostname  => $o_host,
    -community => $o_community,
    -timeout   => $TIMEOUT,
    );


my $tableview = $session->get_table(Baseoid => $bladetable) or die ("Problem " . $session->error);

my @oid    = undef;
my @index  = undef;
my $i      = 0;
my $numact = 0;
my $numstd = 0;

foreach my $key (keys %$tableview)
{
    my $status = $session->get_request($key) or die ("Problem " . $session->error);

    if($status->{$key} == 4)
    {
	$numact++;
    }
    elsif($status->{$key} == 5)
    {
	$numstd++;
    }   
}

###################################

$session->close;

###################################

if($o_active == $numact && $o_standby == $numstd)
{
    print "Active($o_active) found $numact : Standby($o_standby) found $numstd :OK\n";
    exit $ERRORS{"OK"};
}
print "Active($o_active) found $numact : Standby($o_standby) found $numstd :CRITICAL\n";
exit $ERRORS{"CRITICAL"};
