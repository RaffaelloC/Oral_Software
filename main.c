#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h> 
const char* help = "Welcome to this simple and intuitive study software!\nCommands work as follows:\nn: get new question\na: get the answer for the question\ns: source for the answer (if there is one)\nq: exit\nh: print this message";

int first_num = 1;

typedef struct{
  char **matrix;
  int num_ques;
} phrase_matrix;

typedef struct{
  phrase_matrix *matrix;
  FILE *file;
} t_args;

void *tbody(void *temp){
  t_args *f_args = (t_args *) temp;
  char buffer[2048]; 
  while(fgets(buffer, 2048*sizeof(char), f_args->file)){
    int i = 0;
    int pass = 0;
    char num[4];
    while(isdigit(buffer[i])){
      num[i] = buffer[i];
      i++;
    }
    num[i] = '\0';
    if(buffer[i] == ':'){
      if(f_args->matrix->num_ques == 0){
        first_num = atoi(buffer);
      }
        
        fprintf(stderr, "num %d\n", first_num);
      f_args->matrix->num_ques++;
      f_args->matrix->matrix[f_args->matrix->num_ques - 1] = malloc(4096 * sizeof(char));
			strcpy(f_args->matrix->matrix[f_args->matrix->num_ques - 1],buffer);
    }
    else{
      if(f_args->matrix->matrix == NULL){
        fprintf(stderr, "%s\n", "wrong parsing file, aborting");
        abort();
      }
      strcat(f_args->matrix->matrix[f_args->matrix->num_ques - 1], buffer);
    }
    fprintf(stderr, "%s\n", f_args->matrix->matrix[f_args->matrix->num_ques - 1]);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  srand(time(NULL));
  if(argc != 3 || (argv[1][0] != 'q' && argv[1][0] != 'a') || (argv[2][0] != 'q' && argv[2][0] != 'a') || (argv[1][0] == argv[2][0])){
    fprintf(stdout, "%s %s %s\n", "wrong usage:", argv[0], "file1 file2");
    fprintf(stdout, "%s\n","files starting with q_ are used as questions and those starting with a_ are used as answers");
    exit(0);
  }
  FILE *ques;
  FILE *answ;
  int up[2];
  pipe(up);
  if(argv[1][0] == 'q'){
    ques = fopen(argv[1],"r");
    answ = fopen(argv[2],"r");
  }
  else{
    ques = fopen(argv[2],"r");
    answ = fopen(argv[1],"r");
  }
  if(ques == NULL || answ == NULL){
    fprintf(stderr, "files not found\n");
    exit(0);
  }
  phrase_matrix questions, answers;
  questions.matrix = malloc(2048 * sizeof(char *));
  questions.num_ques = 0;
  answers.matrix = malloc(2048 * sizeof(char *));
  answers.num_ques = 0;
  pthread_t ques_t, answ_t;
  t_args answ_args;
  answ_args.file = answ;
  answ_args.matrix = &answers;
  t_args ques_args;
  ques_args.file = ques;
  ques_args.matrix = &questions;
  pthread_create(&ques_t, NULL, &tbody, &ques_args);
  pthread_create(&answ_t, NULL, &tbody, &answ_args);
  pthread_join(ques_t, NULL);
  pthread_join(answ_t, NULL);
  pid_t process = fork();
  if(process == 0){
    char dir[256];
    getcwd(dir, 256*sizeof(char));
    strcat(dir, "/sources/");
    int num_file;
    close(up[1]);
    while(1){
      read(up[0], &num_file, sizeof(int));
      num_file++;
      printf("%d quiiii\n", num_file);
      char file_str[256];
      fprintf(stderr, "%s\n", dir);
      strcpy(file_str, dir);
      fprintf(stderr, "%s\n", file_str);
      char temp_str[10];
      snprintf(temp_str, 10 * sizeof(char), "%d", num_file);
      fprintf(stderr, "%s\n", temp_str);
      strcat(file_str, temp_str);
      strcat(file_str, ".png");
      fprintf(stderr, "%s\n", file_str);
      pid_t show_img = fork();
      if(show_img == 0){
        execl("/bin/imv","imv", file_str, (char *) NULL);
      }
      else{
        wait(NULL);
      }
    }
    
    fprintf(stderr, "%s\n", "boo!");
  }
  else{
    int c;
    close(up[0]);
    fprintf(stderr, "%s\n", help);
    int r_num = 0;
    int num_buff = 0;
    fprintf(stderr, "num of questions: %d\n", questions.num_ques);
    while((c = getchar()) != 'q'){
      switch (c){
        case '\n':
                if(num_buff != 0){
                    r_num = num_buff - 1;
                    fprintf(stderr, "%s\n", questions.matrix[r_num]);
                    num_buff = 0;
                }
                break;
        case 'n':
          r_num = rand() % questions.num_ques;
          fprintf(stderr, "%s\n", questions.matrix[r_num]);
          break;
        case 'a':
          if(r_num < 0){
            fprintf(stderr, "%s\n", "no question asked!\npress 'n' to get a question");
            break;
          }
          fprintf(stderr, "%s\n", answers.matrix[r_num]);
          break;
        case 's':
          if(r_num <= 0){
              fprintf(stderr, "%s\n", "no question asked!\npress 'n' to get a question");
              break;
            }
          fprintf(stderr, "random num: %d\n", r_num); 
          write(up[1], &r_num, sizeof(int));
          break;
        case 'h':
          fprintf(stderr, "%s\n", help);
          break;
        default:
            if(isdigit(c)){
                num_buff *= 10;
                num_buff += c - 48;
            }
            fprintf(stdout, "num: %d\n", num_buff);
            break;
      }
    }
    fprintf(stderr, "%s\n", "parent process exiting!");
    kill(process, SIGTERM);
  }
  fclose(ques);
  fclose(answ);
  return 0;
}
