//
//  p02_time_reader.h
//  BS2_Praktikum
//
//  Created by Markus Klemm on 15.04.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#ifndef __BS2_Praktikum__p02_time_reader__
#define __BS2_Praktikum__p02_time_reader__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/shm.h>
#include <stdbool.h>
#include "my_semaphore.h"

extern bool runFlag;

int reader(int shared_id,my_sem_id mutex,my_sem_id wri);

#endif /* defined(__BS2_Praktikum__p02_time_reader__) */
