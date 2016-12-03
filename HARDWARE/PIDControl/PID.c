#include"PID.h"

void IncPIDInit(void)
{
sptr->SumError=0;
sptr->LastError=0;//Error[-1]
sptr->PrevError=0;//Error[-2]
sptr->Proportion=0;//
sptr->Integral=0;//
sptr->Derivative=0;//
sptr->SetPoint=0;
}



int IncPIDCalc(int NextPoint)
{
 register int iError, iIncpid;          
 iError=sptr->SetPoint-NextPoint; 
 iIncpid=sptr->Proportion*iError
 -sptr->Integral*sptr->LastError
 +sptr->Derivative*sptr->PrevError; 
 sptr->PrevError=sptr->LastError;  
 sptr->LastError=iError;     
 return(iIncpid);
}


unsigned int LocPIDCalc(int NextPoint)
{
register int iError,dError;
iError=sptr->SetPoint - NextPoint;
 sptr->SumError += iError;       
 dError = iError - sptr->LastError; 
 sptr->LastError = iError;
 return(sptr->Proportion*iError 
 +sptr->Integral*sptr->SumError
 +sptr->Derivative*dError);
}






