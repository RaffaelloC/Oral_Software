#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>

const char* help = "Welcome to this simple and intuitive study software!\nCommands work as follows:\nn: get new question\na: get the answer for the question\ns: source for the answer (if there is one)\nq: exit\nh: print this message";

typedef struct{
  char **matrix;
  int num_ques;
} phrase_matrix;


int main(int argc, char *argv[]){
  srand(time(NULL));
  if(argc != 3 || (argv[1][0] != 'q' && argv[1][0] != 'a') || (argv[2][0] != 'q' && argv[2][0] != 'a') || (argv[1][0] == argv[2][0])){
    fprintf(stdout, "%s %s %s\n", "wrong usage:", argv[0], "file1 file2");
    fprintf(stdout, "%s\n","files startinng with q_ are used as questions and those starting with a_ are used as answers");
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
  questions.matrix = malloc(100 * sizeof(char *));
  questions.num_ques = 0;
  answers.matrix = malloc(100 * sizeof(char *));
  char buffer[256];
  int last_num = 1;
  while(fgets(buffer, 256*sizeof(char), ques)){
    int i = 0;
    char num[4];
    while(isdigit(buffer[i])){
      num[i] = buffer[i];
      i++;
    }
    num[i] = '\0';
    if(buffer[i] == ':'){
      if(questions.num_ques > 0){
        questions.matrix[questions.num_ques - 1][strcspn(questions.matrix[questions.num_ques - 1], "\n")] = '\0';
      }
      questions.num_ques++;
      int number = atoi(num);
      fprintf(stderr, "%s\n", "ah!");
      //thats a pointer you are assigning, not copying a string 
      //dipshit
      questions.matrix[questions.num_ques - 1] = malloc(1024 * sizeof(char));
      strcpy(questions.matrix[questions.num_ques - 1],buffer);
      last_num = number;
    }
    else{
      fprintf(stderr, "%s\n", questions.matrix[questions.num_ques - 1]);
      fprintf(stderr, "%s\n", buffer);
      strcat(questions.matrix[questions.num_ques - 1], buffer);
    }
    fprintf(stderr, "%s\n", questions.matrix[questions.num_ques - 1]);
  }
  questions.matrix[questions.num_ques - 1][strcspn(questions.matrix[questions.num_ques - 1], "\n")] = '\0';
  pid_t process = fork();
  char string[256];
  if(process == 0){
    close(up[1]);
    while(1){
      read(up[0], string, 256*sizeof(char));
      printf("%s\n", string);
      char dir[256];
      getcwd(dir, 256*sizeof(char));
      strcat(dir, "/");
      strcat(dir, string);
      dir[strcspn(dir, "\n")] = '\0';
      fprintf(stderr, "%s\n", dir);
      execl("/bin/imv","imv", dir, (char *) NULL);
    }
  }
  else{
    int c;
    close(up[0]);
    fprintf(stderr, "%s\n", help);
    while((c = getchar()) != 'q'){
      switch (c){
        case 'q':
          break;
        case 'n':
          fprintf(stderr, "%s\n", questions.matrix[rand() % questions.num_ques]);
          break;
        case 's':
          fgets(string, 256*sizeof(char), answ);
          int lel = write(up[1], string, 256*sizeof(char));
          fprintf(stderr, "%d\n", lel);
          break;
        case 'h':
          fprintf(stderr, "%s\n", help);
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
