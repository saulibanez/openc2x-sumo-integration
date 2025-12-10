#!/bin/bash

TOTAL=100           # Número total de vehículos
DEPART_INTERVAL=1   # Incremento de departure cada X segundos

for ((i=0; i<TOTAL; i++)); do

    veh_id="veh$i"

    # type car_1..car_5 (por modulo)
    type_index=$(( (i % 5) + 1 ))
    veh_type="car_$type_index"

    # route r0..r7 (por modulo)
    route_index=$(( i % 8 ))
    route="r$route_index"

    depart=$(( i * DEPART_INTERVAL ))
    echo "    <vehicle id=\"$veh_id\" type=\"$veh_type\" route=\"$route\" depart=\"$depart\" departSpeed=\"max\" departLane=\"free\"/>"

done
