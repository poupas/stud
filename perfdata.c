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
static char *perfdata_path = NULL;

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

static void clear_perfdata(void)
{
	memset(pdata, 0, sizeof(struct perfdata) * workers);
}

void report_perfdata(const char *config)
{

	size_t hits = 0, misses = 0, conns = 0;
	FILE *fp = NULL;

	for (size_t i = 0; i < workers; i++) {
		hits += pdata[i].ssl_hit;
		misses += pdata[i].ssl_miss;
		conns += pdata[i].conn;
	}

	clear_perfdata();

	if (perfdata_path == NULL) {
		asprintf(&perfdata_path, "%s/%s", PERFDATA_REPORT_DIR, config);
	}

	for (int i = 0; i < 2; i++) {
		if ( (fp = fopen(perfdata_path, "w")) != NULL) {
			break;
		}

		if (errno != ENOENT) {
			return;
		}

		int ret = mkdir(PERFDATA_REPORT_DIR, 0700);
		if (ret != 0 && ret != EEXIST) {
			return;
		}
	}

	fprintf(fp, "%zu, %zu, %zu\n", conns, hits, misses);
	fclose(fp);
}
