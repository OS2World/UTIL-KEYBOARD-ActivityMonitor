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

ULONG DisplayErrorMsgBox(APIRET rc)
{
ULONG result;
result=WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, TEXT_ERROR, TITLE_ERROR, 0, MB_OK | MB_ERROR);
return result;
}

BOOL LoadProps(HAB hab, PPROPERTIES pProperties)
{
BOOL result;

result=TRUE;
/* Read properties from INI and set them up */
pProperties->KeyLow=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYLOW, 100);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYLOW, 100)")!=SEVERITY_NOERROR)
   {
   pProperties->KeyLow=100;
   }
pProperties->KeyHigh=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYHIGH, 200);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYHIGH, 200)")!=SEVERITY_NOERROR)
   {
   pProperties->KeyHigh=200;
   }
pProperties->MouseLow=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSELOW, 500);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSELOW, 500)")!=SEVERITY_NOERROR)
   {
   pProperties->MouseLow=500;
   }
pProperties->MouseHigh=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSEHIGH, 1000);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSEHIGH, 1000)")!=SEVERITY_NOERROR)
   {
   pProperties->MouseHigh=1000;
   }
pProperties->StatusLow=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSLOW, 120);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSLOW, 120)")!=SEVERITY_NOERROR)
   {
   pProperties->StatusLow=120;
   }
pProperties->StatusHigh=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSHIGH, 240);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSHIGH, 240)")!=SEVERITY_NOERROR)
   {
   pProperties->StatusHigh=240;
   }
pProperties->Beep=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_BEEP, FALSE);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_BEEP, FALSE)")!=SEVERITY_NOERROR)
   {
   pProperties->Beep=FALSE;
   }
pProperties->Log=PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOG, FALSE);
if (WinErrorMsg(hab, "PrfQueryProfileInt(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOG, FALSE)")!=SEVERITY_NOERROR)
   {
   pProperties->Log=FALSE;
   }
if (PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_COMPUTERID, DEFAULTCOMPUTERID, (PVOID)pProperties->ComputerID, 33)==FALSE)
   {
   WinErrorMsg(hab, "PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_COMPUTERID, DEFAULTCOMPUTERID, (PVOID)pProperties->ComputerID, 33)");
   strcpy((char *)(pProperties->ComputerID), DEFAULTCOMPUTERID);
   }
if (PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOGFILENAME, DEFAULTLOGFILENAME, (PVOID)pProperties->LogFileName, CCHMAXPATH)==FALSE)
   {
   WinErrorMsg(hab, "PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOGFILENAME, DEFAULTLOGFILENAME, (PVOID)pProperties->LogFileName, CCHMAXPATH)");
   strcpy((char *)(pProperties->LogFileName), DEFAULTLOGFILENAME);
   }
if (PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_VIEWER, DEFAULTVIEWER, (PVOID)pProperties->Viewer, CCHMAXPATH)==FALSE)
   {
   WinErrorMsg(hab, "PrfQueryProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_VIEWER, DEFAULTVIEWER, (PVOID)pProperties->Viewer, CCHMAXPATH)");
   strcpy((char *)(pProperties->Viewer), DEFAULTVIEWER);
   }
return result;
}

BOOL SaveProps(HAB hab, PPROPERTIES pProperties)
{
char temp[10];
BOOL result;

result=TRUE;
sprintf(temp, "%u", pProperties->KeyLow);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYLOW, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYLOW, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->KeyHigh);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYHIGH, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_KEYHIGH, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->MouseLow);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSELOW, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSELOW, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->MouseHigh);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSEHIGH, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_MOUSEHIGH, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->StatusLow);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSLOW, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSLOW, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->StatusHigh);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSHIGH, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_STATUSHIGH, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->Beep);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_BEEP, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_BEEP, temp)");
   result=FALSE;
   }
sprintf(temp, "%u", pProperties->Log);
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOG, temp)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOG, temp)");
   result=FALSE;
   }
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOGFILENAME, (PCSZ)pProperties->LogFileName)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_LOGFILENAME, (PCSZ)pProperties->LogFileName)");
   result=FALSE;
   }
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_VIEWER, (PCSZ)pProperties->Viewer)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_VIEWER, (PCSZ)pProperties->Viewer)");
   result=FALSE;
   }
if (PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_COMPUTERID, (PCSZ)pProperties->ComputerID)==FALSE)
   {
   WinErrorMsg(hab, "PrfWriteProfileString(HINI_USERPROFILE, PRF_APP, PRF_KEY_COMPUTERID, (PCSZ)pProperties->ComputerID)");
   result=FALSE;
   }

return result;
}


