
#include <stdio.h>     /* printf */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* fork */
#include <sys/wait.h>  /* waitpid */
#include <sys/types.h> /* pid_t */


void process(int number, int time) {
    printf("Prosess %d kjører\n", number);
    sleep(time);
    printf("Prosess %d kjørte i %d sekunder\n", number, time);
}

int processValidation(pid_t p){
    /* process failed*/
    if(p == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    /* ok */
    else if(p == 0){
        return 1;
    }
    return 0;
}

int main(void){
    pid_t p1 = fork();
    if(processValidation(p1)){
        process(2, 3);
        exit(EXIT_SUCCESS);
    }
    else{
        process(0, 1);
        pid_t p2 = fork();
        if(processValidation(p2)){
            process(4, 3);
            exit(EXIT_SUCCESS);
        }
        else{
            process(1, 2);
            pid_t p3 = fork();
            if(processValidation(p3)){
                if(EXIT_FAILURE == waitpid(p1, NULL, 0)){
                    exit(EXIT_FAILURE);
                }
                else{
                    process(3, 2);
                    exit(EXIT_SUCCESS);
                }
            }
            else{
                if(EXIT_FAILURE == waitpid(p2, NULL, 0)){
                    exit(EXIT_FAILURE);
                }
                else{
                    process(5, 3);
                }
            }
            if(EXIT_FAILURE == waitpid(p3, NULL, 0)){
                exit(EXIT_FAILURE);
            }
        }

    }
    return EXIT_SUCCESS;

}
