/**
  *	This program is a try to work with file attribute: time.	*
  *	It has also been a test to see what pointers do.		*
  */

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int
*getTime (struct tm *clock, struct stat attrib)
{
  static int fileAttribs[3];
  stat ("testSetTime.c", &attrib);	// get the attributes of afile.txt
  clock = gmtime (&(attrib.st_mtime));	// Get the last modified time and put it into the time structure

  fileAttribs[0] = clock->tm_year + 1900;
  fileAttribs[1] = clock->tm_mon + 1;
  fileAttribs[2] = clock->tm_mday;
  
  return fileAttribs;
}

int
main ()
{
  struct tm *clock;		// create a time structure
  struct stat attrib;		// create a file attribute structure
  int *time;
  time = getTime (clock, attrib);

  printf ("Time changed for testSetTime.c is:\n");
  printf ("\tYear:\t\t %d\n", time[0]);
  printf ("\tMonth:\t\t %s\n", (switch (time[1])
                                  case 0: "January"
                                    break;
                                  case 1: "February"
                                    break;
                                  case 2: "Mars"
                                    break;
                                  case 3: "April"
                                    break;
                                  case 4: "May"
                                    break;
                                  case 5: "June"
                                    break;
                                  case 6: "July"
                                    break;
                                  case 7: "August"
                                    break;
                                  case 8: "September"
                                    break;
                                  case 9: "October"
                                    break;
                                  case 10: "November"
                                    break;
                                  case 11: "December"
                                    break;));
  printf ("\tDay of month:\t %d\n", time[2]);

  return 0;
}
