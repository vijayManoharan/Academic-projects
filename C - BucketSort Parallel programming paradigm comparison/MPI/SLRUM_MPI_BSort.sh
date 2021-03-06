#!/bin/bash
#SBATCH --partition=general-compute
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=2
#SBATCH --job-name=BSort_MPI
#SBATCH --time=00:10:00
#SBATCH --mail-user=manohara@buffalo.edu
#SBATCH --output=Result_BSort_MPI_10p6_8node_2task.out
#SBATCH --error=Result_BSort_MPI_ca.out

echo "SLURN Enviroment Variables:"
echo "Job ID = "$SLURM_JOB_ID
echo "Job Name = "$SLURM_JOB_NAME
echo "Job Node List = "$SLURM_JOB_NODELIST
echo "Number of Nodes = "$SLURM_NNODES
echo "Tasks per Nodes = "$SLURM_NTASKS_PER_NODE
echo "CPUs per task = "$SLURM_CPUS_PER_TASK
echo "/scratch/jobid = "$SLURMTMPDIR
echo "submit Host = "$SLURM_SUBMIT_HOST
echo "Subimt Directory = "$SLURM_SUBMIT_DIR
echo 

module load intel
module load intel-mpi
module list
ulimit -s unlimited

#
export I_MPI_PMI_LIBRARY=/usr/lib64/libpmi.so #used for cooperating MPI with slurm.
srun ./MPI -t 1000000 16
#

echo "All Dones!"
