/*
 * Copyright (C) 2002-2015 Igor Sysoev
 * Copyright (C) 2011-2015 Nginx, Inc.
 * Copyright (C) 2015 Marcin Kaciuba
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef NGX_URL_PARSER
#define NGX_URL_PARSER
/* #define NGX_DEBUG 0 */

typedef enum {
    sw_schema = 0,
    sw_schema_slash,
    sw_schema_slash_slash,
    sw_userpass,
    sw_host_start,
    sw_host,
    sw_host_end,
    sw_host_ip_literal,
    sw_port,
    sw_host_http_09,
    sw_after_slash_in_uri,
    sw_check_uri,
    sw_check_uri_http_09,
    sw_uri,
    sw_almost_done
} sw_state;

typedef struct {
    const char * url_start;
    const char * url_end;
    const char * schema_end;
    const char * host_start;
    const char * host_end;
    const char * userpass_start;
    const char * userpass_end;
    const char * uri_start;
    const char * uri_end;
    const char * args_start;
    const char * fragment_start;
    const char * port_end;

    sw_state state;

} ngx_http_url_meta ;

typedef struct {
    char * schema;
    char * host;
    char * path;
    char * query;
    char * fragment;
    char * port;
    char * userpass;

} ngx_http_url;

#define NGX_URL_INVALID 0
#define NGX_URL_OK 1

int ngx_url_parser(ngx_http_url *r, const char *b);

int ngx_url_free(ngx_http_url * url);
#endif
