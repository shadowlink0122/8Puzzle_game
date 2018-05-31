#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <handy.h>
#include "puzzle_search.h"
#define W 300

int STEP;
int MemoSpace[2];
int PlayMode = 1;
bool goal_flag;
bool Using_Random = false;
bool OpenFile = false;
bool prepare = false;
unsigned short file_name_size = 0;
char input_file_name[32] = "";
char file_name[32] = "";
int picpzl_size = 0;
int now_picpzl = 0;
int now_pic_num = 0;
int change_position = 0;
double x = 0,y = 0;
time_t time_start;
double seconds = 0;
int changing_position = -1;
bool create_ok = false;
short check = 1;
bool save = false;
bool printans = false;
double count = 0;
bool count_clock = false;
short mode3_mode = 0;
int Score = 0,add_Score = 0;

short his[MAX_HISTORY][SIZE];

char DefaultNumber[9] = {
  1,0,3,4,5,2,7,6,8
};

NUMBER_STATUS pzl[SIZE] = {
  {0,0, 1 },{0,1, 6 },{0,2, 7 },
  {1,0, 2 },{1,1, 3 },{1,2, 4 },
  {2,0, 5 },{2,1, 0 },{2,2, 8 }
};

double X_List[SIZE] = {
  2,0,1,2,0,1,2,0,1
};

double Y_List[SIZE] = {
  0,2,2,2,1,1,1,0,0
};

void Swap(char *x,char *y){
  if(goal_flag == false)STEP++;
  char change;
  change = *x;
  *x = *y;
  *y = change;
}

void swap(char *swapx,char *swapy){
  char change;
  change = *swapx;
  *swapx = *swapy;
  *swapy = change;
}

void Create_Line(int *WindowID){
  for(int i = 0;i < 4;i++){
    HgWLine(*WindowID,200+i*(W/3),100,200+i*(W/3),100+W);
    HgWLine(*WindowID,200,100+i*(W/3),200+W,100+i*(W/3));
  }
}

void EraseGoal(){
  HgSetFillColor(HG_WHITE);
  HgBoxFill(220,0,250,90,0);
}

int Goal(int *layerID){
  for(int i = 0;i < SIZE;i++)if(final_state[i] != pzl[i].name)return 0;
  goal_flag = true;
  HgWSetColor(*layerID,HG_RED);
  HgWSetFont(*layerID,HG_TI,70.0);
  if(STEP == StepForPzl-1 && goal_flag == true){
    HgWText(*layerID,230,0,"Perfect!");
  }else if(goal_flag){
    HgWText(*layerID,260,0,"GOAL");
  }
  return 1;
}

void EraseStep(){
  HgSetFillColor(HG_WHITE);
  HgBoxFill(250,W+110,250,100,0);
  HgSetColor(HG_BLACK);
}

void PrintStepNumber(int *WindowID){
  EraseStep();
  HgWSetFont(*WindowID,HG_TI,40.0);
  HgWSetColor(*WindowID,HG_BLACK);
  if(STEP > StepForPzl-1)HgWSetColor(*WindowID,HG_RED);
  HgWText(*WindowID,300,W+120,"%d  手",STEP);
  HgWSetColor(*WindowID,HG_BLACK);
}

void PrintMinStep(int n,int *WindowID){
  HgWBoxFill(*WindowID,0,0,220,100,0);
  HgWSetColor(*WindowID,HG_BLACK);
  HgWSetFont(*WindowID,HG_HI,40.0);
  if(n == -1){
    HgWText(*WindowID,0,0,"Creating");
  }else if(n == 1){
    HgWSetFont(*WindowID,HG_HI,39.0);
    HgWText(*WindowID,20,20,"最短 %d 手",StepForPzl-1);
  }else{
    HgWText(*WindowID,0,0,"No Answer");
  }
}

