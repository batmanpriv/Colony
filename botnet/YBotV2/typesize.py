#!/usr/bin/python

import subprocess, time

def system(cmd):
  subprocess.call(cmd, shell=True)

file = open("/proc/sys/fs/file-max", "w")
file.write ("999999999999999999")
file.close()
time.sleep(1)
print("File max was set!")
system ("sed -i 's/1024/999999/g' /usr/include/bits/typesizes.h")
system ("ulimit -n999999; ulimit -u999999; ulimit -e999999")
time.sleep(1)
print("Typesizes file was edited!")
time.sleep(0.5)
print("All done!")