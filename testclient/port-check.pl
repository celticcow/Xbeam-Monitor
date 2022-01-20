#!/usr/bin/perl -w

use strict;
use IO::Socket;

my $server = gethostbyname($ARGV[0]) or die "gethostbyname: cannot locate host:$!";
my $proto = $ARGV[1];
my $port = $ARGV[2];


my $sock = "NA";
$sock = new IO::Socket::INET(PeerAddr => inet_ntoa($server), PeerPort => $port, Proto => $proto, Timeout => "5");
                                                                                
#print($sock);
#print("\n");

my $beg = 0;
if($sock =~ "IO::Socket::INET=GLOB")
{
    #socket connected
    
    print("********************\n");
    print("*     Connected    *\n");
    print("********************\n");

    print("********************\n");
    print("*  Message to Send *\n");
    print("********************\n");


    my $data = <STDIN>;
    
    print $sock "$data\n";
    my $data2 = <STDIN>;
    
    print $sock "$data2\n";


    #print $sock "GET / HTTP/1.0\r\n\r\n"; #works for all so far
    
    for(my $j = 0; $j < 24; $j++)
	{
    	my $read = <$sock>;
	chomp($read);
    	print("------------\n");
    	print($read);
	print("---");
    	print("\n");
	}
    #while($read = <$sock>)
    #{                                                                        
    #    print ($read);
    #}
    close($sock);
}
else
{
    print("********************\n");
    print("*  NO CONNECTION   *\n");
    print("********************\n");
}
 
