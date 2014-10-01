#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "perfdata.h"

static struct perfdata *pdata = NULL;
static size_t workers = 0;

/* Initialize TLS/SSL performance data
 * worker_count is the number of active workers
 * Returns -1 on failure, 0 on success
 */
int init_perfdata(size_t worker_count)
{

	if (worker_count > MAX_WORKERS) {
		return -1;
	}

	workers = worker_count;

	pdata = mmap(NULL,sizeof(struct perfdata) * worker_count,
			PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (pdata && pdata != MAP_FAILED) {
		return 0;
	}

	return -1;
}

struct perfdata *fetch_perfdata(size_t workerid)
{
	if (workerid < workers) {
		return &pdata[workerid];
	}

	return NULL;
}

void report_perfdata(const char *path)
{
	size_t hits = 0, misses = 0, conns = 0;
	FILE *fp = NULL;

	if (path == NULL) {
		return;
	}

	for (size_t i = 0; i < workers; i++) {
		hits += pdata[i].ssl_hit;
		misses += pdata[i].ssl_miss;
		conns += pdata[i].conn;
	}

	for (int i = 0; i < 2; i++) {
		if ( (fp = fopen(path, "w")) != NULL) {
			break;
		}

		if (errno != ENOENT) {
			return;
		}

		char *parent = NULL;
		char path_copy[PATH_MAX];
		strncpy(path_copy, path, PATH_MAX - 1);

		parent = dirname(path_copy);
		int ret = mkdir(parent, 0700);

		free(parent);

		if (ret != 0 && ret != EEXIST) {
			return;
		}
	}

	fprintf(fp, "%zu, %zu, %zu\n", conns, hits, misses);
	fclose(fp);
}
