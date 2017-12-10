#ifndef __SHM_SEM_H__
#define __SHM_SEM_H__

#include <sys/sem.h>
#include <sys/shm.h>

#define READ_KEY	7982
#define WRITE_KEY	7983
#define SHM_KEY		7984
#define SHM_SIZE	128

void p(int semid);
void v(int semid);

#endif
