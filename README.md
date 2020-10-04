# Sequence-Alignment---MPI-openMP
Parallel implementation of Sequence Alignment with MPI and openMP


Sequence Alignment – a way to estimate a similarity of two strings of letters - is an important field in bioinformatics1. Sequence is a string of capital letters including hyphen sign (-), for example:

    "PSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHLPSQHL"
    
Each letter in the sequence represents DNA, RNA, or protein. Identification of region of similarity of set of Sequences is extremely time consuming. This project deals with a simplified version of Sequence Alignment of two sequences. The purpose of the project is to parallelize the basic algorithm to produce an efficient computations within MPI, OpenMP environment.
