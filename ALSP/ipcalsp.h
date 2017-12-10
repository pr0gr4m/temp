#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !(_SEM_SEMUN_UNDEFINED)

#else

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
};

#endif

int sysv_semget(char *tok, key_t sem_fixkey, int n_sem, int sem_value, int user_mode);
int sysv_semrm(int sem_id);
int sysv_semwait(int sem_id, int sem_idx);
int sysv_sempost(int sem_id, int sem_idx);
int sysv_semzwait(int sem_id, int sem_idx);
int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode);
int sysv_msgrm(int msg_id);
