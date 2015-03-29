Activity Monitor 1.0
Copyright 1999, 2000 D.J. van Enckevort

This file was last edited on September 18, 2000

Table of Contents
=================
1.0 Introduction
2.0 License Agreement
3.0 Installation
4.0 Configuration
5.0 Known bugs and limitations
    5.1 Limitations
    5.2 Bugs
6.0 Reporting bugs and suggestions
7.0 Program history
8.0 Thanks

1.0 Introduction
================
Activity Monitor is a program to monitor and log the use of the keyboard and
mouse. It can take action based upon the state of use of mouse and keyboard.
The program is based on the program KeySpy also written by me.
Since Activity Monitor only uses documented functions (though some are hardly
documented) and does not use any special device driver it can not be trusted
to be accurate. You should take this into consideration if you use the program.
Activity Monitor is distributed as a WarpIN package. I am grateful to the OS/2
Netlabs team for creating this installation tool.

2.0 License Agreement
=====================
This program is published under the GNU public license. A copy of it is
included in the file COPYING

3.0 Installation
================
To install Activity Monitor you need to have WarpIN installed. If you haven't
got it you can find the latest version at http://warpin.netlabs.org
Installation of WarpIN is really easy. Just type the following commands:

unzip warpin-0-9-4.zip -d c:\warpin
c:
cd \warpin
warpin

In this example I assumed warpin-0-9-4.zip is the warpin package and that you
want to install it in c:\warpin. If any is different, just change the commands
respectively. If you don't have unzip you can download it from Hobbes:

http://hobbes.nmsu.edu

If you have done all this you can simply run actmon10.exe and it will install
the program on your computer. The installation program will ask you several
questions. I don't think they are hard to answer but I will discuss them here
briefly.
1. Packages to install:
There is only one package to install and it will be automatically selected. On
the bottom of the screen you can select the location to install to:
2. Location:
On the same page as the package you can select the location to install the
program. Any directory on a local (i.e. not network) drive will be okay.


4.0 Configuration
=================
Activity Monitor can be configured through its properties dialog. You can find
the properties dialog as the first item in the System Menu after you started
Activity Monitor.
In the dialog you can configure Activity Monitor to write a log file. It will always
be written in the current directory and have the name actmon.log. If you want
Activity Monitor to write a logfile you should also specify a name for your computer
(for example use your TCP/IP hostname). This name has a maximum of 32
characters. The logfile can be used to analyse the settings you made for the
'keyboard use', 'mouse use' and 'status'. It has the following layout:

YYYYMMDD HHMMSS Name Keyb Mouse Status

Keyb and Mouse show you the number of WM_CHAR and WM_MOUSEMOVE events in an one
minute interval.

Status shows you the value of the Status flag.

The status flag is incremented or decremented each minute on basis of the
following rules:

No keyboard input: -2
Keyboard input below the Keyboard Low value: + 1
Keyboard input between the Keyboard Low and High values: +2
Keyboard input above the Keyboard High value: +3
No mouse input: -2
Mouse input below the Mouse Low value: +1
Mouse input between the Mouse Low and High values: +2
Mouse input above the Mouse High value: +3

The Keyboard Low and High values are set in the properties dialog in the
"keyboard use" box. The Low value is between GREEN en YELLOW the High value
is between YELLOW and RED. In the same way the Mouse Low and High values are
set in the "mouse use" box.

If the Status has a value below the value between GREEN and YELLOW in the
"Status" box in the properties dialog, Activity Monitor is coloured GREEN. If its
value is between this value and the value set between YELLOW and RED in the
"Status" box it is coloured YELLOW. Above the value between YELLOW and RED
Activity Monitor is coloured RED. If the checkbox "Beep on status RED" is checked
Activity Monitor will also give a one second long beep every minute.

All settings you make are saved in the OS/2 User INI file.

5.0 Known bugs and limitations
==============================
5.1 Limitations
---------------
* The program does not record mouse activity in OS/2 Fullscreen sessions.
* DOS and WinOS/2 fullscreen sessions aren't logged at all.

5.2 Bugs
--------
BUG: OS/2 Lockup function interferes with correct mouse monitoring
Explanation: The OS/2 Lockup function moves the mouse cursor around the screen.
             and thus produces WM_MOUSEMOVE window messages, which are counted
             by KeySpy.
Workaround:  Use another screensaver
Notes:       In the near future Activity Monitor will be able to start deskpic
             screensaver modules


6.0 Reporting bugs and suggestions
==================================
To report bug please send an e-mail to van.enckevort@hccnet.nl
Please specify the OS version, Activity Monitor version and include the logfiles
(.log) plus the output of pstat.


7.0 Program history
===================
18/03/1999: Release of KeySpy 1.0
../../2000: Release of ActMon 1.0


8.0 Thanks
==========
There are many people who serve thanks for helping me out while writing this program.
Here I would especially thank the following people:
* Randell Flint of Sundial Systems for all his tips and help with programming issues
* Everyone at Mensys for testing Activity Monitor
* Everyone who replied to my messages in comp.os.os2.developer.misc
* Everyone on the device driver mailing list who kindly tolerated my abuse of this mailing
  list for general OS/2 PM programming questions.
* My brother for introducing me to OS/2 and for lending me his copy of Real-World Programming
  for OS/2 2.1 (published in 1993 and _STILL CURRENT_ (is there any book on Windows of which
  you can say that!)
* The authors of Real-World Programming for OS/2 2.1: Derrel R. Blain, Kurt R. Delimon &
  Jeff English for writing this book that covers some of the parts of OS/2 Programming that
  were out of the scope of Michael Drapkins' "OS/2 Warp Presentation Manager Mentor"
* Michael Drapkin for his excellent introduction to OS/2 PM programming: "OS/2 Warp
  Presentation Manager Mentor"
* EDM/2 & its editors and the authors of its articles for this excellent source of OS/2
  PM Programming information.
* Everyone I accidently left out in this list
* ehm, maybe IBM for creating OS/2 and VAC++?
