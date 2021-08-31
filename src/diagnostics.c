#include "../include/diagnostics.h"

void GetTime(char* buffer, size_t size) {
	time_t     currentTime = time(NULL);
	strftime(buffer, size, "%X", localtime(&currentTime));
}

