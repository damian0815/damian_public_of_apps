#!/bin/bash

## launch pd
#pd_patch_path=$PWD/`dirname $0`/../pdbits/new/_main.pd
##pd -nogui -oss -blocksize 1024 ~/pd-test.pd &
#echo ~/code/puredata-0.42.5/bin/pd -nogui -sleepgrain 2 -verbose -alsa $pd_patch_path &

## now sleep
#pd_pid=$!
#echo "pd opened, pid $pd_pid, now sleeping 5 seconds"
#sleep 5

# now open control app
echo starting luciolinae_ctl
ctl_app_path=`dirname $0`/../bin
cd $ctl_app_path
echo $ctl_app_path
ls
./clickToLaunchApp_Release.sh


