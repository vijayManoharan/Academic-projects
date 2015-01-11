#!/bin/bash
rm -r Result* config output* Mapinput* node*
sbatch SLURM_myHadoop.sh
squeue -u manohara -i 5