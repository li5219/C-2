/*
  Found this program on DreamInCode.com that shows a bit how pointers work. So I copied it and tried it out.
  [http://www.dreamincode.net/forums/topic/167766-plain-c-returning-a-string-from-a-function/page__st__15]
  */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
  const char *prompt;
  char *response;
} UserResponse;


void
askQuestion (UserResponse * q)
{
  char buffer[50];
  printf ("%s: ", q->prompt);
  scanf ("%s", buffer);
  q->response = malloc (strlen (buffer) + 1);
  strcpy (q->response, buffer);
}


void
askQuestions (UserResponse * qList, int qListSize)
{
  int i;
  for (i = 0; i < qListSize; i++)
    {
      askQuestion (qList + i);
    }
}

void
showQuestions (UserResponse qList[], int qListSize)
{
  int i;
  for (i = 0; i < qListSize; i++)
    {
      printf ("%d\t%s: %s\n", (i + 1), qList[i].prompt, qList[i].response);
    }
}

void
freeQuestions (UserResponse qList[], int qListSize)
{
  while (--qListSize >= 0)
    {
      free (qList[qListSize].response);
    }
}



int
main ()
{
#define Q_NUM 3
  UserResponse questions[Q_NUM] = {
    {"First Name", NULL}, {"Last Name", NULL},
    {"Favorite Color", NULL},
  };

  askQuestions (questions, Q_NUM);
  showQuestions (questions, Q_NUM);
  freeQuestions (questions, Q_NUM);
  return 0;
}
