# GraphicsServices
A reimplementation of OS X's Event Pump in the style of GraphicsServices.

# Why
OS X, like any modern operating system, has two faces: The kernel and userland.
While the kernel is written in tight, modular, K&R-style C, userland is
a hodge-podge of decisions [read: mistakes] made decades ago that still have not
been cleaned up.  Notably, though we are not allowed to interact with Carbon and
its constituent sub-frameworks, Carbon has been kept alive on all of our systems
and is the driving force behind integral UI components like voiceover, print
panels, status bar windows, and event handling.  Carbon's influence is so broad
even AppKit must delegate to it frequently for platform behaviors.

Carbon controls far too much, and interacting with it in some form remains
a necessary evil if one wishes to get an app off the ground and into the dock.
That is, frankly, unacceptable.  We can do better, we must do better!  In that
spirit this framework draws upon the essence of 
[GraphicsServices](http://iphonedevwiki.net/index.php/GraphicsServices.framework), 
iOS' eventing and high-level graphics extension library to redo the fundamental
event pump machinery in OS X.