void PrintPzl(int *WindowID){
  HgWSetFont(*WindowID,HG_TI,80.0);
  for(int i = 0;i < SIZE;i++){
    // if(i % 3 == 0)puts("");
    // printf("%d",pzl[i].name);
    int x1 = i/3;
    int y1 = i % 3;
    HgWSetFillColor(*WindowID,HG_WHITE);
    if(pzl[i].name != real_space[space_number]){
      HgWBoxFill(*WindowID,200+pzl[i].x*(W/3),100+pzl[i].y*(W/3),W/3-2,W/3-2,0);
      HgWImageDrawRect(*WindowID,200+(100*x1),100+(100*y1),100,100,now_pic_num,X_List[(int)pzl[i].name]*x/3,Y_List[(int)pzl[i].name]*y/3,x/3,y/3);
    }else{
      HgWSetFillColor(*WindowID,HG_WHITE);
      HgWBoxFill(*WindowID,200+(100*x1),100+(100*y1),100,100,0);
      MemoSpace[0] = i / 3;
      MemoSpace[1] = i % 3;
    }
  }
  // puts("");
}

void Retry(int *WindowID){
  STEP = 0;
  goal_flag = false;
  for(int i = 0;i < SIZE;i++){
    pzl[i].name = answer_table[0][i];
    if(pzl[i].name == real_space[space_number]){
      MemoSpace[0] = pzl[i].x;
      MemoSpace[1] = pzl[i].y;
    }
  }
  EraseGoal();
  PrintPzl(WindowID);
}

void PrintAnswer(hgevent *event,int *WindowID){
  if(event != NULL)if(event->ch == 'b'){
    printans = false;
    // STEP--;
    return;
  }
  int p = count;
  if(StepForPzl-1 == STEP){
    printans = false;
    return;
  }
  for(int i = 0;i < SIZE;i++){
    pzl[i].name = answer_table[p][i];
    if(pzl[i].name == real_space[space_number]){
      MemoSpace[0] = i / 3;
      MemoSpace[1] = i % 3;
    }
  }
  if(StepForPzl != STEP)STEP = p;
}

int Evaluate(int yourtime,int minstep,int yourstep,int myScore){
  int time_point = (60 - yourtime) + (2000 / (myScore + 500));
  int step_point = 100 + 2 * (2 * minstep - yourstep) - (myScore / 10);
  if(yourtime <= 30)time_point += 150;
  if(minstep == yourstep + 1)step_point += 300;
  return time_point + step_point;
}

int NegativeEvaluate(int yourtime,int minstep,int yourstep,int myScore){
  int time_point = yourtime * 2;
  int step_point = yourstep * 3;
  if(minstep > yourstep)step_point += 50;
  else step_point *= 4 / 3;
  return -1 * (time_point + step_point + myScore / 10);
}

void Random_shuffle(int *WindowID){
  goal_flag = false;
  EraseGoal();
  PrintMinStep(-1,WindowID);
  STEP = 0;
  first:;
  InitializeAStar();
  srand((unsigned)time(NULL));
  char CopyInstance[9];
  int empty = 0;
  int obj = 0;
  
  for(int i = 0;i < SIZE;i++)CopyInstance[i] = DefaultNumber[i];
  while(empty < 9){
    obj = rand()%9+0;
    if(CopyInstance[obj] != '!'){
      init_state[empty] = pzl[empty].name = CopyInstance[obj];
      CopyInstance[obj] = '!';
      if(pzl[empty].name == real_space[space_number]){
        MemoSpace[0] = pzl[empty].x;
        MemoSpace[1] = pzl[empty].y;
        space_position[0] = empty;
      }
      if(final_state[empty] == real_space[space_number]){
        space_position[1] = empty;
      }
      empty++;
    }
  }
  int check_ans = Search();
  if(check_ans != 1 && StepForPzl < 1)goto first;
  PrintMinStep(check_ans,WindowID);
}

