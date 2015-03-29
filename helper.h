#ifndef HELPER_H
#define HELPER_H

#define TEXT_ERROR "Fatal Error in Activity Monitor: error.log or POPUPLOG.OS2 may contain additional information."
#define TITLE_ERROR "Activity Monitor - Fatal Error"

ULONG DisplayErrorMsgBox(APIRET rc);
BOOL LoadProps(HAB hab, PPROPERTIES pProperties);
BOOL SaveProps(HAB hab, PPROPERTIES pProperties);

#endif
