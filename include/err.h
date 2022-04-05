#ifndef ERR_H
#define ERR_H

void fatal_error(const char* fmt, ...);

void report_warning(const char* fmt, ...);

void report_error(const char* fmt, ...);

void begin_custom_error();

void end_custom_error();

#endif //!ERR_H