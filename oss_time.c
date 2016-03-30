#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include "include/oss_time.h"

const char * oss_get_gmt_time()
{
	time_t now;
	struct tm *gmt;
	char *time_val;

	time(&now);
	gmt = gmtime(&now);

	time_val = (char *)malloc(sizeof(char) * 65);
	memset(time_val, '\0', 65);

	strftime(time_val, 64, "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return time_val;
}