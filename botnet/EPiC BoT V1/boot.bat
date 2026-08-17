net share /delete C$ /y 
net share /delete D$ /y 
net share /delete E$ /y 
net share /delete F$ /y 
net share /delete ADMIN$ 
net share /delete IPC$ 
net stop "Remote Registry Service" 
net stop "Computer Browser" 
net stop "REMOTE PROCEDURE CALL" 
net stop "REMOTE PROCEDURE CALL SERVICE" 
net stop "Remote Access Connection Manager" 
net stop "telnet" 
net stop "messenger" 
net stop "netbios" 
