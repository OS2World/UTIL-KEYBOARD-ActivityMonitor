#ifndef ACTMON_H
#define ACTMON_H

typedef struct _PARAMS
{
volatile USHORT size;
volatile HAB hab;
volatile HEV redraw, stopsignal;
volatile HWND frame, client, menu, trayserver;
volatile HPOINTER defaulticon, greenicons[6], yellowicons[6], redicons[6];
volatile char count;
volatile BOOL is_iconized, is_hidden;
} PARAMS, *PPARAMS;

#define ID_WINDOW 1
#define ID_CLIENT 2
#define MN_CONTEXT 200
#define MI_PROPERTIES 210
#define TXT_PROPERTIES "~Properties..."
#define MI_SAVEWINDOWPOS 211
#define TXT_SAVEWINDOWPOS "S~ave position"
#define MI_RESET 212
#define TXT_RESET "R~eset"
#define MI_VIEWLOG 213
#define TXT_VIEWLOG "~Display log"
#define WINDOW_CLASS "ACTMONClass"
#define PRF_APP "ACTMON"
#define PRF_KEY_MAINWINDOW "MAINWINDOW"
#define PRF_KEY_PROPERTIESWINDOW "PROPERTIESWINDOW"
#define PRF_KEY_KEYLOW "KEYLOW"
#define PRF_KEY_KEYHIGH "KEYHIGH"
#define PRF_KEY_MOUSELOW "MOUSELOW"
#define PRF_KEY_MOUSEHIGH "MOUSEHIGH"
#define PRF_KEY_STATUSLOW "STATUSLOW"
#define PRF_KEY_STATUSHIGH "STATUSHIGH"
#define PRF_KEY_BEEP "BEEP"
#define PRF_KEY_LOG "LOG"
#define PRF_KEY_COMPUTERID "COMPUTERID"
#define PRF_KEY_LOGFILENAME "LOGFILENAME"
#define PRF_KEY_VIEWER "VIEWER"
#define DEFAULTLOGFILENAME "actmon.log"
#define DEFAULTCOMPUTERID "Computer01"
#define DEFAULTVIEWER "e.exe"
#define WM_TRAYADDME WM_USER+1
#define WM_TRAYDELME WM_USER+2
#define WM_TRAYICON  WM_USER+3
#define UWM_UPDATEICON WM_USER+4
#define UWM_DATA WM_USER+5
#define SZAPP "SystrayServer"
#define SZTOPIC "TRAY"
#define ICON_DEFAULT 1
#define ICON_GREEN1 11
#define ICON_GREEN2 12
#define ICON_GREEN3 13
#define ICON_GREEN4 14
#define ICON_GREEN5 15
#define ICON_GREEN6 16
#define ICON_YELLOW1 21
#define ICON_YELLOW2 22
#define ICON_YELLOW3 23
#define ICON_YELLOW4 24
#define ICON_YELLOW5 25
#define ICON_YELLOW6 26
#define ICON_RED1 31
#define ICON_RED2 32
#define ICON_RED3 33
#define ICON_RED4 34
#define ICON_RED5 35
#define ICON_RED6 36
#define ACTMON_TITLE "Activity Monitor"
#define VERSIONMAJOR 0
#define VERSIONMINOR 90
#define VERSIONREVISION 10

BOOL StartSemaphores(PPARAMS pparams);
BOOL StopSemaphores(PPARAM pparams);
BOOL CreateWindow(PPARAMS pparams);
BOOL DestroyWindow(PPARAMS pparams);
int main();
BOOL InstallMenuItem(HWND hwnd, USHORT id, SHORT pos, char *name);
MRESULT EXPENTRY WindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY DialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
void APIENTRY AnimateIcon(PPARAMS pparams);
BOOL UpdateIcon(PPARAMS pparams);
BOOL AddTrayIcon(PPARAMS pparams, HPOINTER hicon);
BOOL UpdateTrayIcon(PPARAMS pparams, HPOINTER hicon);
BOOL RemoveTrayIcon(PPARAMS pparams);
void APIENTRY Update(PPARAMS data);

#endif
