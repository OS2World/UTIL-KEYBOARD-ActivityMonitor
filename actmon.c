/* actmon 1.0 */
/* Copyright 1999, 2000 D.J. van Enckevort */
/* Written by D.J. van Enckevort */
/* actmon monitors the keyboard and mouse activity and writes a log */
/*
#define DEBUG
*/
#define INCL_WIN
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_PM
#define INCL_GPI
#define INCL_KBD
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dll\actdll.h"
#include "actmon.h"
#include "helper.h"
#include "properties.h"
#include "dialog.h"


BOOL StartSemaphores(PPARAMS pparams)
{
APIRET rc;

pparams->redraw=NULLHANDLE;
pparams->stopsignal=NULLHANDLE;
if ((rc=DosCreateEventSem(NULL, (PHEV)&(pparams->redraw), 0, FALSE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(NULL, (PHEV)&(pparams->redraw), 0, FALSE)");
   return FALSE;
   }
if ((rc=DosCreateEventSem(NULL, (PHEV)&(pparams->stopsignal), 0, TRUE))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCreateEventSem(NULL, (PHEV)&(pparams->stopsignal), 0, TRUE)");
   return FALSE;
   }
return TRUE;
}

BOOL StopSemaphores(PPARAMS pparams)
{
BOOL result;
APIRET rc;

result=TRUE;
if ((rc=DosCloseEventSem(pparams->redraw))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(pparams->redraw)");
   result=FALSE;
   }
if ((rc=DosCloseEventSem(pparams->stopsignal))!=NO_ERROR)
   {
   ErrorMsg(rc, "DosCloseEventSem(pparams->stopsignal)");
   result=FALSE;
   }
return result;
}

BOOL CreateWindow(PPARAMS pparams)
{
ULONG flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINBUTTON | FCF_SHELLPOSITION | FCF_SIZEBORDER | FCF_TASKLIST;
ULONG size;
BOOL result=TRUE;
int i;
char *title;

if (WinRegisterClass(pparams->hab, WINDOW_CLASS, WindowProc, CS_SIZEREDRAW, sizeof(PPARAM))!=TRUE)
   {
   WinErrorMsg(pparams->hab, "WinRegisterClass(pparams->hab, WINDOW_CLASS, WindowProc, CS_SIZEREDRAW, sizeof(PPARAM)");
   return FALSE;
   }
if ((title=(char *)malloc(strlen(ACTMON_TITLE)+10))==NULL)
   {
   return FALSE;
   }
else
   {
   sprintf(title, "%s %i.%02i.%02i", ACTMON_TITLE, VERSIONMAJOR, VERSIONMINOR, VERSIONREVISION);
   }
if ((pparams->frame=WinCreateStdWindow(HWND_DESKTOP, 0, &flCreate, WINDOW_CLASS, title, 0, (HMODULE)NULL, ID_WINDOW, (PHWND)&(pparams->client)))==NULLHANDLE)
   {
   WinErrorMsg(pparams->hab, "pparams->frame=WinCreateStdWindow(HWND_DESKTOP, 0, &flCreate, NULL, title, 0, (HMODULE)NULL, ID_WINDOW, (PHWND)&(params->client))");
   free(title);
   return FALSE;
   }
free(title);
if ((result=(BOOL)WinSendMsg(pparams->client, UWM_DATA, pparams, NULL))!=TRUE)
   {
   WinErrorMsg(pparams->hab, "result=(BOOL)WinSendMsg(pparams->client, UWM_DATA, pparams, NULL)");
   return result;
   }
if (PrfQueryProfileSize(HINI_USERPROFILE, PRF_APP, PRF_KEY_MAINWINDOW, &size)==TRUE)
   {
   if (WinRestoreWindowPos(PRF_APP, PRF_KEY_MAINWINDOW, pparams->frame)==FALSE)
      {
      if (WinSetWindowPos(pparams->frame, HWND_TOP, 100, 100, 200, 50, SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW)==FALSE)
         {
         WinErrorMsg(pparams->hab, "WinSetWindowPos(pparams->frame, HWND_TOP, 100, 100, 200, 50, SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW)");
         }
      WinErrorMsg(pparams->hab, "WinRestoreWindowPos(PRF_APP, PRF_KEY_MAINWINDOW, pparams->frame)");
      }
   }
if ((pparams->defaulticon=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 1))!=NULLHANDLE)
   {
   WinSendMsg(pparams->frame, WM_SETICON, MPFROMP(pparams->defaulticon), NULL);
   }
else
   {
   WinErrorMsg(pparams->hab, "WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 1)");
   }
for (i=0; i<6; i++)
   {
   if ((pparams->greenicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_GREEN1+i))==NULLHANDLE)
      {
      WinErrorMsg(pparams->hab, "pparams->greenicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_GREEN1+i");
      }
   if ((pparams->yellowicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_YELLOW1+i))==NULLHANDLE)
      {
      WinErrorMsg(pparams->hab, "pparams->yellowicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_YELLOW1+i");
      }
   if ((pparams->redicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_RED1+i))==NULLHANDLE)
      {
      WinErrorMsg(pparams->hab, "pparams->redicons[i]=WinLoadPointer(HWND_DESKTOP, NULLHANDLE, ICON_RED1+i");
      }
   }
