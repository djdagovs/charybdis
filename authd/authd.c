/* authd/authd.c - main code for authd
 * Copyright (c) 2016 William Pitcock <nenolod@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice is present in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ratbox_lib.h>
#include <stdio.h>

#include "setup.h"
#include "common.h"

#define MAXPARA 10

static rb_helper *authd_helper = NULL;

static void
parse_request(rb_helper *helper)
{
	static char *parv[MAXPARA + 1];
	static char readbuf[READBUF_SIZE];
	int parc;
	int len;

	while((len = rb_helper_read(helper, readbuf, sizeof(readbuf))) > 0)
	{
		parc = rb_string_to_array(readbuf, parv, MAXPARA);

		if(parc < 1)
			continue;

		switch (parv[0][0])
		{
		default:
			break;
		}
	}
}

static void
error_cb(rb_helper *helper)
{
	exit(1);
}

#ifndef WINDOWS
static void
dummy_handler(int sig)
{
	return;
}
#endif

static void
setup_signals(void)
{
#ifndef WINDOWS
	struct sigaction act;

	act.sa_flags = 0;
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGPIPE);
	sigaddset(&act.sa_mask, SIGALRM);
#ifdef SIGTRAP
	sigaddset(&act.sa_mask, SIGTRAP);
#endif

#ifdef SIGWINCH
	sigaddset(&act.sa_mask, SIGWINCH);
	sigaction(SIGWINCH, &act, 0);
#endif
	sigaction(SIGPIPE, &act, 0);
#ifdef SIGTRAP
	sigaction(SIGTRAP, &act, 0);
#endif

	act.sa_handler = dummy_handler;
	sigaction(SIGALRM, &act, 0);
#endif
}

int
main(int argc, char *argv[])
{
	setup_signals();

	authd_helper = rb_helper_child(parse_request, error_cb, NULL, NULL, NULL, 256, 256, 256);	/* XXX fix me */
	if(authd_helper == NULL)
	{
		fprintf(stderr, "authd is not meant to be invoked by end users\n");
		exit(1);
	}

	rb_helper_loop(authd_helper, 0);

	return 0;
}