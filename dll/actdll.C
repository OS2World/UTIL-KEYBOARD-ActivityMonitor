/*
 * This file was created for D.J. van Enckevort
 * by Project Smarts on 9 Jan 1999.
*/
/*
#define DEBUG
*/
#define INCL_DOS
#define INCL_KBD
#define INCL_DOSERRORS
#define INCL_WIN
#define INCL_PM
#define INCL_GPI
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <string.h>

#include "actdll.h"
#include "actdll-private.h"

/* Declare global variables */
volatile TID *kbdTID;
volatile LONG Status;
USHORT hKBD;
PROPERTIES Props;
HMTX mutexdata;
HEV eventcontinue, eventtime, eventkey, eventmouse;
HTIMER htimer;
APIRET init_result;

void APIENTRY ErrorMessage(APIRET rc, char *message, ULONG line, char *function, char *file)
{
FILE *f;
time_t ut;
struct tm *lt;

if ((f=fopen(ERRORLOG, "a"))!=NULLHANDLE)
   {
   ut=time(NULL);
   lt=localtime(&ut);
   fprintf(f, ERRORMSG, lt->tm_mday, lt->tm_mon+1, lt->tm_year+1900, lt->tm_hour, lt->tm_min, lt->tm_sec, file, line, rc, function, message);
   fclose(f);
   }
}

USHORT APIENTRY WinErrorMessage(HAB hab, char *message, ULONG line, char *function, char *file)
{
PERRINFO errorinfo;
ERRORID errorid;
char *emsg;
int i;

if ((errorinfo=WinGetErrorInfo(hab))!=NULLHANDLE)
   {
   emsg=(char*)&errorinfo->offaoffszMsg;
   emsg+=errorinfo->offaoffszMsg;
   errorid=errorinfo->idError;
   if (ERRORIDSEV(errorid)!=SEVERITY_NOERROR)
      {
      ErrorMessage(errorid, message, line, function, file);
      for (i=0; i<(errorinfo->cDetailLevel); i++)
         {
         emsg=(char*)&errorinfo->offaoffszMsg;
         emsg+=errorinfo->offaoffszMsg+(2*i);
         ErrorMessage(errorid, emsg, line, function, file);
         }
      }
   WinFreeErrorInfo(errorinfo);
   }
return ERRORIDSEV(errorid);
}

BOOL EXPENTRY MsgInputHook(HAB hab, PQMSG pqmsg, ULONG fs)
{
static USHORT mousex, mousey;
HEV mouse, keyboard, cont;
APIRET rc;

mouse=0;
keyboard=0;
cont=0;
if ((rc=DosOpenEventSem(SEM_MOUSE, &mouse))!=NO_ERROR)
   {
/*   ErrorMsg(rc, "DosOpenEventSem(SEM_MOUSE, &mouse)");
*/   return FALSE;
   }
if ((rc=DosOpenEventSem(SEM_KEYBOARD, &keyboard))!=NO_ERROR)
   {
/*   ErrorMsg(rc, "DosOpenEventSem(SEM_KEYBOARD, &keyboard)");
*/   return FALSE;
   }
if ((rc=DosOpenEventSem(SEM_CONTINUE, &cont))!=NO_ERROR)
   {
/*   ErrorMsg(rc, "DosOpenEventSem(SEM_CONTINUE, &cont)");
*/   return FALSE;
   }
if ((rc=DosWaitEventSem(cont, SEM_IMMEDIATE_RETURN))==NO_ERROR)
   {
   switch (pqmsg->msg)
      {
      case WM_CHAR:
         {
         DosPostEventSem(keyboard);
         break;
         }
      case WM_BUTTON1DOWN:
         {
         DosPostEventSem(mouse);
         break;
         }
      case WM_BUTTON2DOWN:
         {
         DosPostEventSem(mouse);
         break;
         }
      case WM_BUTTON3DOWN:
         {
         DosPostEventSem(mouse);
         break;
         }
      case WM_MOUSEMOVE:
         {
         if (SHORT1FROMMP(pqmsg->mp1)!=mousex || SHORT2FROMMP(pqmsg->mp1)!=mousey)
            {
            DosPostEventSem(mouse);
            mousex=SHORT1FROMMP(pqmsg->mp1);
            mousey=SHORT2FROMMP(pqmsg->mp1);
            }
         break;
         }
      default:
         {
         break;
         }
      }
   }
/*else
   {
   ErrorMsg(rc, "DosWaitEventSem(cont, SEM_IMMEDIATE_RETURN)");
   }
*/if ((rc=DosCloseEventSem(keyboard))!=NO_ERROR)
   {
/*   ErrorMsg(rc, "DosCloseEventSem(keyboard)");
*/   }
if ((rc=DosCloseEventSem(mouse))!=NO_ERROR)
   {
/*   ErrorMsg(rc, "DosCloseEventSem(mouse)");
*/   }
if ((rc=DosCloseEventSem(cont))!=NO_ERROR);
   {
/*   ErrorMsg(rc, "DosCloseEventSem(cont))!=NO_ERROR");
*/   }
return FALSE;
}

