#include "shm_sem.h"

void p(int semid)
{
	struct sembuf p_buf = { 0, -1, SEM_UNDO };
	semop(semid, &p_buf, 1);
}

void v(int semid)
{
	struct sembuf p_buf = { 0, 1, SEM_UNDO };
	semop(semid, &p_buf, 1);
}
