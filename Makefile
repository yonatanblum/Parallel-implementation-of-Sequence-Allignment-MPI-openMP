build:
	mpicxx -fopenmp -c main.c -o main.o
	mpicxx -fopenmp -c MPI_OpenMP_functions.c -o MPI_OpenMP_functions.o
	mpicxx -fopenmp -o mpiCudaOpenMP  main.o MPI_OpenMP_functions.o 
	
clean:
	rm -f *.o ./mpiCudaOpenMP

run:
	mpiexec -np 2 ./mpiCudaOpenMP

runOn2:
	mpiexec -np 2 -machinefile  mf  -map-by  node  ./mpiCudaOpenMP