int PrintMenu_Mode1(hgevent *event,int *WindowID){
  //EraseMenu1
  HgWSetFillColor(*WindowID,HG_WHITE);
  HgWBoxFill(*WindowID,0,100,200,400,0);
  HgWSetColor(*WindowID,HG_BLACK);
  //Exit
  HgWSetFont(*WindowID,HG_TI,60.0);
  HgWText(*WindowID,50,415,"Exit");
  HgWBox(*WindowID,20,420,160,60);
  //TimeAttack
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,60,300," Time\nAttack");
  HgWBox(*WindowID,20,300,160,80);
  //Create
  HgWSetFont(*WindowID,HG_TI,50.0);
  HgWText(*WindowID,35,220,"Create");
  HgWBox(*WindowID,20,220,160,60);
  //PrintAnswer
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,60,100,"Answer ");
  HgWBox(*WindowID,50,100,110,35);
  //Retry
  HgWSetFont(*WindowID,HG_TI,60.0);
  HgWText(*WindowID,35,145,"Retry");
  HgWBox(*WindowID,20,145,160,60);
  //クリックで処理
  if(event == NULL)return 0;
  if(((20 < event->x && event->x < 180) && (420 < event->y && event->y < 480)) || event->ch == 'q')return -1;
  else if(((50 < event->x && event->x < 160) && (100 < event->y && event->y < 135)) || event->ch == 'p'){
    count = 0;
    printans = true;
    Retry(WindowID);
  }
  else if(((20 < event->x && event->x < 180) && (145 < event->y && event->y < 205)) || event->ch == 'r'){
    Retry(WindowID);
    PrintStepNumber(WindowID);
  }
  else if(((20 < event->x && event->x < 180) && (220 < event->y && event->y < 280)) || event->ch == 'c'){
    HgClear();
    HgWSetFillColor(*WindowID,HG_BLACK);
    PlayMode = 2;
  }
  else if(((20 < event->x && event->x < 180) && (300 < event->y && event->y < 380)) || event->ch == 't'){
    HgClear();
    PlayMode = 3;
    mode3_mode = 0;
    Retry(WindowID);
    PrintPzl(WindowID);
    PrintStepNumber(WindowID);
    PrintMinStep(1,WindowID);
    seconds = 0;
  }
  return 0;
}

