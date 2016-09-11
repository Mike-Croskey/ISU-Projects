#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/sysinfo.h>
#include <argp.h>
#include <sys/time.h>
#include <pthread.h>
#include <dirent.h>
#include <stdint.h>

#define HEX_JOB(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

//function defs
void buildData(char *data);
void forkConnection(int dataSocket, int *pipe);
void* conDataSocket(void *val);
void* conData2Socket(void *val);
void* conData3Socket(void *val);

//program version 
const char *argp_program_version = "MPTCP Client for CPRE_489 Final Project";

//program bug address
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";

//program documentation 
const char doc[] = "This program is intended to recreate the behalvior of an MPTCP protocal\nThis program utilizes 1 control connection and 3 data connection to transfer data in a similar fashion to a fully implemented MPTCP algorythm\nThis is the Client Side\nThis program establishes connection on 4 seperate ports given as command line arguments on the server ip also provide as command line argument\n";


//list of supported arguments
static struct argp_option options[]={

{"server_ip",			's', "s", 0, "Reciever IP address "},
{"prefered_connection_port_1",	'a', "a", 0, "Prefered port for control connection"},
{"prefered_data_port_1",	'b', "b", 0, "Prefered port for data connection 1"},
{"prefered_data_port_2",	'c', "c", 0, "Prefered port for data connection 2"},
{"prefered_data_port_3",	'd', "d", 0, "Prefered port for data connection 3"},	
{0}
};

///struct for storing the command line arguments
struct arguments{

	//number of arguments
	char *args[40];	
	//ftp server ip address
	char *server_ip;
	//Prefered port for connection
	char *pcp_1;
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

		case 's':
			arguments->server_ip = arg;
			break;
		case 'a':
			arguments->pcp_1 = arg;
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
			
