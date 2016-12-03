#ifndef PID_H
#define PID_H
#include "sys.h"
typedef struct PID{
 int SetPoint;        
 long SumError;
 double Proportion;
 double Integral;
 double Derivative;
 int LastError; //Error[-1] 
 int PrevError;//Error[-2] 
 }PID;

static PID sPID;
static PID *sptr=&sPID;
void IncPIDInit(void);
int IncPIDCalc(int NextPoint);
unsigned int LocPIDCalc(int NextPoint);
#endif
