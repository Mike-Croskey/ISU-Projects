/**
 * @file     cclass_server.c
 * @author    Mike Croskey
 * @date      2015-04-11: Created
 * @brief     Emulate a class audit system that tracks degree progress
 * @copyright MIT License (c) 2015
 */
 
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <semaphore.h>
#include <argp.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "cclass_shm.h"




/// program version string
const char *argp_program_version = "ISU CprE308 ClassTracker Server 0.1";
/// program bug address string
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";
/// program documentation string
static char doc[] = "Class Tracker Server";
//txt file used for to store class information
static FILE *data_file;
//log file use for recording server activity
static FILE *log_file;
//verbose flage
static int Vflag=0;
//count for number of classes created 150 max
static int class_count = 0;
//Flag to indicate new audit has been created
static int naFlag = 0;



// list of options supported
static struct argp_option options[] = 
{

	{"verbose", 		'v', 0, 	0, "Produce verbose output"},
	{"quiet", 		'q', 0, 	0, "Don't produce any output"},
	{"log-file", 		'o', "FILE",    0, "The output log file"},
	{"daemon", 		'd', 0,   	0, "Run the server in Daemon mode"},
	{0}
	
};

/// arugment structure to store the results of command line parsing
struct arguments{

	/// are we in verbose mode?
	int verbose_mode;
	/// version number
	int quiet_mode;
	/// name of the log file
	char* log_file_name;
	///daemon
	int daemon;
	//number of arguments
	char *args[20];

	
};

struct class{

		unsigned int in_use;
		
		int id;
	
		char grade[3];
	
		char semester_completed[30];

		char student_name[50];
	
		char class_name[200];
	
		char instr_name[50];
	
		long int credits;
	
		char class_type[50];
	
		char class_description[3000];


}class_t[150];

typedef struct audit_d{

	char student_name[50];
	
	char degree[200];
	
	long int credits_req;
	
	long int credits_complete;
	
	int gpa;


}audit_t;

static audit_t usr_audit;

