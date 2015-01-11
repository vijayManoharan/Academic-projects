#!/bin/bash
#SBATCH --partition=gpu
#SBATCH --gres=gpu:1
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --job-name=BucketSort_GPU
#SBATCH --time=00:40:00
#SBATCH --mail-user=manohara@buffalo.edu
#SBATCH --output=Result_BucketSort_GPU_10p5.out
#SBATCH --error=Result_BucketSort_GPU.out

echo "SLURM Environment Variables:"
echo "Job ID = "$SLURM_JOB_ID
echo "Job Name = "$SLURM_JOB_NAME
echo "Job Node List = "$SLURM_JOB_NODELIST
echo "Number of Nodes = "$SLURM_NNODES
echo "Tasks per node = "$SLURM_NTASKS_PER_NODE
echo "CPUs per task = "$SLURM_CPUS_PER_TASK
echo "/scratch/jobid = "$SLURMTMPDIR
echo "Submit Host = "$SLURM_SUBMIT_HOST
echo "Submit Directory = "$SLURM_SUBMIT_DIR
echo 
echo

ulimit -s unlimited
#
./BucketSort_GPU -t 100000

#
echo "All Done!"
