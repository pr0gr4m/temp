# Makefile - General Makefile for build
#

# include gerneral Makefile configuration file
include Makefile.inc

# execute file list
EXE = pipe msgsnd msgrcv msgrm fifo shm_ex \
	  echo_serv echo_clnt select_serv select_clnt \
	  pthread_cleanup pthread_mutex \
	  shm_read shm_write

all : ${EXE}

clean : 
	${RM} *.o ${EXE}

showall :
	@ echo ${EXE}

${EXE} : ${IMPL_THREAD_FLAGS}

shm_read : shm_read.c shm_sem.c shm_sem.h

shm_write : shm_write.c shm_sem.c shm_sem.h
