#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
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

//fuction declaration
const char *recvCmd(int conSocket);
const char *getDir();

//program version 
const char *argp_program_version = "FTP Server for CPRE_489 Lab 4";

//program bug address
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";

//program documentation 
const char doc[] = "This program is designed to act as a simplified ftp server\nThis program accepts a control channel port number\nThe server will wait for a connection from a client on control port.\nAfter establishing a control connection the server will wait for a command\nOnce a command is recieved the server will establish a data connection on outbound port prot# client specified port in recived command\nDepending on the command received the server will then handle the neccesary directory listing or file tranfer requested by the client\n\nAccepted commands are as follows:\n\nList port# <CRLF>\n -This command will query the servery for file list in /ftp dir\nRET file_name port# <CRLF>\n -This command will create a connection on given port#\n -The file will then be transfered.\n";


//list of supported arguments
static struct argp_option options[]={

		{"con_port",	'a', "a", 0, "Specify the desired incoming control connection port number"},
		{0}

};
///struct for storing the command line arguments
struct arguments{

	//number of arguments
	char *args[40];	
	//incomming connection port
	char *con_port;
	
	
};


//struct for holding results of command parse
struct parse{
	
	//parsed commands
	char *cmds[20];
	//numnber of commands
	int cmdCnt;
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


		case 'a':
			arguments->con_port = arg;
			break;
		
		case ARGP_KEY_ARG:
			
