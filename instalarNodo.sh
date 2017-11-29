#!/bin/bash

echo -ne "\e[1m\e[95mRuta del Nodo: \e[0m"
read RUTA

echo -ne "\e[1m\e[95mNúmero de Nodo: \e[0m"
read NOMBRE

mkdir -p $RUTA/nodo$NOMBRE
cp dataNode/ -r $RUTA/nodo$NOMBRE
cp worker/ -r $RUTA/nodo$NOMBRE

#FOLDERS
mkdir  -p $RUTA/nodo$NOMBRE/properties
touch $RUTA/nodo$NOMBRE/properties/node.properties
mkdir -p $RUTA/nodo$NOMBRE/worker/tmp
mkdir -p $RUTA/nodo$NOMBRE/worker/tmp_scripts

#DATA.BIN
echo -ne "\e[1m\e[95mTamaño data.bin: \e[0m"
read SIZE
SIZE+=MB

truncate -s $SIZE $RUTA/nodo$NOMBRE/data.bin

#SESSION SORT CONFIG
export LC_ALL=C

#PROPERTIES
echo -ne "\e[1m\e[95mIP de FILESYSTEM: \e[0m"
read IP_FILESYSTEM

echo -ne "\e[1m\e[95mPuerto de FILESYSTEM: \e[0m"
read PORT_FILESYSTEM

echo -ne "\e[1m\e[95mIP de WORKER: \e[0m"
read IP_WORKER

echo -e "IP_FILESYSTEM=$IP_FILESYSTEM\nPORT_FILESYSTEM=$PORT_FILESYSTEM\nIP_WORKER=$IP_WORKER\nPORT_WORKER=5050\nNAME_NODO=$NOMBRE\nPATH_DATABIN=../data.bin\n" > $RUTA/nodo$NOMBRE/properties/node.properties

echo -e "\e[1m\e[36mInstalación de nodo $NOMBRE finalizada correctamente!\e[0m"
