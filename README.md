# openc2x-sumo-integration
OpenC2X ported to Ubuntu 24.04 and integrated with SUMO using TraCI. Includes V2V CAM evaluation tests and V2I DENM event generation.

## 1. Requirements

### System
- Ubuntu 24.04 LTS
- CMake ≥ 3.16
- GNU GCC/G++ ≥ 11
- Python 3.8+
- SUMO ≥ 1.23.1 (with TraCI support)

### Build dependencies
``` bash
sudo apt update
sudo apt install -y \
    build-essential cmake git \
    libpcap-dev libxerces-c-dev libboost-all-dev \
    libsqlite3-dev libssl-dev libxml2-dev pkg-config \
    libcurl4-openssl-dev libglib2.0-dev libgtest-dev \
    libprotobuf-dev protobuf-compiler \
    libzmq3-dev libasio-dev doxygen graphviz \
    libnl-3-dev libnl-genl-3-dev \
    asn1c libuci-dev libgps-dev \
    lua5.1 liblua5.1-0-dev libjson-c-dev tmux
```
Ubuntu 24.04 does not provide compatible versions of uci and libubox, which OpenC2X requires.
They must be compiled manually:

### Install libubox
``` bash
git clone https://git.openwrt.org/project/libubox.git ~/libubox
cd ~/libubox
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```
### Install uci
``` bash
git clone https://git.openwrt.org/project/uci.git ~/uci
cd ~/uci
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

### Add libraries to the environment:
``` bash
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```
## 2. Build OpenC2X
``` bash
git clone https://github.com/saulibanez/openc2x-sumo-integration.git
cd openc2x-sumo-integration/OpenC2X-standalone
mkdir build && cd build
cmake ..
make -j$(nproc)
```
## 3. Running V2V and V2I scenarios
### Inside SUMO/sumo_v2v/:
``` bash
./runv2v.sh
```
### Inside SUMO/sumo_v2i/:
``` bash
./runv2i.sh
```
## 4. Results analysis
Two Python scripts are included to analyze the logs:
### V2V analysis
``` bash
python3 analyze_v2v.py cam.log ldm.log
```
### V2I analysis
``` bash
python3 analyze_v2i.py gps.log ldm.log
```

## 5. Project structure
```text
openc2x-sumo-integration/
│
├── OpenC2X-standalone/
│      Patched and updated OpenC2X source code:
│      - Compatible with Ubuntu 24.04
│      - Updated CMake and library includes
│      - SUMO/TraCI integration (SumoInterface)
│      - Timing utilities for latency/PDR measurement
│
├── SUMO/
│   ├── hello_openC2x/
│   │      Minimal SUMO + TraCI test used during early integration
│   │
│   ├── sumo_v2v/
│   │   ├── runv2v.sh
│   │   ├── generar_vehiculos.sh
│   │   ├── analyze_v2v.py
│   │   ├── sumo/
│   │   │      SUMO network files (.net.xml, .rou.xml, .sumocfg)
│   │   └── log/
│   │          Results for evaluation scenarios:
│   │          - 2_vehicles/
│   │          - 5_vehicles/
│   │          - 10_vehicles/
│   │          - 20_vehicles/
│   │          - 50_vehicles/
│   │          - 100_vehicles/
│   │
│   └── sumo_v2i/
│       ├── runv2i.sh
│       ├── generar_vehiculos.sh
│       ├── analyze_v2i.py
│       ├── sumo/
│       │      SUMO network files (.net.xml, .rou.xml, .sumocfg)
│       └── log/
│              Results for scenarios with:
│              - 1 traffic light
│              - 2 traffic lights
│              - 3 traffic lights
│              - 4 traffic lights
│              - 5 traffic lights
│              - 10_vehicles/
│              - 20_vehicles/
│              - 50_vehicles/
│              - 100_vehicles/
│
└── memoria/
       LaTeX sources of the TFM
       Final PDF of the thesis
```
