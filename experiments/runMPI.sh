#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N tournamentAndDissemination
OMPI_MCA_mpi_yield_when_idle=0
for processor in 2 4 6 8 10 12 
do 
mpirun --hostfile $PBS_NODEFILE -np $processor ./tournament >> tournament$processor.log
##/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np $processor ./dissemination >> dissemination$processor.log
done


