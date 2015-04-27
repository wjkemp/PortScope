#Overview

PortScope is a Serial Port Diagnostics tool that runs on Windows. It is similar to PortMon from SysInternals in that it installs a filter driver on top of the serial port driver to intercept data, but I am hoping to provide a UI that is more protocol-orientated. The current design allows users to write their own plugins that can be layered on top of each other to provide an in-depth view of the protocol being used over the serial link.

#Disclaimer

PortScope contains a Windows Kernel Mode driver that has not been WHQL-tested, and that has the ability to crash your entire OS (and it probably will at some point, it's a work in progress and it still needs a lot of testing). I accept absolutely no responsibility for any data loss or damage that occurs from using it, you do so at your own risk. 
