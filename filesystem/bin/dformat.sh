#formateo por script
rm metadata -r
mkdir metadata/archivos -p
mkdir metadata/bitmaps -p
truncate -s 0MB metadata/directorios.dat
truncate -s 0MB metadata/nodos.bin
