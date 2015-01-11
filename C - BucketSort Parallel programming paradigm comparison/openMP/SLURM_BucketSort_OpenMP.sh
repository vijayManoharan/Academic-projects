#!/bin/bash

#SBATCH --partition=general-compute
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --job-name=Bsort_OpenMP
#SBATCH --time=00:10:00
#SBATCH --mail-user=manohara@buffalo.edu
#SBATCH --output=Result_BSort_sequential_10p6_8tasks.out
#SBATCH --error=Result_BSort_sequential_err.out

echo "SLURN Enviroment Variables:"
echo "Job ID = "$SLURM_JOB_ID
echo "Job Name = "$SLURM_JOB_NAME
echo "Job Node List = "$SLURM_JOB_NODELIST
echo "Number of Nodes = "$SLURM_NNODES
echo "Tasks per Nodes = "$SLURM_NTASKS_PER_NODE
echo "/scratch/jobid = "$SLURMTMPDIR
echo "submit Host = "$SLURM_SUBMIT_HOST
echo "Subimt Directory = "$SLURM_SUBMIT_DIR
echo
module load intel
module load intel-mpi
module list
ulimit -s unlimited

#
./OpenMP -t 1000000

#
echo "All Dones!"
