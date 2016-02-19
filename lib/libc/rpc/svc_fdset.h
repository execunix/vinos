/*	$NetBSD: svc_fdset.h,v 1.1 2013/03/05 19:55:23 christos Exp $	*/

#ifndef _LIBC

void init_fdsets(void);
void alloc_fdset(void);
fd_set *get_fdset(void);
int *get_fdsetmax(void);

#else
# define	get_fdset()	(&svc_fdset)
# define	get_fdsetmax()	(&svc_maxfd)
#endif
