/*
 * globalReduction.c
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"

char* obtainNodeFile(rg_node datos){
//ESTABLEZCO CONEXIÓN CON OTRO WORKER PARA PEDIR TMP
	openNodeConnection(datos.node, datos.ip, datos.port);
//ENVIO DATOS
	int bufferSize = sizeof(char)+28;
	void* buffer = malloc(bufferSize);
	memcpy(buffer,'R',sizeof(char));
	memcpy(buffer+sizeof(char),&(datos.rl_tmp),28);
	send(socket_nodes[datos.node],buffer,bufferSize,0);
	free(buffer);

//OBTENGO RESPUESTA
	nodeData_rs nodeAnswer;
	readBuffer(socket_nodes[datos.node],sizeof(char),&(nodeAnswer.code));
	readBuffer(socket_nodes[datos.node],sizeof(int),&(nodeAnswer.fileSize));
	nodeAnswer.file = malloc(nodeAnswer.fileSize);
	readBuffer(socket_nodes[datos.node],nodeAnswer.fileSize,&(nodeAnswer.file));
	free(nodeAnswer.file);
	close(socket_nodes[datos.node]);

//GENERO ARCHIVO Y DEVUELVO EL NOMBRE
	return generateFile(nodeAnswer.file, 'G',socket_nodes[datos.node]);
}

//======PASO ARCHIVO TMP A OTRO WORKER================
void sendNodeFile(char* fileName){
//RECUPERO ARCHIVO SOLICITADO
	char* fileContent = NULL;
	strcpy(fileContent, serializeFile(fileName));

//PREPARO PAQUETE
	nodeData_rs* answer = malloc(sizeof(nodeData_rs));
	answer->code = 'R';
	answer->fileSize = strlen(fileContent)+1;
	answer->file = malloc(answer->fileSize);
	strcpy(answer->file, fileContent);

//SERIALIZO RESPUESTA
	int bufferSize = sizeof(char)+sizeof(int)+(answer->fileSize);
	void* buffer = malloc(bufferSize);
	memcpy(buffer,&(answer->code),sizeof(char));
	memcpy(buffer+sizeof(char),&(answer->fileSize),sizeof(int));
	memcpy(buffer+sizeof(char)+sizeof(int),answer->file,answer->fileSize);

//LIBERO MEMORIA
	free(answer->file);
	free(answer);
	free(buffer);
}

//========RESPONDO A MASTER================
void answerMaster(){
	int bufferSize = sizeof(char)+sizeof(int);
	rg_node_rs* answer = malloc(sizeof(rg_node_rs));
	void* buffer = malloc(bufferSize);

	answer->result = 'O';
	answer->runTime = 12;
	log_info(logger,"Enviando resultado de Reducción Global a Master");

	memcpy(buffer,&(answer->result),sizeof(char));
	memcpy(buffer+sizeof(char),&(answer->runTime),sizeof(int));
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
	//printf("%s %s\n",datos.rg_tmp,datos.rl_tmp);

	char* scriptName = regenerateScript(datos.file,script_reduction,'R',socket_master);

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


	rl_tmp* rlFilesNames = malloc((datos.nodesQuantity+1)*sizeof(rl_tmp)); //+1 por el del anfitrion

/*
	//genero todos los temps, los guardo y devuelvo el nombre
	for(i=0;i<(datos.nodesQuantity);++i){
		strcpy(rlFilesNames[i],obtainNodeFile(datos.nodes[i]));
		printf("FILE:%s\n",rlFilesNames[i]);
	}
	strcpy(rlFilesNames[datos.nodesQuantity], datos.rl_tmp);				//pongo último el del anfitrion
	printf("FILE:%s\n",rlFilesNames[datos.nodesQuantity]);
	//Aplico reducción (reutilizo funct de redloc)
	reduceFiles(datos.nodesQuantity+1,rlFilesNames, script_reduction, datos.rg_tmp);

*/
	answerMaster(); //agregar error
	free(datos.file);
	free(datos.nodes);
};
