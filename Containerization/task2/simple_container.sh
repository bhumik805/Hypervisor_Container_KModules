#!/bin/bash

SIMPLE_CONTAINER_ROOT=container_root

mkdir -p $SIMPLE_CONTAINER_ROOT

gcc -o container_prog container_prog.c

## Subtask 1: Execute in a new root filesystem



# 1.1: Copy any required libraries to execute container_prog to the new root container filesystem 

cp container_prog $SCR/
list_of_dependables_for_container_prog=$(ldd ./container_prog | sed -n  's/[^/]*\(\/.*\)(.*/ \1/p')
for depend_ecy in $list_of_dependables_for_container_prog; do cp --parents  $depend_ecy ./container_root/; done

SCR=$SIMPLE_CONTAINER_ROOT
mkdir $SCR/bin
cp /bin/bash $SCR/bin/bash
list_of_depen_dables_for_bash=$(ldd /bin/bash | sed -n  's/[^/]*\(\/.*\)(.*/ \1/p')
for depend_ecy in $list_of_depen_dables_for_bash; do cp --parents  $depend_ecy ./container_root/; done


chmod +x $SCR/container_prog
echo -e "\n\e[1;32mOutput Subtask 2a\e[0m"
# 1.2: Execute container_prog in the new root filesystem using chroot. You should pass "subtask1" as an argument to container_prog
sudo chroot $SCR  ./container_prog subtask1

echo "__________________________________________"
echo -e "\n\e[1;32mOutput Subtask 2b\e[0m"


## Subtask 2: Execute in a new root filesystem with new PID and UTS namespace
# The pid of container_prog process should be 1
# You should pass "subtask2" as an argument to container_prog

sudo unshare -pu -- chroot $SCR  ./container_prog subtask2



echo -e "\nHostname in the host: $(hostname)"


## Subtask 3: Execute in a new root filesystem with new PID, UTS and IPC namespace + Resource Control
# Create a new cgroup and set the max CPU utilization to 50% of the host CPU. (Consider only 1 CPU core)



echo "__________________________________________"
echo -e "\n\e[1;32mOutput Subtask 2c\e[0m"
# Assign pid to the cgroup such that the container_prog runs in the cgroup
# Run the container_prog in the new root filesystem with new PID, UTS and IPC namespace
# You should pass "subtask1" as an argument to container_prog
sudo bash -c 'echo "+cpu" >> /sys/fs/cgroup/cgroup.subtree_control'
sudo bash -c 'echo "+cpuset" >> /sys/fs/cgroup/cgroup.subtree_control'
sudo mkdir /sys/fs/cgroup/custom_group
cgr_oup_name=custom_group

sudo bash -c "echo "50000" > /sys/fs/cgroup/$cgr_oup_name/cpu.max"
pid_proc=$$
echo $pid_proc | sudo tee /sys/fs/cgroup/$cgr_oup_name/cgroup.procs > /dev/null


sudo unshare -pu -- chroot $SCR  ./container_prog subtask3


# Remove the cgroup
echo $$ | sudo tee /sys/fs/cgroup/cgroup.procs > /dev/null
sudo rmdir /sys/fs/cgroup/$cgr_oup_name