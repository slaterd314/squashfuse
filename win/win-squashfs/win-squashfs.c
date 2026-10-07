/*
 * Copyright (c) 2020 Daniel R. Slater <dslater38@gmail.com>
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

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

static char *sqfs_parse_unc_path(const char *arg);
// static char *squashfuse_path();

// const char *PROGNAME = "d:\\Dev\\squashfuse\\win\\x64\\Debug\\squashfuse.exe";
const char *VOLUMEPREFIX = "--VolumePrefix=%s";

#define SQUASHFS_ARGS                      \
    "-orellinks",                       \
    "-ofstypename=squashfs",               \
    "-oUserKnownHostsFile=/dev/null",   \
    "-oStrictHostKeyChecking=no"

static void log_cmdline(int argc, const wchar_t **argv)
{
	FILE *fp = NULL;
	auto err = _wfopen_s(&fp,L"c:\\Users\\dslat\\win-squashfs.log", L"a+");
	if (fp)
	{
		for (int i = 0; i < argc; ++i)
		{
			fwprintf(fp,L"%s ", argv[i]);
		}
		fwprintf(fp,L"\n");
		fflush(fp);
		fclose(fp);
	}
}

static void log_cmdlineA(int argc, const char **argv)
{
	FILE *fp = NULL;
	auto err = fopen_s(&fp, "c:\\Users\\dslat\\squashfuse.log", "a+");
	if (fp)
	{
		for (int i = 0; i < argc; ++i)
		{
			fprintf(fp, "%s ", argv[i]);
		}
		fprintf(fp, "\n");
		fflush(fp);
		fclose(fp);
	}
}

int ansi_main(int argc, char **argv)
{
	if (argc >= 3) {
		char buffer[256];
		const char *unc_path = argv[1];
		const char *drive = argv[2];
		char *volumePrefix = NULL;

		//char *PROGNAME = squashfuse_path();
		//char FNAME[_MAX_FNAME];
		//char EXT[_MAX_EXT];
		//_splitpath_s(PROGNAME, NULL, 0, NULL, 0, FNAME, _MAX_FNAME, EXT, _MAX_EXT);
		//char ProgName[_MAX_FNAME + _MAX_EXT + 1];
		//strcpy_s(ProgName, sizeof(ProgName), FNAME);
		//strcat_s(ProgName, sizeof(ProgName), EXT);

		char *archivePath = sqfs_parse_unc_path(unc_path);
		if(NULL != archivePath) {
			sprintf_s(buffer, sizeof(buffer), VOLUMEPREFIX, unc_path);
			volumePrefix = buffer;
		} else {
			archivePath = _strdup(unc_path);
		}

		const char *squashfs_args[256] = {
			argv[0]
		};

		size_t i = 0;
		if(NULL != volumePrefix) {
			squashfs_args[++i] = buffer;
			squashfs_args[++i] = "-f";
		}
#if WIN_FUSE==0
		for (int j = 3; j < argc; ++j) {
			squashfs_args[++i] = argv[j];
		}
#endif
		squashfs_args[++i] = archivePath;
		squashfs_args[++i] = drive;

		squashfs_args[++i] = NULL;

		log_cmdlineA((int)i, squashfs_args);

		// int retVal = (int)_spawnv(_P_WAIT ,PROGNAME, squashfs_args);
		extern int main(int, const char **);
		int retVal = main((int)i, squashfs_args);
		//if (retVal == -1) {
		//	fprintf(stderr, "Error: _spawnv failed. errorno %d\n", errno);
		//}
		// free(PROGNAME);
		free(archivePath);
		return retVal;
	}
	return 1;
}

extern int glb__WinFspLoaded;

int wmain(int argc, wchar_t *wargv[], wchar_t *wenvp[])
{
	log_cmdline(argc, wargv);
	if (glb__WinFspLoaded != 0)
	{
		return glb__WinFspLoaded;
	}
	int total_size = 0;
	for (int i = 0; i < argc; ++i)
	{
		total_size += (int)(wcslen(wargv[i]) + 1) * sizeof(char);
	}
	total_size += argc * sizeof(char *);

	char **argv = (char **)malloc(total_size);
	if (argv)
	{
		char *pArg = (char *)(argv + argc);
		total_size -= sizeof(char *)*argc;
		for (int i = 0; i < argc; ++i)
		{
			int numChars = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, pArg, total_size, NULL, NULL);
			argv[i] = pArg;
			pArg += numChars;
		}
		int retVal = ansi_main(argc, argv);
		free(argv);
		return retVal;
	}
}
//
//#ifdef _M_X64
//#define WIN_SQUASHFS_KEY L"SOFTWARE\\WOW6432Node\\WinFsp\\Services\\squashfs"
//#else
//#define WIN_SQUASHFS_KEY L"SOFTWARE\\WinFsp\\Services\\squashfs"
//#endif
//
//static
//char *squashfuse_path()
//{
//	HKEY hKey;
//	LSTATUS status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, WIN_SQUASHFS_KEY, 0, KEY_READ, &hKey);
//	if (ERROR_SUCCESS == status) {
//		DWORD bufSize = _MAX_PATH;
//		char *buffer = (char *)calloc(_MAX_PATH, 3);
//		if (buffer) {
//			status = RegQueryValueExA(hKey, "Executable", NULL, NULL, buffer, &bufSize);
//			if (ERROR_SUCCESS == status) {
//				char drive[_MAX_DRIVE];
//				char path[_MAX_PATH];
//
//				_splitpath_s(buffer, drive, _MAX_DRIVE, path, _MAX_PATH, NULL, 0, NULL, 0);
//				_makepath_s(buffer, _MAX_PATH, drive, path, "squashfuse", ".exe");
//				return buffer;
//			}
//		}
//	}
//	return NULL;
//}

static
char *sqfs_parse_unc_path(const char *arg)
{

	/* This only applies when we're building against the WinFsp version of FUSE */

	const char *unc_path = arg;
	/* skip over the server names */
	/* const char *unc_prefix = unc_path; */
	if (*unc_path == '\\') {
		unc_path = strchr(++unc_path, '\\');
		char *ptr = NULL;
		if (isalpha(*(unc_path + 1)) && *(unc_path + 2) == '\\') {
			/* We'll interpret a single letter share name as a drive letter */
			size_t sLen = strlen(unc_path) + 1;
			ptr = malloc(sLen);
			*ptr = *(++unc_path);
			*(ptr + 1) = ':';
			strcpy_s(ptr + 2, sLen - 2, ++unc_path);
		}
		else {
			/* We'll interpret the everything after the \\squashfuse as a UNC path to the file */
			++unc_path;
			size_t sLen = strlen(unc_path) + 3;
			ptr = malloc(sLen);
			*ptr = '\\';
			*(ptr + 1) = '\\';
			strcpy_s(ptr + 2, sLen - 2, unc_path);
		}
		return ptr;
#if 0
		int i = 0;
		for (i = 0; i < 2 && (*unc_path == '\\'); ++i) {
			unc_path = strchr(++unc_path, '\\');
		}

		if (i == 2 && unc_path != NULL) {
			/* Add the WinFsp VolumePrefix argument so we show up as a network drive */
			/* Set the image path to the unc_path with the \server\share\ prefix stripped off.*/
			size_t sLen = strlen(unc_path) + 1;
			char *ptr = malloc(sLen);
			*ptr = *(++unc_path);
			*(ptr + 1) = ':';
			strcpy_s(ptr + 2, sLen - 2, ++unc_path);
			return ptr; //  ++unc_path;
		}
#endif // 0
	}
	return NULL;
}