BOOL EXPENTRY ResetStatus(void)
{
APIRET rc;

if ((rc=DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL))==NO_ERROR)
   {
   Status=0;
   if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
      {
      ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
      }
   return TRUE;
   }
else if (rc!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL)");
   }
return FALSE;
}

LONG EXPENTRY GetStatus(void)
{
LONG result;
APIRET rc;

result=UNKNOWN_STATUS;
if ((rc=DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL))==NO_ERROR)
   {
   result=Status;
   if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
      {
      ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
      }
   }
else if (rc!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL)");
   }
return result;
}

BOOL EXPENTRY SetProperties(PPROPERTIES NewProps)
{
APIRET rc;

if ((rc=DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL))==NO_ERROR)
   {
   Props.KeyLow=NewProps->KeyLow;
   Props.KeyHigh=NewProps->KeyHigh;
   Props.MouseLow=NewProps->MouseLow;
   Props.MouseHigh=NewProps->MouseHigh;
   Props.StatusLow=NewProps->StatusLow;
   Props.StatusHigh=NewProps->StatusHigh;
   Props.Beep=NewProps->Beep;
   Props.Log=NewProps->Log;
   strcpy((unsigned char *)Props.ComputerID, (unsigned char *)NewProps->ComputerID);
   strcpy((unsigned char *)Props.LogFileName, (unsigned char *)NewProps->LogFileName);
   strcpy((unsigned char *)Props.Viewer, (unsigned char *)NewProps->Viewer);
   if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
      {
      ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
      }
   return TRUE;
   }
else if (rc!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL)");
   }
return FALSE;
}

BOOL EXPENTRY GetProperties(PPROPERTIES ThisProps)
{
APIRET rc;
if ((rc=DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL))==NO_ERROR)
   {
   ThisProps->KeyLow=Props.KeyLow;
   ThisProps->KeyHigh=Props.KeyHigh;
   ThisProps->MouseLow=Props.MouseLow;
   ThisProps->MouseHigh=Props.MouseHigh;
   ThisProps->StatusLow=Props.StatusLow;
   ThisProps->StatusHigh=Props.StatusHigh;
   ThisProps->Beep=Props.Beep;
   ThisProps->Log=Props.Log;
   strcpy((unsigned char *)ThisProps->ComputerID, (unsigned char *)Props.ComputerID);
   strcpy((unsigned char *)ThisProps->LogFileName, (unsigned char *)Props.LogFileName);
   strcpy((unsigned char *)ThisProps->Viewer, (unsigned char *)Props.Viewer);
   if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
      {
      ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
      }
   return TRUE;
   }
else if (rc!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL)");
   }
return FALSE;
}

