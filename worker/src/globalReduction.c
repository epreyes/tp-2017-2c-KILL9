/*
 * globalReduction.c
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"

char* obtainNodeFile(rg_node datos){
//ESTABLEZCO CONEXIÓN CON OTRO WORKER PARA PEDIR TMP
	char* file=NULL;

	if(openNodeConnection(datos.node, datos.ip, datos.port)){
		log_warning(logger,"No se pudo conectar con el Nodo %d", datos.node);
		return NULL;
	};
	printf("\nFILE:%s\n",datos.rl_tmp);
//ENVIO DATOS
	char code = 'R';
	int bufferSize = sizeof(char)*29;
	void* buffer = malloc(bufferSize);
	memcpy(buffer,&code,sizeof(char));
	memcpy(buffer+sizeof(char),&(datos.rl_tmp),28);
	log_info(logger,"Solicitando archivo %s al nodo %d(socket:%d)",datos.rl_tmp,datos.node,socket_nodes[datos.node]);
	send(socket_nodes[datos.node],buffer,bufferSize,0);
	free(buffer);
//OBTENGO RESPUESTA
	nodeData_rs nodeAnswer;
	readBuffer(socket_nodes[datos.node],sizeof(char),&(nodeAnswer.code));
	readBuffer(socket_nodes[datos.node],sizeof(int),&(nodeAnswer.fileSize));
	nodeAnswer.file = malloc(nodeAnswer.fileSize);
	readBuffer(socket_nodes[datos.node],(nodeAnswer.fileSize),nodeAnswer.file);
	log_info(logger,"Archivo obtenido, conexión con nodo %d(socket:%d) finalizada",datos.node,socket_nodes[datos.node]);
//	file = generateFile(nodeAnswer.file, 'G',socket_nodes[datos.node]);
	file = generateGLFile(nodeAnswer.file,datos.rl_tmp);

	log_info(logger,"Archivo generado");
	//cierro conexión con otro worker
	close(socket_nodes[datos.node]);
	free(nodeAnswer.file);
//GENERO ARCHIVO Y DEVUELVO EL NOMBRE
	return file;
/*
*/
}

//======PASO ARCHIVO TMP A OTRO WORKER================
void sendNodeFile(int worker){
//OBTENGO DATOS
	char fileName[28];
	readBuffer(worker,sizeof(char)*28,&fileName);
	log_trace(logger,"Worker (socket:%d) Solicitó el archivo %s", worker, fileName);

//RECUPERO ARCHIVO SOLICITADO
	char* fileContent = NULL;
	fileContent=serializeFile(fileName+1);
	log_trace(logger,"Archivo serializado");
//PREPARO PAQUETE
	nodeData_rs* answer = malloc(sizeof(nodeData_rs));
	answer->code = 'R';
	answer->fileSize = strlen(fileContent)+1;
	answer->file = malloc(answer->fileSize);
	strcpy(answer->file, fileContent);

//SERIALIZO RESPUESTA
	log_trace(logger, "Enviando archivo %s a worker por socket %d", fileName, worker);
	int bufferSize = sizeof(char)+sizeof(int)+(answer->fileSize);
	void* buffer = malloc(bufferSize);
	memcpy(buffer,&(answer->code),sizeof(char));
	memcpy(buffer+sizeof(char),&(answer->fileSize),sizeof(int));
	memcpy(buffer+sizeof(char)+sizeof(int),answer->file,answer->fileSize);

	send(worker,buffer,bufferSize,0);
	log_trace(logger, "Archivo %s enviado a worker (socket: %d)", fileName, worker);

//LIBERO MEMORIA
	free(fileContent);
	free(answer->file);
	free(answer);
	free(buffer);
}

//========RESPONDO A MASTER================
void answerMaster(char result){
	int bufferSize = sizeof(char);
	rg_node_rs* answer = malloc(sizeof(rg_node_rs));
	void* buffer = malloc(bufferSize);

	answer->result = result;
	log_info(logger,"Enviando resultado de Reducción Global a Master");

	memcpy(buffer,&(answer->result),sizeof(char));
	send(socket_master,buffer,bufferSize,0);
	log_info(logger,"Resultados enviados");

	free(buffer);
	free(answer);
}

void globalReduction(){
	typedef char rl_tmp[28];
	rg_node_rq datos;
	int i=0;

	log_info(logger,"Master %d: Obteniendo datos de reducción global",socket_master);

//OBTENIENDO DATOS DEL NODO ANFITRION
	readBuffer(socket_master,sizeof(int),&(datos.fileSize));
	datos.file = malloc(datos.fileSize);
	readBuffer(socket_master,datos.fileSize,datos.file);
	readBuffer(socket_master,28,&(datos.rl_tmp));					//archivo RL
	readBuffer(socket_master,24,&(datos.rg_tmp));					//archivo RG
	printf("%s %s\n",datos.rg_tmp,datos.rl_tmp);


//OBTENIENDO DATOS DE LOS OTROS NODOS
	readBuffer(socket_master,sizeof(int),&(datos.nodesQuantity));	//cantidad de NODOS
	datos.nodes = malloc(sizeof(rg_node)*datos.nodesQuantity);		//

	for(i=0; i<datos.nodesQuantity; ++i){
		readBuffer(socket_master,sizeof(int),&(datos.nodes[i].node));
		readBuffer(socket_master,16,(datos.nodes[i].ip));
		readBuffer(socket_master,sizeof(int),&(datos.nodes[i].port));
		readBuffer(socket_master,28,&(datos.nodes[i].rl_tmp));
		//printf("N:%d(%s:%d) TMP:%s\n",datos.nodes[i].node,datos.nodes[i].ip,datos.nodes[i].port,datos.nodes[i].rl_tmp);
	}
	log_info(logger,"Master %d: Datos de Reducción Global obtenidos",socket_master);

	char* scriptName = regenerateScript(datos.file,script_reduction,'R',socket_master);

	rl_tmp* rlFilesNames = malloc((datos.nodesQuantity+1)*sizeof(rl_tmp)); //+1 por el del anfitrion
	rl_tmp auxFile;
//genero todos los temps, los guardo y devuelvo el nombre
	for(i=0;i<(datos.nodesQuantity);++i){
		strcpy(rlFilesNames[i],obtainNodeFile(datos.nodes[i]));
		printf("FILE:%s\n",rlFilesNames[i]);
	}
	printf("obteniendo archivos\n");
	strcpy(rlFilesNames[datos.nodesQuantity], datos.rl_tmp);				//pongo último el del anfitrion
	printf("FILE:%s\n",rlFilesNames[datos.nodesQuantity]);
	//Aplico reducción (reutilizo funct de redloc)

	char result = reduceFiles(datos.nodesQuantity+1,rlFilesNames, scriptName, datos.rg_tmp);

	answerMaster(result);
	free(datos.file);
	free(datos.nodes);
	log_trace(logger,"Reducción global finalizada");
};
