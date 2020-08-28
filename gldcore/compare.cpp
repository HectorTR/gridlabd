/* compare.cpp
 * Copyright (C) 2008, Battelle Memorial Institute
 */

#include "gldcore.h"

#define COMPAREOPI(T) COMPARE_EQI(T) COMPARE_LEI(T) COMPARE_GEI(T) COMPARE_NEI(T) COMPARE_LTI(T) COMPARE_GTI(T) COMPARE_INI(T) COMPARE_NII(T)
#define COMPARE_EQI(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return (signed)*(T*)x==(signed)*(T*)a; }
#define COMPARE_LEI(T) int compare_tc_##T##_le(void* x,void* a,void* b) { return (signed)*(T*)x<=(signed)*(T*)a; }
#define COMPARE_GEI(T) int compare_tc_##T##_ge(void* x,void* a,void* b) { return (signed)*(T*)x>=(signed)*(T*)a; }
#define COMPARE_NEI(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return (signed)*(T*)x!=(signed)*(T*)a; }
#define COMPARE_LTI(T) int compare_tc_##T##_lt(void* x,void* a,void* b) { return (signed)*(T*)x<(signed)*(T*)a; }
#define COMPARE_GTI(T) int compare_tc_##T##_gt(void* x,void* a,void* b) { return (signed)*(T*)x>(signed)*(T*)a; }
#define COMPARE_INI(T) int compare_tc_##T##_in(void* x,void* a,void* b) { return (signed)*(T*)a<=(signed)*(T*)x && b!=NULL && (signed)*(T*)x<=(signed)*(T*)b; }
#define COMPARE_NII(T) int compare_tc_##T##_ni(void* x,void* a,void* b) { return !((signed)*(T*)a<=(signed)*(T*)x && b!=NULL && (signed)*(T*)x<=(signed)*(T*)b); }

#define COMPAREOPB(T) COMPARE_EQB(T) COMPARE_NEB(T)
#define COMPARE_EQB(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return *(T*)x==*(T*)a; }
#define COMPARE_NEB(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return *(T*)x!=*(T*)a; }

#define COMPAREOPO(T) COMPARE_EQO(T) COMPARE_NEO(T)
//#define COMPARE_EQO(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return strcmp((*(OBJECT**)x)->name,(*(OBJECT**)a)->name)==0; }
//#define COMPARE_NEO(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return strcmp((*(OBJECT**)x)->name,(*(OBJECT**)a)->name)!=0; }
#define COMPARE_EQO(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return (*(OBJECT**)x)==(*(OBJECT**)a); }
#define COMPARE_NEO(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return (*(OBJECT**)x)!=(*(OBJECT**)a); }

#define COMPAREOPF(T) COMPARE_EQF(T) COMPARE_LEF(T) COMPARE_GEF(T) COMPARE_NEF(T) COMPARE_LTF(T) COMPARE_GTF(T) COMPARE_INF(T) COMPARE_NIF(T)
#define COMPARE_EQF(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { if (b==NULL) return *(T*)x==*(T*)a; else return fabs((*(T*)x)-(*(T*)a))<=(*(T*)b); }
#define COMPARE_LEF(T) int compare_tc_##T##_le(void* x,void* a,void* b) { return *(T*)x<=*(T*)a; }
#define COMPARE_GEF(T) int compare_tc_##T##_ge(void* x,void* a,void* b) { return *(T*)x>=*(T*)a; }
#define COMPARE_NEF(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { if (b==NULL) return *(T*)x!=*(T*)a; else return fabs((*(T*)x)-(*(T*)a))>(*(T*)b); }
#define COMPARE_LTF(T) int compare_tc_##T##_lt(void* x,void* a,void* b) { return *(T*)x<*(T*)a; }
#define COMPARE_GTF(T) int compare_tc_##T##_gt(void* x,void* a,void* b) { return *(T*)x>*(T*)a; }
#define COMPARE_INF(T) int compare_tc_##T##_in(void* x,void* a,void* b) { return *(T*)a<=*(T*)x && b!=NULL && *(T*)x<=*(T*)b; }
#define COMPARE_NIF(T) int compare_tc_##T##_ni(void* x,void* a,void* b) { return !(*(T*)a<=*(T*)x && b!=NULL && *(T*)x<=*(T*)b); }

#define COMPAREOPS(T) COMPARE_SEQ(T) COMPARE_SLE(T) COMPARE_SGE(T) COMPARE_SNE(T) COMPARE_SLT(T) COMPARE_SGT(T) COMPARE_SIN(T) COMPARE_SNI(T)
#define COMPARE_SEQ(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)==0; }
#define COMPARE_SLE(T) int compare_tc_##T##_le(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)!=1; }
#define COMPARE_SGE(T) int compare_tc_##T##_ge(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)!=-1; }
#define COMPARE_SNE(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)!=0; }
#define COMPARE_SLT(T) int compare_tc_##T##_lt(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)==-1; }
#define COMPARE_SGT(T) int compare_tc_##T##_gt(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)==1; }
#define COMPARE_SIN(T) int compare_tc_##T##_in(void* x,void* a,void* b) { return strcmp((char*)x,(char*)a)!=-1 && b!=NULL && strcmp((char*)x,(char*)b)!=1; }
#define COMPARE_SNI(T) int compare_tc_##T##_ni(void* x,void* a,void* b) { return !(strcmp((char*)x,(char*)a)!=-1 && b!=NULL && strcmp((char*)x,(char*)b)!=1); }

#define COMPAREOPC(T) COMPARE_CEQ(T) COMPARE_CLE(T) COMPARE_CGE(T) COMPARE_CNE(T) COMPARE_CLT(T) COMPARE_CGT(T) COMPARE_CIN(T) COMPARE_CNI(T)
#define COMPARE_CEQ(T) int compare_tc_##T##_eq(void* x,void* a,void* b) { return *(varchar*)x == (char*)a; }
#define COMPARE_CLE(T) int compare_tc_##T##_le(void* x,void* a,void* b) { return *(varchar*)x <= (char*)a; }
#define COMPARE_CGE(T) int compare_tc_##T##_ge(void* x,void* a,void* b) { return *(varchar*)x >= (char*)a; }
#define COMPARE_CNE(T) int compare_tc_##T##_ne(void* x,void* a,void* b) { return *(varchar*)x != (char*)a; }
#define COMPARE_CLT(T) int compare_tc_##T##_lt(void* x,void* a,void* b) { return *(varchar*)x < (char*)a; }
#define COMPARE_CGT(T) int compare_tc_##T##_gt(void* x,void* a,void* b) { return *(varchar*)x > (char*)a; }
#define COMPARE_CIN(T) int compare_tc_##T##_in(void* x,void* a,void* b) { return *(varchar*)x >= (char*)a && b != NULL && *(varchar*)x <= (char*)b; }
#define COMPARE_CNI(T) int compare_tc_##T##_ni(void* x,void* a,void* b) { return !(*(varchar*)x >= (char*)a && b != NULL && *(varchar*)x <= (char*)b); }

/* basic ops */
COMPAREOPF(double)
COMPAREOPF(float)
COMPAREOPI(uint16)
COMPAREOPI(uint32)
COMPAREOPI(uint64)
COMPAREOPB(bool)
COMPAREOPS(string)
COMPAREOPO(object)
COMPAREOPC(varchar)
