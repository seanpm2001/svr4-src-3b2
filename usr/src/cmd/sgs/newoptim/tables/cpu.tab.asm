/*#ident	"@(#)newoptim:tables/cpu.tab.asm	1.4"*/

{"EXTOP", (CPUOPC), OLOWER, OLOWER, NOFLAG, NOFLAG, TBYTE, TNONE, TNONE, TNONE,0, NOFLAG, NOFLAG, NOFLAG, NOFLAG}, 
{"SPOP", (CPUOPC|SPOPC), OLOWER, OLOWER, NOFLAG, NOFLAG, TWORD, TNONE, TNONE, TNONE,0, NOFLAG, NOFLAG, NOFLAG, NOFLAG}, 
{"SPOPD2", (CPUOPC|SPOPC), OLOWER, SPOPS2, NOFLAG, NOFLAG, TWORD, T2WORD, T2WORD, TNONE,0, OOPR1, OOPR2, NOFLAG, NOFLAG}, 
{"SPOPRD", (CPUOPC|SPOPC), OLOWER, SPOPRS, NOFLAG, NOFLAG, TWORD, T2WORD, TNONE, TNONE,0, OOPR1, NOFLAG, NOFLAG, NOFLAG}, 
{"SPOPRS", (CPUOPC|SPOPC), OLOWER, OLOWER, NOFLAG, NOFLAG, TWORD, T1WORD, TNONE, TNONE,0, OOPR1, NOFLAG, NOFLAG, NOFLAG}, 
{"SPOPRT", (CPUOPC|SPOPC), OLOWER, SPOPRD, NOFLAG, NOFLAG, TWORD, T3WORD, TNONE, TNONE,0, OOPR1, NOFLAG, NOFLAG, NOFLAG}, 
{"SPOPS2", (CPUOPC|SPOPC), OLOWER, OLOWER, NOFLAG, NOFLAG, TWORD, T1WORD, T1WORD, TNONE,0, OOPR1, OOPR2, NOFLAG, NOFLAG}, 
{"SPOPT2", (CPUOPC|SPOPC), OLOWER, SPOPD2, NOFLAG, NOFLAG, TWORD, T3WORD, T3WORD, TNONE,0, OOPR1, OOPR2, NOFLAG, NOFLAG}, 
{"SPOPWD", (CPUOPC|SPOPC), OLOWER, SPOPWS, NOFLAG, NOFLAG, TWORD, T2WORD, TNONE, TNONE,0, NOFLAG, OOPR1, NOFLAG, NOFLAG}, 
{"SPOPWS", (CPUOPC|SPOPC), OLOWER, OLOWER, NOFLAG, NOFLAG, TWORD, T1WORD, TNONE, TNONE,0, NOFLAG, OOPR1, NOFLAG, NOFLAG}, 
{"SPOPWT", (CPUOPC|SPOPC), OLOWER, SPOPWD, NOFLAG, NOFLAG, TWORD, T3WORD, TNONE, TNONE,0, NOFLAG, OOPR1, NOFLAG, NOFLAG}, 
