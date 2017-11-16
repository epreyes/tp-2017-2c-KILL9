# parametro 1 path al directorio del nuevo datanode
# parametro 2 id del nuevo nodo
cp  dataNode -r $1/dataNode$2
echo -e 'IP_FILESYSTEM=127.0.0.1\nIP_DATANODE=127.0.0.1\nIP_WORKER=127.0.0.1\nPORT_FILESYSTEM=5003\nNAME_NODO='$2'\nPORT_WORKER=5050\nPORT_DATANODE=5060\nPATH_DATABIN=data.bin\n' > $1/dataNode$2/properties/dataNode.properties
