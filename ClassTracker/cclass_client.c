/**
 * @file     cclass_client.c
 * @author    Mike Croskey
 * @date      2015-04-11: Created
 * @brief     Emulate a user client for a class audit system that tracks degree progress
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
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <getopt.h>
#include "cclass_shm.h"

/// program version string
const char *argp_program_version = "ISU CprE308 ClassTracker User Client 0.1";
/// program bug address string
const char *argp_program_bug_address = "Mike Croskey: <mcroskey@iastate.edu>";
/// program documentation string
static char doc[] = "Class Tracker Client";
/// txt file used for to store class information
static FILE *data_file;
/// log file use for recording server activity
static FILE *log_file;
/// verbose flage
static int vFlag = 0;

//the students name for current audit
static char *std_name;
//the current type of degree 
static char *degree;
//number of credits required for current degree
static long int credits_req;
//number of credits completed for current degree
static int credits_complete;
//current users gpa
static int gpa;



// list of options supported
static struct argp_option options[] = 
{

	{"verbose", 		'v', 0, 	0, "Produce verbose output"},
	{"quiet", 		'q', 0, 	0, "Don't produce any output"},
	{"log-file", 		'o', "FILE",    0, "The output log file"},
	{0}
	
};

/// arugment structure to store the results of command line parsing
struct arguments
{

	/// are we in verbose mode?
	int verbose_mode;
	/// version number
	int quiet_mode;
	/// name of the log file
	char* log_file_name;
	//number of arguments
	char *args[5];

	
};



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
/// struct for passing build
struct class_d *course;



int main(int argc, char* argv[]){

	///creates a semaphore if not already created or opens existing used to protect shared memory
	sem_t *sem = sem_open(SEM, O_CREAT, 0777, 0);	

	struct arguments arguments;
	
	///Default values for  argp parsing struct
	arguments.verbose_mode = 0;
	arguments.quiet_mode = 0;
	arguments.log_file_name = "";
	
	///parsing call to argp
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	
	///calls to set the program to run in deamon mode
	int deam;
	
	if(arguments.verbose_mode == 1){
	
		vFlag = 1;
	
	}
	


	log_file = fopen(arguments.log_file_name, "a");
	
	//error check for log file
	if(log_file = NULL){
		perror("failed to open log file");
    	    	exit(1);
	}
	
	///perform shared memory operations
	
	int shmd;
	// open the shared memory area creating it if it doesn't exist
	shmd = shm_open("/ccshm", O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP); 
	
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
	
	sem_init(sem, 0, 1);
	
	sem_wait(sem);
	///////////////
	//put access to shared memory location 
	///////////////
	
	sh_mem->job_status = 0;
	
	sem_post(sem);

	//remove_class(sem);

	/////to do:

	//	create functionality for main menu, add classes, remove classes
	//	-send data in char string to server via ipc
		
	//	class info is seperated in char string by open symbol #cd and close symbol #eocd
	//	user data is seperated in cahr string by symbol #ud and close symbol #eoud
	
	//	create a function for printing current audit to screen
	//
	
	//top level loop that runs user interaction
	int exit = 0;
	while(exit!=1){
		
		//loop for main menu
		int eloop_main = 0;
		while(eloop_main != 1){

			
			int mr = mmenu();
			if( mr == 3 ){
				
				printf("Thank You for Using Class Tracker Audit System");
				return 0;
				
			}else if( mr == 2){
				//to do open_audit();	extra credit
				printf("Thank You for Using Class Tracker Audit System");
				return 0;
				
								
			}else if( mr == 1){
					
				//recursive menu loop for open audit menu
				int eloop_create = 0;
				while(eloop_create != 1){
						
					//create new audit
					int ca = create_audit();
					if(ca == 1){
						
						
						sem_wait(sem);
						//sets shared memory for new job with type 1 for new audit and sets info needed to create new audit in server
						sh_mem->job_status = 1;
						sh_mem->job_code = 1;
						sh_mem->job_finished = 0;
						strcpy(sh_mem->student_name, std_name);
						strcpy(sh_mem->degree, degree);
						sh_mem->credits_req = credits_req;
						
					
						sem_post(sem);
						
						//recursive modify audit loop
						int eloop_mod = 0;
						while(eloop_mod != 1){
						
							//calls function for modify audit menu
							int ma = mod_audit();
							if(ma == 1 ){
						
								//recursive loop for add remove
								int eloop_ar = 0;
								while (eloop_ar != 1){
					
									//calls add or remove course function
									int ar_r = add_remove_audit();
									if(ar_r == 1){
										
										//recursive loop for adding course menu
										int eloop_add = 0;
										while(eloop_add != 1){
																			
											//calls add method
											int aa = add();
											if(aa == 1){
												sem_wait(sem);
												//sets shared memory pointer to course struct created by add
												
												sh_mem->job_code = 3;
							//<<------------------------------------<<
												
							strcpy(sh_mem->student_name, course->student_name);
							strcpy(sh_mem->class_name, course->class_name);
							strcpy(sh_mem->instr_name, course->instr_name);
							strcpy(sh_mem->semester_completed, course->semester_completed);	
							strcpy(sh_mem->class_type, course->class_type);
							strcpy(sh_mem->class_description, course->class_description);
							strcpy(sh_mem->grade, course->grade);
							sh_mem->id = course->id;
							sh_mem->credits = course->credits;
							//<<------------------------------------->>
												//sh_mem->job_finished = -1;
												sh_mem->job_status = 1;
												sem_post(sem);
												break;
												
											}else if(aa == 2){
												eloop_add = 1;
												eloop_ar = 1;
											}
										}
										
									}else if(ar_r ==2){
									
										//recursive loop for removeing course menu
										int eloop_rm = 0;
										while(eloop_rm != 1){
											//calls remove method
											int rr = remove_class(sem);
											if(rr == 1){
											
											eloop_rm =1;
											}else if(rr == 2){
											
											
											eloop_rm =1;
											
											}else{
											//handle errors
											}	
										}
										
									}else if(ar_r == 3){
										//sets flag for user exiting menu
										eloop_ar = 1;
									}
								}
								
							}else if(ma == 2){
								//View Audit
								//recursive loop for viewing audit
								int eloop_view = 0;
								while(eloop_view != 1){
									
									//calls fuciton for veiwing current audit
									int va = view_audit(sem,sh_mem);
									if(va == 2){
										eloop_view = 1;
									}
								}
								
							}else if(ma == 3){
								//sets flag to break on user exit from mod menu
								eloop_mod = 1;
								eloop_create = 1;	
							}
								
						}
							
					}else if(ca == 2){
						//sets flag to break out of loop from user exit
						eloop_create = 1;
					}		
					
				}						
			}
				
		}
	
	}

}

int mmenu(void){
		
	printf("Welcome to Class Tracker! Your personal system for keeping records on all of you courses. \n Please select from the menu options: \n 1) Create an audit \n 2) Exit \n Enter 1 or 2: \n");
	char str[2];
	
	char *result = fgets(str, sizeof(str), stdin);
	
	char len = strlen(str);
	
	if(result != NULL && str[len - 1] == '\n'){
  		str[len - 1] = '\0';
	}else{
 	 // handle erro
  	}
	
	long int value = strtol(result,NULL,10);

	return value;
}

int create_audit(void){

	//variable for temporary storing user data
	char c_user[50];
	char adegree[200];
	char tcredS[4];
	long int tcred;
	
	int k=0;
	
	while(k!=1){
		
		printf("Create a new Degree Audit:\n--------------------------------\n");
		///requests student name from stdin
		printf("Enter Student Name: (max length 50)\n");
		
		
		fgets(c_user, sizeof(c_user) ,stdin);
		
		char *result = fgets(c_user, sizeof(c_user) ,stdin);
		
		char len = strlen(c_user);
	
		if(result != NULL && c_user[len - 1] == '\n'){
  			c_user[len - 1] = '\0';
		}else{
 		 // handle erro
  		}
		
		
	
		//requests type of degree from stdin
		printf("Enter Type of Degree: (max length 200) \n");
		result = fgets(adegree,200,stdin);
		len = strlen(adegree);
		
		//removes /n character from string
		if(result != NULL && adegree[len - 1] == '\n'){
  			adegree[len - 1] = '\0';
		}else{
 		 // handle erro
  		}
		
	
		//requests total credits required
		printf("Enter total credits required for degree completion: (max 3 digit)\n");
		fgets(tcredS,4,stdin);
		tcred = strtol(tcredS,NULL,10);
				
		while(1){
			
			printf("Is the following information correct?\n Name = %s\n Degree = %s\n Total Credits = %ld \n\n Enter 1 (yes) or 2 (no) or 3 (exit):\n" , c_user, adegree, tcred);
			char result[2];
			char *results = fgets(result,sizeof(result), stdin);
			
			char len = strlen(result);
			
			if(result != NULL && c_user[len - 1] == '\n'){
  				c_user[len - 1] = '\0';
			}else{
 			 // handle erro
  			}
			
			//checks for null
			if(result==NULL){
				printf("invalid entry");
			}else{
			
				//converts string to long int using strtol
				long int value = strtol(result,NULL,10);
				if(value == 1 || value == 2 || value == 3){
				
					//if yes sets break flag and sets global values for current audit
					if(value == 1){
						
						//sets break flag
						k=1;
						//sets Student name
						std_name = c_user;
						//sets degree name
						degree = adegree;
						//sets total credits
						credits_req = tcred;
						
						return 1;
					}else if(value == 2){
					
						break;
						
					}else if(value == 3){
						
						//exits function
						return 2;
					}				
					
				}else{
					printf("Invalid Entry");
				}
			}
		}
	
	}
	
	return 0;
}

int mod_audit(void){

	
	int k = 0;
	
	while(k!=1){
	
		printf("Please select the desiered Action:\n 1) Add/Remove courses \n 2) View Curent Audit \n 3) Exit \n Please Enter 1 or 2 or 3 : \n");
		
		char result[2];
		
		fgets(result, sizeof(result), stdin);
		
		char *results = fgets(result, sizeof(result), stdin);
				
		char len = strlen(result);
	
		if(result != NULL && result[len - 1] == '\n'){
  			result[len - 1] = '\0';
		}else{
 		 // handle erro
  		}
		
				
		//checks for null
		if(result==NULL){
			printf("invalid entry");
		}else{
			
			//converts string to long int using strtol
			long int value = strtol(result,NULL,10);
			if(value == 1 || value == 2 || value == 3){
				
				if(value == 1){
					return 1;
				}else if(value == 2){
					return 2;
				}else if(value == 3){
					return 3;
				}
		
			}else{
				printf("Invalid Entry");
			}
		}
	}

	return 0;
}

int add_remove_audit(void){

	int k = 0;
	while(k!=1){
	
		printf("Here you can Add and/or Remove Courses from the current audit: \n 1) Add \n 2) Remove \n 3) Exit \n Enter 1 or 2 or 3 :\n");
		
		char result[2];
		fgets(result, sizeof(result), stdin);
		fgets(result, sizeof(result), stdin);
				
		//checks for null
		if(result==NULL){
			printf("invalid entry");
		}else{
			
			//converts string to long int using strtol
			long int value = strtol(result,NULL,10);
			if(value == 1 || value == 2 || value == 3){
				
				if(value == 1){
					return 1;
				}else if(value == 2){
					return 2;
				}else if(value == 3){
					return 3;
				}
		
			}else{
				printf("Invalid Entry");
			}	
		}
	}

	return 0;
}


int view_audit(sem_t *sem, struct SHM_CCLASS *sh_mem){

	///build parser to read data file and print to screen
	///wait untill data file builder is created
	sem_wait(sem);
	
	sh_mem->job_code = 5;
	sh_mem->job_finished = 0;
	sh_mem->job_status = 1;
	
	sem_post(sem);
	
	char file[1500];
	
	while(1){
	
		printf("Waiting for audit to complete building.\n");
		
		sem_wait(sem);
		//checks for job completion from server	
		int val = sh_mem->job_finished;
		sem_post(sem);
		
		if(val == 1){
			
			sem_wait(sem);
			strcpy(file, sh_mem->file_name);
			sem_post(sem);
			break;
		}else{
		
			//handle errors
		}

	}	

	printf("Recieved File :: %s\n", file);
	//open the file specified under by file_name
	data_file = fopen(file, "r");
	
	//error check for data file
	if(data_file == NULL){
		perror("failed to open cclass_data.txt");
    	    	exit(1);
	}

	char f_txt[1000];
	int cnt = 0;
	while( fgets( f_txt , 1000, data_file ) ){
		
		if(strcmp(f_txt, "END") == 0){
			break;
		}
		
		
		printf("%d :: " , cnt );
		printf("%s", f_txt );
		cnt++;
	}

	

	fclose(data_file);

	return 2;

}

int add(void){

	/* struct variables
	
	char *grade;
	
	char *semester_completed;

	char *student_name;
	
	char *class_name;
	
	char *instr_name;
	
	long int *credits;
	
	char *class_type;
	
	char *class_description;

	*/
	
	course = calloc(1,sizeof(course));
	
	//variables for temporary storing user data
	char sname[50];
	char cname[200];
	char iname[50];
	char ctype[50];
	char sm_comp[30];
	char cdesc[3000];
	char credS[4];
	long int cred;
	char grade[3];
	
	
	int k=0;
	
	while(k!=1){
		
		printf("Add a new Course to the current Class Tracker Degree Audit:\n");
		///requests student name from stdin
		printf("Enter Student Name: (max length 50) \n");
		fgets(sname,sizeof(sname),stdin);
		
		//sets pointer equal to return value of fgets
		char *result = fgets(sname,sizeof(sname),stdin);
		
		//removes newline char from string for proper name comparison on server
		char len = strlen(result);
		if(result != NULL && sname[len - 1] == '\n'){
  			sname[len - 1] = '\0';
		}else{
 		 
 		 	//add vflag arror msg for new line removal failure
  		}
		
		strcpy(course->student_name, sname);
		
		//requests type of degree from stdin
		printf("Enter Class Name : (max length 200) \n");
		fgets(cname,sizeof(cname),stdin);
		strcpy(course->class_name, cname);
		
		printf("Enter Instructor Name: (max length 50) \n");
		fgets(iname,sizeof(iname),stdin);
		strcpy(course->instr_name, iname);
		
		printf("Enter Class type: ex -- general education (max length 50) \n");
		fgets(ctype,sizeof(ctype),stdin);
		strcpy(course->class_type, ctype);
		
		printf("Enter Semester of Completion: ex -- Fall 2015 (max length 30) \n");
		fgets(sm_comp,sizeof(sm_comp),stdin);
		strcpy(course->semester_completed, sm_comp);
	
		//requests credits earned
		printf("Enter credits earn for course completion: (max 1 digit)\n");
		result = fgets(credS,sizeof(credS),stdin);
				//removes newline char from string for proper name comparison on server
		len = strlen(credS);
		if(result != NULL && credS[len - 1] == '\n'){
  			credS[len - 1] = '\0';
		}else{
 		 
 		 	//add vflag arror msg for new line removal failure
  		}
		
		cred = strtol(credS,NULL,10);
		course->credits = cred;
		
		//requests grade earned
		printf("Enter Grade Recived: (A,B,C,D,F,I  no(+/-))\n");
		result = fgets(grade,sizeof(grade),stdin);
		
		len = strlen(grade);
		if(result != NULL && grade[len - 1] == '\n'){
  			grade[len - 1] = '\0';
		}else{
 		 
 		 	//add vflag arror msg for new line removal failure
  		}
		
		strcpy(course->grade, grade);
		
		printf("Enter Brief Course Description: (max length 3000 characters) \n");
		
		fgets(cdesc,sizeof(cdesc),stdin);
		strcpy(course->class_description, cdesc);
		
		//loop for info check		
		while(1){
			
			///print statment for checking entered information
			printf("Is the following information correct?\n");
			printf("Student = %s\n", course->student_name);
			printf("Class Name = %s\n", course->class_name);
			printf("Instructor Name = %s\n", course->instr_name);
			printf("Class Type = %s\n", course->class_type);
			printf("Semester Completed = %s\n", course->semester_completed);
			printf("Credits = %ld\n\n", course->credits);
			printf("Grade =%s\n\n", course->grade);
			printf("Description = %s\n", course->class_description);
			printf("Enter 1 (yes) or 2 (no) or 3 (exit):\n");
			char result[2];
			fgets(result,sizeof(result), stdin);
			
			//checks for null
			if(result==NULL){
				printf("invalid entry");
			}else{
			
				//converts string to long int using strtol
				long int value = strtol(result,NULL,10);
				if(value == 1 || value == 2 || value == 3){
				
					//if yes sets break flag and sets global values for current audit
					if(value == 1){
						//exits function
						return 1;
					
					}else if(value == 3){
					
						//exits function
						return 2;
					}				
			
				}else{
					printf("Invalid Entry");
				}
			}
		}
	
	}

	return 0;
}