//Create PZL
int PrintMenu_Mode2(hgevent *event,int *WindowID){
  if(event == NULL)goto end;
  if(((20 < event->x && event->x < 180) && (420 < event->y && event->y < 480))){
    //Back
    if(Using_Random)Random_shuffle(WindowID);
    else if(!create_ok){
      InitializeAStar();
      for(int i = 0;i < SIZE;i++)init_state[i] = pzl[i].name;
      for(int i = 0;i < SIZE;i++){
        if(init_state[i] == real_space[space_number])space_position[0] = i;
        if(final_state[i] == real_space[space_number])space_position[1] = i;
      }
      if(Search() != 1){
        puts("Random!");
        Random_shuffle(WindowID);
      }
    }
    OpenFile = false;
    PlayMode = 1;
    create_ok = false;
    changing_position = -1;
    save = false;
    PrintMenu_Mode1(NULL,WindowID);
    Retry(WindowID);
    PrintPzl(WindowID);
    PrintStepNumber(WindowID);
    PrintMinStep(1,WindowID);
  }
  if(((20 < event->x && event->x < 180) && (340 < event->y && event->y < 400))){
    //check
    check = 1;
  }
  else if(((20 < event->x && event->x < 180) && (260 < event->y && event->y < 320))){
    //Random
    Using_Random = (Using_Random == true) ? false : true;
  }
  else if(((35 < event->x && event->x < 155) && (200 < event->y && event->y < 240))){
    //OpenFile
    OpenFile = (OpenFile == true) ? false : true;
  }
  else if(((45 < event->x && event->x < 145) && (150 < event->y && event->y < 190))){
    //Start HgWBox(*WindowID,45,110,100,40);
    change_position = (change_position == 1) ? 0 : 1;
    if(change_position == 1){
      for(int i = 0;i < SIZE;i++){
        init_state[i] = pzl[i].name = final_state[i];
      }
    }
  }
  else if(((45 < event->x && event->x < 145) && (100 < event->y && event->y < 140))){
    //Goal HgWBox(*WindowID,45,60,100,40);
    change_position = (change_position == 2) ? 0 : 2;
  }
  // クリックでスタートとゴールを変更
  // OriginalSortPictureが適応してから使える
  else if((200 < event->x && event->x < 500) && (100 < event->y && event->y < 400)){
    int spacex = (event->x-200)/100;
    int spacey = (event->y-100)/100;
    //スタート
    if(change_position == 1){
      if(changing_position == -1){
        changing_position = 3*(spacex)+(spacey);
      }else{
        //指定した場所と交換
        swap(&pzl[3*(spacex)+(spacey)].name,&pzl[changing_position].name);
        changing_position = -1;
      }
    }
    //ゴール
    if(change_position == 2){
      changing_position = -1;
      space_number = (spacex)+3*(2-spacey);
    }
    PrintPzl(WindowID);
  }
  else if(event->ch == ' '){
    for(int i = 0;i < file_name_size;i++)input_file_name[i] = 0;
    file_name_size = 0;
  }
  end:;
  // //Back
  HgWSetFont(*WindowID,HG_TI,60.0);
  HgWText(*WindowID,40,415,"Back");
  HgWBox(*WindowID,20,420,160,60);
  //Check
  HgWSetFont(*WindowID,HG_TI,40.0);
  HgWText(*WindowID,35,345,"CHECK");
  HgWBox(*WindowID,20,340,160,60);
  if(check == 1){
    InitializeAStar();
    for(int i = 0;i < SIZE;i++)init_state[i] = pzl[i].name;
    for(int i = 0;i < SIZE;i++){
      if(init_state[i] == real_space[space_number])space_position[0] = i;
      if(final_state[i] == real_space[space_number])space_position[1] = i;
    }
    HgWSetFont(*WindowID,HG_TI,40.0);
    check = Search() + 10;
  }
  if(check == 11){
    HgWText(*WindowID,220,0,"%d手",StepForPzl-1);
    create_ok = true;
    save = true;
  }else{
    HgWText(*WindowID,220,0,"No Answer");
    create_ok = false;
    for(int i = 0;i < SIZE;i++)init_state[i] = pzl[i].name;
  }
  //Random
  if(Using_Random)HgWSetColor(*WindowID,HG_BLUE);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,35,270,"RANDOM");
  HgWBox(*WindowID,20,260,160,60);
  HgWSetColor(*WindowID,HG_BLACK);
  //FileOpen
  if(OpenFile)HgWSetColor(*WindowID,HG_BLUE);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,40,200,"OpenFile");
  HgWBox(*WindowID,35,200,120,40);
  HgWSetColor(*WindowID,HG_BLACK);
  //Start
  if(change_position == 1)HgWSetColor(*WindowID,HG_BLUE);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,65,150,"Start");
  HgWBox(*WindowID,45,150,100,40);
  HgWSetColor(*WindowID,HG_BLACK);
  //Goal
  if(change_position == 2)HgWSetColor(*WindowID,HG_BLUE);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,65,100,"Goal");
  HgWBox(*WindowID,45,100,100,40);
  HgWSetColor(*WindowID,HG_BLACK);
  if(OpenFile)return -10;
  return 0;
}

