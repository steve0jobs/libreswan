/* expectation failure, for libreswan
 *
 * Copyright (C) 2017 Andrew Cagney
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 */

#include <stdio.h>
#include <stdarg.h>

#include "lswlog.h"
#include "lswalloc.h"

#define DOTS "..."

const struct lswlog empty_lswlog = {
	.parrot = LSWLOG_PARROT,
	.buf = "",
	.canary = LSWLOG_CANARY,
	.len = 0,
};

static int lswlog_debugf_nop(const char *format UNUSED, ...)
{
	return 0;
}

int (*lswlog_debugf)(const char *format, ...) = lswlog_debugf_nop;

/*
 * Determine where, within LOG's message buffer, to write the string.
 */

struct dest {
	char *start;
	size_t size;
};

static struct dest dest(struct lswlog *log, const size_t bound)
{
	lswlog_debugf("dest(.log=%p,.bound=%zd)\n", log, bound);
	PASSERT_LSWLOG(log, bound);

	/*
	 * Where will the next message be written?
	 */
	passert(bound <= LOG_WIDTH);
	passert(log->len <= bound);
	char *start = log->buf + log->len;
	lswlog_debugf("\tstart=%p\n", start);
	passert(start <= log->buf + LOG_WIDTH);
	passert(start[0] == '\0');

	/*
	 * How much space remains?
	 *
	 * If the buffer is full (LEN==BOUND-1) then size=1 - a string
	 * of length 0 (but size 1 - the NUL) will still fit.
	 *
	 * If the buffer has overflowed (LEN==BOUND) (output has
	 * already been truncated) then size=0.
	 */
	passert(bound <= LOG_WIDTH);
	passert(log->len <= bound);
	size_t size = bound - log->len;
	lswlog_debugf("\tsize=%zd\n", size);
	passert(log->len + size < sizeof(log->buf));

	struct dest d = {
		.start = start,
		.size = size,
	};

	lswlog_debugf("\t->{.start=%p,.size=%zd}\n",
		      d.start, d.size);
	return d;
}

/*
 * The output needs to be truncated, overwrite the end of the buffer
 * with DOTS.
 */
static void truncate(struct lswlog *log, const size_t bound, const char *dots)
{
	lswlog_debugf("truncate(.log=%p,.bound=%zd,.dots=%s)\n", log, bound, dots);
	PASSERT_LSWLOG(log, bound);

	/*
	 * Transition from "full" to overfull (truncated).
	 */
	passert(log->len == bound - 1);
	log->len = bound;

	/*
	 * Backfill with DOTS.
	 */
	passert(bound < sizeof(log->buf));
	passert(bound >= strlen(dots));
	char *dest = log->buf + bound - strlen(dots);
	lswlog_debugf("\tdest=%p\n", dest);
	memcpy(dest, dots, strlen(dots) + 1);
}

/*
 * Try to append output to BUF.  Either copy the raw string or
 * VPRINTF.
 */

static size_t concat(struct lswlog *log, size_t bound,
		     const char *dots, const char *string)
{
	struct dest d = dest(log, bound);

	/*
	 * N (the return value) is the number of characters, not
	 * including the trailing NUL, that should have been written
	 * to the buffer.
	 */
	size_t n = strlen(string);

	if (d.size > n) {
		/*
		 * There is space for all N characters and a trailing
		 * NUL, copy everything over.
		 */
		memcpy(d.start, string, n + 1);
		log->len += n;
	} else if (d.size > 0) {
		/*
		 * Not enough space, perform a partial copy of the
		 * string ...
		 */
		memcpy(d.start, string, d.size - 1);
		d.start[d.size - 1] = '\0';
		log->len += d.size - 1;
		passert(log->len == bound - 1);
		/*
		 * ... and then go back and blat the end with DOTS.
		 */
		truncate(log, bound, dots);
	}
	/* already overflowed */

	PASSERT_LSWLOG(log, bound);
	return n;
}

static size_t append(struct lswlog *log, size_t bound, const char *dots,
		     const char *format, va_list ap)
{
	struct dest d = dest(log, bound);

	/*
	 * N (the return value) is the number of characters, not not
	 * including the trailing NUL, that should have been written
	 * to the buffer.
	 *
	 * If N is negative than an "output error" (will that happen?)
	 * occured (that or a very old, non-compliant, s*printf()
	 * implementation that returns -1 instead of the required
	 * size).
	 */
	int sn = vsnprintf(d.start, d.size, format, ap);
	if (sn < 0) {
		/*
		 * Return something "HUGE" so callers can assume all
		 * values are unsigned.
		 *
		 * Calling PEXPECT_LOG() here is recursive; is this a
		 * problem? (if it is then hopefully things crash).
		 */
		PEXPECT_LOG("vsnprintf() unexpectedly returned the -ve value %d", sn);
		return LOG_WIDTH;
	}
	size_t n = sn;

	if (d.size > n) {
		/*
		 * Everything, including the trailing NUL, fitted.
		 * Update the length.
		 */
		log->len += n;
	} else if (d.size > 0) {
		/*
		 * The message didn't fit so only d.size-1 characters
		 * of the message were written.  Update things ...
		 */
		log->len += d.size - 1;
		passert(log->len == bound - 1);
		/*
		 * ... and then mark the buffer as truncated.
		 */
		truncate(log, bound, dots);
	}
	/* already overflowed */

	PASSERT_LSWLOG(log, bound);
	return n;
}

size_t lswlogvf(struct lswlog *log, const char *format, va_list ap)
{
	return append(log, LOG_WIDTH, DOTS, format, ap);
}

size_t lswlogf(struct lswlog *log, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	size_t n = append(log, LOG_WIDTH, DOTS, format, ap);
	va_end(ap);
	return n;
}

size_t lswlogs(struct lswlog *log, const char *string)
{
	return concat(log, LOG_WIDTH, DOTS, string);
}

size_t lswlogl(struct lswlog *log, struct lswlog *buf)
{
	return concat(log, LOG_WIDTH, DOTS, buf->buf);
}