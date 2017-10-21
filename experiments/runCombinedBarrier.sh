#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:20:00
#PBS -N tournamentAndDissemination
OMPI_MCA_mpi_yield_when_idle=0
for processor in 2 4 8 10 12
do
  for threads in 2 4 6 8
  do
     mpirun --hostfile $PBS_NODEFILE -np $processor ./mixedBarrier $threads 1000 1000  >> HeavyProcessThread$threads-Proc-$processor.log
     mpirun --hostfile $PBS_NODEFILE -np $processor ./mixedBarrier $threads 1000 100  >>  LightProcessThread$threads-Proc-$processor.log
  done
done
