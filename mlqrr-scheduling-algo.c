#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

typedef struct process
{
	int arr_time;
	int bur_time;
	int rem_burst;
	int wt;
	int tt;
	bool included;
}process;

//System time, number of processes, index for maintaining completed processes
int systime,n,comp_index;

//List of processes	
process* processes;

//Completed processes, waiting & turnaround time for RR Scheduling
int *completed, *rr_wait, *rr_tt;


//Sort according to arrival time
void SortByArrTime()
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n-1;j++)
		{
			if(processes[j].arr_time>processes[j+1].arr_time)
			{
				process temp=processes[j];
				processes[j]=processes[j+1];
				processes[j+1]=temp;
			}
		}
	}
}

//Sort by Burst Time
void SortByBurTime(int* q, int qc)
{
	int i,j;
	for(i=0;i<qc;i++)
	{
		for(j=0;j<qc-1;j++)
		{	
			if(processes[q[j+1]].arr_time<systime && processes[q[j]].arr_time<systime)
			{	
				if(processes[q[j+1]].bur_time<processes[q[j]].bur_time)
				{
					int temp=q[j];
					q[j]=q[j+1];
					q[j+1]=temp;
				}
			}
		}
	}
}


void printq(int* q1,int *q2,int q1c,int q2c)
{
	printf("Q1:\n");
	int i;
	for(i=0;i<q1c;i++)
		printf("%d\t",q1[i]);
	printf("\nQ2:\n");
	for(i=0;i<q2c;i++)
		printf("%d\t",q2[i]);
	printf("\n");
}

bool enqueue(int* q1, int* q1c, int* q2, int* q2c, bool all)
{
	int avg=0,i=0,count=0;
	bool comp_flag=false;
	if(!all)
	{
		if(processes[0].rem_burst>10)
		{
			processes[0].rem_burst-=10;
			systime+=10;
			// printf("0 rem %d\n",processes[0].rem_burst );
		}
		else
		{
			systime+=processes[0].rem_burst;
			processes[0].rem_burst=0;
			processes[0].tt=systime;
			comp_flag=true;
		}
	}
	//Include incoming processes for queueing
	if(!all)
	{
		int inc=0;
		processes[0].included=true;
		for(i=1;i<n;i++)
		{
			inc=processes[i].arr_time;
			if(inc<=processes[0].bur_time)
				processes[i].included=true;
			else
				processes[i].included=false;
		}
	}
	else
	{
		for(i=0;i<n;i++)
			processes[i].included=true;
	}
	if(comp_flag)
		i=1;
	else
		i=0;
	for(;i<n;i++)
	{
		if(processes[i].included)
		{
			avg+=processes[i].rem_burst;
			count++;
		}
	}
	avg=avg/count;
	if(comp_flag)
		i=1;
	else
		i=0;
	
	for(;i<n;i++)
	{
		if(processes[i].included)
		{
			if(processes[i].rem_burst<=avg)
				q1[(*q1c)++]=i;
			else
				q2[(*q2c)++]=i;
		}
	}

	return comp_flag;
}



void print()
{
	printf("Process\tArrival Time\tBurst Time\n");
	int i;
	for(i=0;i<n;i++)
		printf("P%d\t\t%d\t\t%d\n",i,processes[i].arr_time,processes[i].bur_time);
}

void WaitingTime(){
	for(int i=0;i < n;i++){
		processes[i].wt = processes[i].tt - processes[i].bur_time;
	}
}

