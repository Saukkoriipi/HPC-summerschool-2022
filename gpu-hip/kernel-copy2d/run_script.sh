#!/bin/bash
#SBATCH --job-name=d_query
#SBATCH --account=project_2000745
#SBATCH --reservation=summerschool-gpu
#SBATCH --partition=gpu
#SBATCH --time=00:05:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
#SBATCH --mem-per-cpu=4000
#SBATCH --gres=gpu:v100:1
#SBATCH --output=output.txt

srun ./copy2d

## NOTES TO COMPILE AND LOAD GPU CODE:
# module load nvhpc/21.9  nvhpc-mpi/openmpi-4.0.5
# nvc -o hello hello.c -mp=gpu -gpu=cc70