return TRUE;
}

BOOL DestroyWindow(PPARAMS pparams)
{
int i;

if (WinDestroyPointer(pparams->defaulticon)==FALSE)
   {
   WinErrorMsg(pparams->hab, "WinDestroyPointer(pparams->defaulticon");
   }
for (i=0; i<6; i++)
   {
   if (WinDestroyPointer(pparams->greenicons[i])==FALSE)
      {
      WinErrorMsg(pparams->hab, "WinDestroyPointer(pparams->greenicons[i]");
      }
   if (WinDestroyPointer(pparams->yellowicons[i])==FALSE)
      {
      WinErrorMsg(pparams->hab, "WinDestroyPointer(pparams->yellowicons[i]");
      }
   if (WinDestroyPointer(pparams->redicons[i])==FALSE)
      {
      WinErrorMsg(pparams->hab, "WinDestroyPointer(pparams->redicons[i]");
      }
   }
if (WinDestroyWindow(pparams->frame)==FALSE)
   {
   WinErrorMsg(pparams->hab, "(WinDestroyWindow(pparams->frame)");
   return FALSE;
   }
return TRUE;
}

/* Main function */
int main(void)
{
HMQ hmq;
QMSG qmsg;
APIRET rc, rcinit;
PROPERTIES Properties;
TID tid;
PARAMS params;

params.size=sizeof(PARAMS);
params.hab = WinInitialize(0);
hmq = WinCreateMsgQueue(params.hab, 0);

if (StartSemaphores(&params)==TRUE)
   {
   if (LoadProps(params.hab, &Properties)==TRUE)
      {
      if ((rcinit=Init(params.hab, &Properties, params.redraw))==NO_ERROR)
         {
         if (CreateWindow(&params)==TRUE)
            {
            if ((rc=DosCreateThread(&tid, (PFNTHREAD)Update, (ULONG)(&params), CREATE_READY, 8192))==NO_ERROR)
               {
               if ((rc=DosCreateThread(&tid, (PFNTHREAD)AnimateIcon, (ULONG)(&params), CREATE_READY, 8192))==NO_ERROR)
                  {
                  while (WinGetMsg(params.hab, &qmsg, 0L, 0, 0))
                     {
                     WinDispatchMsg(params.hab, &qmsg);
                     }
                  }
               else
                  {
                  ErrorMsg(rc, "DosCreateThread(&tid, (PFNTHREAD)AnimateIcon, &params, CREATE_READY, 4096)");
                  WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to start thread.", "Error", 0, MB_OK);
                  }
               }
            else
               {
               ErrorMsg(rc, "DosCreateThread(&tid, (PFNTHREAD)Update, &params, CREATE_READY, 4096)");
               WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to start thread.", "Error", 0, MB_OK);
               }
            DestroyWindow(&params);
            }
         else
            {
            /* Creation of main window failed */
            WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to Create Window.", "Error", 0, MB_OK);
            }
         if ((rc=Terminate(params.hab))!=NO_ERROR)
            {
            DisplayErrorMsgBox(rc);
            }
         }
      else
         {
         /* Startup of the DLL functions failed */
         DisplayErrorMsgBox(rc);
         if ((rc=Terminate(params.hab))!=NO_ERROR)
            {
            DisplayErrorMsgBox(rc);
            }
         }
      }
   else
      {
      /* Loading of the data stored in the INI file failed */
      }
   }
else
   {
   /* Creation of the event semaphores failed */
   }

if (WinDestroyMsgQueue(hmq)==FALSE)
   WinErrorMsg(params.hab, "WinDestroyMsgQueue(hmq)");
WinTerminate(params.hab);
return 0;
}

