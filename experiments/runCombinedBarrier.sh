#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N tournamentAndDissemination
OMPI_MCA_mpi_yield_when_idle=0
for processor in 2 4 6 8 10 12
do
  for threads in 2 4 6 8
  do
     mpirun --hostfile $PBS_NODEFILE -np $processor ./combinedBarrier $threads 5000 >> Thread-$threads-Proc-$processor.log
  done
done
