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

BOOL InitDlg(HWND hwnd)
{
BOOL result;
PROPERTIES props;

result=TRUE;
/* Set the current values */
GetProperties(&props);
WinPostMsg(WinWindowFromID(hwnd, SB_KEYLOW), SPBM_SETLIMITS, MPFROMLONG(props.KeyHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_KEYHIGH), SPBM_SETLIMITS, MPFROMLONG(500L), MPFROMLONG(props.KeyLow));
WinPostMsg(WinWindowFromID(hwnd, SB_MOUSELOW), SPBM_SETLIMITS, MPFROMLONG(props.MouseHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_MOUSEHIGH), SPBM_SETLIMITS, MPFROMLONG(2500L), MPFROMLONG(props.MouseLow));
WinPostMsg(WinWindowFromID(hwnd, SB_STATUSLOW), SPBM_SETLIMITS, MPFROMLONG(props.StatusHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_STATUSHIGH), SPBM_SETLIMITS, MPFROMLONG(10000L), MPFROMLONG(props.StatusLow));
WinPostMsg(WinWindowFromID(hwnd, SB_KEYLOW), SPBM_SETCURRENTVALUE, MPFROMLONG(props.KeyLow), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_KEYHIGH), SPBM_SETCURRENTVALUE, MPFROMLONG(props.KeyHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_MOUSELOW), SPBM_SETCURRENTVALUE, MPFROMLONG(props.MouseLow), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_MOUSEHIGH), SPBM_SETCURRENTVALUE, MPFROMLONG(props.MouseHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_STATUSLOW), SPBM_SETCURRENTVALUE, MPFROMLONG(props.StatusLow), 0L);
WinPostMsg(WinWindowFromID(hwnd, SB_STATUSHIGH), SPBM_SETCURRENTVALUE, MPFROMLONG(props.StatusHigh), 0L);
WinPostMsg(WinWindowFromID(hwnd, CB_BEEP), BM_SETCHECK, MPFROMSHORT((SHORT)props.Beep), 0L);
WinPostMsg(WinWindowFromID(hwnd, CB_CREATELOG), BM_SETCHECK, MPFROMSHORT((SHORT)props.Log), 0L);
WinSetWindowText(WinWindowFromID(hwnd, EF_LOGFILENAME), (PCSZ)(props.LogFileName));
WinSetWindowText(WinWindowFromID(hwnd, EF_VIEWER), (PCSZ)(props.Viewer));
WinSetWindowText(WinWindowFromID(hwnd, EF_COMPUTERNAME), (PCSZ)(props.ComputerID));
return result;
}