BOOL InstallMenuItem(HWND hwnd, USHORT id, SHORT pos, char *name)
{
USHORT idSysMenu;
HWND hwndSysMenu;
MENUITEM miSysMenu, mi;
MRESULT mr;
HAB hab;

hab=WinQueryAnchorBlock(hwnd);

if ((hwndSysMenu=WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_SYSMENU))==NULLHANDLE)
   {
   WinErrorMsg(hab, "hwndSysMenu=WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_SYSMENU)");
   return FALSE;;
   }
if ((idSysMenu=SHORT1FROMMR(WinSendMsg(hwndSysMenu, MM_ITEMIDFROMPOSITION, NULL, NULL)))==MIT_ERROR)
   {
   WinErrorMsg(hab, "idSysMenu=SHORT1FROMMR(WinSendMsg(hwndSysMenu, MM_ITEMIDFROMPOSITION, NULL, NULL))");
   return FALSE;;
   }
if ((WinSendMsg(hwndSysMenu, MM_QUERYITEM, MPFROM2SHORT(idSysMenu, FALSE), MPFROMP(&miSysMenu)))==FALSE)
   {
   WinErrorMsg(hab, "WinSendMsg(hwndSysMenu, MM_QUERYITEM, MPFROM2SHORT(idSysMenu, FALSE), MPFROMP(&miSysMenu))");
   return FALSE;;
   }
/* Fill in MENUITEM structure */
mi.iPosition=pos;
mi.afStyle=MIS_TEXT;
mi.afAttribute=0;
mi.id=id;
mi.hwndSubMenu=(HWND)NULL;
mi.hItem=0L;
mr=WinSendMsg(miSysMenu.hwndSubMenu, MM_INSERTITEM, (MPARAM)&mi, MPFROMP(name));
if (SHORT1FROMMR(mr)==MIT_MEMERROR || SHORT1FROMMR(mr)==MIT_ERROR)
   {
   WinErrorMsg(hab, "mr=WinSendMsg(miSysMenu.hwndSubMenu, MM_INSERTITEM, (MPARAM)&mi, MPFROMP(name))");
   return FALSE;
   }

return TRUE;
}

BOOL AddTrayIcon(PPARAMS pparams, HPOINTER hicon)
{
if (pparams->trayserver!=NULLHANDLE && pparams->client!=NULLHANDLE)
   {
   if (WinPostMsg(pparams->trayserver, WM_TRAYADDME, (MPARAM)pparams->client, (MPARAM)hicon)==TRUE)
      {
      pparams->is_iconized=TRUE;
      return TRUE;
      }
   else
      {
      WinErrorMsg(pparams->hab, "WinPostMsg(pparams->trayserver, WM_TRAYADDME, (MPARAM)pparams->client, (MPARAM)hicon)");
      pparams->is_iconized=FALSE;
      return FALSE;
      }
   }
return FALSE;
}

