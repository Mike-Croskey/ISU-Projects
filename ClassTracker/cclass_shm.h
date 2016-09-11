/**
 * @file      cclass_shm.h
 * @author    Mike Croskey
 * @date      2015-04-17: Last updated
 * @brief     Shared memory space for IPC communnication between cclass server and client
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




#define SEM "/CLASSSEM"

//struct for storing individual class data
typedef struct class_d{

		int id;
	
		char grade[3];
	
		char semester_completed[30];

		char student_name[50];
			
		char class_name[200];
	
		char instr_name[50];
	
		long int credits;
	
		char class_type[50];
	
		char class_description[3000];

} cclass_t;

struct SHM_CCLASS{


	
	//indicates if a new job has been added to shared memory space
	unsigned int job_status;
	//indicates when job is finished
	unsigned int job_finished;
	//indicates what type of job is being requested 
	unsigned int job_code;
	//the students name for current audit
	char student_name[50];
	//the current type of degree 
	char degree[200];
	//number of credits required for current degree
	unsigned int credits_req;
	//pointer to finalized audit file	
	char file_name[1500];
	//pointer to class_d struct used for class data
	
	
	int id;
	
	char grade[3];
	
	char semester_completed[30];
			
	char class_name[200];
	
	char instr_name[50];
	
	long int credits;
	
	char class_type[50];
	
	char class_description[3000];
	
};

struct SHM_CLIST{
	
	//indicates if new class list request gevin
	int job_status;
	//indicates if class list has been generated
	int job_finished;
	//indicates what type of job is requested
	int job_code;
	//class id used for deleting from server
	long int class_id;
	//current class in audit
	char class_list[22250];
	

};

