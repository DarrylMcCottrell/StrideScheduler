//Darryl McCottrell


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

					using namespace std;

		/*This function calls the StartJob function below after the the Main function, if it doesn't then you will get a lot
		of errors mainly because the runnable executable is going to be declared out of scope. Depending on which way you do it
		you can avoid doing this */
	void startJob(void);

	/* This struct includes the three needed in the stride scheduler and in inputed into a vector in order to execute the arguments, runnables
	and new job type functions for the program. Either a struct or a class is sort of needed in my case I thought a struct would be easier to 
	handle the name, stride, pass, and priority. */
struct job
{
	string name;
	int stride;
	int pass;
	int priority;

};

		
vector<string> arguments;
vector<job> runnable;
vector<job> blocked;
job newjob;
job running;
job clean;

/*In main the first few implementations basically checks for a text file, if there are way too many arguments in argc then it should produce an
error, if it produces to little than it should produce an error on how many arguments are needed in order for the program to actually work, I then use an
ifstream to clarify if the file exist or not and if it doesn't then it exits out of the program*/
int main(int argc, char* argv[])
{
	//
	if(argc > 2)
	{
		cerr << "too many agruments \n";
		exit(0);
	}
	if(argc < 2)
	{
		cerr << ("argument expected \n");
		exit(0);
	}
	
	ifstream file(argv[1]);
	if (file.good());
	else
	{
		cerr << "file does not exit \n";
		exit(0);
	}


	/* This implementation puts all of the arguments into the vector above. This collects and starts the newJob function required, it specifies that a new
	priority and name has arrived upon the schedule*/

	string input;
	while(getline(file, input)) 
	{
	arguments.push_back(input);
	}
	
	for(int i = 0; i < arguments.size(); i++)
	{
		
		if(arguments[i].substr(0,6) == "newjob")
		{
			
			newjob.name = arguments[i].substr(7,1);
			stringstream stream(arguments[i].substr(9));
			stream >> newjob.priority;
			if(newjob.priority == 0)
				newjob.priority = 1;
			newjob.stride = 100000/ newjob.priority;

			cout << "New job: " << newjob.name << " added with priority: " << newjob.priority << endl;
			if(running.name == "")
			{
				running = newjob;
				newjob = clean;
				cout << "Job: " << running.name << " scheduled." << endl;
			}
			else
			{
				if(runnable.size() == 0)
				{
					newjob.pass = running.pass;
					runnable.push_back(newjob);
				}
				else
				{
					startJob();
					newjob.pass = runnable[0].pass;
					runnable.push_back(newjob);
				}
			}
			newjob = clean;
		}

		/* This implementation is going through the process of newjob until the job is completed, otherwise if it doesn't complete it will clarify that the system
		is idle and go again and run a newjob, if it doesn't finish it will produce an error */
	
		if(arguments[i].substr(0,6) == "finish")
		{
			if(running.name != "")
			{
				cout << "Job: " << running.name << " completed." << endl;
				running = clean;
				if(runnable.size() == 0)
					cout << "System is idle" << endl;
				else
				{
					startJob();
					running = runnable[0];
					runnable.erase(runnable.begin());
					cout << "Job: " << running.name << " scheduled." << endl;
				}
			}
			else
				cout << "Error. System is idle." << endl;
		}

		/* This implementation is of interrupt, it shows that the system is idle and produces an error otherwise it will produce the name of the job and if
		it was scheduled or not. */ 
		if(arguments[i].substr(0,6) == "interrupt")
		{
			if(running.name == "")
				cout << "Error. System is idle." << endl;
			else
			{
				running.pass = running.pass + running.stride;
				runnable.push_back(running);
				startJob();
				running = clean;
				running = runnable[0];
				runnable.erase(runnable.begin());
				cout << "Job: " << running.name << " scheduled." << endl;
			}
		}

		/* This implemtation determines whether a job gets blocked or not through the runnable vector if it does it produces which job got blocked and 
		pushes back the running function. If that doesn't go through it reruns the runnable */
		if(arguments[i].substr(0,6) == "block")
		{
			if(running.name == "")
				cout << "Error. System is idle." << endl;
			else
			{	
				cout << "Job: " << running.name << " blocked." << endl;
				blocked.push_back(running);
				running = clean;
				if(runnable.size() == 0)
					cout << "System is idle." << endl;
				else
				{
					startJob();
					running = runnable[0];
					runnable.erase(runnable.begin());
					cout << "Job: " << running.name << " scheduled." << endl;
				}
			}
		}

			/* After the block implementation, we move to the unblock where the bool checks if its true or false after iterating within the for loop, if its true
		the it will run through as normal and clarify whether the job has been unblocked and where it has been passed to toward, otherwise if its false than it will 
		produce in error and clarify the job was not blocked. */
		if(arguments[i].substr(0,6) == "unblock")
		{
			bool verbose = false;
			for(int n = 0; n < blocked.size(); n++)
			{
				if(blocked[n].name == arguments[i].substr(8))
				{
					if(running.name != "")
					{
						blocked[n].pass = running.pass;
					}
					else
					{
						if(runnable.size() == 0)
							blocked[n].pass = 0;
						else
						{
							startJob();
							blocked[n].pass = runnable[0].pass;
						}
					}

					int passed = blocked[n].pass;
					runnable.push_back(blocked[n]);
					blocked.erase(blocked.begin()+ n);
					verbose = true;
					
					cout << "Job: " << arguments[i].substr(8) << " has unblocked. Pass set to: " << passed << endl;
			
				}
			}
			if(verbose == false)
				
				cout << "Error. Job: " << arguments[i].substr(8) << " not blocked." << endl;
		}

		/* This implementation of arguments is the runnable fuction it runs and produces and calls the StartJob function and prints out the Name, Stride, Pass and the Pri. 
		The set width spaces them apart, just as the programs asks it to be.  */
		//runnable
		if(arguments[i].substr(0,6) == "runnable")
		{
			cout << "Runnable:" << endl;

			if(runnable.size() == 0)
				cout << "None" << endl;
			else
			{
				startJob();
				cout << setw(8) << left << "NAME";
				cout << setw(8) << left << "STRIDE";
				cout << setw(6) << left << "PASS";
				cout << setw(6) << right << "PRI" << endl;
				for(int i = 0; i < runnable.size(); i++)
				{
					cout << setw(8) << left << runnable[i].name;
					cout << setw(8) << left << runnable[i].stride;
					cout << setw(6) << left << runnable[i].pass;
					cout << setw(6) << right << runnable[i].priority;
					cout << endl;
				}
			}
		}

		/*What this does is it prints the current running information name that's originally from the runnable function above meaning 
		the Name, Stride, Pass and Priority*/
		if(arguments[i].substr(0,6) == "runnin")
		{
			cout << "Running:" << endl;

			if(running.name == "")
				cout << "None" << endl;
			else
			{
				cout << setw(8) << left << "NAME" << setw(8) << left << running.name;
				cout << setw(8) << left << "STRIDE" << setw(8) << left << running.stride;
				cout << setw(6) << left << "PASS"  << setw(6) << left << running.pass;
				cout << setw(6) << right << "PRI" << endl << setw(6) << right << running.priority << endl;
			}
		}

		/* This implementation of blocked just prints out the information about the jobs that are currently running and 
		determines whether it is going to get blocked or not */
		if(arguments[i].substr(0,6) == "blocked")
		{
			cout << "Blocked:" << endl;

			if(blocked.size() == 0)
				cout << "None" << endl;
			else
			{
				
				cout << setw(8) << left << "NAME";
				cout << setw(8) << left << "STRIDE";
				cout << setw(6) << left << "PASS";
				cout << setw(6) << right << "PRI" << endl;

				for(int i = 0; i < blocked.size(); i++)
				{
					cout << setw(8) << left << blocked[i].name;
					cout << setw(8) << left << blocked[i].stride;
					cout << setw(6) << left << blocked[i].pass;
					cout << setw(6) << right << blocked[i].priority;
					cout << endl;
				}
			}
		}
	}


	return 0;
}

/* Since the runnable executable is declared above I put the startJob function below the main function. Therefore when it is called in main you shouldn't recieve
any type of errors because the runnable function is already declared. What this function does is it takes the vector of jobs thats stored in the struct and basically
sorts out and swap the functions in order to start the jobs needed to print the information */

void startJob()
{
	if(runnable.size() < 2)
	{
		return;
	}
	else 
	{
		for(int i = 0; i < runnable.size(); i++)
		{
			for(int n = i + 1; n < runnable.size(); n++)
			{
				if(runnable[i].pass > runnable[n].pass)
					swap(runnable[i], runnable[n]);
				else if(runnable[i].pass == runnable[n].pass)
				{
					if(runnable[i].name[0] > runnable[n].name[0])
						swap(runnable[i], runnable[n]);
				}
			}
		}
	}
}
