
#ifndef MPI_OpenMP_FUNCTIONS_H_
#define MPI_OpenMP_FUNCTIONS_H_

#include <stdlib.h>
#include "stdio.h"
#include <math.h>
#include <string.h>
#include <time.h>

#define LENGHT_OF_SEQ 3000
#define FILE_NAME_INPUT "input.txt"
#define FILE_NAME_OUTPUT "output.txt"


typedef struct {
	double maxWeight;
	int offset;
	int mutantSignPosition;
	int* id_Arr;
	double weights[4];
	char seq1[3000];
	int numberOfSeqs;
	char** seqs;
}Seqs_Details;

typedef struct {
	double* maxWeight;
	int* offset;
	int* mutantSignPosition;
	int* id_Arr;

}Seqs_Results;



void read_Seqs_Details_From_File(Seqs_Details *seqs_d);
void write_Seqs_Results_To_File(Seqs_Results results,int numOfSeqs);

void init_Results(Seqs_Results* results,int numberOfSeqForSlaves);
void saveResults(Seqs_Results* results,Seqs_Details* seqs_d,int i);

void compareSeq1Seq2(Seqs_Details* seqs_d,char* seq1, char* seq2,int indexOfSeq2);
void compare_Seqs_With_Offset(Seqs_Details* seqs_d,char* seq1,char* seq2,int delta,int indexOfMutantSign);
char* createMutant(char* mutant,int index);

int isColons(char c1,char c2);
int isPoint(char c1,char c2);
void sumWeights(Seqs_Details* seqs_d,char* seq3, int size,int newOffset,int indexOfMutantSign);

void free_All(Seqs_Details* seqs_d);

#endif /* MPI_OpenMP_FUNCTIONS_H_ */
