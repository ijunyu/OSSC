#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "include/oss_put_object_result.h"

static const char *_put_object_result_get_etag(oss_put_object_result_t *result)
{
	return result->etag;
}

static inline void __put_object_result_set_etag(oss_put_object_result_t *result, const char *etag, unsigned int etag_len)
{
	if (result->etag != NULL) {
		free(result->etag);
		result->etag = NULL;
	}

	result->etag = (char *)malloc(sizeof(char) * etag_len + 1);
	strncpy(result->etag, etag, etag_len);
	(result->etag)[etag_len] = '\0';
}

static void _put_object_result_set_etag(oss_put_object_result_t *result, const char *etag)
{
	assert(etag != NULL);

	unsigned int etag_len = strlen(etag);
	__put_object_result_set_etag(result, etag, etag_len);
}

oss_put_object_result_t *put_object_result_initialize()
{
	oss_put_object_result_t *result;
	result = (oss_put_object_result_t *)malloc(sizeof(oss_put_object_result_t));
	result->etag = NULL;

	result->get_etag = _put_object_result_get_etag;
	result->set_etag = _put_object_result_set_etag;

	return result;
}

void put_object_result_finalize(oss_put_object_result_t *result)
{
	if (result != NULL) {
		if (result->etag) {
			free(result->etag);
			result->etag = NULL;
		}
		free(result);
	}
}