int remove_class(sem_t *sem){


	///perform shared memory operations
	
	int shmd;
	// open the shared memory area creating it if it doesn't exist
	shmd = shm_open("/ccshm", O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP); 
	
	if(!shmd){
		perror("failed to open shared memory\n");
		return -1;
	}
	
	if(ftruncate(shmd, sizeof(struct SHM_CLIST))){
		perror("failed to ftruncate\n");
		return -1;
	}
	
	struct SHM_CLIST *sh_mem;
				
	sh_mem = mmap(NULL, sizeof(struct SHM_CLIST), PROT_READ | PROT_WRITE, MAP_SHARED, shmd, 0);
	
	if(!sh_mem){
		perror("failed to set mmap\n");
		return -1;
	}
	
	if(close(shmd)){
		perror("failed to close shared memory\n");
		return -1;
	}	

	
	
	sem_wait(sem);
	///////////////
	//put access to shared memory location 
	///////////////
	sh_mem->job_code = 6;
	sh_mem->job_finished = 0;
	sh_mem->job_status = 1;
	sem_post(sem);
	

	char *classl;
	//waits for server to complete the class list and set job_finished flag = 1
	while(1){
	
		printf("waiting for list to complete\n");
		
		sem_wait(sem);
		//checks for job completion from server	
		int val = sh_mem->job_finished;
		sem_post(sem);
		
		if(val == 1){
			sem_wait(sem);
			classl = sh_mem->class_list;
			sem_post(sem);
			break;
		}else{
		
			//handle errors
		}

	}
	
	printf("[CLASS NAME] --	[ID]");
	printf("%s\n", classl);
	
	char result[10];
	fgets(result, sizeof(result), stdin);

	int k = 0;
	while(k!=1){
	
		printf("Select the Class ID to Remove \n Enter <id> or 0 (exit)\n");
		
		char *result1 = fgets(result, sizeof(result), stdin);
		printf("result = %s",result1);
		
		
		
		//converts string to long int using strtol
		long int value = strtol(result,NULL,10);
		printf("value = %ld\n",value);
		
		if(value<0 || value>150){
		
			printf("Invalid Id");
		
		}else{
		
				
			//checks for null
			if(value == 0){
			
				return 2;
			
			}else{
			printf("Waiting for class removal confirmation of :: %d\n", value);
				
			
			sem_wait(sem);
		
			sh_mem->job_code = 4;
			sh_mem->job_finished = 0;
			sh_mem->job_status = 1;
			sh_mem->class_id = value;
		
			sem_post(sem);
			
			while(1){
				
				printf("--\n");
			
				sem_wait(sem);
				//checks server for job completion
				int val = sh_mem->job_finished;
				sem_post(sem);
			
				if(val == 1){
					printf("Class has been Removed\n");
					break;
				}else if(val == 0){
					printf("Invalid Class Id\n");
					break;
				}
				
			}
			
			return 1;
			}
		}
	}
	
	return 0;
	
}
