INTRODUCTION:

Barriers are used in multithreaded programs where the application requires that all threads
reach a certain point of execution in the program before anything else executes. These are thus, phases
in the program execution.

*****************************************Open MP Barriers*****************************************

	Open MP barriers are generally used in multithreaded environment to bring the threads to a certain
standpoint. We have implemented three barriers in OpenMP, all from the MCS paper:

--Dissemination Barrier
--MCS Tree Barrier

**************************************Open MPI Barriers******************************************

	Open MPI barriers are used to synchronize different processors on a SMP system. We have
implemented two barriers in Open MPI again from the MCS paper:

-- Tournament Barrier
-- Dissemination Barrier

**************************************Open MP-MPI Combined Barriers******************************

	We combined the MCS barrier of the OpenMP and Dissemination of OpenMPI to
get a combined barrier. We synchronize every thread in every process and then synchronize the
processes. We did this so since each processor will have some amount of work to do.



******************************Compiling and running the code:****************************************

	We include three makefiles, one for OpenMP barriers and one for MPI barriers and other for MPI-MP
barrier. Running make creates the object files for the barrier implementations that can be run as
./barriername from the shell prompt. We have configured the Open MP barrier implementations to
accept the number of threads to scale to and also the number of barriers. We are doing some computation inside barrier(  calculate Nth prime number). we pass n from the commandline as well. Low value of N is a light weight process while higer values of N lead to a heavy processes. They can be run from the shell
prompt as follows: 

#./barriername numthreads numbarriers nthPrime
(./barriername : ( disseminationBarrier , mcsBarrier)).

MPI barriers can be run as follows:
#mpirun –np (number_of_processors) –hostfile (hosts_to_run_on) barriername num_barriers nthPrime
(barriername : (dissemination , tournament)).

The OpenMPI-MP barrier can be run as follows
./mixedBarrier num_threads num_bariers nthPrime varying the value of nth prime you can run light or heavy process and record the data.

