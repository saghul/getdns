/**
 * \file
 * unit tests for getdns_dict helper routines, these should be used to
 * perform regression tests, output must be unchanged from canonical output
 * stored with the sources
 */

/*
 * Copyright (c) 2013, NLNet Labs, Verisign, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the names of the copyright holders nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Verisign, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#ifdef HAVE_EVENT2_EVENT_H
#  include <event2/event.h>
#else
#  include <event.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testmessages.h"
#include <getdns/getdns.h>
#include <getdns/getdns_ext_libevent.h>
#include <sys/time.h>

/* Set up the callback function, which will also do the processing of the results */
void
this_callbackfn(struct getdns_context *this_context,
    getdns_callback_type_t this_callback_type,
    struct getdns_dict *this_response,
    void *this_userarg, getdns_transaction_t this_transaction_id)
{
	if (this_callback_type == GETDNS_CALLBACK_COMPLETE) {	/* This is a callback with data */
		char *res = getdns_pretty_print_dict(this_response);
		fprintf(stdout, "%s\n", res);
		free(res);

	} else if (this_callback_type == GETDNS_CALLBACK_CANCEL)
		fprintf(stderr,
		    "The callback with ID %llu was cancelled. Exiting.",
		    (unsigned long long)this_transaction_id);
	else
		fprintf(stderr,
		    "The callback got a callback_type of %d. Exiting.",
		    this_callback_type);
	getdns_dict_destroy(this_response);
}

int
main(int argc, char** argv)
{
	/* Create the DNS context for this call */
	struct getdns_context *this_context = NULL;
	getdns_return_t context_create_return =
	    getdns_context_create(&this_context, 1);
	if (context_create_return != GETDNS_RETURN_GOOD) {
		fprintf(stderr, "Trying to create the context failed: %d",
		    context_create_return);
		return (GETDNS_RETURN_GENERIC_ERROR);
	}
	getdns_context_set_resolution_type(this_context, GETDNS_RESOLUTION_STUB);

	getdns_context_set_timeout(this_context, 5000);
	/* Create an event base and put it in the context using the unknown function name */
	struct event_base *this_event_base;
	this_event_base = event_base_new();
	if (this_event_base == NULL) {
		fprintf(stderr, "Trying to create the event base failed.");
		getdns_context_destroy(this_context);
		return (GETDNS_RETURN_GENERIC_ERROR);
	}
	if (getdns_extension_set_libevent_base(this_context,
	    this_event_base) != GETDNS_RETURN_GOOD) {
        fprintf(stderr, "Setting event base failed.");
        getdns_context_destroy(this_context);
        return (GETDNS_RETURN_GENERIC_ERROR);
    }
	/* Set up the getdns call */
	const char *this_name = argc > 1 ? argv[1] : "getdnsapi.net";
	char *this_userarg = "somestring";	// Could add things here to help identify this call
	getdns_transaction_t this_transaction_id = 0;

	/* Make the call */
	getdns_return_t dns_request_return =
	    getdns_general(this_context, this_name, GETDNS_RRTYPE_A,
	    NULL, this_userarg, &this_transaction_id, this_callbackfn);
	if (dns_request_return == GETDNS_RETURN_BAD_DOMAIN_NAME) {
		fprintf(stderr, "A bad domain name was used: %s. Exiting.",
		    this_name);
		getdns_context_destroy(this_context);
		event_base_free(this_event_base);
		return (GETDNS_RETURN_GENERIC_ERROR);
	}
//    dns_request_return = getdns_service(this_context, this_name, NULL, this_userarg, &this_transaction_id,
//                                        this_callbackfn);
//    if (dns_request_return == GETDNS_RETURN_BAD_DOMAIN_NAME)
//      {
//              fprintf(stderr, "A bad domain name was used: %s. Exiting.", this_name);
//              return(GETDNS_RETURN_GENERIC_ERROR);
//      }
	else {
		/* Call the event loop */
        event_base_loop(this_event_base, EVLOOP_ONCE);
        while (getdns_context_get_num_pending_requests(this_context, NULL) > 0) {
		    event_base_loop(this_event_base, EVLOOP_ONCE);
        }
		// TODO: check the return value above
	}
	/* Clean up */
	getdns_context_destroy(this_context);
	/* the event base can only be free'd after the context has removed
	 * all of its events from it */
	event_base_free(this_event_base);
	/* Assuming we get here, leave gracefully */
	exit(EXIT_SUCCESS);
}				/* main */

/* tests_stub_async.c */