void EXPENTRY Evaluate(HEV redrawevent)
{
time_t T;
struct tm *LT;
FILE *f;
ULONG postcount;
LONG keyboard, mouse;
APIRET rc, result;

result=DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN);
while ((result=DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN))==NO_ERROR)
   {
   if ((rc=DosWaitEventSem(eventtime, 60000))!=NO_ERROR)
      {
      if (rc!=ERROR_TIMEOUT)
         { /* the semaphore is not valid anymore, we should exit this thread */
         ErrorMsg(rc, "DosCloseEventSem(cont)");
         DosExit(EXIT_THREAD, rc);
         }
      }
   if ((result=DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN))==NO_ERROR)
      {
      if ((rc=DosResetEventSem(eventtime, &postcount))!=NO_ERROR)
         {
         if (rc!=ERROR_ALREADY_RESET)
            { /* The semaphore is not valid anymore, we should exit this thread */
            ErrorMsg(rc, "DosResetEventSem(eventtime, &postcount)");
            DosExit(EXIT_THREAD, rc);
            }
         }
      /* Query date & time */
      /* Convert date & time to locale */
      T=time(NULL);
      LT=localtime(&T);
      if ((rc=DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL))==NO_ERROR)
         {
         if ((rc=DosResetEventSem(eventkey, (PULONG)&keyboard))!=NO_ERROR)
            {
            if (rc!=ERROR_ALREADY_RESET)
               {
               ErrorMsg(rc, "DosResetEventSem(eventkey, (PULONG)&keyboard)");
               keyboard=-1;
               }
            }
         if ((rc=DosResetEventSem(eventmouse, (PULONG)&mouse))!=NO_ERROR)
            {
            if (rc!=ERROR_ALREADY_RESET)
               {
               ErrorMsg(rc, "DosResetEventSem(eventmouse, (PULONG)&mouse)");
               mouse=-1;
               }
            }
         /* Calculate penalty */
         if (keyboard==0)
            {
            if (Status>2)
               Status-=2;
            else
               Status=0;
            }
         else if (keyboard<Props.KeyLow)
            Status+=1;
         else if (keyboard<Props.KeyHigh)
            Status+=2;
         else
            Status+=3;

         if (mouse==0)
            {
            if (Status>2)
               Status-=2;
            else
               Status=0;
            }
         else if (mouse<Props.MouseLow)
            Status+=1;
         else if (mouse<Props.MouseHigh)
            Status+=2;
         else
            Status+=3;

         if (Props.Log==TRUE)
            {
            if ((f=fopen((unsigned char*)Props.LogFileName, "a"))!=NULLHANDLE)
               {
               /* Write log in the following format:
                  YYYYMMDD HHMMSS ComputerID KEYBOARD MOUSE STATUS
                  In the struct tm tm_year=year-1900 and tm_mon=month-1, so we have to correct for that
               */
               fprintf(f, "%04u%02u%02u %02u%02u%02u %s %i %i %i\n", LT->tm_year+1900, LT->tm_mon+1, LT->tm_mday, LT->tm_hour, LT->tm_min, LT->tm_sec, Props.ComputerID, keyboard, mouse, Status);
               fclose(f);
               }
            }

         if (Status>Props.StatusHigh && Props.Beep==TRUE)
            {
            if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
               {
               ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
               }
            /* This DosBeep() is no debugging code! */
            DosBeep(440, 1000);
            }
         else
            {
            if ((rc=DosReleaseMutexSem(mutexdata))!=NO_ERROR)
               {
               ErrorMsg(rc, "DosReleaseMutexSem(mutexdata)");
               }
            }
         }
      else if (rc!=ERROR_TIMEOUT)
         {
         ErrorMsg(rc, "DosRequestMutexSem(mutexdata, TIMEOUT_NORMAL)");
         }
      if ((rc=DosPostEventSem(redrawevent))!=NO_ERROR)
         {
         if (rc!=ERROR_ALREADY_POSTED && rc!=ERROR_TOO_MANY_POSTS)
            ErrorMsg(rc, "DosPostEventSem(redrawevent)");
         }
      }
   }
if (result!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN");
   }
}

