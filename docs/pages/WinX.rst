WinX
====

About WinX
----------

WinX was the working name of WinXINU while it was in the development
stages.

Working in coordination with `Dr. Dennis
Brylow <http://www.mscs.mu.edu/~brylow/>`__ a senior design project was
set up to create a piece of software to allow the direct inclusion of
Windows based computers in the development and testing of XINU operating
systems. The goal is to create a front end program that has all the
functionally of current shell based front end tied into a graphical user
interface. The project codename, WinX, was installed at the beginning of
the implementation phase; ultimately the software may be released under
a different name for various legal reasons.

-  Working Objective Statement

   -  Design and implement a Windows program to allow Windows-based
      computers to participate directly in the MSCS Systems Lab for
      remote testing and execution of embedded operating system kernels
      by 28 April 2008 at a cost of $0.

-  Current developers include:

   -  Adam Koehler
   -  Nicholas McMillan
   -  Matthew Thomson
   -  Christopher Swiderski

Phases of WinX
--------------

-  Design

   -  The design phase spanned the whole first academic semester
      starting in September 2007 and ending in December 2007.

-  Implementation

   -  The implementation stage will span the whole second academic
      semester starting in January 2008 and ending in late April 2008.

-  Major Implemenation Phases

   -  Internal: January - Alpha Stage (Completed)
   -  Alpha: March 12, 2008 (Completed)
   -  Beta: [STRIKEOUT:April 9, 2008] April 16, 2008 (Active)
   -  Live: April 28, 2008

WinX Development
----------------

WinX implementation is an ongoing process and members of the software
development team can access the subversion repository at
http://xinu.mscs.mu.edu/svn/winx/ to view the current status of the
software.

WinXINU Testing
---------------

The team has uploaded several files for external beta testing of
WinXINU.

-  Step 1: Copy the WinXINU-Installer folder via SSH to your desktop:
   web location ~ http://www.mscs.mu.edu/~nmcmilla/WinXINU-Installer/
-  Step 2: Go into Cygwin Install Folder

   -  2a) Run Setup.exe
   -  2b) Select Install from Local - Folder is the only other folder in
      the Cygwin Install directory
   -  2c) Click ALL to set it to Uninstall all packages its at top of
      package selection screen
   -  2d) Click Devel package to set it to Install it is found within in
      the package listings

-  Step 3: Change the first line of the compiler/makeVars file (found in
   your XINU code directory) to the location of the WinXINU cross
   compiler bin

| ``  Example:``
| ``   MIPS_ROOT = "C:\Documents and Settings\nmcmilla\Desktop\winx\compiler\cross\bin"``

-  Step 4: Run WinXINU executable
-  Step 5: Navigate to config tab, change location of cygwin\\bin to
   location on local machine
-  Step 6: Navigate to config tab, change location of compile directory
   where the Makefile is located for final image build

WinXINU Testing Feedback
------------------------

| ``  Use a structure similar to the following:``
| ``   *name``
| ``   **Error/Comment Title``
| ``   ***Description of error/comment``

--------------

mschul
~~~~~~

-  Improve cygwin installation process.

   -  (Didn't work very well on a computer that had already been setup.)
      - Outside scope, no its not
   -  Get rid of stuff in devel package that is unnecessary

compile pane
^^^^^^^^^^^^

-  [STRIKEOUT:I should be able to click 'make' or 'make clean' whenever
   a damn well feel like.] **Fixed: 4/19/08 by cswiders**

   -  [STRIKEOUT:and there should be a mechanism for making other
      targets as well.] - Outside scope

-  [STRIKEOUT:Implement 'Errors and Warnings' checkbox.] **Fixed:
   4/19/08 by cswiders**

xinu-status
^^^^^^^^^^^

-  Perhaps only query morbius at most once per second (disabling the
   'update now' button for a second)

xinu interaction
^^^^^^^^^^^^^^^^

-  [STRIKEOUT:Connect to backend, select mips-console, program complains
   and goes on anyway. No good.] **Fixed: 4/21/08 by nmcmilla** - Checks
   whether connected to backend or not, if connected, continues with
   process, if not, then runs normally.
-  [STRIKEOUT:Run 'testsuite' and compare output to linux version
   (ignoring the VT100 thing).] **Fixed: 4/16/08 by akoehler**

   -  [STRIKEOUT:also typing a single \\ should produce a single slash.
      Not typing \\\\ to produce] **Fixed: 4/22/08 by akoehler**

-  [STRIKEOUT:Verify \*ALL\* control characters are not printed.]
   **Fixed: 4/16/08 by akoehler**
-  [STRIKEOUT:Is there any way that hitting backspace won't jump to the
   top and back down?]
-  [STRIKEOUT:Something with 'mips-console' and xinu just going,
   seems...odd.]

   -  [STRIKEOUT:if the download fails it still keeps on a truckin'.]
      **Fixed: 4/22/08 by akoehler**

-  [STRIKEOUT:Continue session doesn't continue session until a client
   side character is typed.] '''Fixed: 4/22/08 by nmcmilla '''

   -  [STRIKEOUT:Also, I can backspace over previous characters after
      typing ctrl-space.] '''Fixed: 4/22/08 by nmcmilla '''
   -  [STRIKEOUT:Perhaps, ctrl-space should also be the continue code
      (once will pause, twice will continue).] '''Fixed: 4/22/08 by
      nmcmilla '''

-  [STRIKEOUT:Clear history in the interaction tabs (or even have a
   value to specify lines of scrollback).] **Fixed: 4/18/08 by
   akoehler**
-  [STRIKEOUT:xsh$ clear (makes WinX go boom).] **Fixed: 4/16/08 by
   akoehler**
-  'Command request timed out' ... clearly it did not.

serial port 2
^^^^^^^^^^^^^

-  [STRIKEOUT:Should have option to connect to same backend2 name as
   serial port 1 (if connected on 1)]. **Fixed: 4/18/08 by akoehler**
-  [STRIKEOUT:Clicking cancel in the select specific backend2 should not
   produce an error message.] **Fixed: 4/17/08 by nmcmilla**
-  [STRIKEOUT:Connect to preferred when already connected produces no
   warnings.] **Fixed: 4/17/08 by nmcmilla**
-  [STRIKEOUT:Connecting to backend2 hops to the top of the textbox.]
   **Fixed: 4/21/08 by nmcmilla**
-  [STRIKEOUT:Is there any way that hitting backspace won't jump to the
   top and back down?] - NO
-  [STRIKEOUT:Ctrl-space should work here too. And make sure continue
   session still works on this pane.]\ **Fixed: 4/21/08 by nmcmilla**

config
^^^^^^

-  [STRIKEOUT:New user profile should auto-set in config after
   creation.] **Fixed: 4/18/08 by akoehler**

GUI
^^^

-  [STRIKEOUT:The color seems funky (tab color doesn't match background
   color).]
-  Needs icon.
-  [STRIKEOUT:Needs About.]\ **Fixed 4/23/08 by mthomson**
-  Needs 'command descriptions'
-  Possibly needs a quick start guide in the help menu.-should be
   covered on the Wiki
-  [STRIKEOUT:No keybindings for 'New Profile,' 'Save Profile,'
   etc.]\ **Fixed 4/21/08 by nmcmilla**

--------------

