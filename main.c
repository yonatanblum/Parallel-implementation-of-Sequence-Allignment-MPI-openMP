#include "MPI_OpenMP_functions.h"
#include <mpi.h>


#define NUMBER_OF_PROCESSES 2
#define NUMBER_OF_WEIGHTS 4
#define SLAVE 1
#define MASTER 0



int main(int argc, char *argv[])
{	  
	int size, rank, i;
	Seqs_Details seqs_d;
	Seqs_Results results;
	double startTime, stopTime;
    int numberOfSeqForSlaves;
	int slaveIndex;
	MPI_Status  status;

	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != NUMBER_OF_PROCESSES) {
       	printf("This program is capable of running with only two processes\n");
       	MPI_Abort(MPI_COMM_WORLD, __LINE__);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	// ###############################Init both processes master and slave#################################
	if (rank == MASTER) 
	{
		//Deletes the file: ouput.txt -->Before each start of the program
		if (remove(FILE_NAME_OUTPUT) == 0)
			printf("Deleted successfully");
		else
			printf("Unable to delete the file");

		//Read the input file: input.txt
		startTime = MPI_Wtime(); // initial time count
		read_Seqs_Details_From_File(&seqs_d);	
		numberOfSeqForSlaves = seqs_d.numberOfSeqs/NUMBER_OF_PROCESSES;
		slaveIndex= seqs_d.numberOfSeqs - numberOfSeqForSlaves;
		
		
		init_Results(&results,seqs_d.numberOfSeqs);
		MPI_Send(seqs_d.weights, NUMBER_OF_WEIGHTS, MPI_DOUBLE, SLAVE, 0, MPI_COMM_WORLD);
		MPI_Send(seqs_d.seq1, LENGHT_OF_SEQ, MPI_CHAR, SLAVE, 0, MPI_COMM_WORLD);
		MPI_Send(&numberOfSeqForSlaves, 1, MPI_INT, SLAVE, 0, MPI_COMM_WORLD);

		for(int i=numberOfSeqForSlaves;i<seqs_d.numberOfSeqs;i++)
		{
			MPI_Send(&(seqs_d.id_Arr[i]), 1, MPI_INT, SLAVE, 0, MPI_COMM_WORLD);
			MPI_Send(seqs_d.seqs[i], LENGHT_OF_SEQ, MPI_CHAR, SLAVE, 0, MPI_COMM_WORLD);
		}
		
		

	}else
	{
		//##################Receiving details from the master about the seqs
		MPI_Status  status;
		MPI_Recv(seqs_d.weights, NUMBER_OF_WEIGHTS, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(seqs_d.seq1, LENGHT_OF_SEQ, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&(seqs_d.numberOfSeqs), 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
		init_Results(&results,seqs_d.numberOfSeqs);
		seqs_d.id_Arr = (int*)calloc(seqs_d.numberOfSeqs,sizeof(int));
		seqs_d.seqs = (char**)malloc(seqs_d.numberOfSeqs * sizeof(char*));
		for(int i=0;i<seqs_d.numberOfSeqs;i++)
		{
			MPI_Recv(&(seqs_d.id_Arr[i]), 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
			seqs_d.seqs[i] = (char*)malloc(LENGHT_OF_SEQ * sizeof(char));
			MPI_Recv(seqs_d.seqs[i], LENGHT_OF_SEQ, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, &status);
		}
	}	
	
	// ###############################Tasks apply to both processes master and slave - Every process start his mission#################################
	for(int i=0;i<seqs_d.numberOfSeqs;i++)
	{
		//Ensure that the master does not slip into the slave's tasks
		if(i == slaveIndex && rank == 0) break;
		
		
		compareSeq1Seq2(&seqs_d,seqs_d.seq1,seqs_d.seqs[i],seqs_d.id_Arr[i]);
		
		saveResults(&results,&seqs_d,i);
		
	}

	//################################After completing the calculations send the results from the slave to the master###################################
	if(rank != MASTER)
	{
	
		MPI_Send(results.maxWeight, seqs_d.numberOfSeqs, MPI_DOUBLE, MASTER, 0, MPI_COMM_WORLD);
		MPI_Send(results.offset, seqs_d.numberOfSeqs, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		MPI_Send(results.mutantSignPosition, seqs_d.numberOfSeqs, MPI_INT, MASTER, 0, MPI_COMM_WORLD);	
		MPI_Send(results.id_Arr, seqs_d.numberOfSeqs, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
		
		
	}

	if(rank == MASTER)
	{
		//################################After completing the calculations receive the results from the slave to the master###################################
		MPI_Recv(&(results.maxWeight[slaveIndex]), numberOfSeqForSlaves, MPI_DOUBLE, SLAVE, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&(results.offset[slaveIndex]), numberOfSeqForSlaves, MPI_INT, SLAVE, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&(results.mutantSignPosition[slaveIndex]), numberOfSeqForSlaves, MPI_INT, SLAVE, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&(results.id_Arr[slaveIndex]), numberOfSeqForSlaves, MPI_INT, SLAVE, 0, MPI_COMM_WORLD, &status);
		
		// Write the final result to the file: "output.txt" 
		write_Seqs_Results_To_File(results,seqs_d.numberOfSeqs);
		
		//free allocations
		free_All(&seqs_d);
		stopTime = MPI_Wtime();
		double time_spent = (double)(stopTime - startTime);
		printf("\n\nTotal time spent = %lf\n",time_spent);
		
	}	
	MPI_Finalize();

return 0;

}

//Data structure Seqs_Results initialization
void init_Results(Seqs_Results* results,int numberOfSeqForSlaves)
{
	results->maxWeight = (double*)malloc(numberOfSeqForSlaves* sizeof(double));
	results->offset = (int*)malloc(numberOfSeqForSlaves* sizeof(int));
	results->mutantSignPosition = (int*)malloc(numberOfSeqForSlaves* sizeof(int));
	results->id_Arr = (int*)malloc(numberOfSeqForSlaves* sizeof(int));
}

//Transferring information from structure Seqs_Details to structure Seqs_Results.
void saveResults(Seqs_Results* results,Seqs_Details* seqs_d,int i){
	results->maxWeight[i] = seqs_d->maxWeight;
	results->offset[i] = seqs_d->offset;
	results->mutantSignPosition[i] = seqs_d->mutantSignPosition;
	results->id_Arr[i] = seqs_d->id_Arr[i];	
}


// Write the final result to the file: "output.txt" 
void write_Seqs_Results_To_File(Seqs_Results results,int numOfSeqs)
{
	FILE *f = fopen(FILE_NAME_OUTPUT, "a");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	for(int i = 0;i<numOfSeqs;i++)
	{
	/* Print integers and floats */
	fprintf(f, "Number Of Seq2= %d, Max Weight = %lf , Offset_N = %d , MutantSignPosition_K= %d\n",results.id_Arr[i],results.maxWeight[i],results.offset[i],results.mutantSignPosition[i]);


	}

	fclose(f);
}

//free allocations for Seqs_Details
void free_All(Seqs_Details* seqs_d)
{
	for(int i=0; i<seqs_d->numberOfSeqs; i++)
	{
		free(seqs_d->seqs[i]);
	}
	free(seqs_d->seqs);
	free(seqs_d->id_Arr);
}
