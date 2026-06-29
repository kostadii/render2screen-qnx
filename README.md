# Text & Graphics Renderer through QNX SDP 8 Screen driver

Licensing:
Before all, you will need a valid QNX SW license to use this. Thankfully, SDP8 non-commercial licenses are available if you register. (Thanks Blackberry folk!)
This project is aimed mostly as an example under GNU 3.0. 

This project is aiming:
* Make a minimalistic frame-buffer renderer under QNX using the Screen driver interfaces.
* Minimal footprint in dependencies - fast start
* Can be used to display a basic "Loading ..." screen until a proper graphics engine using EGL/OpenGLES or similar has loaded enough resources to take over.
* Could be used to create a layer in a graphics stack, a fall-back graphics layer for "blue screen" messages, a basic HMI for rescue partition, flash loader progress screen, etc. 

Dependencies:
* Screen driver already running. (you should make use of 'waitfor /dev/screen' before starting. Otherwise bad things happen)
* Screen and display configuration files properly defined.
* imglib library and imglib conf file present and properly defined.
* freetype library present
* A freetype supported freetype. The default is the DejaVuSans.ttf that comes
* The usual support libraries - check current makefile -l / -L section for exact list

Runs on:
* The official Raspberry Pi 4 SDP8 image (aarch64le / QNX8.0)
* Should run on armv7 and/or QNX7.x, but not tested (no non-commercial licenses are available)
* Planning to test eventually on x86_64 / QNX8.0

How to build & debug this:
* Put somewhere Momentics can access.
* Edit the Make file to change the target from aarch64le (if needed).
* Edit the .cproject file to search & replace the target. You could do the same later through project properties in Momentics instead (if you have time to waste). 
* Import the project in Momentics workspace:
  1) "Import..."
  2) "Existing Project into Workspace".
  3) In the dialog click "Browse" next to "Select Root directory:" field. Select the root folder (one with .cproject in it)
  4) click Finish.
* Build
* Make sure the target is configured, accessable and has at least one font and image file to display.
* Debug/Run:
  - Setup a Launch Configuration: Click the gear next to the Launch Configuration on the toolbar. Punch in a name; In "Arguments" tab add all needed parameters like "-file=/tmp/image-to-display.jpg".
  - Selecr "Run" or "Debug" in "Launch Mode".
  - Select your lucky target in "on: 'Launch Target'" targets list.
* Run or debug
