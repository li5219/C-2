#include <stdio.h>

typedef long Align;		/* for alignment to long boundary */

union header
{				/* block header */
  struct
  {
    union header *ptr;		/* next block if on free list */
    unsigned size;		/* size of this block */
  } s;
  Align x;			/* force alignment of blocks */
};

typedef union header Header;

static Header base;		/* empty list to get started */
static Header *freep = NULL;	/* start of free list */

void *krmalloc (unsigned);
static Header *morecore (unsigned);
void krfree (void *);
void printFreeList ();
void printFreeEntry (Header *);
void printAllocEntry (Header *);
void printAllBlocks ();
int isFree (Header *);

/* malloc:  general-purpose storage allocator */
void *
krmalloc (unsigned nbytes)
{
  Header *p, *prevp;
  /* Header *morecore (unsigned); */
  unsigned nunits;

  nunits = (nbytes + sizeof (Header) - 1) / sizeof (Header) + 1;
  if ((prevp = freep) == NULL)
    {				/* no free list yet */
      base.s.ptr = freep = prevp = &base;
      base.s.size = 0;
    }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {
      if (p->s.size >= nunits)
	{			/* big enough */
	  if (p->s.size == nunits)
	    {			/* exactly */
	      prevp->s.ptr = p->s.ptr;
	    }
	  else
	    {			/* allocate tail end */
	      p->s.size -= nunits;
	      p += p->s.size;
	      p->s.size = nunits;
	    }
	  freep = prevp;
	  return (void *) (p + 1);
	}
      if (p == freep)
	{			/* wrapped around free list */
	  if ((p = morecore (nunits)) == NULL)
	    {
	      return NULL;	/* none left */
	    }
	}
    }
}

#define NALLOC 128		/* minimum #units to request */

Header *firstMoreCoreAddr = NULL;
Header *lastMoreCoreAddr = NULL;

/* morecore:  ask system for more memory */
static Header *
morecore (unsigned nu)
{
  char *cp, *sbrk (int);
  Header *up;

  if (nu < NALLOC)
    {
      nu = NALLOC;
    }
  cp = sbrk (nu * sizeof (Header));
  if (firstMoreCoreAddr == NULL)
    {
      firstMoreCoreAddr = (Header *) cp;
    }
  lastMoreCoreAddr = (Header *) (cp + nu * sizeof (Header) - 1);

  if (cp == (char *) -1)
    {				/* no space at all */
      return NULL;
    }
  up = (Header *) cp;
  up->s.size = nu;
  krfree ((void *) (up + 1));
  return freep;
}

/* free:  put block ap in free list */
void
krfree (void *ap)
{
  Header *bp, *p;

  bp = (Header *) ap - 1;	/* point to  block header */
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    {
      if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
	{
	  break;		/* freed block at start or end of arena */
	}
    }

  if (bp + bp->s.size == p->s.ptr)
    {				/* join to upper nbr */
      bp->s.size += p->s.ptr->s.size;
      bp->s.ptr = p->s.ptr->s.ptr;
    }
  else
    {
      bp->s.ptr = p->s.ptr;
    }
  if (p + p->s.size == bp)
    {				/* join to lower nbr */
      p->s.size += bp->s.size;
      p->s.ptr = bp->s.ptr;
    }
  else
    {
      p->s.ptr = bp;
    }
  freep = p;
}

/* Print information about the free blocks. */
void
printFreeList ()
{
  Header *p;
  printf ("Free blocks:\n");
  p = freep;
  do
    {
      printFreeEntry (p);
      p = p->s.ptr;
    }
  while (p != freep);
}

void
printFreeEntry (Header * p)
{
  printf ("\t(free) addr = %p, size = %u, next = %p\n", p, p->s.size,
	  p->s.ptr);
}

void
printAllocEntry (Header * p)
{
  printf ("\t(alloc) addr = %p, size = %u\n", p, p->s.size);
}

void
printAllBlocks ()
{
  Header *p;
  printf ("All blocks:\n");
  for (p = firstMoreCoreAddr; p < lastMoreCoreAddr; p += p->s.size)
    {
      if (isFree (p))
	{
	  printFreeEntry (p);
	}
      else
	{
	  printAllocEntry (p);
	}
    }
  printf ("\n");
}

int
isFree (Header * toFind)
{
  Header *p;
  for (p = base.s.ptr; p != &base; p = p->s.ptr)
    {
      if (p == toFind)
	{
	  return 1;
	}
    }
  return 0;
}

int
main ()
{
  char cmd[2];
  int arg;
  char *ptr;
  char *p;
  int blockNum = 0;

  while (1)
    {
      /* read commands of the form "a #", in order to allocate the given
       * number of bytes, or "f ptr", in order to free the space pointed to
       * by the given pointer. 
       */
      printf ("Command? ");
      if (scanf ("%1s", cmd) == EOF)
	{
	  return 0;
	}
      if (cmd[0] == 'a')
	{
	  scanf ("%d", &arg);
	  blockNum++;
	  ptr = (char *) krmalloc (arg);
	  if (ptr == NULL)
	    {
	      printf ("Can't allocate!\n");
	    }
	  else
	    {
	      /* write a bunch of identifying stuff 
	         to the memory just acquired */
	      for (p = ptr; p < ptr + arg / sizeof (int); p++)
		{
		  *p = '0' + blockNum;
		}
	      printf ("address returned by malloc = %p\n", ptr);
	    }
	}
      else if (cmd[0] == 'f')
	{
	  scanf ("%x", &arg);
	  krfree ((void *) arg);
	}
      else
	{
	  printf ("ASCII %x is not a legal command.\n", cmd[0]);
	}
      printFreeList ();
      printf ("--------------------------\n");
      printAllBlocks ();
    }
}
