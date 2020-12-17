
/* @(#)fdlibm.h 1.5 04/04/22 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* Sometimes it's necessary to define __LITTLE_ENDIAN explicitly
   but these catch some common cases. */

/**
 * Org:   Nankai University
 * Name:  Liu Zhuang
 * Email: 3331353075@qq.com
 * Date； 2020/12/11
 * 
 * 对fdlibm.h做一定的修改，根据ARM的情况删除不需要的宏定义
 * 删除不需要的算术函数，仅保留sqrt和exp。
 */

/*ARM默认使用小端存储*/
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN
#endif

#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1+(int*)x)
#endif

/*使用标准库函数的声明方式*/
#ifndef __STDC__
#define __STDC__
#endif

#ifndef __P
#ifdef __STDC__
#define	__P(p)	p
#else
#define	__P(p)	()
#endif
#endif

#define	MAXFLOAT	((float)3.40282346638528860e+38)

struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define	HUGE		MAXFLOAT

/* 
 * set X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define X_TLOSS		1.41484755040568800000e+16 

#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

/* ieee style elementary functions */
extern double __ieee754_sqrt __P((double));		
extern double __ieee754_exp __P((double));
