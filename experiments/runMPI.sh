#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N tournamentAndDissemination
OMPI_MCA_mpi_yield_when_idle=0
for processor in 2 4 6 8 10 12 
do 
mpirun --hostfile $PBS_NODEFILE -np $processor ./repos/BarrierSynchronization/mpi/tournament 100000 100000 >> tournamentHeavy$processor.log
mpirun --hostfile $PBS_NODEFILE -np $processor ./repos/BarrierSynchronization/mpi/tournament 100000 500 >> tournamentLight$processor.log

mpirun --hostfile $PBS_NODEFILE -np $processor ./repos/BarrierSynchronization/mpi/dissemination 100000 100000 >> disseminationHeavy$processor.log
mpirun --hostfile $PBS_NODEFILE -np $processor ./repos/BarrierSynchronization/mpi/dissemination 100000 500 >> disseminationLight$processor.log
done


