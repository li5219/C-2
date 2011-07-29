#include <unistd.h>
#include <string.h>
#include "malloc.h"

#ifndef STRATEGY
#define STRATEGY 4
#endif

#ifndef NRQUICKLISTS
#define NRQUICKLISTS 5
#endif

#if STRATEGY == 2
#define CRITERIA before->s.ptr->s.size < nunits||(before->s.ptr->s.size > p->s.size && p->s.size >= nunits)
#elif STRATEGY == 3
#define CRITERIA before->s.ptr->s.size < p->s.size
#endif


static Header base;       /* empty list to get started */
static Header *freep = NULL;     /* start of free list */

#if STRATEGY == 4
static Header quick[NRQUICKLISTS] = {{{NULL}}}; /*empty lists for fixed size blocks, initiated to 0*/
Header *getblock(int);
void freeblock(Header*, int);

/* getblock: get memory block of fixed size */
Header *getblock(int page)
{
	char * cp;
	Header *p;
	unsigned nunits;
	nunits = 2<<page;

	if(quick[page].s.ptr == NULL)
	{
		/* no free list yet, create empty list */
		quick[page].s.ptr =  quick+page;
		quick[page].s.size = 0;
	};

	if(quick[page].s.ptr == quick+page)
	{ 	/* we're out of free blocks, fetch some more*/
		cp = (char *)sbrk(getpagesize());
		if(cp == (char *) -1)
			return(NULL); /* no space at all */ 

		p = (Header *) cp;
		quick[page].s.ptr = p; /* point to the newly created block*/
		int i;
		for(i = (getpagesize()/sizeof(Header))/nunits; i>0; i--, p+=nunits){ /* slice it up into smaller blocks of fixed size*/
			p->s.size = nunits;
			p->s.ptr = p+nunits;
		}
		p -= nunits; /* last jump took us outside of our memory, go back one and link it to the head */
		p->s.ptr = quick+page;
	}
	p = quick[page].s.ptr; /* unlink the first entry after the head and return it*/
	quick[page].s.ptr = quick[page].s.ptr->s.ptr;
	return p;
}

/* getblock: free memory block of fixed size into a specified list*/
void freeblock(Header* bp, int page)
{
	if(bp == NULL)
		return;

	bp->s.ptr = quick[page].s.ptr;
	quick[page].s.ptr = bp;
}

#endif

/* malloc: general-purpose storage allocator */
void *malloc(unsigned nbytes)
{
	if(nbytes==0)
		return(NULL);

	Header *p, *prevp;
	unsigned nunits;
	nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
	
	#if STRATEGY == 4
	int page;
	for(page = 0; (2<<page) < nunits; page++); /* Check to see if there is a list for blocks of this size*/

	if(page<NRQUICKLISTS)
		return ((void *)(getblock(page)+1)); /*Get a block of fixed size*/
	#endif
	
	if ((prevp = freep) == NULL) {   /* no free list yet */
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	
	#ifdef CRITERIA /* For best and worst fit */
	Header *before;
	before = prevp;
	for (p = prevp->s.ptr; p != freep; prevp = p, p = p->s.ptr) /* iterate over all entries, find an apropriate entry, but remember its parent */
		if(CRITERIA) before = prevp; /* best match so far? */
	if(CRITERIA) before = prevp; /* previous loop misses last element */
		
	if(before->s.ptr->s.size < nunits) { /* best match not a fit */
		if ((prevp = morecore(nunits)) == NULL) /* create new block */
			return NULL;	/* none left */
		for (p = prevp->s.ptr; p != freep; prevp = p, p = p->s.ptr) /* now check again, there will always be a fit now*/
			if(CRITERIA) before = prevp;
		if(CRITERIA) before = prevp;
	}
	
	prevp = before;
	p = prevp->s.ptr;
	
	if (p->s.size == nunits) /* match is exact size */
		prevp->s.ptr = p->s.ptr;
	else {			  /* allocate tail end */
		p->s.size -= nunits;
		p += p->s.size;
		p->s.size = nunits;
	}
	freep = prevp;
	return (void *)(p+1);
	#else /* For first fit and quick fit where no fixed block size is used */
	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) { /* iterate over list items until match is found*/
		if (p->s.size >= nunits) { /* big enough */
			if (p->s.size == nunits) /* match is exact size */
				prevp->s.ptr = p->s.ptr;
			else {			  /* allocate tail end */
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			return (void *)(p+1);
		}
		if (p == freep) /* wrapped around free list, fetch more memory */
			if ((p = morecore(nunits)) == NULL)
				return NULL;	/* none left */
	}
	#endif
}

/* realloc: resize an allocated block to new size */
void *realloc(void *old, unsigned nbytes) {
	if(old==NULL) return malloc(nbytes);
	Header *p, *prevp;
	unsigned nunits;
	int diff;
	p = (Header *)old - 1;
	nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;
	#if STRATEGY == 4
	int page;
	for(page = 0; (2<<page) < nunits; page++); /* Check to see if there is a list for blocks of this size*/
	if(page<NRQUICKLISTS) nunits = 2<<page; /*New size is a fixed block size*/
	#endif
	diff = nunits - p->s.size;
	if(diff==0) return old; /*Same size, do nothing*/
	if(diff < 0) {	/*Smaller size, resize the block and free the rest*/
		prevp = p;
		prevp->s.size = nunits;
		p += nunits;
		p->s.size = -diff;
		free(p + 1);
		return old;
	}
	/* Larger size, we need to allocate more memory */
	prevp = p;
	p = (Header *)malloc(nbytes) - 1; /* get new block of proper size */
	memcpy(p+1, prevp+1, sizeof(Header)*(prevp->s.size -1)); /* copy the old data */
	free(prevp+1); /* free the old block */
	return p+1; /* return the new one */
}

#define NALLOC  1024   /* minimum #units to request */
/* morecore: ask system for more memory, used for blocks of non-fixed size */
static Header *morecore(unsigned nu) {
	char *cp;
	Header *up;
	if (nu < NALLOC) nu = NALLOC;
	cp = (char *)sbrk(nu * sizeof(Header));
	/* no space at all */ 
	if (cp == (char *) -1) return NULL;
	up = (Header *) cp;
	up->s.size = nu;
	free((void *)(up+1));
	return freep;
}

/* free: put block ap in free list */
void free(void *ap) {
	if(ap == NULL) return;
	Header *bp, *p;
	bp = (Header *)ap - 1;	/* point to block header */
	
	#if STRATEGY == 4
	int page;
	for(page = 0; (2<<page) < bp->s.size; page++); /* check if there is a list for blocks of this size */
	if(page<NRQUICKLISTS && 2<<page == bp->s.size) { /* free fixed size block */
		freeblock(bp, page);
		return;
	}
	#endif
		
	for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr) /* find place in list for block */
		 if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			 break; /* freed block at start or end of arena */
	if (bp + bp->s.size == p->s.ptr) {	/* join to upper nbr */
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	}
	else bp->s.ptr = p->s.ptr;
	if (p + p->s.size == bp) {			/* join to lower nbr */
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}
	else p->s.ptr = bp;
	freep = p;
}
