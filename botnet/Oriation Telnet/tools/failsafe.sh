#Used As A Fail-Safe InCase The CNC Server Crashes, it Will Automatically Restart.
echo "[!] Virum Fail-Safe Started [!]"
while true
do
echo "Initializing..."
ulimit -n999999; ulimit -u999999; ulimit -e999999
cd ~/virum-0/
./virumcnc

echo "If you want to completely stop the cnc server process now, press Ctrl+C before the time is up!"
echo "Rebooting in:"
for i in 1
do
echo "$i..."
sleep 1
done
echo "Rebooting now!"
done
