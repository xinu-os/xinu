WinXinu Installation
====================

System Requirements
-------------------

-  Operating System: Windows XP or Windows Vista
-  Ability to connect to XINU server
-  Microsoft .Net Framework v3.0 or higher
-  `Cygwin <http://www.cygwin.com>`__ w/ Development package
-  HDD Space

   -  60 megabytes (WinXINU)
   -  1310 megabytes (Cygwin)
   -  Total ~1.4 gigabytes

Installation Instructions
-------------------------

Installation Guide
~~~~~~~~~~~~~~~~~~

-  `WinXINU + Cygwin Installation & Configuration
   Guide <http://xinu.mscs.mu.edu/wiki/images/9/93/WinXINU-install_guide.pdf>`__

Installing Cygwin
~~~~~~~~~~~~~~~~~

-  Step 1: Go to `Cygwin.com <http://www.cygwin.com/>`__
-  Step 2: Download the installation file, setup.exe
-  Step 3: Run Cygwin Setup

   -  Note: During setup, the only package that needs to be set to
      install is the devel package; all others can be set to uninstall
      or default

Installing WinXINU
~~~~~~~~~~~~~~~~~~

-  Step 1: Download the
   `setup <http://www.mscs.mu.edu/~brylow/xinu/WinXINU-setup.exe>`__
   file
-  Step 2: Run the WinXINU setup file and follow the `Instruction
   Gude <http://xinu.mscs.mu.edu/wiki/images/9/93/WinXINU-install_guide.pdf>`__

Program Setup Instructions
--------------------------

-  Modify the make file of your code

   -  Change the first line of the mipsVars file to the location of the
      WinXINU cross compiler bin

      -  NOTE: this should be found within your XINU code directory in
         the compile subdirectory
      -  e.g. xinu/compile/mipsVars

| ``  Example:``
| ``   MIPS_ROOT = "C:/WinXINU/cross/bin"``

-  Modify the server settings XML file

   -  Ensure the server IP is correct
   -  Ensure that the server port number is correct
   -  Ensure that the IP address in the automation command is correct

-  Modify the configuration profile settings within the program

   -  Navigate to the config tab, then change:

      -  the location of cygwin\\bin to its location on local machine
      -  the location of compile directory where the Makefile is located
         for final image build