BOOL UpdateIcon(PPARAMS pparams)
{
LONG status;
PROPERTIES props;
HPOINTER hicon;

if (pparams!=NULL)
   { /* We have a valid pointer */
   if ((status=GetStatus())>=0)
      { /* We did get the status */
      if (GetProperties(&props)==TRUE)
         if (status>props.StatusHigh)
            hicon=pparams->redicons[pparams->count];
         else if (status>props.StatusLow)
            hicon=pparams->yellowicons[pparams->count];
         else
            hicon=pparams->greenicons[pparams->count];
      } /* We did get the status */
   else
      hicon=pparams->defaulticon;
   WinSendMsg(pparams->frame, WM_SETICON, MPFROMP(hicon), NULL);
   if (pparams->trayserver!=NULLHANDLE)
      { /* We have a trayserver */
      if (pparams->is_iconized==FALSE)
         { /* Install icon in tray */
         if ((pparams->is_iconized=AddTrayIcon(pparams, hicon))==FALSE)
            {
              /* We failed to install icon in tray, thus we conclude the
                 trayserver is not available and remove the handle to it
              */
            pparams->trayserver=NULLHANDLE;
            return FALSE;
            } /* Failed to install icon */
         } /* Install icon in tray */
      else
         { /* Update icon in tray */
         if ((pparams->is_iconized=UpdateTrayIcon(pparams, hicon))==FALSE)
            {
              /* We failed to update icon in tray, thus we conclude the
                 trayserver is not available and remove the handle to it
              */
            pparams->trayserver=NULLHANDLE;
            return FALSE;
            } /* Failed to update icon */
         } /* Update icon in tray */
      } /* Is trayserver available */
   } /* Did we get a valid pointer */
return TRUE;
}

void APIENTRY AnimateIcon(PPARAMS pparams)
{
LONG status;
PROPERTIES props;
ULONG postcount;
HAB hab;
HMQ hmq;
APIRET rc, result;

pparams->is_iconized=FALSE;
pparams->count=0;

if (pparams!=NULL)
   {
   /* Each thread needs his own anchor block and handle to the messagequeue */
   if ((hab=WinInitialize(0))!=NULLHANDLE)
      {
      if ((hmq=WinCreateMsgQueue(hab, 0))!=NULLHANDLE)
         {
         while ((result=DosWaitEventSem(pparams->stopsignal, SEM_IMMEDIATE_RETURN))==NO_ERROR)
            {
            UpdateIcon(pparams);
            pparams->count++;
            if (pparams->count==6)
               {
               pparams->count=0;
               }
            DosSleep(1000);
            }
         if (result!=ERROR_TIMEOUT)
            {
            ErrorMsg(result, "rc=DosWaitEventSem(pparams->stopsignal, SEM_IMMEDIATE_RETURN)");
            }
         if (WinDestroyMsgQueue(hmq)==FALSE)
            {
            WinErrorMsg(hab, "WinDestroyMsgQueue(hmq)");
            }
         }
      WinTerminate(hab);
      }
   }
}


BOOL UpdateTrayIcon(PPARAMS pparams, HPOINTER hicon)
{
if (pparams->trayserver!=NULLHANDLE && pparams->client!=NULLHANDLE)
   {
   if (WinPostMsg(pparams->trayserver, WM_TRAYICON, (MPARAM)pparams->client, (MPARAM)hicon)==TRUE)
      {
      pparams->is_iconized=TRUE;
      return TRUE;
      }
   else
      {
      WinErrorMsg(pparams->hab, "WinPostMsg(pparams->trayserver, WM_TRAYICON, (MPARAM)pparams->client, (MPARAM)hicon)");
      return FALSE;
      }
   }
return FALSE;
}