/**
 * @brief     Callback to parse a command line argument
 * @param     key
 *                 The short code key of this argument
 * @param     arg
 *                 The argument following the code
 * @param     state
 *                 The state of the arg parser state machine
 * @return    0 if succesfully handeled the key, ARGP_ERR_UNKONWN if the key was uknown
 */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{

	struct arguments *arguments = state->input;
	switch(key)
	{
		
		case 'v':
			arguments->verbose_mode = 1;
			break;
		case 'q':
			arguments->quiet_mode = 1;
			break;
		case 'o':
			arguments->log_file_name = arg;
			break;
		case 'd':
			arguments->daemon = 1;
			break;

		case ARGP_KEY_ARG:
    			
    			if (state->arg_num >= 5){
      				/* Too many arguments. */
        			argp_usage(state);
			}
      			
      			arguments->args[state->arg_num] = arg;

     			break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}



/// The arg parser object
static struct argp argp = {options, parse_opt, 0, doc};

int main(int argc, char* argv[]){

	//argp struct for handling input arguments
	struct arguments arguments;
	
	///Default values for  argp parsing struct
	arguments.verbose_mode = 0;
	arguments.quiet_mode = 0;
	arguments.log_file_name = "";
	arguments.daemon = 0;

	
	///parsing call to argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	///calls to set the program to run in deamon mode
	int deam;
	
	if(arguments.daemon == 1){
	
		printf("changing to deamon");
		deam = daemon(0,0);
		
	}
	
	if(arguments.verbose_mode == 1){
	
		Vflag = 1;
	
	}
	
	///creates a semaphore if not already created or opens existing used to protect shared memory
	sem_t *sem = sem_open(SEM, O_CREAT, 0644, 0);	
	sem_init(sem, 0, 1);
	
	///creates a file used for storing the finalized class audit
	
	char *file_name = "./cclass_data.txt";

	
	log_file = fopen(arguments.log_file_name, "a");
	
	//error check for log file
	if(log_file = NULL){
		perror("failed to open log file");
    	    	exit(1);
	}
	
	///first shared memory operation for creating new audits and adding new class to audit
	
	int shmd;
	// open the shared memory area creating it if it doesn't exist
	shmd = shm_open("/ccshm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP); 
	
	if(!shmd){
		perror("failed to open shared memory\n");
		return -1;
	}
	
	if(ftruncate(shmd, sizeof(struct SHM_CCLASS))){
		perror("failed to ftruncate\n");
		return -1;
	}
	
	struct SHM_CCLASS* sh_mem;
				
	sh_mem = mmap(NULL, sizeof(struct SHM_CCLASS), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
	
	if(!sh_mem){
		perror("failed to set mmap\n");
		return -1;
	}
	
	if(close(shmd)){
		perror("failed to close shared memory\n");
		return -1;
	}	


	//second shared memory access for handling class removal

	int shmd2;
	// open the shared memory area creating it if it doesn't exist
	shmd2 = shm_open("/ccshm", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP); 
	
	if(!shmd2){
		perror("failed to open shared memory\n");
		return -1;
	}
	
	if(ftruncate(shmd2, sizeof(struct SHM_CLIST))){
		perror("failed to ftruncate\n");
		return -1;
	}
	
	struct SHM_CLIST *sh_mem2;
				
	sh_mem2 = mmap(NULL, sizeof(struct SHM_CLIST), PROT_READ | PROT_WRITE, MAP_SHARED, shmd2, 0);
	
	if(!sh_mem2){
		perror("failed to set mmap\n");
		return -1;
	}
	
	if(close(shmd2)){
		perror("failed to close shared memory\n");
		return -1;
	}	

	if(Vflag == 1){
	
		printf("Verbose Enabled\n");
	}

	sem_wait(sem);
	
	sh_mem->job_status = 0;
	
	sem_post(sem);

	int j;
	for(j=0;j<150;j++){
	
		class_t[j].in_use = 0;

	}
	
	//struct created to hold audit data passed into the server from client

	int newj_status = 0;
	int newj_status2 = 0;
	int newj_code = 0;
	int newj_code2 = 0;


	int exit_server = 0;
	while(exit_server != 1){
		
		int ac = 0;
		
		sem_wait(sem);
		//jobset 1
		newj_status = sh_mem->job_status;
		newj_code = sh_mem->job_code;
		//jobset 2
		newj_status2 = sh_mem2->job_status;
		newj_code2 = sh_mem2->job_code;
		sem_post(sem);
		
		if(newj_status == 1 && newj_code != 0){
		
			if(Vflag == 1){
	
				printf("Job Recived : Job Code = %d\n", newj_code);
			}
			
			
			if(newj_code == 1){
			
				new_audit(sem,sh_mem);
				//printf("job status = %ld\n",newj_status);
				
				sem_wait(sem);
				sh_mem->job_status = 0;
				sem_post(sem);
				
			
			}else if(newj_code == 3){
				
				ac = add_class(sem,sh_mem);
				
				if(ac == 1){
					class_count++;
					printf("class_count = %d\n",class_count);
				
					sem_wait(sem);
					sh_mem->job_status = 0;
					sem_post(sem);
				}else{
					
					sem_wait(sem);
					sh_mem->job_status = 0;
					sem_post(sem);
					//job failed
					if(Vflag == 1){
	
						printf("Job failed :: class not added");
					}
					
				}	
			
			}else if(newj_code == 5){
			
				//Build audit and print to file for client to view
				build_audit(file_name, sem, sh_mem2);
				
				sem_wait(sem);
				sh_mem->job_status = 0;
				sh_mem->job_finished = 1;
				sh_mem->job_code = 0;
				sem_post(sem);
			
			
			}else{
				//invalid job code
				sem_wait(sem);
				sh_mem->job_status = 0;
				sh_mem->job_code = 0;
				sem_post(sem);
			
			}
			
		}
		
		if (newj_status2 == 1 && newj_code2 != 0){
		
			if(Vflag == 1){
	
				printf("Job Recived : Job Code = %d\n", newj_code2);
			}
			
			if(newj_code2 == 4){
				
				//calls remove user method
				remove_class(sem,sh_mem2);
							
				//sets job status flag to 0 indicating its finished
				sem_wait(sem);
				sh_mem2->job_status = 0;
				sh_mem2->job_finished = 1;
				sh_mem2->job_code = 0;
				sem_post(sem);
				
			
			}else if(newj_code2 == 6){
				
				//creates a list of current classes for removal method in client
				class_list(sem,sh_mem2);
				//sets job status flag to 0 indicating its finished
				sem_wait(sem);
				sh_mem2->job_status = 0;
				sh_mem2->job_finished = 1;
				sh_mem2->job_code = 0;
				sem_post(sem);
		
			}else{
				//invalid job code
				sem_wait(sem);
				sh_mem2->job_status = 0;
				sh_mem2->job_code = 0;
				sem_post(sem);
			
			}
				
		}

	}

}

int new_audit(sem_t *sem, struct SHM_CCLASS *sh_mem){

	
	sem_wait(sem);

	strcpy(usr_audit.student_name, sh_mem->student_name);
	strcpy(usr_audit.degree, sh_mem->degree);
	usr_audit.credits_req = sh_mem->credits_req; 
	
	//add audit creation verification for user if time permits
	sh_mem->job_finished = 1;
	
	sem_post(sem);
	
	if(Vflag == 1){
	
		printf("Created new audit:\n");
		printf("student name = %s :: ", usr_audit.student_name);
		printf("degree = %s :: ", usr_audit.degree);
		printf("student credits = %ld\n", usr_audit.credits_req);
	
	}
	
	class_count = 0;
	
	return 1;
}

int add_class(sem_t *sem, struct SHM_CCLASS *sh_mem){


	if(Vflag == 1){
	
		printf("begin class add @ server\n");
	}
	
	char s_name[50];
	
	sem_post(sem);
	
	strcpy(s_name, sh_mem->student_name);
	
	sem_wait(sem);
	
	
	//compares name to make sure audit matches the class
	if(strcmp(usr_audit.student_name,s_name)!=0){
		
		if(Vflag == 1){
			printf("%s\n", usr_audit.student_name);
			printf("%s",s_name);
			printf("failed to add class Student Name is differnt from audit @ server\n");
		}
		
		return 0;
	}else{
		
		//checkes existing array for removed classes that are set to null
		//this measure helps keep objects oreineted towards the begining array and speeds up list creation
		int c=0;
		while(c<149){
			if(class_t[c].in_use == 0){
		
				sem_post(sem);
				
				strcpy(class_t[c].student_name, sh_mem->student_name);
				strcpy(class_t[c].class_name, sh_mem->class_name);
				strcpy(class_t[c].instr_name, sh_mem->instr_name);
				strcpy(class_t[c].semester_completed, sh_mem->semester_completed);
				strcpy(class_t[c].grade, sh_mem->grade);
				strcpy(class_t[c].class_type, sh_mem->class_type);
				strcpy(class_t[c].class_description, sh_mem->class_description);
				class_t[c].credits = sh_mem->credits;
		
				sem_wait(sem);
	
				class_t[c].id = c + 1;	
				class_t[c].in_use = 1;
				//add return call for successfuly adding class
		
				if(Vflag == 1){
		
					printf("class add completed @ server\n");
					printf("Class = %s\n" ,class_t[c].class_name);
					printf("Instructor = %s\n" , class_t[c].instr_name);
					printf("Semster Completed = %s\n" , class_t[c].semester_completed);
					printf("Grade = %s\n" , class_t[c].grade);
					printf("Class Type = %s\n" , class_t[c].class_type);
					printf("Class Descr. = %s\n" , class_t[c].class_description);
					printf("Class Credits = %ld\n" , class_t[c].credits);
				}
				break; 
			}
		c++;	
		}
	}
	return 1;
}

int remove_class(sem_t *sem, struct SHM_CLIST *sh_mem){

	if(Vflag == 1){
	
		printf("begin class removal @ server\n");
	}
	
	int rjob_id;
	sem_post(sem);
	rjob_id = sh_mem->class_id;
	sem_wait(sem);

	if(Vflag == 1){
	
		printf("removing Job id : %d @ server\n",rjob_id);
		
	}
	

	int i;
	for(i=0; i<149; i++){
		if(class_t[i].in_use == 1){
			
			int irjob_id = 0;
	
			irjob_id = class_t[i].id;
			
			printf("search jobs :: %d\n" ,irjob_id);	
			
			if(irjob_id == rjob_id){
					
				class_t[i].in_use = 0;				
				//deincrements audits class count
				class_count--;
			}
		}	
	}

	
	if(Vflag == 1){
	
		printf("class removal completed @ server\n");
		printf("Removed Job Id :: %d @ server\n", rjob_id);
	}
	
	return 0;
}

int build_audit(char *file_name, sem_t *sem, struct SHM_CCLASS *sh_mem){
	
	if(class_count == 0){
		//open the file specified under by file_name
		data_file = fopen(file_name, "w+");
	
		//error check for data file
		if(data_file == NULL){
			perror("failed to open cclass_data.txt");
    		    	exit(1);
		}
		
		fprintf(data_file,"");
		fflush(data_file);
		
		fclose(data_file);
		return 0;	
		
	}
	
	if(Vflag == 1){
	
		printf("Begin class audit creation @ server: %s\n", usr_audit.student_name );
		printf("degree @ server: %s\n", usr_audit.degree );
		printf("credits @ server: %d\n", usr_audit.credits_req );
	}
	
	
	//open the file specified under by file_name
	data_file = fopen(file_name, "w+");
	
	//error check for data file
	if(data_file == NULL){
		perror("failed to open cclass_data.txt");
    	    	exit(1);
	}

	char *str_b1;
	char *str_b2;
	char *str_b3;
	char *str_b4;
	char *str_b5;
	char *str_b6;
	char *str_b7;
	char *str_b8;

	asprintf(&str_b1 ,"%s", "______________________________________________________________\n\n" );
	asprintf(&str_b2 ,"%s", "---------------------- CLASS TRACKER--------------------------\n\n" );
	asprintf(&str_b3 ,"%s", "______________________________________________________________\n\n" );
	asprintf(&str_b4 ,"Student Name :: %s \n",usr_audit.student_name);
	asprintf(&str_b5 ,"Degree :: %s \n", usr_audit.degree);
	asprintf(&str_b6 ,"Credits Required = %d \n", usr_audit.credits_req);

	
	fprintf(data_file,"%s%s%s%s%s%s",str_b1,str_b2,str_b3,str_b4,str_b5,str_b6);
	fflush(data_file);
	
	int t_cred = 0;
	float gpa = 0;
	float gpa_t = 0;
	int a;
	for(a=0;a<150;a++){
		
		if(class_t[a].in_use == 1){
			
			char *str_a1;
			char *str_a2;
			char *str_a3;
			char *str_a4;
			char *str_a5;
			char *str_a6;
			char *str_a7;
			char *str_a8;
		
			asprintf(&str_a1 ,"%s", "______________________________________________________________\n\n" );
			asprintf(&str_a2 ,"Course Name          :: %s \n" ,class_t[a].class_name);
			asprintf(&str_a3 ,"Instructor           :: %s \n" , class_t[a].instr_name  );
			asprintf(&str_a4 ,"Semester Completed    :: %s \n" , class_t[a].semester_completed);
			asprintf(&str_a5 ,"Grade                :: %s \n\n" , class_t[a].grade);
			asprintf(&str_a6 ,"Class Type           :: %s \n" , class_t[a].class_type);
			asprintf(&str_a7 ,"Credits              :: %ld \n\n" , class_t[a].credits);
			asprintf(&str_a8 ,"Class Description    :: %s \n" , class_t[a].class_description);
		
			t_cred = t_cred + class_t[a].credits;
			
			fprintf(data_file,"%s%s%s%s%s%s%s%s",str_a1,str_a2,str_a3,str_a4,str_a5,str_a6,str_a7,str_a8);
			fflush(data_file);
			
			
			/*
			A = 4.00 grade points
			A- = 3.70 grade points 
			B+ = 3.33 grade points
			B = 3.00 grade points
			B- = 2.70 grade points
			C+ = 2.30 grade points
			C = 2.00 grade points
			C- = 1.70 grade points
			D+ = 1.30 grade points
			D = 1.00 grade points
			D- = 0.70 grade points
			*/
			if(strcmp(class_t[a].grade, "A") == 0){
			
				gpa = gpa + 4;
				gpa_t = gpa_t + (4*class_t[a].credits);
				
			}else if(strcmp(class_t[a].grade, "B") == 0){
			
				gpa = gpa + 3;
				gpa_t = gpa_t + (3*class_t[a].credits);
			
			}else if(strcmp(class_t[a].grade, "C") == 0){
			
				gpa = gpa + 2;
				gpa_t = gpa_t + (2*class_t[a].credits);
			
			}else if(strcmp(class_t[a].grade, "D") == 0){
			
				gpa = gpa + 1;
				gpa_t = gpa_t + (1*class_t[a].credits);
			
			}else if(strcmp(class_t[a].grade, "F") == 0){
				
				//do nothing
			
			}else if(strcmp(class_t[a].grade, "I") == 0){
			
				//do nothing
			
			}
			
			
			
		}	
	
	
	
	}
	
	gpa = (gpa_t/ gpa);
	
	asprintf(&str_b7 ,"%s", "______________________________________________________________\n\n" );
	asprintf(&str_b8,"Total Credits Earned :: %ld\nGPA :: %.2f\n", t_cred , gpa);
	fprintf(data_file,"%s%s", str_b7,str_b8);
	fprintf(data_file,"END\n");
	fflush(data_file);

	sem_wait(sem);
	strcpy(sh_mem->file_name,file_name);	
	sem_post(sem);
	
	
	
	
	if(Vflag == 1){
	
		printf("Finished class audit creation @ server: %s\n",usr_audit.student_name);
	}
	
		
	fclose(data_file);
	
	return 0;
}

int class_list(sem_t *sem, struct SHM_CLIST *sh_mem){
	
	
	if(Vflag == 1){
	
		printf("begin class list creation @ server: %s\n",usr_audit.student_name);
	}
	
	int id;
	char *old_str;
	char *new_str;
	char *final_str;
	
	int t = asprintf(&old_str,"");
	
	int i;
	for(i=0;i<150;i++){
		
		if(class_t[i].in_use == 1){
		
			//sets pointer equal to return value of class name
			char tmp[50];
				
			strcpy(tmp,class_t[i].class_name);
						
			char *result = tmp;
		
			//removes newline char from string for proper name comparison on server
			char len = strlen(result);
			if(result != NULL && tmp[len - 1] == '\n'){
  				tmp[len - 1] = '\0';
			}else{
 		 
 			 	//add vflag arror msg for new line removal failure
  			}
			
		int s = asprintf(&new_str , "%s  :  %d" , tmp, class_t[i].id);
		
			int f = asprintf(&final_str, "%s\n%s" , old_str , new_str);
			
			int k = asprintf(&old_str, "%s" , final_str);	
			
			if(Vflag == 1){
	
				printf("Added class to list: %s\n",tmp);
			}
	
		}
	}
	
	int len = strlen(final_str);
	
	char clist[len+1];
	
	strcpy(clist,final_str);
	
	free(final_str);
	free(new_str);
	free(old_str);
	
	sem_post(sem);
	
	strcpy(sh_mem->class_list, clist);
	
	sem_wait(sem);

	if(Vflag == 1){
	
		printf("class list created @ server %s\n",usr_audit.student_name);
	}

	return 1;
}
