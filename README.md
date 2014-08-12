Guttenberg-model
================
Python implementation by Norman Packard

Aug 2014

Currently opens /tmp/activity for writing activity results each time
step, suitable for writing to the pipe of the same name created by
graphactivity.py

To do:

1.  reorganize so that instead of starting evolutionary program separately from graphactivity.py, the evolutionary program is passed to graphactivity.py on the command line, and lauched by that script.  

2.  eliminate flat line clutter

3.  add graph windows for  total activity vs. time and new activity vs. time.