BOOL RemoveTrayIcon(PPARAMS pparams)
{
if (pparams->trayserver!=NULLHANDLE && pparams->client!=NULLHANDLE)
   {
   if (WinPostMsg(pparams->trayserver, WM_TRAYDELME, (MPARAM)pparams->client, (MPARAM)0L)==TRUE)
      {
      pparams->is_iconized=FALSE;
      return TRUE;
      }
   else
      {
      WinErrorMsg(pparams->hab, "WinPostMsg(pparams->trayserver, WM_TRAYDELME, (MPARAM)pparams->client, (MPARAM)0L)");
      return FALSE;
      }
   }
return FALSE;
}

void APIENTRY Update(PPARAMS data)
{
ULONG postcount;
HAB hab;
HMQ hmq;
APIRET rc, result;
PPARAMS pparams;

if (data!=NULL)
   {
   pparams=data;
   /* Each thread needs his own anchor block and handle to the messagequeue */
   if ((hab=WinInitialize(0))!=NULLHANDLE)
      {
      if ((hmq=WinCreateMsgQueue(hab, 0))!=NULLHANDLE)
         {
         result=DosWaitEventSem(pparams->stopsignal, SEM_IMMEDIATE_RETURN);
         while (result==NO_ERROR)
            {
            if ((rc=DosWaitEventSem(pparams->redraw, SEM_INDEFINITE_WAIT))==NO_ERROR)
               {
               result=DosWaitEventSem(pparams->stopsignal, SEM_IMMEDIATE_RETURN);
               if (result==NO_ERROR)
                  {
                  if ((rc=DosResetEventSem(pparams->redraw, &postcount))!=NO_ERROR)
                     {
                     ErrorMsg(rc, "rc=DosResetEventSem(pparams->redraw, &postcount)");
                     }
                  /* Force a redraw since the status has changed */
                  if (WinInvalidateRect(pparams->client, NULL, FALSE)==FALSE)
                     {
                     WinErrorMsg(hab, "WinInvalidateRect(pparams->client, NULL, FALSE)");
                     }
                  if (WinPostMsg(pparams->client, UWM_UPDATEICON, (MPARAM)0L, (MPARAM)0L)==FALSE)
                     {
                     WinErrorMsg(hab, "WinPostMsg(pparams->client, UWM_UPDATEICON, (MPARAM)0L, (MPARAM)0L)");
                     }
                  }
               }
            else
               {
               ErrorMsg(rc, "rc=DosWaitEventSem(pparams->redraw, SEM_INDEFINITE_WAIT)");
               }
            }
         if (result!=ERROR_TIMEOUT)
            {
            ErrorMsg(result, "rc=DosWaitEventSem(pparams->stopsignal, SEM_IMMEDIATE_RETURN)");
            }
         if (WinDestroyMsgQueue(hmq)==FALSE)
            {
            WinErrorMsg(hab, "WinDestroyMsgQueue(hmq)");
            }
         }
      WinTerminate(hab);
      }
   }
}

MRESULT EXPENTRY WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
PPARAMS pparams;
USHORT command;
HPS hps;
HWND hwndprop;
char args[CCHMAXPATH];
RESULTCODES pgmrc;
APIRET rc;
FILE *f;
PROPERTIES props;
LONG status;
POINTL ptl;
RECTL rcl;
PSWP pswp;
TID tid;
ULONG postcount=0;
char errorid[10];

