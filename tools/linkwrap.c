/*******************************************************************************
 File..........: linkwrap.c
 Date..........: 6/12/2001
 Author........: Jonas Minnberg
 Project.......: 
 Language......: Visual C++
 OS............: Win95/NT
 Description...: Quick-hacked wrapper for the GNU linker to format its error-
                 line in Visual C standard (so you can double-click on them)
*******************************************************************************/

#include <windows.h>
#include <process.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define   OUT_BUFF_SIZE 512
#define   READ_HANDLE 0
#define   WRITE_HANDLE 1
#define   BEEP_CHAR 7

char szBuffer[OUT_BUFF_SIZE];

char linebuf[16384];
char *lineptr = linebuf;
int linesize = 0;
void parse_line(void)
{
	char *slash, *ptr, *colon;

	ptr = slash = strchr(linebuf, '/');
	while(ptr) {
		if(ptr = strchr(&slash[1], '/'))
			slash = ptr;
	}
	if(slash)
		ptr = slash;
	else
		ptr = linebuf;
	if(colon = strchr(ptr, ':')) {
		ptr = colon+1;
		while(isdigit(*ptr))
			ptr++;
		if(*ptr == ':') {
			if(slash)
				*slash = '\\';
			*colon = '(';
			*ptr = ')';

			linesize +=2;
			memmove(&ptr[4], &ptr[2], (lineptr-ptr));				
			ptr[1] = ' ';
			ptr[2] = ':';
			ptr[3] = ' ';
		}
	}
}


int main(int argc, char** argv)
{
	int nExitCode = STILL_ACTIVE;
	if (argc >= 2)
	{
		HANDLE hProcess;
		int hStdOut;
		int hStdOutPipe[2];

		// Create the pipe
		if(_pipe(hStdOutPipe, 512, O_BINARY | O_NOINHERIT) == -1)
		 return   1;

		// Duplicate stdout handle (next line will close original)
		hStdOut = _dup(_fileno(stderr));

		// Duplicate write end of pipe to stdout handle
		if(_dup2(hStdOutPipe[WRITE_HANDLE], _fileno(stderr)) != 0)
		 return   2;

		// Close original write end of pipe
		close(hStdOutPipe[WRITE_HANDLE]);

		// Spawn process
		hProcess = (HANDLE)spawnvp(P_NOWAIT, argv[1], (const char* const*)&argv[1]);

		// Duplicate copy of original stdout back into stdout
		if(_dup2(hStdOut, _fileno(stderr)) != 0)
			return 3;

		// Close duplicate copy of original stdout
		close(hStdOut);

		if(hProcess)
		{
			int nOutRead, i;
			//printf("This is linkproc\n");
			while   (nExitCode == STILL_ACTIVE)
			{
				nOutRead = read(hStdOutPipe[READ_HANDLE], szBuffer, OUT_BUFF_SIZE);
				if(nOutRead)
				{
					//fprintf(stderr, "Read %d bytes\n", nOutRead);
					for(i=0; i<nOutRead; i++) {
						if(szBuffer[i] == 13) {
						} else
						if(szBuffer[i] != 10)
							*lineptr++ = szBuffer[i];
						else {
							linesize = (lineptr-linebuf)+1;
							*lineptr = 10;
							lineptr[1] = 13;
							parse_line();
							//printf("Printing %d chars to stderr\n", linesize);
							fwrite(linebuf, 1, linesize, stderr);
							lineptr = linebuf;
							linesize = 0;
						}
					}
				}

				if(!GetExitCodeProcess(hProcess,(unsigned long*)&nExitCode))
					return 4;
			}
		}
	}
	return nExitCode;
}

