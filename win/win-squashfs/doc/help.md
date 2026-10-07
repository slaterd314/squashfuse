# SQUASHFS-Win #
## Squash filesystem for Windows ##
### The SquashFS Filesystem ###

SquashFS is a compressed read-only filesystem for Linux. 

#### About [FUSE] ####

FUSE (Filesystem in Userspace) is a software interface that allows userspace 
programs to implement a file system without needing to create a kernel driver.

#### About [Squashfuse] ####
The Squashfuse project is an implementation of the SquashFS filesystem using FUSE. 
This enables mounting a SquashFS filesystem is any OS that supports FUSE. 

#### About [WinFsp] ####
WinFsp is a Filesystem in Userspace that allows implementing filesystems in userspace 
for Windows in the same way that FUSE works for UNIX. WinFsp also provides a FUSE compatibility 
layer to enable porting FUSE based filesystems to Windows.

#### About SQUASHFS-Win ####
SQUASHFS-Win (this project) is a port of squashfuse to Windows using Microsoft Visual Studio and WinFsp.  
SQUASHFS-Win allows you to mount a SquashFS archive either using either Windows "Map Network Drive" command, or 
with the included command line tools.

#### Basic Usage with windows network redirector: ####
Map a squash file system archive as a network drive:  
`squashfs` is the server name. The path is the full path to 
your squash archive minus the `:`.    
For example, suppose I have an archive:   
`C:\mysysroots\arm-linux-gnueabihf.sqf`.    
I can mount it on drive `X:` by mapping `X:` to:    
`\\squashfs\C\mysysroots\arm-linux-gnueabihf.sqf`

![map network drive image][mapnetworkdrive]

Or from a command line prompt:

`> net use x: \\squashfs\C\mysysroots\arm-linux-gnueabihf.sqf`

### Command line tools: ###

Map Network Drive only allows you to mount an archive on a drive letter. However, the 
command line tools allow you to mount and unmount SquashFS archives on either a drive letter or 
on a directory. The command line tools are:

mount    

    Usage: mount [squashfs_file] [directory]
    mounts squashfs file [squashfs_file] on directory mount point [directory]
    Note: unlike Unix mount, [directory] must not exist.
    If run with no arguments, displays this help message and lists all mounted 
    filesystems.

umount    

    Usage: umount [directory]
    umounts squashfs file system mounted on [directory]

[mapnetworkdrive]: ./images/MapNetworkDrive.jpg "Map Network Drive"
[WinFsp]:https://winfsp.dev/ "WinFsp Site"
[Squashfuse]: https://github.com/vasi/squashfuse "Squashfuse Project Site"
[FUSE]: https://en.wikipedia.org/wiki/Filesystem_in_Userspace "FUSE Wiki Article"

