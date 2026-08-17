#!/usr/bin/env bash

echo "Setting up IPTABLES RULES"
service iptables restart
ipset create blacklist hash:ip hashsize 4096
iptables -I INPUT 1 -p tcp --dport 7854 -j ACCEPT
iptables -I INPUT 1 -p tcp --dport 8001 -j ACCEPT
iptables -I INPUT -m set --match-set blacklist src -j DROP
iptables -I FORWARD -m set --match-set blacklist src -j DROP
echo "Iptables set correctly!"