int PrintMenu_Mode3(hgevent *event,int *WindowID){
  if(event == NULL)goto end;
  if(event->ch == 'b' || ((20 < event->x && event->x < 180) && (420 < event->y && event->y < 480))){
    if(count_clock)goto end;
    HgClear();
    PlayMode = 1;
    PrintMenu_Mode1(NULL,WindowID);
    PrintPzl(WindowID);
    PrintStepNumber(WindowID);
    PrintMinStep(1,WindowID);
  }else if(event->ch == 's' || ((35 < event->x && event->x < 155) && (250 < event->y && event->y < 290))){
    if(!count_clock){
      if(Goal(WindowID))Random_shuffle(WindowID);
      count_clock = true;
    }
    else goto end;
    mode3_mode = 1;
    if(Score > 2000)Random_shuffle(WindowID);
    else Retry(WindowID);
    time_start = time(NULL);
  }else if(event->ch == 'r' || ((35 < event->x && event->x < 155) && (200 < event->y && event->y < 240))){
    if(count_clock)count_clock = false;
    else goto end;
    mode3_mode = 2;
    seconds = 0;
    add_Score = NegativeEvaluate((int)difftime(time(NULL),time_start),StepForPzl,STEP,Score);
    Score += add_Score;
  }
  end:;
  if(Goal(WindowID)){
    if(count_clock){
      add_Score = Evaluate((int)difftime(time(NULL),time_start),StepForPzl,STEP,Score);
      Score += add_Score;
    }
    count_clock = false;
    mode3_mode = 0;
  }
  EraseGoal();
  //Timer
  if(count_clock)seconds = difftime(time(NULL),time_start);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,50,330,"%d分%d秒",(int)seconds / 60,(int)seconds % 60);
  // //Back
  HgWSetColor(*WindowID,HG_BLACK);
  HgWSetFont(*WindowID,HG_TI,60.0);
  HgWText(*WindowID,40,415,"Back");
  HgWBox(*WindowID,20,420,160,60);
  //Start
  if(mode3_mode == 1)HgWSetColor(*WindowID,HG_BLUE);
  HgWSetFont(*WindowID,HG_TI,30.0);
  HgWText(*WindowID,40,250,"   Start");
  HgWBox(*WindowID,35,250,120,40);
  HgWSetColor(*WindowID,HG_BLACK);
  //Retire
  if(mode3_mode == 2)HgWSetColor(*WindowID,HG_BLUE);
  HgWText(*WindowID,50,200," Retire");
  HgWBox(*WindowID,35,200,120,40);
  HgWSetColor(*WindowID,HG_BLACK);
  //Score
  if(Score < 0)Score = 0;
  if(add_Score >= 0)HgWText(*WindowID,0,0,"Your Score\n %d (+%d)",Score,add_Score);
  else HgWText(*WindowID,0,0,"Your Score\n %d (%d)",Score,add_Score);
  HgWSetColor(*WindowID,HG_BLACK);
  return 0;
}

void Exchange(int x,int y,int *WindowID){
  int obj,space_obj;
  for(int i = 0;i < SIZE;i++){
    if(pzl[i].x == x && pzl[i].y == y){
      obj = i;
      MemoSpace[0] = pzl[i].x;
      MemoSpace[1] = pzl[i].y;
    }else if(pzl[i].name == real_space[space_number]){
      space_obj = i;
    }
  }
  Swap(&pzl[obj].name,&pzl[space_obj].name);
  PrintPzl(WindowID);
  PrintStepNumber(WindowID);
  Goal(WindowID);
}

void InputKey(int key,int *WindowID){
  switch(key){
    case HG_R_ARROW:
      if(MemoSpace[0] != 0)Exchange(MemoSpace[0]-1,MemoSpace[1],WindowID);
      break;
    case HG_L_ARROW:
      if(MemoSpace[0] != 2)Exchange(MemoSpace[0]+1,MemoSpace[1],WindowID);
      break;
    case HG_U_ARROW:
      if(MemoSpace[1] != 0)Exchange(MemoSpace[0],MemoSpace[1]-1,WindowID);
      break;
    case HG_D_ARROW:
      if(MemoSpace[1] != 2)Exchange(MemoSpace[0],MemoSpace[1]+1,WindowID);
      break;
    default:
      break;
  }
}

int CreateOriginalPazle(int *picnum,int *WindowID){
  if(*picnum == -1)return -1;
  HgWSetFillColor(*WindowID,HG_WHITE);
  HgWBoxFill(*WindowID,200,100,W,W,0);
  HgWSetColor(*WindowID,HG_BLACK);
  HgImageSize(*picnum,&x,&y);
  HgWImageDrawRect(*WindowID,200,100,W,W,*picnum,0,0,x,y);
  Create_Line(WindowID);
  for(int i = 0;i < strlen(input_file_name);i++)file_name[i] = input_file_name[i];
  prepare = true;
  now_picpzl = picpzl_size++;
  now_pic_num = *picnum;
  if(picpzl_size == 100)picpzl_size = 0;
  PrintPzl(WindowID);
  return 0;
}

