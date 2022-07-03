
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


#define Frame_no 2
#define FramevMax 9
#define pictMax 7 
#define overFlow 1000000000

int frame[FramevMax + 1][Frame_no + 1];
int count = 0;
int score = 0;
struct timespec startTime, goesTime;
long startnsec, goesnsec,overflow;
bool endflag,scoreflag;
char pictPattern[pictMax][1] = {\
"1"\
,"2"\
,"3"\
,"4"\
,"5"\
,"6"\
,"7"\
};


int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void initSlotFrame()
{
  frame[0][0] = 0;
  frame[1][0] = 0;
  frame[2][0] = 0;
  frame[3][0] = 1;
  frame[4][0] = 1;
  frame[5][0] = 2;
  frame[6][0] = 2;
  frame[7][0] = 3;
  frame[8][0] = 3;
  frame[9][0] = 4;

  frame[0][1] = 1;
  frame[1][1] = 0;
  frame[2][1] = 1;
  frame[3][1] = 0;
  frame[4][1] = 0;
  frame[5][1] = 2;
  frame[6][1] = 3;
  frame[7][1] = 2;
  frame[8][1] = 3;
  frame[9][1] = 4;

  frame[0][2] = 4;
  frame[1][2] = 0;
  frame[2][2] = 0;
  frame[3][2] = 2;
  frame[4][2] = 1;
  frame[5][2] = 1;
  frame[6][2] = 3;
  frame[7][2] = 2;
  frame[8][2] = 2;
  frame[9][2] = 0;

  clock_gettime(CLOCK_REALTIME,&startTime);
  startnsec = startTime.tv_nsec;
  goesnsec = goesTime.tv_nsec;
}

void updateSlotFrame()
{
  int w;
  int v;
  int bslot,aslot; 
  for(w = 0; w <= Frame_no; w++)
  {
    bslot = frame[FramevMax][w];
    for(v = 0; v <= FramevMax; v++)
    {
      if(w < count) break;
      aslot = frame[v][w];
      frame[v][w] = bslot;
      bslot = aslot;
    }
  }
}

void animator()
{
  int i;
  int m;

  system("clear");

  for(i = 0; i <= Frame_no; i++)
  {
    for(m = 0; m <= Frame_no; m++)
    {
      printf("  %c  ",pictPattern[frame[i][m]][0]);
    }
    printf("\n");
  }
}

int keyCheck()
{
    if(kbhit())
    {
      switch(getchar())
      {
        case('q'):
          endflag = true;
          break;
      
        case(' '):
          count = (count + 1) % (Frame_no + 2);
          scoreflag = true;
          break;

        default:
          break;
      }
    }
    

    return 0;
} 

int line_match_g = 0;

int lineCheck(int y, int x, int vec_y, int line_match)
{
  if((x + 1) > Frame_no || 0 > (y + vec_y) || (y + vec_y) > Frame_no) return line_match;
  if(frame[y][x] == frame[y + vec_y][x + 1]){ line_match++;}
  else {line_match = 0;}
  line_match = lineCheck(y + vec_y, x + 1, vec_y, line_match);
  return line_match;
}

int scoreCheck()
{
  int y;
  int score_local= 0;
  
  for(y = 0; y <= Frame_no; y++)
  {
    score_local += (((lineCheck(y, 0, 0,0)) / Frame_no) *  (frame[y][0] + 1))  * 100;
    if(y == 0) score_local += (((lineCheck(y, 0, 1,0)) / Frame_no) * (frame[y][0] + 1))  * 100;
    if(y == Frame_no) score_local += (((lineCheck(y, 0, -1,0)) / Frame_no) * (frame[y][0] + 1))  * 100;
  }
  return score_local;
}

int sceneCheck()
{
  if(endflag) exit(EXIT_SUCCESS);

  if(count >= 3 && scoreflag)
  {
    scoreflag = false;
    score += scoreCheck();
  }

  return 0;
}

void timekeeper()
{
  if(((goesnsec + overflow) - (startnsec)) > 200000000 )
    {
      updateSlotFrame();
      animator();
      printf("score %d\n",score);
      startnsec = goesnsec;
      overflow = 0;
    }

  clock_gettime(CLOCK_REALTIME,&goesTime);
  goesnsec = goesTime.tv_nsec;
  
  if((goesnsec - startnsec) <= 0)
  {
    overflow = overFlow - startnsec;
    startnsec = 0;
  }
}

void stopProgram()
{
  fputs("common INPUT",stderr);
  while(getchar()!='\n');
}

void scoreCalc()
{
}

int main()
{
  initSlotFrame();
  
  while(1)
  {
    keyCheck();
    timekeeper();
    sceneCheck();
  }
  
  return 0;
}