MRESULT EXPENTRY DialogProc_Properties(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
USHORT command;
ULONG limit;
HPS hps;
PROPERTIES Properties;
char temp[33];
PPARAMS pparams;
switch (msg)
   {
   case WM_INITDLG:
      {
      if (InitDlg(hwnd)==FALSE)
         {
         WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, ERRORTEXT_PROPERTIESFAILED, TITLE_ERROR, 0, MB_OK | MB_WARNING);
         }
      WinSetWindowULong(hwnd, QWL_USER, (ULONG)mp2);
      break;
      }
   case WM_COMMAND:
      {
      command=SHORT1FROMMP(mp1);
      switch (command)
         {
         case PB_OK:
            {
            if ((pparams=(PPARAMS)WinQueryWindowULong(hwnd, QWL_USER))!=NULL)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_KEYLOW), SPBM_QUERYVALUE, MPFROMP(&(Properties.KeyLow)), 0L);
               WinSendMsg(WinWindowFromID(hwnd, SB_KEYHIGH), SPBM_QUERYVALUE, MPFROMP(&(Properties.KeyHigh)), 0L);
               WinSendMsg(WinWindowFromID(hwnd, SB_MOUSELOW), SPBM_QUERYVALUE, MPFROMP(&(Properties.MouseLow)), 0L);
               WinSendMsg(WinWindowFromID(hwnd, SB_MOUSEHIGH), SPBM_QUERYVALUE, MPFROMP(&(Properties.MouseHigh)), 0L);
               WinSendMsg(WinWindowFromID(hwnd, SB_STATUSLOW), SPBM_QUERYVALUE, MPFROMP(&(Properties.StatusLow)), 0L);
               WinSendMsg(WinWindowFromID(hwnd, SB_STATUSHIGH), SPBM_QUERYVALUE, MPFROMP(&(Properties.StatusHigh)), 0L);
               Properties.Beep=(BOOL)SHORT1FROMMR(WinSendMsg(WinWindowFromID(hwnd, CB_BEEP), BM_QUERYCHECK, 0L, 0L));
               Properties.Log=(BOOL)SHORT1FROMMR(WinSendMsg(WinWindowFromID(hwnd, CB_CREATELOG), BM_QUERYCHECK, 0L, 0L));
               WinQueryWindowText(WinWindowFromID(hwnd, EF_COMPUTERNAME), 33, (PCH)(Properties.ComputerID));
               WinQueryWindowText(WinWindowFromID(hwnd, EF_VIEWER), CCHMAXPATH, (PCH)(Properties.Viewer));
               WinQueryWindowText(WinWindowFromID(hwnd, EF_LOGFILENAME), CCHMAXPATH, (PCH)(Properties.LogFileName));
               SaveProps(pparams->hab, &Properties);
               if (SetProperties(&Properties)==TRUE)
                  {
                  DosPostEventSem(pparams->redraw);
                  WinPostMsg(hwnd, WM_CLOSE, 0L, 0L);
                  }
               else
                  {
                  WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Failed to set properties.", "Warning", 0, MB_OK);
                  }
               }
            else
               {
               WinErrorMsg(WinQueryAnchorBlock(hwnd), "pparams=(PPARAMS)WinGetWindowULong(hwnd, QWL_USER)");
               }
            break;
            }
         case PB_CANCEL:
            {
            WinPostMsg(hwnd, WM_CLOSE, 0L, 0L);
            break;
            }
         default:
            {
            break;
            }
         }
      break;
      }
   case WM_CONTROL:
      {
      switch (SHORT1FROMMP(mp1))
         {
         case SB_KEYLOW:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_KEYLOW), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_KEYHIGH), SPBM_SETLIMITS, MPFROMLONG(500L), MPFROMLONG(limit));
               }
            break;
            }
         case SB_KEYHIGH:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_KEYHIGH), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_KEYLOW), SPBM_SETLIMITS, MPFROMLONG(limit), 0L);
               }
            break;
            }
         case SB_MOUSELOW:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_MOUSELOW), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_MOUSEHIGH), SPBM_SETLIMITS, MPFROMLONG(2500L), MPFROMLONG(limit));
               }
            break;
            }
         case SB_MOUSEHIGH:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_MOUSEHIGH), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_MOUSELOW), SPBM_SETLIMITS, MPFROMLONG(limit), 0L);
               }
            break;
            }
         case SB_STATUSLOW:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_STATUSLOW), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_STATUSHIGH), SPBM_SETLIMITS, MPFROMLONG(10000L), MPFROMLONG(limit));
               }
            break;
            }
         case SB_STATUSHIGH:
            {
            if (SHORT2FROMMP(mp1)==SPBN_KILLFOCUS)
               {
               WinSendMsg(WinWindowFromID(hwnd, SB_STATUSHIGH), SPBM_QUERYVALUE, MPFROMP(&limit), 0L);
               WinPostMsg(WinWindowFromID(hwnd, SB_STATUSLOW), SPBM_SETLIMITS, MPFROMLONG(limit), 0L);
               }
            break;
            }
         default:
            return WinDefDlgProc(hwnd, msg, mp1, mp2);
         }
      break;
      }
   case WM_CLOSE:
      {
      WinDismissDlg(hwnd, TRUE);
      break;
      }
   default:
      return WinDefDlgProc(hwnd, msg, mp1, mp2);
   }
return (MRESULT) FALSE;
}

