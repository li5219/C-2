/* filecopy.c - användning av open(), close(), read() och write() */
#include <sys/stat.h>		/* definierar S_IREAD och S_IWRITE */
#include <errno.h>		/* definierar errno och strerror() */
#include <fcntl.h>		/* definierar bland annat open() och O_RDONLY */
#include <limits.h>		/* definierar bland annat SSIZE_MAX */
#include <stdio.h>		/* definierar bland annat stderr */
#include <stdlib.h>		/* definierar bland annat exit() */
#include <unistd.h>		/* definierar bland annat read() och write() */
#define BUFFERSIZE (1024)

int main (int argc, char *argv[])
{
  char buffer[BUFFERSIZE];
  char *oldfilename;
  char *newfilename;
  int oldfiledesc, newfiledesc;
  int read_chars, written_chars;
  int return_value;		/* returvärde från close */
  /* Programmet ska ha två kommandoradsargument förutom namnet
   * vilket totalt blir 3 */
  if (3 != argc)
    {
      printf ("Kommando: %s gammalfil nyfil\n", argv[0]);
      exit (1);
    }
  /* Kommer vi hit så har programmet anropats med två argument */
  oldfilename = argv[1];	/* Första argumentet är gammalfil */
  newfilename = argv[2];	/* Andra argumentet är nyfil */
  /* Försök öppna gammalfil för läsning */
  oldfiledesc = open (oldfilename, O_RDONLY);
  if (-1 == oldfiledesc)
    {
      fprintf (stderr, "Cannot open ");
      perror (oldfilename);
      exit (1);
    }
  /* Försök öppna nyfil för skrivning */
  newfiledesc = open (newfilename,
		      O_CREAT | O_WRONLY | O_TRUNC, S_IREAD | S_IWRITE);
  if (-1 == newfiledesc)
    {
      fprintf (stderr, "Cannot create ");
      perror (newfilename);
      exit (1);
    }
  /* slinga som utför kopieringen */
  read_chars = -1;
  while (0 != read_chars)
    {
      read_chars = read (oldfiledesc, buffer, sizeof (buffer));
      if (read_chars < 0)
	{
	  fprintf (stderr, "Cannot read ");
	  perror (oldfilename);
	  exit (1);
	}
      if (read_chars > 0)
	{
	  written_chars = write (newfiledesc, buffer, read_chars);
	  if (written_chars != read_chars)
	    {
	      fprintf (stderr, "Problems writing ");
	      perror (newfilename);
	      exit (1);
	    }
	}
    }
  return_value = close (oldfiledesc);
  if (-1 == return_value)
    {
      fprintf (stderr, "Cannot close ");
      perror (oldfilename);
      exit (1);
    }
  return_value = close (newfiledesc);
  if (-1 == return_value)
    {
      fprintf (stderr, "Cannot close ");
      perror (newfilename);
      exit (1);
    }
  exit (0);
}
