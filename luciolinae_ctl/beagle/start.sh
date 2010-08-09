#!/bin/bash

# launch pd
pd_patch_path=$PWD/`dirname $0`/../pdbits/new/_main.pd
#pd -nogui -oss -blocksize 1024 ~/pd-test.pd &
echo pd -nogui -noadc -oss -blocksize 1024 $pd_patch_path &

# now sleep
pd_pid=$!
echo "pd opened, pid $pd_pid, now sleeping 5 seconds"
sleep 5

# now open control app
echo starting luciolinae_ctl
ctl_app_path=$PWD/`dirname $0`/../bin
cd $ctl_app_path
./clickToLaunchApp_Release.sh

echo killing pd
kill $pd_pid

