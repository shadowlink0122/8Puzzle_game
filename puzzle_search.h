// #include <stdio.h>
#include <string.h>
// #include <assert.h>
#ifndef ASTAR_H
#define ASTAR_H

#define SIZE 9

#define TRUE 1
#define FALSE 0
#define FORWARD 1
#define BACKWARD 2
#define NIL (-1)

#define MAX_STATE 362880//181440 * 2 = 9!

#define MAX_HISTORY 20

typedef struct PZL_STATUS_FOR_NUMBER{
  int x;
  int y;
  char name;
}NUMBER_STATUS;

const char adjacent[SIZE][5] = {
  {1,3,NIL,NIL,NIL},
  {0,4,2,NIL,NIL},
  {1,5,NIL,NIL,NIL},
  {0,4,6,NIL,NIL},
  {1,3,5,7,NIL},
  {2,4,8,NIL,NIL},
  {3,7,NIL,NIL,NIL},
  {4,6,8,NIL,NIL},
  {5,7,NIL,NIL,NIL}
};

int StepForPzl;

char init_state[SIZE] = {
  //1,0,3,4,5,2,7,6,8
  7,4,1,8,5,2,0,6,3
};

char final_state[SIZE] = {
  7,4,1,8,5,2,0,6,3
};

int space_number = 8;
int real_space[SIZE] = {
  1,2,3,
  4,5,6,
  7,8,0,
};

char state[MAX_STATE+1][SIZE];
char space_position[MAX_STATE];
int prev_state[MAX_STATE];
int number_table[MAX_STATE];

char check_table[MAX_STATE];
char answer_table[32][SIZE];

int CONFIRM_STATE = 0;

void InitializeAStar(){
  StepForPzl = 0;
  for(int i = 0;i < SIZE;i++)init_state[i] = 0;
  for(int i = 0;i < MAX_STATE;i++){
    space_position[i] = 0;
    prev_state[i] = number_table[i] = 0;
  }
  for(int i = 0;i < MAX_STATE+1;i++){
    for(int j = 0;j < SIZE;j++){
      state[i][j] = 0;
    }
  }
  for(int i = 0;i < MAX_STATE*2;i++){
    check_table[i] = 0;
  }
  CONFIRM_STATE = 0;
}

extern void CountState(int n){
  for(int i = 0;i < SIZE;i++){
    answer_table[StepForPzl][i] = state[n][i];
  }
  StepForPzl++;
}

extern void CountAnswerForward(int n){
  if(1 < n)CountAnswerForward(prev_state[n]);
  CountState(n);
}

extern void CountAnswerBackward(int n){
  do{
    n = prev_state[n];
    CountState(n);
  }while(prev_state[n] != NIL);
}

extern int CountAnswer(int first_pos,int first_num,int sec_num){
  int sec_pos = first_pos-1;
  while(sec_num != number_table[sec_pos])sec_pos--;
  if(check_table[first_num] == FORWARD){
    CountAnswerForward(first_pos);
    CountAnswerBackward(sec_pos);
  }else{
    CountAnswerForward(sec_pos);
    CountAnswerBackward(first_pos);
  }
  return 1;
}

extern int ChangeNumber(char *board){
  char work[SIZE];
  static int fact_table[SIZE] = {
    40320, 5040, 720, 120, 24, 6, 2, 1, 0
  };
  int value = 0;
  memcpy(work,board,SIZE);
  for(int i = 0;i < SIZE-1;i++){
    value += fact_table[i] * work[i];
    for(int j = i+1;j < SIZE;j++){
      if(work[i] < work[j])work[j]--;
    }
  }
  return value;
}

extern void InitQueue(){
  int num;
  memcpy(state[0],init_state,SIZE);
  prev_state[0] = NIL;
  num = ChangeNumber(init_state);
  number_table[0] = num;
  check_table[num] = FORWARD;
  memcpy(state[1],final_state,SIZE);
  prev_state[1] = NIL;
  num = ChangeNumber(final_state);
  number_table[1] = num;
  check_table[num] = BACKWARD;
}

int Search(){
  int front = 0,rear = 2;
  InitQueue();
  while(front < rear){
    // printf("rear:%d\n",(int)rear);
    int s = space_position[front];
    int first_num = number_table[front];
    int sec_num,n;
    for(int i = 0;(n = adjacent[s][i]) != NIL;i++){
      memcpy(state[rear],state[front],SIZE);
      state[rear][s] = state[rear][n];
      state[rear][n] = real_space[space_number];
      space_position[rear] = n;
      prev_state[rear] = front;
      sec_num = ChangeNumber(state[rear]);
      if(!check_table[sec_num]){
        number_table[rear] = sec_num;
        check_table[sec_num] = check_table[first_num];
        rear++;
      }else if(check_table[first_num] != check_table[sec_num]){
        CountAnswer(rear,first_num,sec_num);
        // printf("状態数 %d 個\n",rear);
        return 1;
      }
    }
    front++;
  }
  // puts("Not found answer");
  return 0;
}

#endif
