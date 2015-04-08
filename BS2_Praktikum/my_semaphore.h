//
//  my_semaphore.h
//  BS2.P01
//
//  Created by Markus Klemm on 30.03.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#ifndef __BS2_P01__my_semaphore__
#define __BS2_P01__my_semaphore__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

typedef int my_sem_id;

/*! @brief Creates and initializes one new semaphore
 @param sem_init Value to initialize the semaphore with
 @return An identifier for the new semaphore
 @attention After use destruct semaphore with close_sem()
 */
my_sem_id create_sem(const int sem_init);

/*! @brief Closes and deletes given semaphore
@warning Undefined behaviour if called more than once
 @param sem Identifier of to be deleted semaphore
 */
void delete_sem(my_sem_id sem_id);

/*! @brief Blocks on given semaphore
 @param sem Semaphore to block on
*/
void P(my_sem_id sem_id);

/*! @brief Unlocks semaphore
 @param sem Semaphore to unlock
 */
void V(my_sem_id sem_id);

#endif /* defined(__BS2_P01__my_semaphore__) */
