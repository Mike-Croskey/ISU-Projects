#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <argp.h>
#include <sys/time.h>

//program version 
const char *argp_program_version = "FTP Client for CPRE_489 Lab 4";

//program bug address
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";

//program documentation 
const char doc[] = "This Program is designed to act as a client to an ftp server (ftpServer.c)\nThe program excepts 4 arguments: [ftp serverip], [ftp control port], [prefered data port 1], [prefered data port 2]\nThe client will establish an initial control connection with [ftp serverip] on [ftp control port]\nUpon connecting to the server the client will bind [prefered data port 1] and begin to wait for server response\nA LIST command will be sent to the server requesting the FTP download directory be returned on [prefered data port 1]\nThe client will dysplay the recieved directory and promt the user to enter a file\nThe client will then bind [prefered data port 2] and begin to wait for server response\nAlso sending RET command to server requesting file name entered by user\nThe client will recive the file and place it in current running directory\n";

//function declarations
const char *retList(int dataSocket);
const char *getFileName(const char *fileList);

//list of supported arguments
static struct argp_option options[]={

		{"server_ip",			's', "s", 0, "Desired Ftp Server's IP"},
		{"prefered_data_port_1",	'a', "a", 0, "Prefered port for sending data connection"},
		{"prefered_data_port_2",	'b', "b", 0, "Prefered port for sending data connection"},
		{"control port",		'c', "c", 0, "Prefered port for established control connection"},
		{0}
};

///struct for storing the command line arguments
struct arguments{

	//number of arguments
	char *args[40];	
	//ftp server ip address
	char *server_ip;
	//Prefered port for sending data connection
	char *pp_1;
	//Prefered port for sending data connection
	char *pp_2;
	//Prefered port for establishing data connection
	char *cp;
	
};

