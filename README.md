#set8dot3fn

##Overview

8.3 file name is a named convention used by FAT file system. For compatibility with legacy program come from FAT era, OSes supported NTFS will distrribute an 8.3 file name to every files whose full name don't comply with 8.3 convention when 8dot3filename is enabled.

For purpose of project testing, I need to manually set the 8.3 file name in my work, but I can't find any tools can be used to do it in the Internet. So I write a utility set8dot3fn to complete it.


##Usage

Copy set8dot3fn.exe to the directory of the files you want to modify. Run CMD as administrator and execute set8dot3fn.exe.

     set8dot3fn <targetfile> <8dot3name>

 
##Example

 set8dot3fn AnTestFile.txt TESTTE~1.TXT

##Release:

Download from [here](http://gallery.technet.microsoft.com/A-utility-set8dot3fn-to-c53ef199)