int main(int argc,const char *argv[]){

  int WindowID;
  int layerID;
  int choosed_picture = -1;
  doubleLayer layers;
  hgevent *event = NULL;

  WindowID = HgOpen(200+W,100+W+100);
  layers = HgWAddDoubleLayer(WindowID);
  layerID = HgLSwitch(&layers);

  // You should prepare any(Max:10) image in the same directory 
  // and write the name of the image here.
  char name[10][32] = {
    "","","","","",
    "","","","",""
  };

  if(argc > 1){
    choosed_picture = HgImageLoad(argv[1]);
    strcpy(input_file_name,argv[1]);
    goto bf;
  }
  for(int i = 0;i < SIZE;i++){
    printf("%s\n",name[i]);
    choosed_picture = HgImageLoad(name[i]);
    strcpy(input_file_name,name[i]);
    if(choosed_picture != -1)break;
  }
  bf:;
  if(choosed_picture == -1){
    puts("Error");
    return 0;
  }
  for(int i = 0;i < 32;i++)input_file_name[i] = 0;
  // Create_Line(&WindowID);
  CreateOriginalPazle(&choosed_picture,&layerID);
  PrintMenu_Mode1(NULL,&layerID);
  Random_shuffle(&layerID);
  PrintStepNumber(&layerID);
  PrintPzl(&layerID);

  //画面操作
  HgSetEventMask(HG_KEY_DOWN|HG_MOUSE_DOWN);
  while(1){
    layerID = HgLSwitch(&layers);
    HgLClear(layerID);
    event = HgEventNonBlocking();
    if(printans){
      count += 0.1;
      PrintAnswer(event,&layerID);
      // if(event != NULL)if(event->ch != 'b')continue;
    }
    PrintPzl(&layerID);
    if(event != NULL)if(event->ch == 'q')break;
    if(PlayMode == 1){
      EraseStep();
      Goal(&layerID);
      PrintStepNumber(&layerID);
      PrintMinStep(1,&layerID);
      PrintMenu_Mode1(event,&layerID);
      if(event != NULL){
        if(PrintMenu_Mode1(event,&layerID) == -1)break;
        else if(event->ch == ' '){
          Random_shuffle(&layerID);
          PrintPzl(&layerID);
        }else{
          InputKey(event->ch,&layerID);
        }
      }
    }
    if(PlayMode == 2){
      Create_Line(&WindowID);
        int Mode = PrintMenu_Mode2(event,&layerID);
        if(Mode == -10 && event != NULL){
          if(('a' <= event->ch && event->ch <= 'z') || ('A' <= event->ch && event->ch <= 'Z') 
            || ('0' <= event->ch && event->ch <= '9') || event->ch == '!'){
            if(file_name_size <= 10)input_file_name[file_name_size++] = event->ch;
          }
          if(event->ch == 13){
            bool flag = false;
            for(int i = 0;i < 20;i++)if(input_file_name[i] == '.' || input_file_name[i] == '!')flag = true;
            if(flag)goto skip;
            strcat(input_file_name,".jpg");
            file_name_size += 4;
            skip:;
            choosed_picture = HgImageLoad(input_file_name);
            CreateOriginalPazle(&choosed_picture,&layerID);
          }
      }
      HgWSetFont(layerID,HG_TI,30.0);
      HgWText(layerID,10,10,"%s",input_file_name);
      if(PlayMode == 1){
        HgWClear(layerID);
        PrintMenu_Mode1(NULL,&layerID);
        PrintPzl(&layerID);
        PrintStepNumber(&layerID);
        PrintMinStep(1,&layerID);
      }
    }
    if(PlayMode == 3){
      PrintMenu_Mode3(event,&layerID);
      if(event != NULL && count_clock == true)InputKey(event->ch,&layerID);
      PrintStepNumber(&layerID);
    }
  }
  HgSleep(0.1);
  HgWClose(WindowID);
  return 0;
}
