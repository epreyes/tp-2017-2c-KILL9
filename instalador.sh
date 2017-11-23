git clone https://github.com/sisoputnfrba/so-commons-library.git

cd so-commons-library
make clean all 
sudo make install

cd ..

#Clono proyecto de develop (debe cambiarse a master)
git clone https://github.com/sisoputnfrba/tp-2017-2c-KILL9.git -b develop
cd tp-2017-2c-KILL9

echo '-------------------------------------------------------'
echo 'Compilando e instalando tplibraries'
echo '-------------------------------------------------------'
cd tplibraries/tplibraries
sudo make uninstall
sudo make install 
cd ..
cd ..
echo '-------------------------------------------------------'
echo 'Compilando fs'
echo '-------------------------------------------------------'
cd filesystem/bin
make all
echo 'Escribiendo archivos iniciales para fs'
./dformat.sh
cd ..
cd ..
cd yama
echo '-------------------------------------------------------'
echo 'Compilando yama'
echo '-------------------------------------------------------'
make all
cd ..
cd master
echo '-------------------------------------------------------'
echo 'Compilando master'
echo '-------------------------------------------------------'
make all
cd ..
echo '-------------------------------------------------------'
echo 'Compilando datanode'
echo '-------------------------------------------------------'
cd dataNode
make all
#Genero bin de 100MB
./generarbin.sh 100
cd ..
echo '-------------------------------------------------------'
echo 'Compilando worker'
echo '-------------------------------------------------------'
cd worker
make all
cd ..






