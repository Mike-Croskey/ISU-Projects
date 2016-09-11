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

int createControl();
int createData(int type);
void* accData(void* val);
void* accControl(void* val);
void forkConnection(int dataSocket, int *pipe);
void dataParser(int *pipe1, int *pipe2, int *pipe3,int conSocket);

//program version 
const char *argp_program_version = "ver 0.43b";

//program bug address
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";

//program documentation 
const char doc[] = "This is the Server\n";


//list of supported arguments
static struct argp_option options[]={

{"con_port",	'a', "a", 0, "Prefered port for control connection port"},	
{"prefered_data_port_1",	'b', "b", 0, "Prefered port for data connection 1"},
{"prefered_data_port_2",	'c', "c", 0, "Prefered port for data connection 2"},
{"prefered_data_port_3",	'd', "d", 0, "Prefered port for data connection 3"},
{0}

};
///struct for storing the command line arguments
struct arguments{

	//number of arguments
	char *args[40];	
	//incomming connection port
	char *con_port;
	//Prefered port for data1
	char *pp_1;
	//Prefered port for data2
	char *pp_2;
	//Prefered port for data3
	char *pp_3;

	
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
		case 'b':
			arguments->pp_1 = arg;
			break;
		case 'c':
			arguments->pp_2 = arg;
			break;
		case 'd':
			arguments->pp_3 = arg;
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
struct sockaddr_in con_sai,data1_sai,data2_sai,data3_sai;
// struct to hold the cmd line arguments
struct arguments arguments;
// struct for holding parsed command
struct parse cmdParse;


//struct used for packet
struct packet{
	
	//packet type	
	char type;
	//current packets sequence num hex
	char pnum;
	//current packets data array
	char data[2];	
	//packet CRC feild
	char crcF[2];
	
};

//creats new packet struct
typedef struct packet packet_t;

//struct used for storing data connection details
struct sData{

	int fd;

	int type;

	int pipe[2];

};

//struct used for storing connection details
struct cData{

	int fd;

