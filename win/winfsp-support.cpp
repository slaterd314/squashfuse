/*
 * Copyright (c) 2019 Daniel R. Slater <6516534+slaterd314@users.noreply.github.com>
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// forward declare IUnknown or we get a compile error out of <combaseapi.h>
struct IUnknown;

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <winternl.h>
#pragma warning(push)
#pragma warning(disable:4005)           /* macro redefinition */
#include <ntstatus.h>
#pragma warning(pop)
/* #include <winfsp/winfsp.h> */
#include <signal.h>
#include <process.h>

#if WIN_FUSE==1
#define FSP_LIBPATH L"opt\\libfuse\\bin\\"
#define INSTALL_KEY L"Software\\WinFuse"
#define FSP_DLLNAME L"fuse3-x64.dll"
#else
#define FSP_LIBPATH L"bin\\"
#if defined(_WIN64)
#define INSTALL_KEY L"Software\\WOW6432Node\\WinFsp"
#define FSP_DLLNAME L"winfsp-x64.dll"
#else
#define INSTALL_KEY L"Software\\WinFsp"
#define FSP_DLLNAME L"winfsp-x86.dll"
#endif
#endif

extern "C"
{
#include "../squashfuse.h"

	static NTSTATUS FspLoad(PVOID *PModule)
	{
#define FSP_DLLPATH                     FSP_LIBPATH FSP_DLLNAME

		WINADVAPI
			LSTATUS
			APIENTRY
			RegGetValueW(
				HKEY hkey,
				LPCWSTR lpSubKey,
				LPCWSTR lpValue,
				DWORD dwFlags,
				LPDWORD pdwType,
				PVOID pvData,
				LPDWORD pcbData);

		WCHAR PathBuf[MAX_PATH];
		DWORD Size;
		HKEY RegKey;
		LONG Result;
		HMODULE Module;

		if (0 != PModule)
			*PModule = 0;

		Module = LoadLibraryW(L"" FSP_DLLNAME);
		if (0 == Module)
		{
			Result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, INSTALL_KEY,
				0, KEY_READ , &RegKey);
			if (ERROR_SUCCESS == Result)
			{
				Size = sizeof PathBuf - sizeof L"" FSP_DLLPATH + sizeof(WCHAR);
				Result = RegGetValueW(RegKey, 0, L"InstallDir",
					RRF_RT_REG_SZ, 0, PathBuf, &Size);
				RegCloseKey(RegKey);
			}
			if (ERROR_SUCCESS != Result)
				return STATUS_OBJECT_NAME_NOT_FOUND;

			RtlCopyMemory(PathBuf + (Size / sizeof(WCHAR) - 1), L"" FSP_DLLPATH, sizeof L"" FSP_DLLPATH);
			Module = LoadLibraryW(PathBuf);
			if (0 == Module)
				return STATUS_DLL_NOT_FOUND;
		}

		if (0 != PModule)
			*PModule = Module;

		return STATUS_SUCCESS;

#undef FSP_DLLNAME
#undef FSP_DLLPATH
	}

	/* delay-load winfsp dynamic link library. */
	/* return 0 on success. */
	int sqfs_load_winfsp()
	{

		if (!NT_SUCCESS(FspLoad(0)))
			return (ERROR_DELAY_LOAD_FAILED);
		return 0;

	}
	int glb__WinFspLoaded = sqfs_load_winfsp();

	static void die(const char *msg) {
		fprintf(stderr, "%s\n", msg);
		exit(1);
	}

	char *make_full_path(const char *path1, const char *path2) {
		if (*path1 == '/') {
			return _strdup(path1);
		}
		else {
			const char *ptr = path2;
			if (*ptr != '/') {
				ptr = strchr(ptr, '/');	/* skip over the extraction root dir */
				char dir[_MAX_DIR];
				if (0 == _splitpath_s(ptr, NULL, 0, dir, _MAX_DIR, NULL, 0, NULL, 0)) {

					size_t len = strlen(dir) + strlen(path1) + 1;
					char *buf = (char *)malloc(len);
					strcpy_s(buf, len, dir);
					strcat_s(buf, len, path1);
					return buf;

				}
			}
		}
		die("_splitpath_s");
		return NULL;
	}

	/* 
	* On Windows, we need to pass in the sqfs *fs filesystem so we can determine if the 
	* target is a directory or not, so we can set the SYMBOLIC_LINK_FLAG_DIRECTORY flag when 
	* creating the symlink. 
	* squashfuse_extract uses this function to create symlinks when extracting files from the squashfs image.
	* and the symbolic links can be extracted before the target is created.
	*/
	int sqfs_symlink(sqfs *fs, const char *path1, const char *path2)
	{
		sqfs_inode inode = { 0 };
		DWORD flags = 0;
		char *fullPath = make_full_path(path1, path2);
		bool found = false;
		sqfs_err err = sqfs_inode_get(fs, &inode, sqfs_inode_root(fs));
		if (err == err ) {
			err = sqfs_lookup_path(fs, &inode, fullPath, &found);
			free(fullPath);
			if (SQFS_OK == err && true == found) {
				if (S_ISDIR(inode.base.mode)) {
					flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;
				}
			}
		}
		BOOLEAN success = CreateSymbolicLinkA(path2, path1, flags);
		if (!success) {

		}
		return (success != 0 ? 0 : -1);
	}


#if 0
	struct Args {
		int argc;
		char **argv;
	};

	static unsigned thread_proc(void *p)
	{
		extern int t_main(int argc, char **argv);
		if (p) {
			struct Args *pArgs = (struct Args *)p;
			return t_main(pArgs->argc, pArgs->argv);
		}
	}

	NTSTATUS SvcStart(FSP_SERVICE *Service, ULONG wargc, PWSTR *wargv)
	{
		FspServiceAllowConsoleMode(Service);
		struct Args *tArgs = (struct Args *)Service->UserContext;
		if (tArgs) {
			// _beginthreadex(NULL, 0, thread_proc, tArgs, 0, NULL);
			thread_proc(tArgs);

			return STATUS_SUCCESS;
		}
		else {
			return STATUS_ACCESS_DENIED;
		}
	}

	NTSTATUS SvcStop(FSP_SERVICE *Service)
	{
		raise(SIGTERM);
		return STATUS_SUCCESS;
	}

	int wmain(int wargc, wchar_t **wargv)
	{
		int argc = (int)wargc;
		size_t len = 0;

		for (int i = 0; i < argc; ++i) {
			len += wcslen(wargv[i]) + 1;
		}

		size_t alloc_len = len + argc * sizeof(char *);
		char **argv = (char **)malloc(alloc_len);
		char *data = (char *)(argv + argc);
		for (int i = 0; i < argc; ++i) {
			argv[i] = data;
			WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, data, alloc_len, NULL, NULL);
			data += strlen(argv[i]) + 1;
			alloc_len -= strlen(argv[i]) + 1;
		}

		struct Args *tArgs = (struct Args *)malloc(sizeof(struct Args));
		tArgs->argc = argc;
		tArgs->argv = argv;

		return FspServiceRunEx((PWSTR)L"squashfs", SvcStart, SvcStop, 0, tArgs);
	}

#endif
}