void Schedule(int* q1, int q1c, int* q2, int q2c)
{
	int i,c1=0,c2=0;

	bool allIn=true;
	bool q1_large=q1c>q2c;
	
	//Assign CPU alternatively to Q1 and Q2
	//Q1 process - full burst time given for qc number of processes
	//Q2 process - fbt burst time once for qc number of processes
	int fbt = processes[q1[0]].rem_burst!=0 ? processes[q1[0]].rem_burst : processes[q1[1]].rem_burst;
	int qc=q1c > q2c ? q2c : q1c;
	for(i=0;i<qc;i++)
	{
		if(processes[q1[i]].rem_burst!=0)
		{
			systime=systime+processes[q1[i]].rem_burst;
			processes[q1[i]].rem_burst=0;
			processes[q1[i]].tt=systime-processes[q1[i]].arr_time;
			completed[comp_index++]=q1[i];
		}
		if(processes[q2[i]].rem_burst!=0)
		{
			if(processes[q2[i]].rem_burst>fbt)
			{
				systime+=fbt;
				processes[q2[i]].rem_burst-=fbt;
			}
			else
			{
				systime+=processes[q2[i]].rem_burst;
				processes[q2[i]].rem_burst=0;
				processes[q2[i]].tt = systime - processes[q2[i]].arr_time;
				completed[comp_index++]=q2[i];
			}
		}
	}

	//After assignment if Q1 is large, assign complete burst time requirement for all Q1 processes
	//Else do the same for Q2
	if(q1_large)
	{
		for(i=qc;i<q1c;i++)
		{
			systime+=processes[q1[i]].rem_burst;
			processes[q1[i]].rem_burst=0;
			processes[q1[i]].tt=systime-processes[q1[i]].arr_time;
			completed[comp_index++]=q1[i];
		}
	}
	else
	{
		for(i=qc;i<q2c;i++)
		{
			if(processes[q2[i]].rem_burst!=0)
			{
				systime+=fbt;
				processes[q2[i]].rem_burst-=fbt;
			}
		}
	}

	//Schedule the other Queue
	int avg=0,count=0;
	for(i=0;i<q2c;i++)
    {
        if(processes[q2[i]].rem_burst>0)
        {
            avg+=processes[q2[i]].rem_burst;
            count++;
        }
    }
    if(count>0)
    	avg=avg/count;
    else
    {
    	printf("Second Queue empty. Algorithm cannot be implemented.\n");
    	exit(0);
    }
    int j;
    for(j=0;j<q2c;j++)
    {
	   	for(i=0;i<q2c;i++)
       	{
           	if(processes[q2[i]].rem_burst>0)
           	{
               	if(processes[q2[i]].rem_burst>avg)
               	{
                   	systime+=avg;
                   	processes[q2[i]].rem_burst-=avg;
               	}
               	else
               	{
               		systime+=processes[q2[i]].rem_burst;
    	           	processes[q2[i]].rem_burst=0;
    	           	processes[q2[i]].tt= systime - processes[q2[i]].arr_time;
                   	completed[comp_index++]=q2[i];
               	}
           	}
        }
    }
    WaitingTime();
    printf("\nCompletion:\n");
    printf("Processes\tTT\tWT\n");
    for(i=0;i<n;i++)
    {
    	if(processes[i].included)
    		printf("P%d\t\t%d\t%d\n",completed[i],processes[completed[i]].tt,processes[completed[i]].wt);
    }
    printf("\n");		
}	

void FindTimes()
{
	int i=0,avgtt=0,avgwt=0,count=0;
	for(;i<n;i++)
	{
		if(processes[i].included)
		{
			avgtt+=processes[i].tt;
			avgwt+=processes[i].wt;
			count++;
		}
	}
	avgwt=avgwt/count;
	avgtt=avgtt/count;
	printf("Average Waiting Time: %d\nAverage Turnaround Time: %d\n",avgwt,avgtt );
}

void RRWaitingTime()
{
	int* rem_burst,count=0;
	rem_burst=(int*) malloc(n*sizeof(int));
	rr_wait=(int*) malloc(n*sizeof(int));
	int i=0;
	for(;i<n;i++)
	{
		if(processes[i].included)
		{
			rem_burst[i]=processes[i].bur_time;
			count++;
		}
	}
	int t=0;
	while(1)
	{
		bool done=true,quantum=10;
		for(i=0;i<n;i++)
		{
			if(processes[i].included)
			{
				if(rem_burst[i]>0)
				{
					done=false;
					if(rem_burst[i]>quantum)
					{
						t+=quantum;
						rem_burst[i]-=quantum;
					}
					else
					{
						t+=rem_burst[i];
						rr_wait[i]=t - processes[i].bur_time;
						rem_burst[i]=0;
					}
				}
			}
		}
		if(done)
			break;
	}
	int sum=0;
	for(i=0;i<count;i++)
		sum+=rr_wait[i];
	sum=sum/count;
	printf("\n\nRound Robin Scheduling:\n");
	printf("Average Waiting Time: %d\n",sum );
}

void RRTurnaroundTime()
{
	rr_tt=(int*) malloc(n*sizeof(int));
	int i=0,count=0;
	for(;i<n;i++)
	{
		if(processes[i].included)
		{
			rr_tt[i]=processes[i].bur_time+rr_wait[i];
			count++;
		}
	}
	int sum=0;
	for(i=0;i<count;i++)
		sum+=rr_tt[i];
	printf("Average Turnaround Time: %d\n",sum/count);
}

