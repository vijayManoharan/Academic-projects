#!/bin/bash
#SBATCH --partition=general-compute
#SBATCH --nodes=8
#SBATCH --tasks-per-node=1
#SBATCH --exclusive
#SBATCH --time=00:15:00
#SBATCH --job-name=Bucket_sort
#SBATCH --mail-user=manohara@buffalo.edu
#SBATCH --output=Result_Bucketsort_hadoop_10p7_8N.out
#SBATCH --error=Result_Bucketsort_hadoop_10p7_8N.out


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

#. $MODULESHOME/init/sh

#myhadoop is tool to help config and run hadoop.
module load myhadoop/0.2a/hadoop-0.20.1
module list
echo "MY_HADOOP_HOME="$MY_HADOOP_HOME
echo "HADOOP_HOME="$HADOOP_HOME

#### Set this to the directory where Hadoop configs should be generated
# Don't change the name of this variable (HADOOP_CONF_DIR) as it is
# required by Hadoop - all config files will be picked up from here
#
# Make sure that this is accessible to all nodes
export HADOOP_CONF_DIR=$SLURM_SUBMIT_DIR/config
echo "MyHadoop config directory="$HADOOP_CONF_DIR
### Set up the configuration
# Make sure number of nodes is the same as what you have requested from PBS
# usage: $MY_HADOOP_HOME/bin/pbs-configure.sh -h
echo "Set up the configurations for myHadoop"
# this is the non-persistent mode
export PBS_NODEFILE=nodelist.$$
srun --nodes=${SLURM_NNODES} bash -c 'hostname' | sort > $PBS_NODEFILE
NNuniq=`cat $PBS_NODEFILE | uniq | wc -l`
echo "Number of nodes in nodelist="$NNuniq
$MY_HADOOP_HOME/bin/pbs-configure.sh -n $NNuniq -c $HADOOP_CONF_DIR

sleep 5
# this is the persistent mode
# $MY_HADOOP_HOME/bin/pbs-configure.sh -n 4 -c $HADOOP_CONF_DIR -p -d /oasis/cloudstor-group/HDFS

#### Format HDFS, if this is the first time or not a persistent instance
echo
echo "Format HDFS"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR namenode -format

echo
echo "start dfs"
$HADOOP_HOME/bin/start-dfs.sh

echo
echo "start jobtracker (mapred)"
$HADOOP_HOME/bin/start-mapred.sh



$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile0.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile1.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile2.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile3.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile4.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile5.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile6.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile7.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile8.txt
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -rm -r Mapinput/InputFile9.txt
echo "make the dir"

echo "copy file to dfs"

sleep 40
echo "ls files in dfs"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR dfs -ls 

echo
echo "run computation"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR jar BucketSort.jar Bucksort.Sorter Mapinput output_Sort 10000000

echo "ls files in dfs"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR fs -ls output_Sort

echo "Copy output from HDFS to local directory"
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR fs -get output_Sort output_Sort
$HADOOP_HOME/bin/hadoop --config $HADOOP_CONF_DIR fs -get Mapinput Mapinput

echo "stop jobtracker (mapred)"
$HADOOP_HOME/bin/stop-mapred.sh

echo "stop dfs"
$HADOOP_HOME/bin/stop-dfs.sh


#### Clean up the working directories after job completion
echo "Clean up"
$MY_HADOOP_HOME/bin/pbs-cleanup.sh -n $NNuniq
echo