			if(state->arg_num>=5){
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
struct sockaddr_in send_sai, data1_sai, data2_sai, data3_sai;

//Creates a new struct to hold the cmd line arguments
struct arguments arguments;
// struct for holding parsed command
struct parse cmdParse;

//struct used for packet structure
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

//struct used for storing connection details
struct sData{

	int fd;

	int type;

	int pipe[2];

};


int main(int argc, char** argv){
	
	//Sets the default values for cmd line arguments
	arguments.server_ip = "";
	arguments.pcp_1 = "";
	arguments.pp_1 = "";
	arguments.pp_2 = "";
	arguments.pp_3 = "";

	//call the argument parsers argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	//testing statments
	printf("%s\n", arguments.server_ip);
	printf("%s\n", arguments.pcp_1);
	printf("%s\n", arguments.pp_1);
	printf("%s\n", arguments.pp_2);
	printf("%s\n", arguments.pp_3);

	printf("Starting MP TCP :: Client\n");

	//creates file for storing cwnd size
	FILE *myFile = fopen("./Cresults.txt","a");
	fprintf(myFile ,"Starting MP TCP :: Client\n");
	//pointer for data buffer
	char *data = malloc(992 * sizeof(char));
	//builds data buffer
	buildData(data);
	data[992]='\0';

	//creates a control connection
	int ctrlSocket = conSendSocket();

	struct sData sock_s[3];

	//init pipe 1
	if(pipe(sock_s[0].pipe) == -1){ printf("Pipe Error"); return 0;}

	//inti pipe 2
	if(pipe(sock_s[1].pipe) == -1){ printf("Pipe Error"); return 0;}

	//init pipe 3
	if(pipe(sock_s[2].pipe) == -1){ printf("Pipe Error"); return 0;}

	//creates thread id
	pthread_t threadId[3];

	unsigned int i;
	for(i=0;i<3;i++){
		//creates atrribute
		pthread_attr_t atr;
		//inititalizes pthread
		pthread_attr_init(&atr);
		
		if(i==0){
		//creates new thread
			
		pthread_create(&threadId[i],&atr,conDataSocket,&sock_s[i]);
		}else if(i==1){
			
		pthread_create(&threadId[i],&atr,conData2Socket,&sock_s[i]);
		}else if(i==2){
			
		pthread_create(&threadId[i],&atr,conData3Socket,&sock_s[i]);
		}

	}


	//sequence num for tracking packets
	unsigned int seqNum = 0;

	//indicates current outgoing connection
	int conSelect = 1;

	//loops thru constucted data string	
	for(i=0;i<strlen(data);i+=4){
		
		//printf("got here\n");

		//data packet to send over stream
		char pData[3];
		//fills pack data segment		
		int k;		
		for(k=1;k<=4;k++){
			
			pData[k-1] = data[i+(k-1)];
			if(i+(k-1)>strlen(data)){
				 pData[k-1]='\0';
			}
			
		}
		
		//connection DSS packet
		char dss[10];
		
		//build DSS	
	
		//sets first 4 bytes to seq number
		dss[0] = HEX_JOB((((seqNum) & 0xF000) >> 12));
		dss[1] = HEX_JOB((((seqNum) & 0xF00) >> 8));	
		dss[2] = HEX_JOB((((seqNum) & 0xF0) >> 4));
		dss[3] = HEX_JOB(((seqNum) & 0x0F));
		
		//sets second 4 bytes to starting bit i
		
		dss[4] = HEX_JOB((((i) & 0xF000) >> 16));
		dss[5] = HEX_JOB((((i) & 0xF00) >> 8));	
		dss[6] = HEX_JOB((((i) & 0xF0) >> 4));
		dss[7] = HEX_JOB(((i) & 0x0F));
		
		dss[8] = conSelect + '0';
		
		printf("dss8 - %c\n",dss[8]);
		
		char *ptr;		

		ptr = dss;

		//selects data connection 
		//sends connection DSS packet on control connection
		switch(conSelect){

			case 1: 
				
				//sends control
				write(ctrlSocket,ptr,9);

				//writes DSS info to file
				fprintf(myFile ,"%d :: DSS :: %c%c%c%c%c%c%c%c%c\n",seqNum, dss[0],dss[1],dss[2],dss[3],dss[4],dss[5],dss[6],dss[7],dss[8]);

				//write packet to pipe
				write(sock_s[0].pipe[1],pData,4);
				
				//writes DSN packet to file
				fprintf(myFile,"%d :: DSN :: %c%c%c%c\n", seqNum,pData[0],pData[1],pData[2],pData[3]);				

				fflush(myFile);
				//advances connection index
				conSelect = 2;
				break;

			case 2:
				
				//sends control
				write(ctrlSocket,ptr,9);

				//writes DSS info to file
				fprintf(myFile ,"%d :: DSS :: %c%c%c%c%c%c%c%c%c\n",seqNum, dss[0],dss[1],dss[2],dss[3],dss[4],dss[5],dss[6],dss[7],dss[8]);

				//write packet to pipe
				write(sock_s[1].pipe[1],pData,4);

				//writes DSN packet to file
				fprintf(myFile,"%d :: DSN :: %c%c%c%c\n", seqNum,pData[0],pData[1],pData[2],pData[3]);
				fflush(myFile);
				//advances connection index
				conSelect = 3;
				break;

			case 3:
				
				//sends control
				write(ctrlSocket,ptr,9);
	
				//writes DSS info to file
				fprintf(myFile ,"%d :: DSS :: %c%c%c%c%c%c%c%c%c\n",seqNum, dss[0],dss[1],dss[2],dss[3],dss[4],dss[5],dss[6],dss[7],dss[8]);


				//write packet to pipe
				write(sock_s[2].pipe[1],pData,4);

				//writes DSN packet to file
				fprintf(myFile,"%d :: DSN :: %c%c%c%c\n", seqNum,pData[0],pData[1],pData[2],pData[3]);
				fflush(myFile);
				//advances connection index
				conSelect = 1;
				break;

		}
		
		
		printf("seqNum = %d\n",seqNum);
		printf("index = %d\n",i);
		//incr seq number
		seqNum++;
		
		//sleep(1);

	}

	char pData[]={'#','F','I','N','\0'};

	write(sock_s[0].pipe[1],pData,4);
	write(sock_s[1].pipe[1],pData,4);
	write(sock_s[2].pipe[1],pData,4);
	write(ctrlSocket,pData,4);

	//joins all threads
	for(i=0;i<3;i++){
		pthread_join(threadId[i],NULL);
	}

	//closes pipes
	close(sock_s[0].pipe[0]);
	close(sock_s[1].pipe[0]);
	close(sock_s[2].pipe[0]);
	close(sock_s[0].pipe[1]);
	close(sock_s[1].pipe[1]);
	close(sock_s[2].pipe[1]);

	
	free(data);
	fclose(myFile);

	return 0;

}


/**
*	outgoing control connection 
*
*/
	

int conSendSocket(){

	int sendSocket;
	
	memset(&send_sai, 0 ,sizeof(send_sai));

	send_sai.sin_family = AF_INET;
	send_sai.sin_port = htons(atoi(arguments.pcp_1));
	int reso = inet_pton(AF_INET,arguments.server_ip, &send_sai.sin_addr);

	//establishes a file discriptor socket to recive data
	sendSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(sendSocket == -1){
		printf("connection failed : Socket\n");
		return 0;
	}

	//connects to reciver
	if(connect(sendSocket,(struct sockaddr *) &send_sai,sizeof(send_sai))==-1){
		printf("connect failed\n");
		close(sendSocket);
		return 0;
	}


	printf("Opening Control Connection to Server :: %s\n",arguments.server_ip);
	printf("On port :: %s\n",arguments.pcp_1);

	return sendSocket;

}

/**
*	creates indicated outgoing data connection 
*
*	@param type : indicated which data connection to create
*/
	

void* conDataSocket(void* val){

	struct sData *conDetails = (struct sData*) val;

	int sendSocket;

	memset(&data1_sai, 0 ,sizeof(data1_sai));

	data1_sai.sin_family = AF_INET;

	data1_sai.sin_port = htons(atoi(arguments.pp_1));

	int reso = inet_pton(AF_INET,arguments.server_ip, &data1_sai.sin_addr);

	//establishes a file discriptor socket to recive data
	sendSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(sendSocket == -1){
		printf("connection failed : Socket\n");
		return 0;
	}

	//connects to reciver
	if(connect(sendSocket,(struct sockaddr *) &data1_sai,sizeof(data1_sai))==-1){
		printf("connect failed :: data 1\n");
		close(sendSocket);
		return 0;
	}

	printf("Opening connection to Server :: %s\n",arguments.server_ip);
	printf("On port :: %s\n",arguments.pp_1);
	

	forkConnection(sendSocket,conDetails->pipe);

}

/**
*	creates indicated outgoing data connection 
*
*	@param type : indicated which data connection to create
*/
	

void* conData2Socket(void* val){

	struct sData *conDetails = (struct sData*) val;

	int sendSocket;

	memset(&data2_sai, 0 ,sizeof(data2_sai));

	data2_sai.sin_family = AF_INET;

	data2_sai.sin_port = htons(atoi(arguments.pp_2));

	int reso = inet_pton(AF_INET,arguments.server_ip, &data2_sai.sin_addr);

	//establishes a file discriptor socket to recive data
	sendSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(sendSocket == -1){
		printf("connection failed : Socket\n");
		return 0;
	}

	//connects to reciver
	if(connect(sendSocket,(struct sockaddr *) &data2_sai,sizeof(data2_sai))==-1){
		printf("connect failed :: data 2\n");
		close(sendSocket);
		return 0;
	}

	printf("Opening connection to Server :: %s\n",arguments.server_ip);
	printf("On port :: %s\n",arguments.pp_2);
	

	forkConnection(sendSocket,conDetails->pipe);

}

/**
*	creates indicated outgoing data connection 
*
*	@param type : indicated which data connection to create
*/
	

void* conData3Socket(void* val){

	struct sData *conDetails = (struct sData*) val;

	int sendSocket;

	memset(&data3_sai, 0 ,sizeof(data3_sai));

	data3_sai.sin_family = AF_INET;

	data3_sai.sin_port = htons(atoi(arguments.pp_3));

	int reso = inet_pton(AF_INET,arguments.server_ip, &data3_sai.sin_addr);

	//establishes a file discriptor socket to recive data
	sendSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	//check for successful connection
	if(sendSocket == -1){
		printf("connection failed : Socket\n");
		return 0;
	}

	//connects to reciver
	if(connect(sendSocket,(struct sockaddr *) &data3_sai,sizeof(data3_sai))==-1){
		printf("connect failed :: data 3\n");
		close(sendSocket);
		return 0;
	}

	printf("Opening connection to Server :: %s\n",arguments.server_ip);
	printf("On port :: %s\n",arguments.pp_3);
	

	forkConnection(sendSocket,conDetails->pipe);

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
		
		close(pipe[1]);

		printf("child\n");

		while(1){

			//sleep(1);
			char readBuff[3];
		
			printf("binging read\n");
			//reads from pipe
			read(pipe[0],&readBuff,4);
		
			printf("ending read\n");

		/*	printf("writing :: %c%c%c%c\n",
			readBuff[0],
			readBuff[1],
			readBuff[2],
			readBuff[3]);  */

			//write info to data socet
			write(dataSocket,readBuff,4);
			printf("write finished\n");

			if((readBuff[0]=='#') && (readBuff[1]=='F') && (readBuff[2]=='I') && (readBuff[3]=='N')){
				break;
			}
		}
		
		close(pipe[0]);
		_exit(EXIT_SUCCESS);

	}

	//char *txt = "Piped from parent\n";
	//close(pipe[0]);
	//writes from pipe
	//write(pipe[1],txt,strlen(txt));	
	//close(pipe[1]);

	wait(NULL);

}


/*
*
*	returns a data buffer with 16 repititions of 0-9 , a-z, A-Z
*
*
*/

void buildData(char* data){

	char *buff = "abcdefghijklmnopqrstuvwxyz";
	
	char *buff2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	int cnt=0;

	int a;
	for(a=0;a<16;a++){

		int b;
		for(b=0;b<10;b++){
			data[cnt] = b +'0';
			cnt++;
		}
	
		int c;
		for(c=0;c<26;c++){
			
			data[cnt] = buff[c];
			cnt++;
		}

		int d;
		for(d=0;d<26;d++){
			
			data[cnt] = buff2[d];
			cnt++;
		}

	}
	data[cnt] = '\0';

}

