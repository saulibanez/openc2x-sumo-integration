#!/bin/bash

# Ir al directorio donde está el ejecutable
cd /home/saul/OpenC2X-standalone/build/cam/src

# Ejecutar el binario con el archivo de configuración
sudo ./cam -c /home/saul/TFM/hello_openc2x/c2x.conf
