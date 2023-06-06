# nuwon_hlpatch
this is the code for the 1.1.1.4/1.1.1.0 half-life client &amp; HLDS patch.  It currently only works when the client is in d3d or opengl video mode.
<br>
The client patch itself (test_hook) can either be injected into hl.exe while it is running (its been awhile since i touched this but i believe it needs to be injected before the engine; hw/sw dlls are loaded.)
<br><br>
this code was never supposed to see the light of day because of how messy and hard to read it is.  It is one of the first big coding projects I did. 
<br> Infact it is what lead to me starting NuWON.NET<br>
the serverhook should be injected after hlds is launched and the engine (swds.dll) is loaded.<br>
the client patch basically does this:<br>
1. adds sv_downloadurl cvar to the client and listen server's
2. add's a few of the missing clc/svs messages from the newer steam based hl engine
3. added the actual url downloading of files which used the built in progress bar as an indicator

<br>
the server patch just added the cvar and the svs/clc messages and also hooked into the resource list generator to make sure the client downloaded all the ncesary files. 

<br>
This is only the code for the hook dll for the 1.1.1.3/4 nuwon hl patch.  Alot of the fixes to the 1.1.1.0 half-life engine and hlds are actually done through hex editing.

<br> This code is uber messy, have fun
