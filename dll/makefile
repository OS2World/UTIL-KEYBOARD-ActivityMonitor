# makefile
# Created by IBM WorkFrame/2 MakeMake at 4:43:53 on 30 Sept 2000
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker
#  Lib::Import Lib

.SUFFIXES:

.SUFFIXES: \
    .C .LIB .dll .obj 

.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /DSTATIC_LINK /O /Gm /Gd /Ge- /Gt /C %s

{e:\data\c\actmon\dll}.C.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /DSTATIC_LINK /O /Gm /Gd /Ge- /Gt /C %s

.dll.LIB:
    @echo " Lib::Import Lib "
    implib.exe %|dpfF.LIB %s

{e:\data\c\actmon\dll}.dll.LIB:
    @echo " Lib::Import Lib "
    implib.exe %|dpfF.LIB %s

all: \
    .\actmon.LIB

.\actmon.dll: \
    .\actdll.obj \
    {$(LIB)}os2386.lib \
    {$(LIB)}os2286.lib \
    {$(LIB)}cppom30.lib \
    {$(LIB)}actmon.def
    @echo " Link::Linker "
    icc.exe @<<
     /B" /packd /optfunc /noe"
     /Feactmon.dll 
     os2386.lib 
     os2286.lib 
     cppom30.lib 
     actmon.def
     .\actdll.obj
<<

.\actdll.obj: \
    e:\data\c\actmon\dll\actdll.C \
    {e:\data\c\actmon\dll;$(INCLUDE);}actdll.h \
    {e:\data\c\actmon\dll;$(INCLUDE);}actdll-private.h

.\actmon.LIB: \
    .\actmon.dll
