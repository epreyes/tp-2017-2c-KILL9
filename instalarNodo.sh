# parametro 1 path al directorio del nuevo datanode
# parametro 2 id del nuevo nodo
mkdir -p $1/nodo$2
cp dataNode/ -r $1/nodo$2
cp worker/ -r $1/nodo$2
echo -e 'IP_FILESYSTEM=127.0.0.1\nIP_DATANODE=127.0.0.1\nIP_WORKER=127.0.0.1\nPORT_FILESYSTEM=5003\nNAME_NODO='$2'\nPORT_WORKER=5050\nPORT_DATANODE=5060\nPATH_DATABIN=data.bin\n' > $1/nodo$2/dataNode/properties/dataNode.properties
cd dataNode
./generarbin.sh $3
cd ..
cd worker
echo -e 'IP_FILESYSTEM=127.0.0.1\nPUERTO_FILESYSTEM=5003\nNOMBRE_NODO=NODO1\nWORKER_PUERTO=5050\nRUTA_DATABIN='$1'nodo'$2'/dataNode/data.bin\n' > $1/nodo$2/worker/properties/worker.properties
cd ..

