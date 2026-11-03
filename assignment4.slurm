#!/bin/bash
#SBATCH --job-name=assignment4
#SBATCH --output=assignment4_%j.out
#SBATCH --error=assignment4_%j.err
#SBATCH --partition=Centaurus
#SBATCH --time=00:30:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --mem=8G


echo "=== Job info ==="
date
echo "User: $USER"
echo "Running on: $(hostname)"
echo "SLURM_JOB_ID: $SLURM_JOB_ID"
echo "Workdir: $(pwd)"
echo
echo "=== Modules ==="
module load gcc
echo
echo "=== Run ==="
make sweep
echo
echo "=== Done ==="
date
