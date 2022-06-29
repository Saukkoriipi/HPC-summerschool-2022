#!/bin/bash
#BATCH --job-name=example
#SBATCH --account=project_2000745
#SBATCH --partition=large
#SBATCH --time=00:05:00
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=1
#SBATCH --reservation=summerschool
#SBATCH --output=our-nice-output.txt

# excecute this program
srun ./cartesian