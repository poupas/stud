#ifndef PDATA_H
#define PDATA_H

#define QUOTE(name) #name

#define MAX_WORKERS 100
#ifdef _PERFDATA_REPORT_DIR
#define PERFDATA_REPORT_DIR QUOTE(_PERFDATA_REPORT_DIR)
#else
#define	PERFDATA_REPORT_DIR "/tmp"
#endif

struct perfdata {
	size_t ssl_hit;
	size_t ssl_miss;
	size_t conn;
};

int init_perfdata(size_t);
struct perfdata *fetch_perfdata(size_t workerid);
void report_perfdata(const char *);

#endif /* PDATA_H */