APIRET APIENTRY KbdMonitor(USHORT session)
{
USHORT abMonIn[2];
USHORT abMonOut[2];
PBYTE pbMonData;
PMONBUFF pmbMonIn;
PMONBUFF pmbMonOut;
USHORT usSzData;
KEYPACKET mkpChar;
APIRET rc;
FILE *f;
TID tid;

rc=NO_ERROR;
abMonIn[0]=sizeof(abMonIn);
abMonOut[0]=sizeof(abMonOut);
/* Query the correct size of the input and output buffers, if we fail exit */
if ((rc=DosMonReg(hKBD, (PMONBUFF)abMonIn, (PMONBUFF)abMonOut, 0, session))!=NO_ERROR)
   {
   if (rc!=ERROR_MON_BUFFER_TOO_SMALL)
      {
      ErrorMsg(rc, "DosMonReg(hKBD, (PMONBUFF)abMonIn, (PMONBUFF)abMonOut, 0, session)");
      kbdTID[session]=0L;
      DosExit(EXIT_THREAD, rc);
      }
   }
/* Allocate memory for the input and output buffers */
if ((rc=DosAllocMem((PPVOID)&pbMonData, abMonIn[1]+abMonOut[1], PAG_COMMIT|PAG_READ|PAG_WRITE|OBJ_TILE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosAllocMem((PPVOID)&pbMonData, abMonIn[1]+abMonOut[1], PAG_COMMIT|PAG_READ|PAG_WRITE|OBJ_TILE)");
   kbdTID[session]=0L;
   DosExit(EXIT_THREAD, rc);
   }
pmbMonIn=(PMONBUFF)pbMonData;
pmbMonOut=(PMONBUFF)(((PBYTE)pbMonData)+abMonIn[1]);
pmbMonIn->cb=abMonIn[1];
pmbMonOut->cb=abMonOut[1];
/* Register the monitor buffer */
if ((rc=DosMonReg(hKBD, pmbMonIn, pmbMonOut, 0, session))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosMonReg(hKBD, pmbMonIn, pmbMonOut, 0, session)");
   DosFreeMem(pbMonData);
   kbdTID[session]=0L;
   DosExit(EXIT_THREAD, rc);
   }

while ((rc=DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN))==NO_ERROR)
   {
   usSzData=sizeof(mkpChar);
   if ((rc=DosMonRead(pmbMonIn, 0, &mkpChar, &usSzData))!=NO_ERROR)
      {
      /* Test if it is caused by the exit routine */
      if (DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN)==NO_ERROR)
         {
         /* We are still supposed to be running, log the error */
         ErrorMsg(rc, "DosMonRead(pmbMonIn, 0, &mkpChar, &usSzData)");
         }
      /* We don't exit, it might be a temporary problem */
      }
   else
      {
      /* We got data, we must write it back and post the event to get it counted */
      if ((rc=DosMonWrite(pmbMonOut, &mkpChar, usSzData))!=NO_ERROR)
         {
         ErrorMsg(rc, "DosMonWrite(pmbMonOut, &mkpChar, usSzData)");
         /* We don't exit, it might be a temporary problem */
         }
      if ((rc=DosPostEventSem(eventkey))!=NO_ERROR)
         {
         if (rc!=ERROR_ALREADY_POSTED && rc!=ERROR_TOO_MANY_POSTS)
            ErrorMsg(rc, "DosPostEventSem(eventkey)");
         /* We don't exit, it might be a temporary problem, and we only lost a count */
         }
      }
   }
if (rc!=ERROR_TIMEOUT)
   {
   ErrorMsg(rc, "DosWaitEventSem(eventcontinue, SEM_IMMEDIATE_RETURN)");
   }
if ((rc=DosFreeMem(pbMonData))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosFreeMem(pbMonData)");
   kbdTID[session]=0L;
   DosExit(EXIT_THREAD, rc);
   }
kbdTID[session]=0L;
DosExit(EXIT_THREAD, rc);
return rc;
}


