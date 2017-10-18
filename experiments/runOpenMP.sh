#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N disseminationAndMCS
for threads in 2 4 6 8
do 
./disseminationBarrier $threads 5000  >> disseminationBarrier$threads.log
./mcsBarrier >> mcsBarrier$threads.log 
done




