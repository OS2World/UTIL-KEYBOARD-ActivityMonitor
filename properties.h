#ifndef PROPERTIES_H
#define PROPERTIES_H

#define ERRORTEXT_PROPERTIESFAILED "Failed to correctly open the properties dialog!"

MRESULT EXPENTRY DialogProc_Properties(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL InitDlg(HWND hwnd);

#endif
