#!/bin/bash

#SBATCH --partition=general-compute
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --job-name=Bsort_SEQ
#SBATCH --time=02:40:00
#SBATCH --mail-user=manohara@buffalo.edu
#SBATCH --output=Result_BSort_sequential_10p6_8bucket.out
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
./sequential 1000000 8

#
echo "All Dones!"
