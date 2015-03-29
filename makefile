# makefile
# Created by IBM WorkFrame/2 MakeMake at 23:40:30 on 20 Sept 2000
#
# The actions included in this make file are:
#  Compile::Resource Compiler
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind

.SUFFIXES:

.SUFFIXES: \
    .c .obj .rc .res 

.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|fF.RES

{e:\data\c\actmon}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|fF.RES

.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Tdp /O /Gm /Gt /C %s

{e:\data\c\actmon}.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Tdp /O /Gm /Gt /C %s

all: \
    .\actmon.exe

.\actmon.exe: \
    .\helper.obj \
    .\properties.obj \
    .\actmon.obj \
    .\actmon.res \
    {$(LIB)}os2386.lib \
    {$(LIB)}cppom30.lib \
    dll\actmon.lib \
    {$(LIB)}actmon.def
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /packd /optfunc"
     /Feactmon.exe 
     os2386.lib 
     cppom30.lib 
     dll\actmon.lib 
     actmon.def
     .\helper.obj
     .\properties.obj
     .\actmon.obj
<<
    rc.exe .\actmon.res actmon.exe

.\actmon.res: \
    e:\data\c\actmon\actmon.rc \
    {e:\data\c\actmon;$(INCLUDE)}RedN.ICO \
    {e:\data\c\actmon;$(INCLUDE)}RedO.ICO \
    {e:\data\c\actmon;$(INCLUDE)}RedM.ICO \
    {e:\data\c\actmon;$(INCLUDE)}RedT.ICO \
    {e:\data\c\actmon;$(INCLUDE)}RedC.ICO \
    {e:\data\c\actmon;$(INCLUDE)}RedA.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowN.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowO.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowM.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowT.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowC.ICO \
    {e:\data\c\actmon;$(INCLUDE)}YellowA.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenN.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenO.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenM.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenT.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenC.ICO \
    {e:\data\c\actmon;$(INCLUDE)}GreenA.ICO \
    {e:\data\c\actmon;$(INCLUDE)}actmon.ico \
    {e:\data\c\actmon;$(INCLUDE)}dialog.dlg \
    {e:\data\c\actmon;$(INCLUDE)}dialog.h \
    {e:\data\c\actmon;$(INCLUDE)}actmon.h

.\helper.obj: \
    e:\data\c\actmon\helper.c \
    dll\actdll.h \
    {e:\data\c\actmon;$(INCLUDE);}actmon.h \
    {e:\data\c\actmon;$(INCLUDE);}helper.h \
    {e:\data\c\actmon;$(INCLUDE);}properties.h \
    {e:\data\c\actmon;$(INCLUDE);}dialog.h

.\actmon.obj: \
    e:\data\c\actmon\actmon.c \
    dll\actdll.h \
    {e:\data\c\actmon;$(INCLUDE);}actmon.h \
    {e:\data\c\actmon;$(INCLUDE);}helper.h \
    {e:\data\c\actmon;$(INCLUDE);}properties.h \
    {e:\data\c\actmon;$(INCLUDE);}dialog.h

.\properties.obj: \
    e:\data\c\actmon\properties.c \
    dll\actdll.h \
    {e:\data\c\actmon;$(INCLUDE);}actmon.h \
    {e:\data\c\actmon;$(INCLUDE);}helper.h \
    {e:\data\c\actmon;$(INCLUDE);}properties.h \
    {e:\data\c\actmon;$(INCLUDE);}dialog.h
