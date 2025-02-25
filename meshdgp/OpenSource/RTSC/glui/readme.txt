Welcome to the GLUI User Interface Library, v2.3bb!
A forked GLUI codebase with modifications by Bill Baxter
March 8, 2005
-------------------------------------------------

This distribution contains Bill Baxter's fork of the GLUI Library.  It
is based on the GLUI v2.1 beta version from Paul Rademacher 
(http://www.cs.unc.edu/~rademach/glui/)
plus the minimal changes made by Nigel Stewart in his "GLUI v2.2"
(http://www.nigels.com/glt/glui)  In accordance with the LGPL under
which the library is released (according to Paul's web page at least),
I'm making these changes available to the community.

WARNING: This version introduces some incompatible changes with
previous versions.

CHANGES:

----------------------------------
- GLUI_String is now a std::string
  This is the main source of most incopatibilities, but I felt it was
  a necessary change, because the previous usage of a fixed-sized
  buffer was just too unsafe.  I myself was bitten a few times passing
  a char* buffer of insufficient size into GLUI as a live variable.
  It is still possible to use a char buffer, but it is not recommended.

  If you used GLUI_String before as a live var type, the easiest way
  to get your code compiling again is to change those to "char
  buf[300]".  The better way, though, is to update your code to treat
  it as a std::string.

  For instance, if you used to pass mystr to functions that take
  'const char*', now use mystr.c_str() method, instead.
  If you used strcpy(mystr, b) to set the value, now just do mystr=b.
  If you used sprintf(mystr,...) to set the value, now do 
  glui_format_string(mystr,...).
  If you used to clear the string with mystr[0]='\0', now just clear
  it with mystr="".

----------------------------------
- Enhanced GLUI_EditText
  Control keys can be used for navigation and control.  The bindings
  are bash-like: Ctrl-B for previous char, Ctrl-F for forward char, etc.
  bindings.  Also control keys that aren't bound to commands are
  simply ignored, whereas before they would be inserted as invisible
  characters.

----------------------------------
- Added GLUI_CommandLine class
  This is a GLUI_EditText with a history mechanism.

----------------------------------
- New, more object oriented construction API.
  Now instead of calling 

    glui->add_button_to_panel( panel, "my button", myid, mycallback );

  you should just call the button constructor:

    new GLUI_Button( panel, "my button", myid, mycallback );

  And similarly to add it to a GLUI instead of a panel, rather than:

    glui->add_button_to_panel( panel, "my button", myid, mycallback );

  just call the constructor with the GLUI as the first argument:

    new GLUI_Button( glui, "my button", myid, mycallback );
    
  The old scheme is now deprecated, but still works.  The benefit of
  this new scheme is that now the GLUI class doesn't have to know
  about all the different types of GLUI_Controls that exist.
  Previously GLUI had to both know about all the controls, and know
  how to initialize them.  Now the responsibility for initialization
  belongs to the GLUI_Control subclasses themselves, where it
  belongs. Additionally it means that you can create your own
  GLUI_Control subclasses which will be on equal footing with the
  built-in controls, whereas before any user-created controls would
  always be "second-class citizens" since they would have to be
  constructed differently from the built-ins.


----------------------------------
- Removed need for type-declaring arguments when argment type suffices.
  This effects GLUI_Spinner and GLUI_EditText (and GLUI_CommandLine?).

  For example, instead of calling 

    new GLUI_Spinner( glui, "myspin", GLUI_SPINNER_INT, &live_int_var );

  you can just omit the GLUI_SPINNER_INT part, because the type of the
  live_int_var tells the compiler which type you want.

    new GLUI_Spinner( glui, "myspin", &live_int_var );

  If you're not using a live, var, you can still use the
  GLUI_SPINNER_INT type argument.  See glui.h for all the new 
  constructor signatures.  Note this only works with the new
  construction API, not with the old "add_blah_to_panel" style of
  API.

----------------------------------
- GLUI_Rotation uses your matrix live-variable now.
  GLUI used to ignore the matrix in your live variable.  This version
  doesn't ignore it, so you'll need to set it to the identity matrix
  yourself if that's what you want it to start as.  There could
  probably be some improvements to this API, though.
  
----------------------------------
- Improvements to 'const' usage.
  Most char*'s in GLUI functions used to be non-const even when the
  functions did not modify the string.  I changed everywhere
  appropriate to use const char* instead.

----------------------------------
- Updated license info in the headers
  Paul's web page says that GLUI is LGPL, but that wasn't declared in
  the code itself.  I've modified all the headers with the standard
  LGPL notice.

----------------------------------
- Updated examples for the API changes

----------------------------------
- Created project files for Visual Studio .NET (MSVC7.1)


That's about it.  Enjoy!


If you find yourself with too much time on your hands, the things I
think would be most useful for future improvements to GLUI would be:

1. Remove the dependency on GLUT.
2. Allow for an enhanced callback type that also passes you a pointer
   to the control and possibly the control's value.  Currently the
   "live variable" is great if you don't need to do anything else as a
   side effect, but if you need to take some action based on a changed
   value, then you need BOTH a callback, AND the live var, or a global
   pointer to the control.  If the callback provided more arguments
   then the callback would be all that you need for those cases.
3. Clipboard integration under Windows/X-Win.


I don't think Paul or Nigel are doing much with GLUI any more so their
mail addresses probably won't be much help.  Honestly, I'm probably
not going to do much with it either.  Feel free to contact me, but
I'll also feel free to ignore you.  :-)

Bill Baxter
baxter
cs unc edu
(Put the above two lines together with an at-mark, and join the second
line with dots to email me.)


=================================================
PAUL'S ORIGINAL README
=================================================

Welcome to the GLUI User Interface Library, v2.0 beta!
-------------------------------------------------

This distribution contains the full GLUI sources, as well as 5 example
programs.  You'll find the full manual under "glui_manual.pdf".  The
GLUI web page is at 

	http://www.cs.unc.edu/~rademach/glui


		    ---------- Windows ----------

The directory 'msvc' contains a Visual C++ workspace entitled
'glui.dsw'.  To recompile the library and examples, open this
workspace and run the menu command "Build:Batch Build:Build".  The 3
executables will be in the 'bin' directory, and the library in the
'lib' directory.

To create a new Windows executable using GLUI, create a "Win32 Console
Application" in VC++, add the GLUI library (in 'msvc/lib/glui32.lib'),
and add the OpenGL libs:

	glui32.lib glut32.lib glu32.lib opengl32.lib   (Microsoft OpenGL)

Include the file "glui.h" in any file that uses the GLUI library.


		      ---------- Unix ----------

An SGI/HP makefile is found in the file 'makefile' (certain lines may need 
to be commented/uncommented).

To include GLUI in your own apps, add the glui library to your
makefile (before the glut library 'libglut.a'), and include "glui.h"
in your sources.



----------------------------------------------------------------------

Please let me know what you think, what you'd like to change or add,
and especially what bugs you encounter.  Also, please send me your
e-mail so I can add you to a mailing list for updates.

Good luck, and thanks for trying this out!

Paul Rademacher
rademach@cs.unc.edu