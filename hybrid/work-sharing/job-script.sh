#!/bin/bash
#SBATCH --job-name=example
#SBATCH --account=project_2000745
#SBATCH --partition=large
#SBATCH --time=00:05:00
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=4
#SBATCH --reservation=summerschool
#SBATCH --output=my-output.txt

# excecute this program
export OMP_NUM_THREADS=4
srun ./vector-addition

# Compile openmp code: mpicxx -fopenmp hello.cpp -o hello
