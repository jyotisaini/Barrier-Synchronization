#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N disseminationAndMCS
for threads in 2 4 6 8
do 
./disseminationBarrier $threads 100000 100000 >> disseminationBarrierHeavy$threads.log
./mcsBarrier $threads 100000 100000 >> mcsBarrierHeavy$threads.log 

./disseminationBarrier $threads 100000 500 >> disseminationBarrierLight$threads.log
./mcsBarrier $threads 100000 500 >> mcsBarrierLight$threads.log 
done




