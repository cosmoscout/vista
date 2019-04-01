#!/bin/bash

# In this file, we will first start the slaves, and then the master

source set_path_for_21ClusterModeDemo.sh

echo starting slave 1
./21ClusterModeDemo -newclusterslave DesktopSlave1 &
echo starting slave 2
./21ClusterModeDemo -newclusterslave DesktopSlave2 &

# We want to sleep two seconds to allow the slaves to start properly, before we start the master
echo sleeping for 3 seconds
sleep 3

echo starting the master
./21ClusterModeDemo -newclustermaster DesktopMaster &
