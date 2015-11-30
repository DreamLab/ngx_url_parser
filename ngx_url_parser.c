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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ngx_url_parser.h"

typedef unsigned char u_char;

static uint32_t  usual[] = {
     0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */

                 /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
     0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */

                 /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
     0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                 /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
     0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

     0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
     0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
     0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
     0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
 };

int ngx_url_parser_meta(ngx_http_url_meta *r, const char *b);

void init_default_meta(ngx_http_url_meta *r) {
    r->url_start = NULL;
    r->url_end = NULL;
    r->schema_end = NULL;
    r->host_start = NULL;
    r->host_end = NULL;
    r->uri_start = NULL;
    r->uri_end= NULL;
    r->args_start = NULL;
    r->fragment_start = NULL;
    r->port_end = NULL;
    r->userpass_start = NULL;
    r->userpass_end = NULL;

    sw_state state = sw_schema;
};

void init_default_url(ngx_http_url* url) {
    url->schema = NULL;
    url->host = NULL;
    url->path = NULL;
    url->query = NULL;
    url->fragment = NULL;
    url->port = NULL;
    url->userpass = NULL;
};

inline void copy_from_meta(char **dest, const char * start, const char * end) {
    int size = end - start + 1;
    *dest = (char*)malloc(size);
    memcpy((void*)*dest, start, size - 1);
    (*dest)[size - 1] = '\0';

}

int ngx_url_parser(ngx_http_url * url, const char *b) {
    init_default_url(url);

    ngx_http_url_meta meta;

    int status = ngx_url_parser_meta(&meta, b);

    if (status != NGX_URL_OK) {
        return status;
    }

    status = 0;

    if (meta.schema_end) {
        copy_from_meta(&(url->schema), meta.url_start, meta.schema_end);
        status++;
    }

    if (meta.host_end && meta.host_end - meta.host_start > 0) {
        copy_from_meta(&url->host, meta.host_start, meta.host_end);
        status++;
    }

    if (meta.uri_start) {
        if (meta.args_start) {
            copy_from_meta(&url->path, meta.uri_start, meta.args_start - 1);
        } else if (meta.fragment_start) {
            copy_from_meta(&url->path, meta.uri_start, meta.fragment_start - 1);
        } else {
            copy_from_meta(&url->path, meta.uri_start, meta.url_end);
        }

    }

    if (meta.args_start) {
        if (meta.fragment_start) {
            copy_from_meta(&url->query, meta.args_start, meta.fragment_start - 1);
            copy_from_meta(&url->fragment, meta.fragment_start, meta.url_end);
        } else {
            copy_from_meta(&url->query, meta.args_start, meta.url_end);
        }
    } else if (meta.fragment_start) {
        copy_from_meta(&url->fragment, meta.fragment_start, meta.url_end);
    }

    if (meta.port_end) {

        if (meta.host_end + 1 == meta.port_end) {
            return NGX_URL_INVALID;
        }

        // +1 skip ":"
        copy_from_meta(&url->port, meta.host_end + 1, meta.port_end);
    }

    if (meta.userpass_end) {
        copy_from_meta(&url->userpass, meta.userpass_start, meta.userpass_end);
    }

    if (status == 2) {
        return NGX_URL_OK;
    }

    return NGX_URL_INVALID;
}

int ngx_url_free(ngx_http_url * url) {

    if (url->schema != NULL) {
        free((void*)url->schema);
    }

    if (url->host != NULL) {
        free((void*)url->host);
    }

    if (url->path != NULL) {
        free((void*)url->path);
    }

    if (url->fragment != NULL) {
        free((void*)url->fragment);
    }

    if (url->port != NULL) {
        free((void*)url->port);
    }

    if (url->userpass != NULL) {
        free((void*)url->userpass);
    }
}

