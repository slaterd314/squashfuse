#ifndef UNISTD_H_INCLUDED
#define UNISTD_H_INCLUDED

#include <stdint.h>
#include <io.h>
#include <fcntl.h>

typedef uint64_t fsblkcnt_t;
typedef uint64_t fsfilcnt_t;

struct statvfs {
	uint64_t  f_bsize;    		/* Filesystem block size */
	uint64_t  f_frsize;   		/* Fragment size */
	fsblkcnt_t     f_blocks;   /* Size of fs in f_frsize units */
	fsblkcnt_t     f_bfree;    /* Number of free blocks */
	fsblkcnt_t     f_bavail;   /* Number of free blocks for
								 unprivileged users */
	fsfilcnt_t     f_files;    /* Number of inodes */
	fsfilcnt_t     f_ffree;    /* Number of free inodes */
	fsfilcnt_t     f_favail;   /* Number of free inodes for
								 unprivileged users */
	uint64_t  f_fsid;     		/* Filesystem ID */
	uint64_t  f_flag;     		/* Mount flags */
	uint64_t  f_namemax;  		/* Maximum filename length */
};

#define O_WRONLY _O_WRONLY

#endif // UNISTD_H_INCLUDED