APIRET APIENTRY StartKeyboardMonitors(void)
{
ULONG sessions, i;
TID tid;
APIRET rc;

rc=NO_ERROR;

if ((rc=DosMonOpen(KBD, &hKBD))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosMonOpen(KBD, &hKBD)");
   return ERROR_STARTKEYBOARDMONITORS;
   }

if ((rc=DosQuerySysInfo(QSV_MAX_TEXT_SESSIONS, QSV_MAX_TEXT_SESSIONS, &sessions, sizeof(ULONG)))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosQuerySysInfo(QSV_MAX_TEXT_SESSIONS, QSV_MAX_TEXT_SESSIONS, &sessions, sizeof(ULONG))");
   return ERROR_STARTKEYBOARDMONITORS;
   }

if ((rc=DosAllocMem((PPVOID)&kbdTID, sessions*sizeof(TID), PAG_COMMIT|PAG_READ|PAG_WRITE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosAllocMem((PPVOID)&kbdTID, sessions*sizeof(TID), PAG_COMMIT|PAG_READ|PAG_WRITE)");
   return ERROR_STARTKEYBOARDMONITORS;
   }

for (i=4; i<sessions; i++)
   {
   if ((rc=DosCreateThread(&tid, (PFNTHREAD)KbdMonitor, i, CREATE_READY, 16384))==NO_ERROR)
      {
      kbdTID[i]=tid;
      }
   else
      {
      kbdTID[i]=0L;
      ErrorMsg(rc, "DosCreateThread(&tid, (PFNTHREAD)KbdMonitor, i, CREATE_READY, 16384)");
      return ERROR_STARTKEYBOARDMONITORS;
      }
   }
return NO_ERROR;
}

APIRET APIENTRY StopKeyboardMonitors(void)
{
ULONG i, sessions;
APIRET rc, result;

rc=result=NO_ERROR;

if ((rc=DosMonClose(hKBD))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosMonClose(hKBD)");
   result=ERROR_STOPKEYBOARDMONITORS;
   /* We don't exit, but try to at least terminate the threads. */
   }

if ((rc=DosQuerySysInfo(QSV_MAX_TEXT_SESSIONS, QSV_MAX_TEXT_SESSIONS, &sessions, sizeof(ULONG)))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosQuerySysInfo(QSV_MAX_TEXT_SESSIONS, QSV_MAX_TEXT_SESSIONS, &sessions, sizeof(ULONG))");
   result=ERROR_STOPKEYBOARDMONITORS;
   }

/* Give some time to the other threads to end gracefully */
DosSleep(100);

for (i=4; i<sessions; i++)
   {
   if (kbdTID[i]!=0L)
      {
      if ((rc=DosKillThread(kbdTID[i]))!=NO_ERROR)
         {
         ErrorMsg(rc, "DosKillThread(kbdTID[i])");
         result=ERROR_STOPKEYBOARDMONITORS;
         }
      }
   }

if ((rc=DosFreeMem((PVOID)kbdTID))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosFreeMem((PVOID)kbdTID)");
   result=ERROR_STOPKEYBOARDMONITORS;
   }

return result;
}

APIRET APIENTRY StartInputHook(HAB hab)
{
APIRET rc;
USHORT sev;
HMODULE hmod;

if ((rc=DosQueryModuleHandle(DLLNAME, &hmod))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosQueryModuleHandle(DLLNAME, &hmod)");
   return ERROR_STARTINPUTHOOK;
   }
if (WinSetHook(hab, NULLHANDLE, HK_INPUT, (PFN)MsgInputHook, hmod)==FALSE)
   {
   sev=WinErrorMsg(hab, "WinSetHook(hab, NULLHANDLE, HK_INPUT, (PFN)MsgInputHook, hmod)");
   if (sev==SEVERITY_NOERROR || sev==SEVERITY_WARNING)
      return NO_ERROR;
   else
      return ERROR_STARTINPUTHOOK;
   }

return NO_ERROR;
}

APIRET APIENTRY StopInputHook(HAB hab)
{
USHORT sev;
APIRET result;
APIRET rc;
HMODULE hmod;

result=NO_ERROR;
if ((rc=DosQueryModuleHandle(DLLNAME, &hmod))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosQueryModuleHandle(DLLNAME, &hmod)");
   result=ERROR_STOPINPUTHOOK;
   }
if (WinReleaseHook(hab, NULLHANDLE, HK_INPUT, (PFN)MsgInputHook, hmod)==FALSE)
   {
   sev=WinErrorMsg(hab, "WinReleaseHook(hab, NULLHANDLE, HK_INPUT, (PFN)MsgInputHook, hmod)");
   result|=ERROR_STOPINPUTHOOK;
   }
/* Give some time to the other processes to release the DLL */
DosSleep(100);
return result;
}

APIRET APIENTRY StartSemaphores(void)
{
APIRET rc;

rc=NO_ERROR;
if ((rc=DosCreateMutexSem(NULL, &mutexdata, DC_SEM_PRIVATE, FALSE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateMutexSem(NULL, &mutexdata, DC_SEM_PRIVATE, FALSE)");
   return ERROR_STARTSEMAPHORES;
   }
if ((rc=DosCreateEventSem(SEM_CONTINUE, &eventcontinue, DC_SEM_SHARED, TRUE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(SEM_CONTINUE, &eventcontinue, DC_SEM_SHARED, TRUE)");
   return ERROR_STARTSEMAPHORES;
   }
if ((rc=DosCreateEventSem(SEM_KEYBOARD, &eventkey, DC_SEM_SHARED, TRUE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(SEM_KEYBOARD, &eventkey, DC_SEM_SHARED, TRUE)");
   return ERROR_STARTSEMAPHORES;
   }
if ((rc=DosCreateEventSem(SEM_MOUSE, &eventmouse, DC_SEM_SHARED, TRUE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(SEM_MOUSE, &eventmouse, DC_SEM_SHARED, TRUE)");
   return ERROR_STARTSEMAPHORES;
   }
return NO_ERROR;
}

APIRET APIENTRY StopSemaphores(void)
{
APIRET rc, result;

rc=result=NO_ERROR;
if ((rc=DosCloseEventSem(eventcontinue))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(eventcontinue)");
   result=ERROR_STOPSEMAPHORES;
   }
if ((rc=DosCloseEventSem(eventkey))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(eventkey)");
   result=ERROR_STOPSEMAPHORES;
   }
if ((rc=DosCloseEventSem(eventmouse))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(eventmouse)");
   result=ERROR_STOPSEMAPHORES;
   }
if ((rc=DosCloseMutexSem(mutexdata))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseMutexSem(mutexdata)");
   result=ERROR_STOPSEMAPHORES;
   }

return result;
}

APIRET APIENTRY StartTimer(void)
{
APIRET rc;

rc=NO_ERROR;
if ((rc=DosCreateEventSem(NULL, &eventtime, DC_SEM_SHARED, FALSE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(NULL, &eventtime, DC_SEM_SHARED, FALSE)");
   return ERROR_STARTTIMER;
   }
if ((rc=DosStartTimer(60000, (HSEM)eventtime, &htimer))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosStartTimer(60000, (HSEM)eventtime, &htimer)");
   return ERROR_STARTTIMER;
   }
return NO_ERROR;
}

APIRET APIENTRY StopTimer(void)
{
APIRET rc, result;

result=NO_ERROR;
if ((rc=DosStopTimer(htimer))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosStopTimer(htimer)");
   result=ERROR_STOPTIMER;
   }
if ((rc=DosCloseEventSem(eventtime))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(eventtime)");
   result=ERROR_STOPTIMER;
   }
return result;
}

APIRET APIENTRY SignalStop(void)
{
APIRET rc, result;
ULONG postcount;

rc=result=NO_ERROR;
if ((rc=DosResetEventSem(eventcontinue, &postcount))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosResetEventSem(eventcontinue, &postcount)");
   result=ERROR_SIGNALSTOP;
   }

if ((rc=DosPostEventSem(eventtime))!=NO_ERROR)
   {
   if (rc!=ERROR_ALREADY_POSTED && rc!=ERROR_TOO_MANY_POSTS)
      {
      ErrorMsg(rc, "DosPostEventSem(eventtime)");
      result=ERROR_SIGNALSTOP;
      }
   }
/* Give some time for the threads to stop */
DosSleep(100);
return result;
}

APIRET APIENTRY StartThreads(HEV redrawevent)
{
APIRET rc;
TID tid;
rc=NO_ERROR;

if ((rc=DosCreateThread(&tid, (PFNTHREAD)Evaluate, redrawevent, CREATE_READY, 8192))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateThread(&tid, (PFNTHREAD)Evaluate, redrawevent, CREATE_READY, 8192)");
   return ERROR_STARTTHREADS;
   }

return NO_ERROR;
}

APIRET APIENTRY Init(HAB hab, PPROPERTIES pProperties, HEV redrawevent)
{
init_result=NO_ERROR;


if ((init_result=StartSemaphores())==NO_ERROR)
   {
   if (SetProperties(pProperties)==FALSE)
      {
      return ERROR_SETPROPERTIES;
      }
   else
      {
      if ((init_result=StartTimer())==NO_ERROR)
         {
         if ((init_result=StartKeyboardMonitors())==NO_ERROR)
            {
            if ((init_result=StartInputHook(hab))==NO_ERROR)
               {
               init_result=StartThreads(redrawevent);
               }
            }
         }
      }
   }

return init_result;
}

APIRET APIENTRY Terminate(HAB hab)
{
APIRET rc;

rc=NO_ERROR;
#ifndef DEBUG
if (init_result<ERROR_SETPROPERTIES)
   {
   rc=SignalStop();
   rc|=StopTimer();
   if (init_result<ERROR_STARTTIMER)
      {
      rc|=StopKeyboardMonitors();
      if (init_result<ERROR_STARTKEYBOARDMONITORS)
         {
         rc|=StopInputHook(hab);
         }
      }
   }
rc|=StopSemaphores();
#endif
return rc;
}

/* If we are statically linking to the runtime libraries, we should */
/* register the exception handler for each function.                */
#ifdef STATIC_LINK
#pragma handler(ErrorMessage)
#pragma handler(WinErrorMessage)
#pragma handler(StartInputHook)
#pragma handler(StopInputHook)
#pragma handler(StartKeyboardMonitors)
#pragma handler(StopKeyboardMonitors)
#pragma handler(StartSemaphores)
#pragma handler(StopSemaphores)
#pragma handler(StartTimer)
#pragma handler(StopTimer)
#pragma handler(StartThreads)
#pragma handler(SignalStop)
#pragma handler(ResetStatus)
#pragma handler(GetStatus)
#pragma handler(MsgInputHook)
#pragma handler(SetProperties)
#pragma handler(GetProperties)
#pragma handler(Evaluate)
#pragma handler(KbdMonitor)
#pragma handler(Init)
#pragma handler(Terminate)
#endif