			if(state->arg_num>=2){
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
struct sockaddr_in sai,ssai,data_sai;
// struct to hold the cmd line arguments
struct arguments arguments;
// struct for holding parsed command
struct parse cmdParse;

int main(int argc, char** argv){


	
	//Sets the default values for cmd line argument
	arguments.con_port = "";
	

	//call the argument parsers argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//testing statments
	
	printf("%s\n", arguments.con_port);
		
	printf("Starting FTP Server\n");
	
	//creates a control connection socket	
	int conSocket = createControl();

	//accepts incomming connection from client
	int ctrCon = accControl(conSocket);		

	
	//receives command from client
	const char *CmdBuff = recvCmd(ctrCon);

	//allocates memory for parsed commands
	memset(&cmdParse, 0 ,sizeof(cmdParse));
	
	//parses the command string recieved from client
	parseCmd(CmdBuff);
	
	//handles the parsed commands
	tokenizedCommandHandler(cmdParse.cmdCnt,cmdParse.cmds);
	
	//loop flag
	int valid = 1;
	//looping structure to allow for invalid file request
	while(valid==1){
		//receives command from client
		CmdBuff = recvCmd(ctrCon);
		
		cmdParse.cmdCnt = 0;
		bzero(cmdParse.cmds,sizeof(cmdParse.cmds));		

		//parses the command string recieved from client
		parseCmd(CmdBuff);

		//handles the parsed commands
		valid = tokenizedCommandHandler(cmdParse.cmdCnt,cmdParse.cmds);

		if(valid == -1){
		
			char *buff = "<101>";
			write(ctrCon,buff,strlen(buff));
		
			valid =1;
		}else{
			char *buff2 = "<000>";
			write(ctrCon,buff2,strlen(buff2));

		}
	}
	

	return 0;

}

/*
*
*	creates a socket for a control connection and begins listining for
*	incomming connections for parsing commands
*
*/

int createControl(){

	int conSocket;

	//establishes a socket
	conSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//check for successful connection
	if(conSocket == -1){
		printf("connection failed : Socket\n");
		return 0;
	}
	
	//allows the reuse of socket connection when bits are left in kernal
	int yes = 1;
	int setSock = setsockopt(conSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	
	if( setSock == -1){
		perror("setsocket");
	}	

	//researves adress mem
	memset(&sai, 0 ,sizeof(sai));
	memset(&ssai, 0 ,sizeof(ssai));

	//set size wrapper
	socklen_t ssaiLen = sizeof(ssai);

	sai.sin_family = AF_INET;
	sai.sin_port = htons(atoi(arguments.con_port));
	sai.sin_addr.s_addr = htonl(INADDR_ANY);
	//int resi = inet_pton(AF_INET, "INADDR_ANY", &sai.sin_addr);

	//binds incomming port specified as connection port argument
	if(bind(conSocket,(struct sockaddr *)&sai, sizeof(sai))==-1){
		printf("bind failed\n");
		return 0;
	}

	//starts listening on provided command line argument for connection port 
	if(listen(conSocket,3)==-1){
		printf("listen failed\n");
		close(conSocket);
		return 0;
	}

	printf("Listining on Control Socket\n");
	
	return conSocket;
}

/**
*	accepts incoming control connection 
*
*/
int accControl(conSocket){

	int caiL = sizeof(ssai);
	// waits and accepts connection on conSocket		
	int con = accept(conSocket, (struct sockaddr *) &ssai, &caiL);

	if(con <0){
			printf("accept failed\n");
			close(conSocket);
			return 0;
	}else{
	
			printf("connection accepted\n");

	}

	return con;
}

/*
*
*	Recives command from provided connection
*
*/

const char *recvCmd(int con){

	int rSizeInt;
	
	//reads accepted socket for cmd msg size
	int rS = read(con,&rSizeInt,sizeof(rSizeInt));

	//creates a buff of size rSizeInt to hold incomming command
	char *conCmdBuff;
	conCmdBuff = calloc(rSizeInt+1 , sizeof(char));

	//reads accepted socket for command
	read(con,conCmdBuff,rSizeInt);

	printf("%s\n",conCmdBuff);	

	return conCmdBuff;

}

/*
*
*	parses command string recived from client
*	parses command string into seperate tokens
*/

int parseCmd(char *CmdBuff){

	//pointer to current token in string
	char *token;

	//sets pointer to first token using space a first delimeter
	token = strtok(CmdBuff,",");

	//iterates remaining tokens and copys them to pointer array
	while(token!=NULL){
	
		//alocates memory for holding string in pointer array
		cmdParse.cmds[cmdParse.cmdCnt] = malloc(strlen(token)+1);
		//copies tokenized string to pointer array
		strcpy(cmdParse.cmds[cmdParse.cmdCnt],token);
		//finds next token in string if any

		token = strtok(NULL,",");
		//updates cmd count
		cmdParse.cmdCnt++;	

	}

	printf("command cnt :: %d\n",cmdParse.cmdCnt);
	return 0;

}

/**
*
*
*	handles tokenized command input recived on control connection
*	@param cmdCnt : number of recived commands
*	@param cmds : pointer to a array of strings holding commands
*
*/

int tokenizedCommandHandler(int cmdCnt,char **cmds){

	//check flags
	int cF1,cF2,cF3,cF4 = 0;

	int port=0;
	//handle tokenized command
	if(cmdCnt==3){

		if(strcmp(cmds[0],"LIST")==0){
				
			cF1 =1;
		
		}else{
			printf("server : invalid command token 1: Expected LIST or RET\n");	
			printf("token 1 :: %s\n",cmds[0]);	
		}

		port = atoi(cmds[1]);			

		if((port>1020)&&(port<65535)){
				
			cF2 =1;
		
		}else{
			printf("server : invalid command token 2 : Expected port range 1024-65535\n");
			printf("Entered Port:: %d\n",port);	
		}


		if(strcmp(cmds[2],"<CRLF>")==0){
				
			cF3 =1;
		
		}else{
			printf("server : invalid command token 3 : Expected <CRLF> termination\n");
			printf("token 3 :: %s\n",cmds[2]);	
		}
		
		if((cF1==1)&&(cF2==1)&&(cF2==1)&&(cF4==0)){
		
			const char *dir = getDir();
			
			sendDir(dir,port);

		}

	}else if(cmdCnt == 4){

		if(strcmp(cmds[0],"RET")==0){
				
			cF1 =1;
		
		}else{
			printf("server : invalid command token 1: Expected LIST or RET\n");	
			printf("token 1 :: %s\n",cmds[0]);	
		}

		//checks to see if file is in directory		
		const char *dir = getDir();

		if(strstr(dir,cmds[1])!=NULL){
				
			cF2 =1;
		
		}else{
			printf("server : invalid command token 2: Expected Valid File Name\n");	
			printf("token 2 :: %s\n",cmds[1]);
			
			cF2 = -1;
		}

		port = atoi(cmds[2]);			

		if((port>1020)&&(port<65535)){
				
			cF3 =1;
		
		}else{
			printf("server : invalid command token 3 : Expected port range 1024-65535\n");
			printf("Entered Port:: %d\n",port);	
		}


		if(strcmp(cmds[3],"<CRLF>")==0){
				
			cF4 =1;
		
		}else{
			printf("server : invalid command token 4 : Expected <CRLF> termination\n");
			printf("token 4 :: %s\n",cmds[3]);	
		}
		
		if((cF1==1)&&(cF2==1)&&(cF2==1)&&(cF4==1)){
		
			
			sendFile(cmds[1],port);
			
		}

	}else if(cmdCnt>4){

		printf("server : invailed command :: Command Count > 4\n");

	}

	if(cF2 == -1){
	
		return -1;

	}else{
	
		return 0;
	}
}

/*
* 	creates a new socket and set address
* 	server connects to client on data port
* 	provided by the client control connection
*
*/

int bindDataSocket(int port){

	int dataSocket;

	//get client address from control connection
	char *ip = inet_ntoa(ssai.sin_addr);	

	memset(&data_sai, 0 ,sizeof(data_sai));

	data_sai.sin_family = AF_INET;
	data_sai.sin_port = htons(port);
	int reso = inet_pton(AF_INET,ip, &data_sai.sin_addr);

	dataSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(dataSocket == -1){
		printf("data connection failed : Socket data1\n");
		return 0;
	}else{
		
		printf("socket success data1\n");		
	
	}
		
	//connects to server
	if(connect(dataSocket,(struct sockaddr *) &data_sai,sizeof(data_sai))==-1){
		printf("connect failed :: data1\n");
		close(dataSocket);
		return 0;
	}else{

		printf("Opening Data connection to Server :: %s\n",ip);
		printf("On port :: %d\n",port);
	}	

	return dataSocket;

}

/**
*
*	
*	this function checks for valid command flags for LIST command
*	if flags pass then the dir is fetched and the call is then made to
*	send the directory on the proided data port. the client control connection
*	is used in the underlying call to set ip address from global struct.
*/

const char *getDir(){
		
		
	//gets current working dir as			
	char path[1024];
	getcwd(path,1024);

	printf("Dir :: %s\n",path);
	
	//struct for holding dir nodes		
	struct dirent *ent;

	//directory pointer
	DIR *directory;
	//opens directory [path] found above
	directory = opendir(path);
	//counter for number of files
	int fileCnt = 0;
	//pointers for building directory string
	char *dirListPtr;
	char *temp;
	//initial run flag
	int flag=0;

	/*
	*	Iterates thru file directory
	*	Builds string from files in dir
	*/

	while((ent = readdir(directory)) != NULL){
		
		//ignores .. . dir pointers
		if(strcmp(ent->d_name,".")==0){
			//printf("%s\n",ent->d_name);
			//do nothing
		}else if(strcmp(ent->d_name,"..")==0){
			//printf("%s\n",ent->d_name);
			//do nothing
		}else{
			fileCnt++;
			if(flag==0){
			
	if(asprintf(&dirListPtr ,"%s\n",ent->d_name));
	flag=1;
			}else{
	if(asprintf(&temp ,"%s",dirListPtr));
	if(asprintf(&dirListPtr ,"%s%s\n",temp, ent->d_name));

			}
		}

	}
	//printf("%s",dirListPtr);	

	return dirListPtr;
		
}

/**
*
*	this function is in charge of sending the directory to the desired port
*	it uses the ip adress of the control connection server-client for ip
*/
int sendDir(char* dir, int port){
	

	//opeens socket
	int dataSocket = bindDataSocket(port);
	
	//length of directory srting
	int size = strlen(dir);
	
	//writes size of command msgs string to socket
	write(dataSocket,&size,sizeof(size));		

	//writes command to command
	write(dataSocket,dir,strlen(dir));

	printf("Directory listing sent\n");
	printf("Closing Data1 connection Socket\n");
	close(dataSocket);


	return 1;
}

/**
*
*	this function is in charge of sending the file to the desired port
*	it uses the ip adress of the control connection server-client for ip
*/
int sendFile(char* filename, int port){	


	FILE *filePtr;
	int fileLen;
	size_t parseLen;
	char *fileBuffer;

	char* filePath;

	asprintf(&filePath,"./%s",filename);

	filePtr = fopen(filename,"rb");
	
	printf("Begining File Transfer :: %s\n",filePath);
	
	//opeens socket
	int dataSocket = bindDataSocket(port);
		
	char dir[] = "pass";

	//this will iterate thru the file and find total size
	fseek(filePtr,0,SEEK_END);
	fileLen = ftell(filePtr);
	//resets the file pointer to the begining of file	
	fseek(filePtr,0,SEEK_SET);

	//allocates memory to hold file in buffer
	fileBuffer = malloc (char*)(sizeof(char)*fileLen);

	//reads file and outputs to buffer
	parseLen = fread(fileBuffer,1,fileLen,filePtr);

	if(parseLen != fileLen){printf("File Read Error\n");}

	fclose(filePtr);

	char *sizeMsgPtr;

	printf("File Length :: %d\n",fileLen);
	
	//writes size of command msgs string to socket
	write(dataSocket,&fileLen,sizeof(fileLen));

	//writes command to command
	write(dataSocket,fileBuffer,strlen(fileBuffer));
		
	printf("File transfer Complete\n");
	printf("Closing Data connection\n");
	close(dataSocket);

	return 1;
}


