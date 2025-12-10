#!/bin/bash

SUMO_CONF="/home/saul/TFM/sumo_v2v/sumo/two_vehicles.sumocfg"
C2X_CONF="/home/saul/TFM/sumo_v2v/c2x.conf"

sumo-gui -c "$SUMO_CONF" --remote-port 9999 --log sumo_log.txt >/dev/null 2>&1 &
sleep 1


tmux new-session -d -s v2v
tmux set-option -g mouse

tmux send-keys "cd /home/saul/OpenC2X-standalone/build/gps/src && sudo ./gpsService -c $C2X_CONF" C-m 

tmux split-window -h 
tmux send-keys "cd /home/saul/OpenC2X-standalone/build/cam/src && sudo ./cam -c $C2X_CONF" C-m 

tmux split-window -v 
tmux send-keys "cd /home/saul/OpenC2X-standalone/build/dcc/src && sudo ./dcc -c $C2X_CONF" C-m 

tmux split-window -v 
tmux send-keys "cd /home/saul/OpenC2X-standalone/build/ldm/src && sudo ./ldm -c $C2X_CONF" C-m 

tmux attach -t v2v


