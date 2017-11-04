/*
 * util.c
 *
 *  Created on: 16/10/2017
 *      Author: migue tomicha
 */
#include "header/util.h"
#include "header/dataNode.h"
#include "header/const.h"
#include "header/connectionDataNodeFileSystem.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdarg.h>
#include <commons/string.h>

static void
check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        fprintf (stderr, "\n");
        exit (EXIT_FAILURE);
    }
}


void map_data_node() {
	log_info(infoLogger," Inicio map_data_node \n");
	/* File descriptor del archivo */
	int fd;
	/*Informacion acerca del archivo */
	struct stat stat_data;
	int status;
	size_t size;

	char * file_name = data_node_config->path_data_bin;

	/*Abrir el archivo para leer e escribir*/
	fd = open(file_name, O_RDWR);
	if(fd < 0){
		log_error(logError,"Error al abrir el archivo\n");
	}
	check (fd < 0, "open %s failed: %s", file_name, strerror (errno));

	/*Obtener el tamanio del archivo */
	 status = fstat(fd,&stat_data);
	 if(status < 0){
	 	log_error(logError,"Error al obtener el tamaño del archivo\n");
	 }
	 check (status < 0, "stat %s failed: %s", file_name, strerror (errno));
	 size = stat_data.st_size;

	 /* Memory-map del archivo. */
	 mapped_data_node = mmap ((caddr_t) 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	 if(mapped_data_node == MAP_FAILED){
		 log_error(logError,"Error al mapear el archivo en memoria\n");
	 }
	 check (mapped_data_node == MAP_FAILED, "mmap %s failed: %s",file_name, strerror (errno));

	 /* Mostrar infoArchivo */
	 //	int i;
	 //	for (i = 0; i < size; i++) {
	 //		char c;
	 //		c = ((char *) mapped_data_node)[i];
	 //		putchar(c);
	 //	}
	log_info(infoLogger,"Fin map_data_node\n");
}

void  load_data_node_properties_files(){
	t_data_node = config_create(DATA_NODE_PATH);
	data_node_config = malloc(sizeof(t_data_node_config));
	data_node_config->file_system_ip= config_get_string_value(t_data_node,IP_FILESYSTEM);
	data_node_config->file_system_port = config_get_string_value(t_data_node,PORT_FILESYSTEM);
	data_node_config->path_data_bin= config_get_string_value(t_data_node,PATH_DATABIN);
	data_node_config->worker_ip = config_get_string_value(t_data_node,IP_WORKER);
	data_node_config->worker_port = config_get_string_value(t_data_node,PORT_WORKER);
	data_node_config->name_nodo = config_get_int_value(t_data_node,NAME_NODO);

	free(t_data_node);

	infoLogger = log_create(LOG_INFO,DATA_NODE,true,LOG_LEVEL_INFO);
	logError = log_create(LOG_ERROR,DATA_NODE,true,LOG_LEVEL_ERROR);


	log_info(infoLogger,INIT_DATA_NODE);

	/**Configuracion datanode **/
	char *config = string_new();
	string_append(&config,INIT_CONFIG);
	string_append(&config,IP_FILESYSTEM);
	string_append_with_format(&config,":%s\n", data_node_config->file_system_ip);

	string_append(&config,PORT_FILESYSTEM);
	string_append_with_format(&config,":%s\n", data_node_config->file_system_port);

	string_append(&config,PATH_DATABIN);
	string_append_with_format(&config,":%s\n", data_node_config->path_data_bin);

	log_info(infoLogger,config);
	free(config);

}


