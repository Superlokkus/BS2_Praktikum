//
//  my_semaphore.c
//  BS2.P01
//
//  Created by Markus Klemm on 30.03.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#include "my_semaphore.h"

my_sem_id create_sem(const int sem_init)
{
    int semid = semget(IPC_PRIVATE,1,SEM_R|SEM_A|IPC_CREAT);
    if (semid == -1) {
        perror("Semget() failed");
        exit(EXIT_FAILURE);
    }
    
    union semun {
        int     val;            /* value for SETVAL */
        struct  semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
        u_short *array;         /* array for GETALL & SETALL */
    } semun;
    semun.val = sem_init;
    if (semctl(semid, 0, SETVAL,semun) != 0) {
        perror("Creating semaphore failed in semctl()");
        exit(EXIT_FAILURE);
    }
    return semid;
}

void delete_sem(my_sem_id sem_id)
{
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Destroy of semaphore failed in semctl()");
        exit(EXIT_FAILURE);
    }

}

void P(my_sem_id sem_id)
{
    struct sembuf sembuf={0,-1,SEM_UNDO};
    
    if (semop(sem_id, &sembuf, 1) != 0) {
        perror("Can't lock semaphore in P() because");
        exit(EXIT_FAILURE);
    }
    
}

void V(my_sem_id sem_id)
{
    struct sembuf sembuf={0,1,SEM_UNDO};
    
    if (semop(sem_id, &sembuf, 1) != 0) {
        perror("Can't unlock semaphore in V() because");
        exit(EXIT_FAILURE);
    }
}


