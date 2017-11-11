#!/bin/bash

echo "-------------------------------"
echo "      CONFIGURANDO MASTER      "
echo "-------------------------------"
mkdir logs
cd logs
touch master.log
cd ..
echo -ne "\e[1m\e[95mIngrese IP de YAMA: \e[0m"
read YAMA_IP
echo -ne "\e[1m\e[95mIngrese Puerto DE YAMA: \e[0m"
read YAMA_PUERTO
mkdir properties
cd properties
echo -e "YAMA_IP=$YAMA_IP\nYAMA_PUERTO=$YAMA_PUERTO">master.properties
cd ..
