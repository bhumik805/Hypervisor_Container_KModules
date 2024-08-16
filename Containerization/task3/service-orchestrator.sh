#!/bin/bash

# Complete this script to deploy external-service and counter-service in two separate containers
# You will be using the conductor tool that you completed in task 3.

# Creating link to the tool within this directory
rm conductor.sh
rm config.sh
ln -s ../task3/conductor.sh conductor.sh
ln -s ../task3/config.sh config.sh

# use the above scripts to accomplish the following actions -

# Logical actions to do:
# 1. Build image for the container
./conductor.sh build mydebian

# 2. Run two containers say c1 and c2 which should run in background. Tip: to keep the container running
#    in background you should use a init program that will not interact with the terminal and will not
#    exit. e.g. sleep infinity, tail -f /dev/null

./conductor.sh stop c1 > /dev/null 2>&1 
./conductor.sh stop c2 > /dev/null 2>&1 
./conductor.sh run mydebian c1 "sleep infinity" &
./conductor.sh run mydebian c2 "sleep infinity" &

# 3. Copy directory external-service to c1 and counter-service to c2 at appropriate location. You can
#    put these directories in the containers by copying them within ".containers/{c1,c2}/rootfs/" directory
sleep 5
cp -r external-service .containers/c1/rootfs/
cp -r counter-service .containers/c2/rootfs/
# 4. Configure network such that:ls
#    4.a: c1 is connected to the internet and c1 has its port 8080 forwarded to port 3000 of the host
    ./conductor.sh addnetwork c1 -i -e 8080-3000
#    4.b: c2 is connected to the internet and does not have any port exposed
    ./conductor.sh addnetwork c2 -i
#    4.c: peer network is setup between c1 and c2
    ./conductor.sh peer c1 c2


# 5. Get ip address of c2. You should use script to get the ip address. 
#    You can use ip interface configuration within the host to get ip address of c2 or you can 
#    exec any command within c2 to get it's ip address
ip_of_c2=$(./conductor.sh exec c2 "hostname -I")
ip_of_c2=$(echo $ip_of_c2 | awk -F " " '{print $NF}')

# 6. Within c2 launch the counter service using exec [path to counter-service directory within c2]/run.sh
./conductor.sh exec c2 "chmod +x /counter-service/run.sh" 
./conductor.sh exec c2 "./counter-service/run.sh" > /dev/null 2>&1 &
sleep 2
counter_service_pid=$(pgrep run.sh)
port=8080
while true; do
    check_open=$(nsenter -t $counter_service_pid -n nc -zv localhost 8080 2>&1 | awk '{print $NF}')
    if [[ $check_open == *"open"* ]]; then
        echo "Port $port is now open"
        break
    else
        echo "Port $port is not open yet, retrying in 1 second..."
        sleep 1
    fi
done

# 7. Within c1 launch the external service using exec [path to external-service directory within c1]/run.sh
./conductor.sh exec c1 "chmod +x /external-service/run.sh" 
./conductor.sh exec c1  "/external-service/run.sh http://$ip_of_c2:8080/" > /dev/null 2>&1 &

hostip=$(hostname -I | awk -F " " '{print $1}')
port=3000
while true; do
    check_open=$(nc -zv $hostip 3000 2>&1 | awk '{print $NF}')
    if [[ $check_open == *"open"* ]]; then
        echo "Port $port is now open"
        break
    else
        echo "Port $port is not open yet, retrying in 1 second..."
        sleep 1
    fi
done
# 8. Within your host system open/curl the url: http://localhost:3000 to verify output of the service
curl http://$hostip:3000
# 9. On any system which can ping the host system open/curl the url: `http://<host-ip>:3000` to verify
#    output of the service
