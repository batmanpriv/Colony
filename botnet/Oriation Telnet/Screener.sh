#!/bin/bash
pkill screen
cd /root/virum-0/tools/
chmod 777 * 
screen -d -m ./failsafe.sh
echo "Virum screened"