	int *pipes[3];

};


int main(int argc, char** argv){

	//Sets the default values for cmd line argument
	arguments.con_port = "";
	arguments.pp_1 = "";	
	arguments.pp_2 = "";
	arguments.pp_3 = "";

	//call the argument parsers argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//testing statments
	printf("%s\n", arguments.con_port);
	printf("%s\n", arguments.pp_1);
	printf("%s\n", arguments.pp_2);
	printf("%s\n", arguments.pp_3);
		
	printf("Starting MP TCP :: Server\n");
	
	// control stuct 
	struct cData sock_d;

	// stuct array for holding data stucts
	struct sData sock_s[3];

	//init pipe 1
	if(pipe(sock_s[0].pipe) == -1){ printf("Pipe Error"); return 0;}

	//assignes pointer in control struct for pipe1
	sock_d.pipes[0] = sock_s[0].pipe;	

	//inti pipe 2
	if(pipe(sock_s[1].pipe) == -1){ printf("Pipe Error"); return 0;}

	//assignes pointer in control struct for pipe1
	sock_d.pipes[1] = sock_s[1].pipe;	

	//init pipe 3
	if(pipe(sock_s[2].pipe) == -1){ printf("Pipe Error"); return 0;}

	//assignes pointer in control struct for pipe1
	sock_d.pipes[2] = sock_s[2].pipe;

	//creates thread id
	pthread_t threadId[4];
	

	int i;
	for(i=0;i<=3;i++){
		//creates atrribute
		pthread_attr_t atr;
		//inititalizes pthread
		pthread_attr_init(&atr);
		
		if(i==0){
			
			sock_d.fd = createControl();
			//accepts incomming connection from client
			pthread_create(&threadId[i],&atr,accControl,&sock_d);
		

		}else if(i==1){
			//creates new thread
		
			sock_s[i-1].fd = createData(i);
			sock_s[i-1].type = 1;
			pthread_create(&threadId[i],&atr,accData,&sock_s[i-1]);
		}else if(i==2){
	
			sock_s[i-1].fd = createData(i);
			sock_s[i-1].type = 2;
			pthread_create(&threadId[i],&atr,accData,&sock_s[i-1]);
		}else if(i==3){
		
			sock_s[i-1].fd = createData(i);
			sock_s[i-1].type = 3;
			pthread_create(&threadId[i],&atr,accData,&sock_s[i-1]);
		}

	}


	for(i=0;i<=3;i++){
		pthread_join(threadId[i],NULL);
	}

	//closes pipes
	close(sock_s[0].pipe[0]);
	close(sock_s[1].pipe[0]);
	close(sock_s[2].pipe[0]);
	close(sock_s[0].pipe[1]);
	close(sock_s[1].pipe[1]);
	close(sock_s[2].pipe[1]);

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
	conSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	memset(&con_sai, 0 ,sizeof(con_sai));

	con_sai.sin_family = AF_INET;
	con_sai.sin_port = htons(atoi(arguments.con_port));
	con_sai.sin_addr.s_addr = htonl(INADDR_ANY);

	//binds incomming port specified as connection port argument
	if(bind(conSocket,(struct sockaddr *)&con_sai, sizeof(con_sai))==-1){
		printf("bind failed\n");
		return 0;
	}

	//starts listening on provided command line argument for connection port 
	if(listen(conSocket,1)==-1){
		printf("listen failed\n");
		close(conSocket);
		return 0;
	}

	printf("Listining on Conection Socket\n");
	
	return conSocket;
}


/*
*
*	creates a socket for a data connection and begins listining for
*	incomming connections from MP TCP client
*
*/

int createData(int type){

	int conSocket;

	//establishes a socket
	conSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//check for successful socket creation
	if(conSocket == -1){
		printf("Create failed : Socket\n");
		return 0;
	}
	
	//allows the reuse of socket connection when bits are left in kernal
	int yes = 1;
	int setSock = setsockopt(conSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	
	if( setSock == -1){
		perror("setsocket");
	}	


switch(type){


case 1:
	//researves adress mem
	memset(&data1_sai, 0 ,sizeof(data1_sai));

	data1_sai.sin_family = AF_INET;
	data1_sai.sin_port = htons(atoi(arguments.pp_1));
	data1_sai.sin_addr.s_addr = htonl(INADDR_ANY);

	//binds incomming port specified as connection port argument
	if(bind(conSocket,(struct sockaddr *)&data1_sai, sizeof(data1_sai))==-1){
		printf("bind failed\n");
		return 0;
	}
	break;

case 2:
	//researves adress mem
	memset(&data2_sai, 0 ,sizeof(data2_sai));

	data2_sai.sin_family = AF_INET;
	data2_sai.sin_port = htons(atoi(arguments.pp_2));
	data2_sai.sin_addr.s_addr = htonl(INADDR_ANY);

	//binds incomming port specified as connection port argument
	if(bind(conSocket,(struct sockaddr *)&data2_sai, sizeof(data2_sai))==-1){
		printf("bind failed\n");
		return 0;
	}
	
	break;
case 3:
	//researves adress mem
	memset(&data3_sai, 0 ,sizeof(data3_sai));

	data3_sai.sin_family = AF_INET;
	data3_sai.sin_port = htons(atoi(arguments.pp_3));
	data3_sai.sin_addr.s_addr = htonl(INADDR_ANY);

	//binds incomming port specified as connection port argument
	if(bind(conSocket,(struct sockaddr *)&data3_sai, sizeof(data3_sai))==-1){
		printf("bind failed\n");
		return 0;
	}
	
	break;

}

	//starts listening for connection port 
	if(listen(conSocket,1)==-1){
		printf("listen failed\n");
		close(conSocket);
		return 0;
	}

	printf("Listining on Data Socket : %d\n",type);
	
	return conSocket;
}


/**
*	accepts incoming control connection 
*
*/
void* accControl(void* val){

	struct cData *conDetails = (struct cData*) val;

	int conSocket = conDetails->fd;

	int caiL = sizeof(con_sai);
	// waits and accepts connection on conSocket		
	int con = accept(conSocket, (struct sockaddr *) &con_sai, (socklen_t*) &caiL);

	if(con <0){
			printf("accept failed\n");
			close(conSocket);
			return 0;
	}else{
	
			printf("connection accepted\n");

	}

	dataParser(     conDetails->pipes[0],
			conDetails->pipes[1],
			conDetails->pipes[2],
			con);

}

/*
*
*	handles the parsing of data 
*
*
*/

void dataParser(int *pipe1, int *pipe2, int *pipe3, int conSocket){

	//pointer for data buffer
	char data[991];
	
	bzero(data,sizeof(data));

	data[991]='\0';
	
	//creates file for storing cwnd size
	FILE *myFile = fopen("./Sresults.txt","a");
	fprintf(myFile ,"Starting MP TCP :: Server\n");


	printf("Control:: Reading\n");
		
	while(1){

		//sleep(1);

		unsigned char conBuff[10];

		read(conSocket,&conBuff,9);
		
		conBuff[9] ='\0';

		printf("inc->  %s\n",conBuff);
		fprintf(myFile,"DSS :: %s\n",conBuff);
		fflush(myFile);
		//printf("con dbit0->> %c\n",conBuff[0]);
		//printf("con dbit1->> %c\n",conBuff[1]);
		//printf("con dbit2->> %c\n",conBuff[2]);
		//printf("con dbit3->> %c\n",conBuff[3]);
		//printf("con dbit4->> %c\n",conBuff[4]);
		//printf("con dbit5->> %c\n",conBuff[5]);
		//printf("con dbit6->> %c\n",conBuff[6]);
		//printf("con dbit7->> %c\n",conBuff[7]);
		//printf("con dbit8->> %c\n",conBuff[8]);
		
		char readBuff[4];
		
		if(conBuff[8] == '1'){
//creates sequence char string used for conversion			
unsigned char seqHex[] = {conBuff[0],conBuff[1],conBuff[2],conBuff[3],'\0'};
			int seq;
			//convertes char string to int
			seq = (int) strtol(seqHex,NULL,16);
			printf("seq num :: %d\n",seq);

//creates index char string used for conversion			
unsigned char indexHex[] = {conBuff[4],conBuff[5],conBuff[6],conBuff[7],'\0'};
			int index;
			//convertes char string to int
			index = (int) strtol(indexHex,NULL,16);
			printf("index num :: %d\n",index);

			//reads from pipe
			read(pipe1[0],&readBuff,4);
			readBuff[4]='\0';
		
			printf("Pipe 1 ->> %s\n",readBuff);
			
//if #FIN packet is recived exits loop
if((readBuff[0]=='#') && (readBuff[1]=='F') && (readBuff[2]=='I') && (readBuff[3]=='N')){			data[991]='\0';
				printf("DATA :: %s\n",data);
				fprintf(myFile, "---Tranfer completed\n");
				fprintf(myFile, "%s\n",data);
				break;
			}

			int s;
			int pos = 0;
			for(s=index;s<index+4;s++){
				
				data[s] = readBuff[pos];
				pos++;
			
			}
			
			fprintf(myFile, "|SEQ :: INDEX :: DATA|\n");
			fprintf(myFile, "|%d :: %d :: %s|\n", seq,index,readBuff);
			fprintf(myFile, "--------------------\n");
			

			fflush(myFile);

		}else if(conBuff[8]== '2'){

//creates char string used for convertion		
unsigned char seqHex[] = {conBuff[0],conBuff[1],conBuff[2],conBuff[3],'\0'};
			int seq;
			//converts the string to int
			seq = (int) strtol(seqHex,NULL,16);
			printf("seq num :: %d\n",seq);

//creates index char string used for conversion			
unsigned char indexHex[] = {conBuff[4],conBuff[5],conBuff[6],conBuff[7],'\0'};
			int index;
			//convertes char string to int
			index = (int) strtol(indexHex,NULL,16);
			printf("index num :: %d\n",index);


			//reads from pipe
			read(pipe2[0],&readBuff,4);
			readBuff[4]='\0';
		
			printf("Pipe 2 ->> %s\n",readBuff);

//if #FIN packet is recived exits loop
if((readBuff[0]=='#') && (readBuff[1]=='F') && (readBuff[2]=='I') && (readBuff[3]=='N')){			
				data[991]='\0';
				printf("DATA :: %s\n",data);
				fprintf(myFile, "---Tranfer completed\n");
				fprintf(myFile, "%s\n",data);
				break;
			}



			int s;
			int pos = 0;
			for(s=index;s<index+4;s++){
				
				data[s] = readBuff[pos];
				pos++;

			
			}
			
			
			fprintf(myFile, "|SEQ :: INDEX :: DATA|\n");
			fprintf(myFile, "|%d :: %d :: %s|\n" ,seq,index,readBuff);
			fprintf(myFile, "--------------------\n");

			fflush(myFile);

		}else if(conBuff[8] == '3'){

//creates char string used for convertion		
unsigned char seqHex[] = {conBuff[0],conBuff[1],conBuff[2],conBuff[3],'\0'};
			int seq;
			//converts string to int
			seq = (int) strtol(seqHex,NULL,16);
			printf("seq num :: %d\n",seq);

//creates index char string used for conversion			
unsigned char indexHex[] = {conBuff[4],conBuff[5],conBuff[6],conBuff[7],'\0'};
			int index;
			//convertes char string to int
			index = (int) strtol(indexHex,NULL,16);
			printf("index num :: %d\n",index);


			//reads from pipe
			read(pipe3[0],&readBuff,4);
			readBuff[4]='\0';
		
			printf("Pipe 3 ->> %s\n",readBuff);

//if #FIN packet is recived exits loop
if((readBuff[0]=='#') && (readBuff[1]=='F') && (readBuff[2]=='I') && (readBuff[3]=='N')){
				data[991]='\0';
				printf("DATA :: %s\n",data);
				fprintf(myFile, "---Tranfer completed\n");
				fprintf(myFile, "%s\n",data);
				break;
			}


			int s;
			int pos = 0;
			for(s=index;s<index+4;s++){
				
				data[s] = readBuff[pos];
				pos++;
			
			}

			
			
			fprintf(myFile, "|SEQ :: INDEX :: DATA|\n");
			fprintf(myFile, "|%d :: %d :: %s|\n" ,seq,index,readBuff);
			fprintf(myFile, "--------------------\n");

			fflush(myFile);
		}

	}

	fclose(myFile);
	

}


/**
*	accepts incoming data connection 
*
*/
void* accData(void* val){

	struct sData *conDetails = (struct sData*) val;

	int daiL;
	int con=0;
	int conSocket = 0;
	conSocket = conDetails->fd;

	switch(conDetails->type){

		case 1:
		daiL = sizeof(data1_sai);
		// waits and accepts connection on conSocket		
		con = accept(conSocket, (struct sockaddr *) &data1_sai,(socklen_t*) &daiL);

		if(con <0){
			printf("accept failed data 1\n");
			close(conSocket);
			return 0;
		}else{
	
			printf("connection accepted : 1\n");

		}

		break;

		case 2:
		daiL = sizeof(data2_sai);
		// waits and accepts connection on conSocket		
		con = accept(conSocket, (struct sockaddr *) &data2_sai,(socklen_t*) &daiL);

		if(con <0){
			printf("accept failed data 2\n");
			close(conSocket);
			return 0;
		}else{
	
			printf("connection accepted : 2\n");

		}		

		break;

		case 3:
		daiL = sizeof(data3_sai);
		// waits and accepts connection on conSocket		
		con = accept(conSocket, (struct sockaddr *) &data3_sai,(socklen_t*) &daiL);

		if(con <0){
			printf("accept failed data 3\n");
			close(conSocket);
			return 0;
		}else{
	
			printf("connection accepted : 3\n");

		}

		break;

	}

	

	forkConnection(con,conDetails->pipe);
}


/*
*
*	Creates a forked child process for the provided connection socket
*
*/

void forkConnection(int dataSocket, int *pipe){

	
	pid_t childPID;

	childPID = fork();
	//child process
	if(childPID == 0){

		printf("child\n");
		
		while(1){	
			
			//sleep(1);
			char writeBuff[3];
			printf("binging read\n");
			read(dataSocket,&writeBuff,4);
			
			printf("ending read\n");

			/*printf("writing :: %c%c%c%c\n",
				writeBuff[0],
				writeBuff[1],
				writeBuff[2],
				writeBuff[3]); */
		
			//write info to data controler pip
			write(pipe[1],writeBuff,4);	
			printf("write finished\n");
			
			if((writeBuff[0]=='#') && (writeBuff[1]=='F') && (writeBuff[2]=='I') && (writeBuff[3]=='N')){
				break;
			}


		}
		
	

		_exit(EXIT_SUCCESS);

	}


	wait(NULL);

}