/**
*@brief Callback for command line agument parsing
*
*@param key
*		This provides a key code reference for an argument
*
*@param arg
*		The argument attached to each key code
*
*@param state	
*		Sets Argument Parser State 
*
*@return	0 on success, ARGP_ERR_UNKNOWN if key is unknown
*/
static error_t parse_opt(int key, char *arg, struct argp_state *state){

	struct arguments *arguments = state->input;
	switch(key){

		case 's':
			arguments->server_ip = arg;
			break;
		case 'a':
			arguments->pp_1 = arg;
			break;
		case 'b':
			arguments->pp_2 = arg;
			break;
		case 'c':
			arguments->cp = arg;
			break;
	
		case ARGP_KEY_ARG:
			
			if(state->arg_num>=4){
				argp_usage(state);
			}
			
			arguments->args[state->arg_num] = arg;

			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

//arg parser object
static struct argp argp = {options,parse_opt,0,doc};

// structs for incoming and outgoing socket addresses
struct sockaddr_in ctrl_sai, data_sai;

//Creates a new struct to hold the cmd line arguments
struct arguments arguments;

int main(int argc, char** argv){
	
	//Sets the default values for cmd line arguments
	arguments.server_ip = "";
	arguments.pp_1 = "";
	arguments.pp_2 = "";
	arguments.cp = "";

	//call the argument parsers argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//testing statments
	printf("%s\n", arguments.server_ip);
	printf("%s\n", arguments.pp_1);
	printf("%s\n",arguments.pp_2);
	printf("%s\n",arguments.cp);


	printf("Starting FTP Client\n");
	//creates a control connection for sending commands
	int ctrlSocket = conCtrlSocket();

	//creates and binds socket for recieveing directory data
	int dataSocket = bindDataSocket(1);

	//pointer to hold command
	char *Msg;
	//creates LIST command to send
	asprintf(&Msg,"LIST,%s,<CRLF>\0",arguments.pp_1);

	//sends the list command to the server
	sendCmd(ctrlSocket,Msg);
	
	//retrieves the directory list from the server on data
	const char *dataBuff = retList(dataSocket);

	//creates and bind socket for reciveing file data
	dataSocket = bindDataSocket(2);	

	const char *fileName;

	while(1){
		//dysplays directory and asks user for file
	    fileName = getFileName(dataBuff);

		//creates RET command to send
	    asprintf(&Msg,"RET,%s,%s,<CRLF>\0",fileName,arguments.pp_2);

		printf("%s\n",Msg);

		//sends the RET command to the Server
		sendCmd(ctrlSocket,Msg);

		char *buff = "<000>";
			
		char check[20];
		bzero(check,20);

		read(ctrlSocket,check,10);
				
		printf("Check :: %s\n",check);		
		
		if(strcmp(check,"<101>")==0){
			printf("Error :: Invalid File Name\n");
		}else{
			break;
		}
	
				
	}
	
	retFile(dataSocket,fileName);
	

	return 0;

}

/**
*	outgoing control connection 
*
*/
	

int conCtrlSocket(){

	int ctrlSocket;
	
	memset(&ctrl_sai, 0 ,sizeof(ctrl_sai));

	ctrl_sai.sin_family = AF_INET;
	ctrl_sai.sin_port = htons(atoi(arguments.cp));
	int reso = inet_pton(AF_INET,arguments.server_ip, &ctrl_sai.sin_addr);

	//establishes a file discriptor socket to recive data
	ctrlSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(ctrlSocket == -1){
		printf("connection failed : Socket");
		return 0;
	}

	//connects to server
	if(connect(ctrlSocket,(struct sockaddr *) &ctrl_sai,sizeof(ctrl_sai))==-1){
		printf("connect failed");
		close(ctrlSocket);
		return 0;
	}

	printf("Opening control connection to Server :: %s\n",arguments.server_ip);
	printf("On port :: %s\n",arguments.cp);

	return ctrlSocket;

}

/*
*	Creates new socket for incomming directory list
*	
*/

int bindDataSocket(int conType){
	

	int dataSocket, caiL;

	//establishes a socket to recive data
	dataSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	

	//check for successful connection
	if(dataSocket == -1){
		printf("data connection failed : Socket data\n");
		return 0;
	}

	memset(&data_sai, 0 ,sizeof(data_sai));

	data_sai.sin_family = AF_INET;
	if(conType == 1){
		data_sai.sin_port = htons(atoi(arguments.pp_1));
	}else{
		data_sai.sin_port = htons(atoi(arguments.pp_2));
	}

	data_sai.sin_addr.s_addr = htonl(INADDR_ANY);

	//binds incomming port specified as connection port argument
	if(bind(dataSocket,(struct sockaddr *)&data_sai, sizeof(data_sai))==-1){
		printf("bind failed\n");
		return 0;
	}

	//starts listening on provided command line argument for connection port 
	if(listen(dataSocket,2)==-1){
		printf("listen failed\n");
		close(dataSocket);
		return 0;
	}	

	printf("Listining on port data %d\n",conType);

	return dataSocket;

}


/*
*
*	Sends command request to server
*
*/

int sendCmd(int ctrlSocket,char* cmd){


	int size = strlen(cmd);
	
	write(ctrlSocket,&size,sizeof(size));		

	//writes command to command
	write(ctrlSocket,cmd,strlen(cmd));


	return 0;

}

	
/*
*
*	accepts connection for directory list retrival from
*	server on provided socket
*
*/

const char *retList(int dataSocket){
	
	
		char *dataBuff ="";
		int data1;
		
		// waits and accepts connection on conSocket		
		data1 = accept(dataSocket,0,0);
		if(data1 <0){
			printf("accept failed\n");
			close(dataSocket);
			return dataBuff;
		}else{
			
			printf("data1 connection accepted on client\n");

		}
	
		int size;
		
		//reads accepted socket for cmd msg size
		int rS = read(data1,&size,sizeof(size));

		//buff of size to hold incomming command
		dataBuff = calloc(size , sizeof(char));

		//readsm accepted socket for command
		read(data1,dataBuff,size);

		close(data1);

		//printf("%s\n",dataBuff);
		return dataBuff;	


}

/*
*
*	lists directory list and promts user for file name
*	wished to be downloaded for server
*
*/

const char *getFileName(const char *fileList){

	char file[LINE_MAX];

	printf("Please Select a File to Download:\n");
	printf("%s",fileList);
	fgets(file,sizeof(file),stdin);

	char *filePtr = file;

	strtok(filePtr, "\n");

	return filePtr; 
}

int retFile(int dataSocket,char* fileName){
	
	
		char *dataBuff ="";
		int data1;
		
		// waits and accepts connection on conSocket		
		data1 = accept(dataSocket,0,0);
		if(data1 <0){
			printf("accept failed\n");
			close(dataSocket);
			return 0;
		}else{
			
			printf("data1 connection accepted on client\n");

		}
	
						
		int size;
		
		//reads accepted socket for cmd msg size
		int rS = read(data1,&size,sizeof(int));
		
		dataBuff = (char*) calloc(size, sizeof(char));
	
		//reads accepted socket for command
		read(data1,dataBuff,size);

		close(data1);
	
		FILE *filePtr = fopen(fileName,"wb");

		fwrite(dataBuff,1,size,filePtr);

		printf("file recived :: %s :: %d\n",fileName,size);
		
		return 0;	


}
