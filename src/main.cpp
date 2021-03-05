#include <windows.h>
#include <Tlhelp32.h>
#include <wingdi.h>
#include <stdio.h>
#include <stdbool.h>
#include <cstdint>
#include <string>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"d3d9.lib")

int64_t input_buffer_size = 0;
uint8_t * input_buffer = NULL;
int64_t buffer_index = 0;
int64_t n;


/*
// I found out this was not necessary
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
	printf("Control %" PRId64 "\n", (int64_t) fdwCtrlType);
    switch (fdwCtrlType) {
		case CTRL_C_EVENT: printf("Ctrl-C event\n\n"); return TRUE;
		case CTRL_CLOSE_EVENT: printf("Ctrl-Close event: confirm that the user wants to exit\n\n"); return TRUE;
		case CTRL_BREAK_EVENT: printf("Ctrl-Break event\n\n"); return FALSE;
		case CTRL_LOGOFF_EVENT: printf("Ctrl-Logoff event\n\n"); return FALSE;
		case CTRL_SHUTDOWN_EVENT: printf("Ctrl-Shutdown event\n\n"); return FALSE;
		default: return FALSE;
    }
	return FALSE;
}
*/

#define BU_VALUE 8 // BUFFER UNCERTAINTY VALUE

int save_buffer_to_clipboard(char * buffer, size_t buffer_size) {
	if (!OpenClipboard(NULL)) {
		printf("OpenClipboard failed\n");
		return -__LINE__;
	}
	HGLOBAL clipboard_global_handle;
   	clipboard_global_handle = GlobalAlloc(GMEM_DDESHARE, (size_t) buffer_size + 1);
	if (clipboard_global_handle == NULL) {
		printf("GlobalAlloc failed\n");
		return -__LINE__;
	}

	char * clipboard_buffer;
	clipboard_buffer = (char*) GlobalLock(clipboard_global_handle);
	if (clipboard_buffer == NULL) {
		printf("GlobalLock failed\n");
		return -__LINE__;
	}

	if (0 >= memcpy(clipboard_buffer, input_buffer, buffer_size)) {
		printf("memcpy failed\n");
		return -__LINE__;
	}

	clipboard_buffer[buffer_size] = '\0';

	EmptyClipboard();
	if (GlobalUnlock(clipboard_buffer) == FALSE) {
		return -__LINE__;
	}
	HANDLE h = SetClipboardData(CF_TEXT, clipboard_buffer);
	if (h == NULL) {
		printf("SetClipboardData failed\n");
		return -__LINE__;
	}
	if (CloseClipboard() == FALSE) {
		printf("CloseClipboard failed\n");
		return -__LINE__;
	}
	// ADVICE: DO NOT FREE OR IT CLIPBOARD WILL NOT WORK:
	// BAD CODE: GlobalFree(clipboard_buffer);
	// REASON: The global buffer will be used by another process
	//     to read clipboard later
	return 0;
}

int main(int argn, char ** argv) {
	/*
	// I found out this was not necessary
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		printf("SetConsoleCtrlHandler failed, see https://docs.microsoft.com/en-us/windows/console/registering-a-control-handler-function\n");
		return -__LINE__;
	}
	*/

	do {
		if (input_buffer == NULL || buffer_index >= input_buffer_size) {
			if (input_buffer == NULL) {
				input_buffer_size = 128;
			} else {
				input_buffer_size *= 2;
			}
			input_buffer = (uint8_t *) realloc(input_buffer, (input_buffer_size + BU_VALUE) * sizeof(uint8_t));
			printf("had to reallocate buffer to fit up to %" PRId64 " bytes, got 0x%" PRIX64 "\n", (uint64_t) input_buffer_size, ((int64_t) input_buffer));
			if (input_buffer == NULL) {
				printf("Allocation failed\n");
				return -__LINE__;
			}
		}
		printf("about to read memory into 0x%" PRIX64 " up to 0x%" PRIX64 "\n", ((uint64_t) &input_buffer[buffer_index]), ((uint64_t) &input_buffer[input_buffer_size - 1]));
		if (input_buffer_size - buffer_index == 0) {
			printf("Cannot read with buffer size of %" PRId64 "\n", (int64_t) (input_buffer_size - buffer_index));
			return -__LINE__;
		}
		n = read(0, &input_buffer[buffer_index], input_buffer_size - buffer_index);
		if (n <= 0) {
			printf("reading returned %" PRId64 "\n", n);
			break;
		}
		if (buffer_index + 2 < input_buffer_size) {
			if (n == 1 && input_buffer[buffer_index] == '\n') {
				break;
			}
		}
		printf("read %" PRId64 " bytes and moved buffer pointer to 0x%" PRIX64 "\n", n, ((uint64_t )&input_buffer[buffer_index]));
		buffer_index += n;
	} while(n > 0);

	if (buffer_index == 0 || input_buffer == NULL) {
		return -__LINE__;
	} else if (!(buffer_index > 0 && buffer_index < input_buffer_size)) {
		printf("Buffer index (%" PRId64 ") outside writable bounds (0, %" PRId64 ")\n", buffer_index, input_buffer_size);
		return -__LINE__;
	}
	int result;

	result = save_buffer_to_clipboard((char *) input_buffer, (size_t) buffer_index);
	if (result == 0) {
		printf("save_buffer_to_clipboard success\n");
	}
	free(input_buffer);
	return result;
}