void BurSort()
{
	int i,j;
	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			if(processes[j].bur_time>processes[j+1].bur_time)
			{
				process temp=processes[j];
				processes[j]=processes[j+1];
				processes[j+1]=temp;
			}
		}
	}
}

void SJFTime()
{
	int* sjf_wait=(int*) malloc(n*sizeof(n));
	int* sjf_rembt=(int*) malloc(n*sizeof(n));
	int* sjf_tt=(int*) malloc(n*sizeof(n));
	int i,count=0;
	BurSort();
	for(i=0;i<n;i++)
	{
		sjf_rembt[i]=processes[i].bur_time;
		if(processes[i].included)
			count++;
	}
	int t=0;
	int j;
	for(i=0;i<n;i++)
	{
		if(processes[i].included)
		{
			if(sjf_rembt[i]>0)
			{
				if(t>=processes[i].arr_time)
				{
					t+=sjf_rembt[i];
					sjf_tt[i]=t - processes[i].arr_time;
					sjf_rembt[i]=0;
					i=0;
				}
			}
		}
		else
			sjf_tt[i]=0;
	}
	 
	int avgwt=0,avgtt=0;
	for(i=0;i<n;i++)
	{
		if(processes[i].included)
			sjf_wait[i]=sjf_tt[i] - processes[i].bur_time;
		else
			sjf_wait[i]=0;
		avgtt+=sjf_tt[i];
		avgwt+=sjf_wait[i];
	}
	avgtt=avgtt/count;
	avgwt=avgwt/count;
	if(avgtt>0 && avgwt>0)
	{
		printf("\n\nSJF Scheduling:\n");
		printf("Average Waiting Time : %d\nAverage Turnaround Time: %d\n",avgwt,avgtt );
	}
}

void main()
{
	printf("MQRR Scheduling:\nAll processes arrive at random times and have random burst times\n\n");
	
	srand(time(NULL));
	n=rand()%6+5;			//Number of processes
	printf("Number of processes: %d\n",n);
	processes=(process*) malloc(n*sizeof(process));

	//Assign values to all processes
	int i;
	for(i=0;i<n;i++)
	{
		//Randomize arrival and burst times
		processes[i].arr_time=rand()%9+2;
		processes[i].bur_time=rand()%17+4;

		//At the beginning, remaining time=burst time and waiting time=0
		processes[i].rem_burst=processes[i].bur_time;
		processes[i].wt=processes[i].tt=0;		
	}
	SortByArrTime(processes,n);					//Sort using arrival times
	int arrival=processes[0].arr_time;			
	for(i=0;i<n;i++)							//The first arrived process should be assigned arrival time 0
		processes[i].arr_time=processes[i].arr_time-arrival;
	print();

	int first_arrived=1;
	for(i=1;i<n;i++)
	{
		if(processes[i].arr_time==0)
			first_arrived++;
	}

	completed=(int*) malloc(n*sizeof(int));
	//If all processes arrive at the same time
	if(first_arrived==n)
	{	
		int q1_count=0,q2_count=0;
		int* q1=(int*) malloc(n*sizeof(int));
        int* q2=(int*) malloc(n*sizeof(int));
        enqueue(q1,&q1_count,q2,&q2_count,true);

        //Sort the queues according to burst time
		SortByBurTime(q1,q1_count);				
		SortByBurTime(q2,q2_count);
		printq(q1,q2,q1_count,q2_count);
		Schedule(q1,q1_count,q2,q2_count);		//Start of the algorithm => time=0
	}
	else
	{
		int q1_count=0,q2_count=0;
        int* q1=(int*) malloc(n*sizeof(int));
        int* q2=(int*) malloc(n*sizeof(int));
        comp_index = enqueue(q1,&q1_count,q2,&q2_count,false);

		//Sort the queues according to burst times
        SortByBurTime(q1,q1_count);
		SortByBurTime(q2,q2_count);
		printq(q1,q2,q1_count,q2_count);

		if(comp_index)
			completed[0]=0;
		Schedule(q1,q1_count,q2,q2_count);		//Start of the algorithm => time=0
		FindTimes();
	}
	RRWaitingTime();
	RRTurnaroundTime();
	SJFTime();
}

