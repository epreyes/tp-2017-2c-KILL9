if [ -n "$1" ]; then
   truncate -s $1MB data.bin
   echo "data.bin de $1MB generado"
else
   echo "Parametro requerido: tamanio de nodo en MB"
   exit 1
fi
