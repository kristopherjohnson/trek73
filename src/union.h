#ident "@(#) TREK73 $Header: /home/ncvs/trek73/src/union.h,v 1.1.1.1 1999/10/26 17:07:14 dillon Exp $"
/*
 * $Source: /home/ncvs/trek73/src/union.h,v $
 *
 * $Header: /home/ncvs/trek73/src/union.h,v 1.1.1.1 1999/10/26 17:07:14 dillon Exp $
 *
 * $Log: union.h,v $
 * Revision 1.1.1.1  1999/10/26 17:07:14  dillon
 *
 * Nov 1988 import of trek73 from comp.sources.usenet.  This is the trek73
 * that was originally ported from basic to C and then further modified by
 * Jeff, Peter, Matt, and a few others in 1985.  Jeff later made additional
 * changes which led to the 1988 posting.
 *
 *
 *
 * Revision 1.1  87/10/09  11:14:09  11:14:09  okamoto (Jeff Okamoto)
 * Initial revision
 * 
 */
/*
 * TREK73: union.h
 *
 * Union for yacc
 */
typedef union {
	double dval;
	char sval[20];
} YYSTYPE;
