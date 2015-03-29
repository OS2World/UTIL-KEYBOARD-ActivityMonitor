/*
 * This file was created for D.J. van Enckevort
 * by Project Smarts on 9 Jan 1999.
*/

typedef struct _PROPERTIES
{
volatile unsigned int KeyLow, KeyHigh, MouseLow, MouseHigh, StatusLow, StatusHigh;
volatile char ComputerID[33], LogFileName[CCHMAXPATH], Viewer[CCHMAXPATH];
volatile BOOL Beep, Log;
} PROPERTIES, *PPROPERTIES;

/* Errorcodes */
#define ERROR_SETPROPERTIES         0x00000080
#define ERROR_STARTSEMAPHORES       0x00000040
#define ERROR_STARTTIMER            0x00000020
#define ERROR_STARTKEYBOARDMONITORS 0x00000010
#define ERROR_STARTINPUTHOOK        0x00000008
#define ERROR_STARTTHREADS          0x00000004
#define ERROR_STARTUNDEFINED1       0x00000002
#define ERROR_STARTUNDEFINED2       0x00000001
#define ERROR_SIGNALSTOP            0x00008000
#define ERROR_STOPTIMER             0x00004000
#define ERROR_STOPKEYBOARDMONITORS  0x00002000
#define ERROR_STOPINPUTHOOK         0x00001000
#define ERROR_STOPSEMAPHORES        0x00000800
#define ERROR_STOPUNDEFINED1        0x00000400
#define ERROR_STOPUNDEFINED2        0x00000200
#define ERROR_STOPUNDEFINED3        0x00000100
#define UNKNOWN_STATUS              0x80000000

#define DLLNAME "ACTMON"

/* Defined macros */
#define ErrorMsg(rc, message) (ErrorMessage(rc, message, __LINE__, __FUNCTION__, __FILE__))
#define WinErrorMsg(hab, message) (WinErrorMessage(hab, message, __LINE__, __FUNCTION__, __FILE__))

/* Declared functions */
void APIENTRY ErrorMessage(APIRET rc, char *message, ULONG line, char *function, char *file);
USHORT APIENTRY WinErrorMessage(HAB hab, char *message, ULONG line, char *function, char *file);
BOOL EXPENTRY ResetStatus(void);
LONG EXPENTRY GetStatus(void);
BOOL EXPENTRY SetProperties(PPROPERTIES NewProps);
BOOL EXPENTRY GetProperties(PPROPERTIES ThisProps);
APIRET APIENTRY Init(HAB hab, PPROPERTIES pProperties, HEV redrawevent);
APIRET APIENTRY Terminate(HAB hab);

