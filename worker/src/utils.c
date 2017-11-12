/*
 * utils.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/utils.h"

void generateTempsFolder(){
	system("mkdir -p tmp");
	system("mkdir -p tmp_scripts");
};

char* regenerateScript(char* fileContent, FILE* script, char operation, int socket){
	char* scriptName = generateScriptName(operation, socket);
	char command[36];

	script = fopen(scriptName,"w");
	if(script == NULL){
		log_error(logger,"Error al guardar el script %s", scriptName);
		//abortar y comunicar a Master
	}
	fputs(fileContent,script);
	fclose(script);
	log_info(logger,"Script almacenado en \"%s\"", scriptName);

	//---doy permisos de ejecución---//
	strcpy(command,"chmod +x ");				//pasar nombre de archivo a dinámico
	strcat(command, scriptName);

	//printf("%s",command);
	system(command);							//pasar nombre de archivo a dinámico
	log_info(logger,"Permisos de ejecución de %s concedidos", scriptName);
	return scriptName;
};


void loadConfigs(){
	char* CONFIG_PATH = "properties/worker.properties";
	if(!(config = config_create(CONFIG_PATH))){
		log_error(logger, "No se pudo cargar el archivo de configuración");
		exit(1);
	};
	if(!(config_has_property(config,"WORKER_PUERTO"))){
		log_error(logger, "Error al cargar archivos de configuración");
		config_destroy(config);
		exit(1);
	}
	log_info(logger, "Archivo de configuraciones cargado");
}


void createLoggers(){
	char* LOG_PATH = "../logs/worker.log";
	if(logger = log_create(LOG_PATH,"worker",1,LOG_LEVEL_TRACE)){
		log_info(logger, "Logger inicializado");
	}else{
		log_warning(logger, "No se pudo generar el logger");
		exit(1);
	};
}

long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_usec+te.tv_sec;
}

char* generateScriptName(char operation, int master){
	char* name = malloc(sizeof(char)*26);
	long timestamp = current_timestamp();
	asprintf(&name, "tmp_scripts/%c%d-%ld",operation,master,timestamp);
	return name;
}

int readBuffer(int socket,int size,void* destiny){
	void* buffer = malloc(size);
	int bytesReaded = recv(socket, buffer, size, MSG_WAITALL);
	if (bytesReaded <= 0){
		log_warning(logger,"Socket %d: desconectado",socket);
		close(socket);
		free(buffer);
		return EXIT_FAILURE;
	}
	memcpy(destiny, buffer, size);
	free(buffer);
	return EXIT_SUCCESS;
}

char* serializeFile(char* fileName){
	int caracter, fileSize, charCounter=0;
	char* stringFile;

	FILE* file;
	file = fopen(fileName,"r");
	if(file==NULL){
		log_warning(logger,"El archivo %s no existe",fileName);
	}

//---obtengo el tamaño del archivo
	fseek(file,0,SEEK_END);
	fileSize = ftell(file)+1;
	stringFile=malloc(fileSize);

//---paso contenido a string

	rewind(file);
	while((caracter = fgetc(file))!= EOF){
		stringFile[charCounter] = caracter;
		charCounter++;
	}
	stringFile[charCounter] = '\0';
	return stringFile;
}

char* generateFile(char* fileContent, char operation, int socket){
	char* fileName = generateScriptName(operation, socket);
	FILE* file = fopen(fileName,"w");
	if(file == NULL){
		log_error(logger,"Error al crear el archivo %s", fileName);
		//abortar y comunicar a Master
	}
	fputs(fileContent,file);
	fclose(file);

	log_info(logger,"Archivo almacenado en \"%s\"", fileName);
	return fileName;
};

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
	log_info(logger," Inicio mapeo del archivo bin en memoria \n");

	int fd;
	/*Informacion acerca del archivo */
	struct stat stat_data;
	int status;
	size_t size;

	char* dataBinName = config_get_string_value(config,"RUTA_DATABIN");
	/*Abrir el archivo para leer e escribir*/
	fd = open(dataBinName, O_RDWR);
	if(fd < 0){
		log_error(logger,"Error al abrir el archivo\n");
	}
	check (fd < 0, "open %s failed: %s", dataBinName, strerror (errno));

	/*Obtener el tamanio del archivo */
	 status = fstat(fd,&stat_data);
	 if(status < 0){
	 	log_error(logger,"Error al obtener el tamaño del archivo\n");
	 }
	 check (status < 0, "stat %s failed: %s", dataBinName, strerror (errno));
	 size = stat_data.st_size;

	 /* Memory-map del archivo. */
	 mapped_data_node = mmap ((caddr_t) 0, size, PROT_READ, MAP_SHARED, fd, 0);
	 if(mapped_data_node == MAP_FAILED){
		 log_error(logger,"Error al mapear el archivo en memoria\n");
	 }
	 check (mapped_data_node == MAP_FAILED, "mmap %s failed: %s",dataBinName, strerror (errno));

	log_info(logger,"Fin mapeo de archivo en memoria \n");
}

/*****************************************************
 *	Implementacion para leer un bloque de un archivo   *
 ******************************************************/
char* getBlockData(int blockNumber, int sizeByte) {
	log_info(logger, "Iniciando lectura  bloque archivo ");

	void * buffer = malloc(sizeByte);

	void * pos = mapped_data_node + blockNumber * BLOCK_SIZE;
	memcpy(buffer, pos, sizeByte);
	log_info(logger, "Fin lectura bloque archivo ");
	return (char *) buffer;
}



