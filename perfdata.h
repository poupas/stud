#ifndef PDATA_H
#define PDATA_H

#define MAX_WORKERS 100

struct perfdata {
	size_t ssl_hit;
	size_t ssl_miss;
	size_t conn;
};

int init_perfdata(size_t);
struct perfdata *fetch_perfdata(size_t workerid);
void report_perfdata(const char *);

#endif /* PDATA_H */