switch (msg)
   {
   case WM_CREATE:
      {
      /* Since we need the data structure we only get in the UWM_DATA message
         we postpone all window creation related stuff to UWM_DATA */
      break;
      }
   case UWM_DATA:
      {
      pparams=(PPARAMS)mp1;
      WinSetWindowULong(hwnd, QWL_USER, (ULONG)pparams);
      pparams->trayserver=NULLHANDLE;
      pparams->is_hidden=FALSE;
      pparams->is_iconized=FALSE;
      /* Add menuitem to system menu, if it doesn't work no harm is done */
      if (InstallMenuItem(hwnd, MI_PROPERTIES, 0, TXT_PROPERTIES)==TRUE)
         {
         if (InstallMenuItem(hwnd, MI_SAVEWINDOWPOS, 1, TXT_SAVEWINDOWPOS)==TRUE)
            {
            if (InstallMenuItem(hwnd, MI_RESET, 2, TXT_RESET)==TRUE)
               {
               InstallMenuItem(hwnd, MI_VIEWLOG, 3, TXT_VIEWLOG);
               }
            }
         }
      else
         {
         WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to install menus, no menu items added to system menu.", "Warning", 0, MB_OK | MB_WARNING);
         }
      if ((pparams->menu=WinLoadMenu(hwnd, (HMODULE)NULL, MN_CONTEXT))==NULLHANDLE)
         {
         WinErrorMsg(pparams->hab, "pparams->menu=WinLoadMenu(hwnd, (HMODULE)NULL, MN_CONTEXT)");
         WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to load menu, no context menu will be available", "Warning", 0, MB_OK | MB_WARNING);
         }
      if (WinDdeInitiate(hwnd,SZAPP,SZTOPIC,NULL)==FALSE)
         {
         /* We can't dock into the tray, but this is no big error, so we continue */
         WinErrorMsg(pparams->hab, "WinDdeInitiate(hwnd,SZAPP,SZTOPIC,NULL)");
         WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to initiate DDE. No docking will be performed", "Warning", 0, MB_WARNING | MB_OK);
         }
      return (MRESULT)TRUE;
      }
   case UWM_UPDATEICON:
      {
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
         {
         UpdateIcon(pparams);
         }
      else
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      break;
      }
   case WM_BUTTON1DBLCLK | 0x2000:
      {
      WinSetWindowPos(WinQueryWindow(hwnd, QW_PARENT), NULLHANDLE, 0, 0, 0, 0, SWP_SHOW | SWP_RESTORE);
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
         {
         DosPostEventSem(pparams->redraw);
         }
      else
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      break;
      }
   case WM_BUTTON2CLICK | 0x2000:
      {
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
         {
         if (pparams->menu!=NULLHANDLE)
            {
            WinQueryPointerPos(HWND_DESKTOP, &ptl);
            WinPopupMenu(HWND_DESKTOP, WinQueryWindow(hwnd, QW_PARENT), pparams->menu, ptl.x, ptl.y+32, MI_PROPERTIES, PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_MOUSEBUTTON3);
            }
         }
      else
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
      break;
      }
   case WM_CONTEXTMENU:
      {
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
         if (pparams->menu!=NULLHANDLE)
            {
            WinQueryPointerPos(HWND_DESKTOP, &ptl);
            WinPopupMenu(HWND_DESKTOP, WinQueryWindow(hwnd, QW_PARENT), pparams->menu, ptl.x, ptl.y, MI_PROPERTIES, PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_MOUSEBUTTON3);
            }
      else
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
      break;
      }
   case WM_COMMAND:
      {
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))==NULL)
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      command = SHORT1FROMMP(mp1);
      switch (command)
         {
         case MI_PROPERTIES:
            {
            if (pparams!=NULL)
               {
               hwndprop=WinLoadDlg(HWND_DESKTOP, WinQueryWindow(hwnd, QW_PARENT), DialogProc_Properties, (HMODULE)0, DLG_PROPERTIES, pparams);
               }
            else
               {
               WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Cannot open Properties Dialog.", "Error", 0, MB_OK | MB_WARNING);
               }
            break;
            }
         case MI_SAVEWINDOWPOS:
            {
            if (WinStoreWindowPos(PRF_APP, PRF_KEY_MAINWINDOW, WinQueryWindow(hwnd, QW_PARENT))!=TRUE)
               {
               WinErrorMsg(WinQueryAnchorBlock(hwnd), "WinStoreWindowPos(PRF_APP, PRF_KEY_MAINWINDOW, WinQueryWindow(hwnd, QW_PARENT)");
               WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to save window position.", "Error", 0, MB_OK);
               }
            break;
            }
         case MI_VIEWLOG:
            {
            GetProperties(&props);
            sprintf(args, "%s %s\0", props.Viewer, props.LogFileName);
            args[strlen((char *)(props.Viewer))]='\0';
            if ((rc=DosExecPgm(NULL, 0, EXEC_ASYNC, args, NULL, &pgmrc, (PCSZ)(props.Viewer)))!=NO_ERROR)
               {
               ErrorMsg(rc, "rc=DosExecPgm(NULL, 0, EXEC_ASYNC, args, NULL, &pgmrc, (PCSZ)(props.Viewer))");
               WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to open external viewer.", "Error", 0, MB_OK);
               }
            break;
            }
         case MI_RESET:
            {
            ResetStatus();
            /* Force a redraw since the status has changed */
            if (pparams!=NULL)
               {
               DosPostEventSem(pparams->redraw);
               }
            break;
            }
         default:
            return WinDefWindowProc(hwnd, msg, mp1, mp2);
         }
      break;
      }
   case WM_PAINT:
      {
      hps = WinBeginPaint(hwnd, 0L, &rcl);
      if ((status=GetStatus())>=0)
         {
         if (GetProperties(&props)==TRUE)
            {
            if (status>props.StatusHigh)
               {
               WinFillRect(hps, &rcl, CLR_RED);
               }
            else if (status>props.StatusLow)
               {
               WinFillRect(hps, &rcl, CLR_YELLOW);
               }
            else
               {
               WinFillRect(hps, &rcl, CLR_GREEN);
               }
            } /* if GetProperties */
         else
            {
            WinFillRect(hps, &rcl, CLR_BLACK);
            }
         } /* if status */
      else
         {
         WinFillRect(hps, &rcl, CLR_BLACK);
         }
      WinEndPaint(hps);
      break;
      }
   case WM_MINMAXFRAME:
      {
      pswp=(PSWP)mp1;
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))==NULL)
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      else
         {
         if ((pswp->fl & SWP_MINIMIZE) || (pswp->fl & SWP_HIDE))
             pparams->is_hidden=TRUE;
         if ((pswp->fl & SWP_RESTORE) || (pswp->fl & SWP_SHOW))
             pparams->is_hidden=FALSE;
         }
      break;
      }
   case WM_DDE_INITIATEACK:
      {
      /* answer dde server */
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
         {
         pparams->trayserver=(HWND)mp1;
         }
      else
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      break;
      }
   case WM_DDE_TERMINATE:
      {
      WinPostMsg((HWND)mp1, WM_DDE_TERMINATE, (MPARAM)hwnd, (MPARAM)NULL);
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))==NULL)
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      else
         {
         pparams->trayserver=NULLHANDLE;
         }
      break;
      }
   case WM_CLOSE:
      {
      if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))==NULL)
         {
         WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER)");
         }
      else
         {
         if ((rc=DosResetEventSem(pparams->stopsignal, &postcount))!=NO_ERROR)
            {
            ErrorMsg(rc, "rc=DosResetEventSem(pparams->stopsignal, &postcount)");
            }
         if ((rc=DosPostEventSem(pparams->redraw))!=NO_ERROR)
            {
            ErrorMsg(rc, "rc=DosPostEventSem(pparams->redraw)");
            }
         if (pparams->trayserver!=NULLHANDLE)
            {
            WinPostMsg(pparams->trayserver, WM_TRAYDELME, (MPARAM)hwnd, 0L);
            WinPostMsg(pparams->trayserver, WM_DDE_TERMINATE, NULL, (MPARAM)DDEPM_RETRY);
            }
         }
      WinPostMsg(hwnd, WM_QUIT, 0L, 0L);
      break;
      }
   default:
      return WinDefWindowProc(hwnd, msg, mp1, mp2);
   }
return ((MRESULT)FALSE);
}
