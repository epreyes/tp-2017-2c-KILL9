install:
	gcc -Wall sockets/*.c protocol/*.h protocol/*.c utils/*.c struct/*.h -shared -o "libtplibraries.so" -lcommons
	sudo cp -u libtplibraries.so /usr/lib
	sudo mkdir /usr/include/tplibraries -p
	sudo cp protocol -r /usr/include/tplibraries/
	sudo cp sockets -r /usr/include/tplibraries/
	sudo cp utils -r /usr/include/tplibraries/
	sudo cp struct -r /usr/include/tplibraries/

uninstall:
	sudo rm /usr/lib/libtplibraries.so
	sudo rm -r /usr/include/tplibraries/
