#ifndef PDATA_H
#define PDATA_H

#define MAX_WORKERS 100

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

struct perfdata {
	size_t ssl_hit;
	size_t ssl_miss;
	size_t conn;
};

int init_perfdata(size_t);
struct perfdata *fetch_perfdata(size_t workerid);
void report_perfdata(const char *);

#endif /* PDATA_H */
