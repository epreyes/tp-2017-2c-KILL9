echo '-------------------------------------------------------'
echo 'Compilando e instalando tplibraries'
echo '-------------------------------------------------------'
cd tplibraries/tplibraries
make uninstall
make install 
cd ..
cd ..
echo '-------------------------------------------------------'
echo 'Compilando fs'
echo '-------------------------------------------------------'
cd filesystem/bin
make all
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
cd ..

