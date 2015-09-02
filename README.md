# CoSoS - Cousin of Son of Strike
A [WinDbg](https://msdn.microsoft.com/en-us/library/windows/hardware/ff551063(v=vs.85).aspx) extension to visualize 32bit native heap and CLR heap in a similar manner to vmmap.

* Tested with WinDbg 10.0 x86
* Does not work with 64bit dumps.
* Requires [SoS](https://msdn.microsoft.com/en-us/library/bb190764(v=vs.110).aspx) extension to be loaded.
* Works with dump files and live debugging session.

Usage:

* .load cosos
* !gcview *shows the heap map in a Qt5.5 window.*

![gcview Qt window](https://github.com/krk/cosos/blob/master/images/gcview%20example.png) 

* !gcview c:\images\dump-101 *saves native heap as dump-101.png and GC heap as dump-101-gc.png*

![gcview dump-101](https://github.com/krk/cosos/blob/master/images/dump101.png) 
![gcview dump-101-gc](https://github.com/krk/cosos/blob/master/images/dump101-gc.png) 