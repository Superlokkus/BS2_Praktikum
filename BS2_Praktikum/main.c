//
//  main.c
//  BS2_Praktikum
//
//  Created by Markus Klemm on 08.04.15.
//  Copyright (c) 2015 net.markusklemm. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>
#include <sys/shm.h>
#include "my_semaphore.h"

int child_main();

int main(int argc, const char * argv[]) {
    const uint_fast8_t child_count = 5;
    pid_t childs[child_count];
    int i;
    for(i = 0; i < child_count; i++)
    {
        childs[i] = fork();
        if (childs[i] == 0)
        {
            //Child
            return child_main();
        } else if(childs[i] == -1)
        {
            perror("Fork of child failed");
            return EXIT_FAILURE;
        }
    }
    
    for (i = 0; i < child_count;) {
        int child_return_value;
        pid_t wait_return = waitpid(childs[i], &child_return_value,0);
        if (wait_return == -1)
        {
            if (errno == EINTR) {
                perror("Waitpid unblocked early because");
                continue;
            } else
            {
                perror("Waitpid failed");
                exit(EXIT_FAILURE);
            }
        }
        if (WIFEXITED(child_return_value)) {
            printf("Child Nr. %d exited with %d\n",i+1,WEXITSTATUS(child_return_value));
            //while( (wpid = wait(&exitStatus)) > 0)
        }
        i++;
    }
    return EXIT_SUCCESS;
}

int child_main()
{
    
    return EXIT_SUCCESS;
}