int ngx_url_parser_meta(ngx_http_url_meta *r, const char *b)
{
    u_char  c, ch;
    const char *p;

    init_default_meta(r);

    r->url_start = b;

    sw_state state = sw_schema;


    for (p = b; p != '\0'; p++) {
        ch = *p;

        switch (state) {

        case sw_schema:

            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            switch (ch) {
            case ':':
                r->schema_end = p;
                state = sw_schema_slash;
                break;
            default:
                #ifdef NGX_DEBUG
                    printf("Schema isn't valid!\n");
                #endif
                return NGX_URL_INVALID;
            }
            break;

        case sw_schema_slash:
            switch (ch) {
            case '/':
                state = sw_schema_slash_slash;
                break;
            default:
                #ifdef NGX_DEBUG
                    printf("No slash after schema\n");
                #endif
                return NGX_URL_INVALID;
            }
            break;

        case sw_schema_slash_slash:
            switch (ch) {
            case '/':
                state = sw_host_start;
                break;
            default:
                #ifdef NGX_DEBUG
                    printf("No second slash after schema\n");
                #endif
                return NGX_URL_INVALID;
            }
            break;

        case sw_host_start:

            r->host_start = p;

            if (ch == '[') {
                state = sw_host_ip_literal;
                break;
            }

            state = sw_host;

            /* fall through */

        case sw_host:

            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-') {
                break;
            }

            /* fall through */


        case sw_host_end:

            r->host_end = p;

            switch (ch) {
            case ':':
                state = sw_port;
                break;
            case '/':
                r->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case '?':
                r->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                r->fragment_start = p + 1;
                state = sw_uri;
                break;
            case '\0':
                goto done;
            default:
                #ifdef NGX_DEBUG
                    printf("Inpropert host\n");
                #endif
                return NGX_URL_INVALID;
            }
            break;

        case sw_host_ip_literal:

            if (ch >= '0' && ch <= '9') {
                break;
            }

            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            switch (ch) {
            case ':':
                break;
            case ']':
                state = sw_host_end;
                break;
            case '-':
            case '.':
            case '_':
            case '~':
                /* unreserved */
                break;
            case '!':
            case '$':
            case '&':
            case '\'':
            case '(':
            case ')':
            case '*':
            case '+':
            case ',':
            case ';':
            case '=':
                /* sub-delims */
                break;
            default:
                #ifdef NGX_DEBUG
                    printf("Inpropert ip\n");
                #endif
                return NGX_URL_INVALID;
            }
            break;

        case sw_port:
            if (ch >= '0' && ch <= '9') {
                break;
            }

            switch (ch) {
            case '\0':
                r->port_end = p;
                goto done;
                break;
            case '/':
                r->port_end = p;
                r->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case '@':
                r->userpass_end = p;
                r->userpass_start = r->host_start;
                r->host_start = NULL;
                r->port_end = NULL;
                state = sw_host_start;
                break;
            }
            break;


        /* check "/.", "//", "%", and "\" (Win32) in URI */
        case sw_after_slash_in_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                state = sw_check_uri;
                break;
            }

            switch (ch) {
            case '.':
                state = sw_uri;
                break;
            case '%':
                state = sw_uri;
                break;
            case '/':
                state = sw_uri;
                break;
            case '?':
                r->uri_end = p - 1;
                r->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                r->fragment_start = p + 1;
                state = sw_uri;
                break;
            case '+':
                break;
            case '\0':
                r->uri_end = p;
                goto done;
            default:
                state = sw_check_uri;
                break;
            }
            break;

        /* check "/", "%" and "\" (Win32) in URI */
        case sw_check_uri:
            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case '/':
                state = sw_after_slash_in_uri;
                break;
            case '%':
                state = sw_uri;
                break;
            case '?':
                r->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                r->fragment_start = p + 1;
                state = sw_uri;
                break;
            /* case '\0': */
            /*     return NGX_URL_INVALID; */
            }
            break;


        /* URI */
        case sw_uri:


            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                break;
            }
            switch (ch) {
            case '\0':
                r->uri_end = p;
                goto done;
            case '#':
                r->fragment_start = p + 1;
                break;
            }
            break;


        /* end of request line */
        case sw_almost_done:
            r->url_end = p - 1;
            switch (ch) {
            case '\0':
                goto done;
            default:
                return NGX_URL_INVALID;
            }
        }
    }
#ifdef NGX_DEBUG
    printf("Error invalid url!");
#endif
    return NGX_URL_INVALID;

done:
    if (r->url_end == NULL) {
        r->url_end = p;
    }

    r->state = sw_schema;

    return NGX_URL_OK;
}
