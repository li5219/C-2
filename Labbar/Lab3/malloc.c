#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define FIRST_FIT 1
#define BEST_FIT  2
#define WORST_FIT 3
#define QUICK_FIT 4


typedef long Align;

union header
{
  struct
  {
    union header *ptr;
    unsigned size;
  } s;
  Align x;
};

typedef union header Header;
const int NALLOC = 1024;
static Header base;
static Header *freep = NULL;
static Header *morecore (unsigned);

/* /////////////////////////////////////////////////////// */
void *malloc (size_t nbytes)
/* ////////////////////////////////////////////////////// */
{
  Header *p, *prevp, *cmp_p, *cmp_prevp;
  unsigned nunits;

  if (nbytes <= 0)
    return NULL;

  nunits = (nbytes + sizeof (Header) - 1) / sizeof (Header) + 1;

  if ((prevp = freep) == NULL)
    {
      base.s.ptr = freep = prevp = &base;
      base.s.size = 0;
    }

  cmp_p = NULL;
  cmp_prevp = NULL;

  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr)
    {

      if (STRATEGY == FIRST_FIT)
	{
	  if (p->s.size >= nunits)
	    break;
	}			/* end first fit */

      if (STRATEGY == BEST_FIT)
	{
	  if (p->s.size >= nunits)
	    {
	      if (cmp_p == NULL)
		{
		  cmp_p = p;
		  cmp_prevp = prevp;
		}
	      else
		{
		  if (p->s.size < cmp_p->s.size)
		    {
		      cmp_p = p;
		      cmp_prevp = prevp;
		    }
		}
	    }
	  if (p == freep && cmp_p != NULL)
	    {
	      p = cmp_p;
	      prevp = cmp_prevp;
	      break;
	    }
	}			/* end best_fit */


      if (STRATEGY == WORST_FIT)
	{
	  if (p->s.size >= nunits)
	    {
	      if (cmp_p == NULL)
		{
		  cmp_p = p;
		  cmp_prevp = prevp;
		}
	      else
		{
		  if (p->s.size > cmp_p->s.size)
		    {
		      cmp_p = p;
		      cmp_prevp = prevp;
		    }
		}
	    }
	  if (p == freep && cmp_p != NULL)
	    {
	      p = cmp_p;
	      prevp = cmp_prevp;
	      break;
	    }
	}			/* end worst_fit */

      if (p == freep)
	if ((p = morecore (nunits)) == NULL)
	  return NULL;
    }				/*end for */

  if (p->s.size == nunits)
    prevp->s.ptr = p->s.ptr;
  else
    {
      p->s.size -= nunits;
      p += p->s.size;
      p->s.size = nunits;
    }
  freep = prevp;
  return (void *) (p + 1);
}

/* ///////////////////////////////////////////////////// */
static Header *morecore (unsigned nu)
/* ///////////////////////////////////////////////////// */
{
  char *cp;
  Header *up;

  if (nu < NALLOC)
    nu = NALLOC;
  cp = sbrk (nu * sizeof (Header));
  if (cp == (char *) -1)
    return NULL;
  up = (Header *) cp;

  up->s.size = nu;
  free ((void *) (up + 1));
  return freep;

}

/* //////////////////////////////////////////// */
void free (void *ap)
/* ///////////////////////////////////////////// */
{
  Header *bp, *p;

  if (ap == NULL)
    return;

  bp = (Header *) ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;

  if (bp + bp->s.size == p->s.ptr)	/* Join to upper nbr */
    {
      bp->s.size += p->s.ptr->s.size;
      bp->s.ptr = p->s.ptr->s.ptr;
    }
  else
    bp->s.ptr = p->s.ptr;

  if (p + p->s.size == bp)		/* Join to lower nbr */
    {
      p->s.size += p->s.ptr->s.size;
      p->s.ptr = p->s.ptr->s.ptr;
    }
  else
    p->s.ptr = bp;

  freep = p;

}

/* ///////////////////////////////////////////// */
void *realloc (void *ptr, size_t new_size)
{
  Header *h_ptr;
  int copy_size;
  void *new_ptr;

  h_ptr = (Header *) ptr - 1;

  if (ptr == NULL)
    return malloc (new_size);

  if (new_size == 0)
    {
      free (ptr);
      return NULL;
    }
  copy_size = (h_ptr->s.size - 1) * sizeof (Header);

  if (new_size < copy_size)
    copy_size = new_size;

/*	printf(" antal kopierade bytes %d \n",copy_size);	*/

  new_ptr = malloc (new_size);
  memcpy (new_ptr, ptr, copy_size);
  free (ptr);

  return new_ptr;

}
