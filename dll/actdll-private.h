/* All functions and data structures in the DLL that are not exported */
/* There is no need for programs that use the DLL to include this header */
/* Copyright 2000 D.J. van Enckevort */
/* Released under the GNU Public License */

/* All private data structures */
/* These structures are missing from the OS/2 Toolkit but are necessary for */
/* the 16bit code used to implement the keyboard monitors for full screen */
/* sessions. If it contains errors blame IBM for taking these data structures */
/* out of the toolkit! */
#pragma pack(1)

typedef struct _KeyPacket /* KBD monitor data record */
{
UCHAR uchFlags;
UCHAR uchOrigScanCode;
KBDKEYINFO kkiKey;
USHORT usDDFlags;
} KEYPACKET, *PKEYPACKET;

typedef struct _MONBUFF
{
  short cb;
  char data[158];
} MONBUFF, *PMONBUFF;

#pragma pack()

/* And now the functions IBM forgot to include in the Toolkit! */
APIRET16 APIENTRY16 DOSMONCLOSE(USHORT);
APIRET16 APIENTRY16 DOSMONOPEN(PSZ, PUSHORT);
APIRET16 APIENTRY16 DOSMONREAD(PMONBUFF, USHORT, PKEYPACKET, PUSHORT);
APIRET16 APIENTRY16 DOSMONREG(USHORT, PMONBUFF, PMONBUFF, USHORT, USHORT);
APIRET16 APIENTRY16 DOSMONWRITE(PMONBUFF, PKEYPACKET, USHORT);

/* And the definitions used to make live a bit easier */
#define DosMonClose DOSMONCLOSE
#define DosMonOpen DOSMONOPEN
#define DosMonRead DOSMONREAD
#define DosMonReg DOSMONREG
#define DosMonWrite DOSMONWRITE
#define MINBUFSIZE 128
#define DC_SEM_PRIVATE 0

/* Global defines for internal use only */
#define FUNCNAME "MsgInputHook"
#define TIMEOUT_NORMAL 100
#define TIMEOUT_CRITICAL SEM_IMMEDIATE_RETURN
#define NO_EVENT       0x0
#define KEYBOARD_EVENT 0x1
#define MOUSE_EVENT    0x2
#define SEM_CONTINUE "\\SEM32\\actmoncont"
#define SEM_MOUSE "\\SEM32\\actmonmouse"
#define SEM_KEYBOARD "\\SEM32\\actmonkeyboard"
#define KBD "KBD$"
#define ERRORLOG "error.log"
#define ERRORMSG "%02u-%02u-%4u %02u:%02u:%02u %s: line: %u\n\trc: %4u in function %s\n\tadditional info: %s.\n"


/* Declare functions that can be accessed only from within the DLL */
/* Other functions you will find in the other header file */
APIRET APIENTRY KbdMonitor(USHORT session);
BOOL APIENTRY MsgInputHook(HAB hab, PQMSG pqmsg, ULONG fs);
void APIENTRY Evaluate(HEV redrawevent);
APIRET APIENTRY StartKeyboardMonitors(void);
APIRET APIENTRY StopKeyboardMonitors(void);
APIRET APIENTRY StartInputHook(HAB hab);
APIRET APIENTRY StopInputHook(HAB hab);
APIRET APIENTRY StartSemaphores(void);
APIRET APIENTRY StopSemaphores(void);
APIRET APIENTRY StartTimer(void);
APIRET APIENTRY StopTimer(void);
APIRET APIENTRY SignalStop(void);
APIRET APIENTRY StartThreads(HEV redrawevent);
