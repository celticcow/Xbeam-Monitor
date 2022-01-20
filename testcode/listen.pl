#!/usr/bin/perl -w

use strict;
use IO::socket;

my $server = "NA";

$server  = IO::Socket::INET->new(LocalPort => $port, Type => SOCK_STREAM, Reuse => 1, Listen => 10) or die "Can't be a tcp server on port $port: $!\n";

my $client = "NA";

while($client = $server->accept())
{
    # $client is the new connection

    my $request = <$client>;
    print($request);
    print("\n");
    print $client "ok\n";
    close $client;
}
