/*
 * Copyright (C) 2002-2015 Igor Sysoev
 * Copyright (C) 2011-2015 Nginx, Inc.
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

struct {
    char * url_start = NULL;
    char * scheme_end = NULL;
    char * host_start = NULL;
    char * host_end = NULL;
    char * uri_start = NULL;
    char * uri_end = NULL;
    int complex_uri = 0;
    int plus_in_uri = 0;

} ngx_http_uri;

int
ngx_url_parser(ngx_http_uri *r, const char *b)
{
    u_char  c, ch, *p, *m;
    enum {
        sw_start = 0,
        sw_spaces_before_uri,
        sw_schema,
        sw_schema_slash,
        sw_schema_slash_slash,
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
        sw_http_09,
        sw_http_H,
        sw_http_HT,
        sw_http_HTT,
        sw_http_HTTP,
    } state;

    state = r->state;

    for (p = b; p != '\0'; p++) {
        ch = *p;

        switch (state) {

        case sw_start:
            r->url_start = p;

            if (ch == CR || ch == LF) {
                break;
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return NGX_HTTP_PARSE_INVALID_METHOD;
            }

            state = sw_schema;
            break;


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
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_schema_slash:
            switch (ch) {
            case '/':
                state = sw_schema_slash_slash;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_schema_slash_slash:
            switch (ch) {
            case '/':
                state = sw_host_start;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
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
            case ' ':
                /*
                 * use single "/" from request line to preserve pointers,
                 * if request line will be copied to large client buffer
                 */
                r->uri_start = r->schema_end + 1;
                r->uri_end = r->schema_end + 2;
                state = sw_host_http_09;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
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
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_port:
            if (ch >= '0' && ch <= '9') {
                break;
            }

            switch (ch) {
            case '/':
                r->port_end = p;
                r->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case ' ':
                r->port_end = p;
                /*
                 * use single "/" from request line to preserve pointers,
                 * if request line will be copied to large client buffer
                 */
                r->uri_start = r->schema_end + 1;
                r->uri_end = r->schema_end + 2;
                state = sw_host_http_09;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;


        /* check "/.", "//", "%", and "\" (Win32) in URI */
        case sw_after_slash_in_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                state = sw_check_uri;
                break;
            }

            switch (ch) {
            case ' ':
                r->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                r->uri_end = p;
                state = sw_almost_done;
                break;
            case LF:
                r->uri_end = p;
                goto done;
            case '.':
                r->complex_uri = 1;
                state = sw_uri;
                break;
            case '%':
                r->quoted_uri = 1;
                state = sw_uri;
                break;
            case '/':
                r->complex_uri = 1;
                state = sw_uri;
                break;
#if (NGX_WIN32)
            case '\\':
                r->complex_uri = 1;
                state = sw_uri;
                break;
#endif
            case '?':
                r->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                r->complex_uri = 1;
                state = sw_uri;
                break;
            case '+':
                r->plus_in_uri = 1;
                break;
            case '\0':
                return NGX_HTTP_PARSE_INVALID_REQUEST;
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
#if (NGX_WIN32)
                if (r->uri_ext == p) {
                    r->complex_uri = 1;
                    state = sw_uri;
                    break;
                }
#endif
                r->uri_ext = NULL;
                state = sw_after_slash_in_uri;
                break;
            case '.':
                r->uri_ext = p + 1;
                break;
            case ' ':
                r->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                r->uri_end = p;
                state = sw_almost_done;
                break;
            case LF:
                r->uri_end = p;
                goto done;
#if (NGX_WIN32)
            case '\\':
                r->complex_uri = 1;
                state = sw_after_slash_in_uri;
                break;
#endif
            case '%':
                r->quoted_uri = 1;
                state = sw_uri;
                break;
            case '?':
                r->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                r->complex_uri = 1;
                state = sw_uri;
                break;
            case '+':
                r->plus_in_uri = 1;
                break;
            case '\0':
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* space+ after URI */
        case sw_check_uri_http_09:
            switch (ch) {
            case ' ':
                break;
            case CR:
                state = sw_almost_done;
                break;
            case LF:
                goto done;
            case 'H':
                r->http_protocol.data = p;
                state = sw_http_H;
                break;
            default:
                r->space_in_uri = 1;
                state = sw_check_uri;
                p--;
                break;
            }
            break;


        /* URI */
        case sw_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case ' ':
                r->uri_end = p;
                state = sw_http_09;
                break;
            case CR:
                r->uri_end = p;
                state = sw_almost_done;
                break;
            case LF:
                r->uri_end = p;
                goto done;
            case '#':
                r->complex_uri = 1;
                break;
            case '\0':
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* space+ after URI */
        case sw_http_09:
            switch (ch) {
            case ' ':
                break;
            case CR:
                state = sw_almost_done;
                break;
            case LF:
                goto done;
            case 'H':
                r->http_protocol.data = p;
                state = sw_http_H;
                break;
            default:
                r->space_in_uri = 1;
                state = sw_uri;
                p--;
                break;
            }
            break;

        case sw_http_H:
            switch (ch) {
            case 'T':
                state = sw_http_HT;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HT:
            switch (ch) {
            case 'T':
                state = sw_http_HTT;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HTT:
            switch (ch) {
            case 'P':
                state = sw_http_HTTP;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HTTP:
            switch (ch) {
            case '/':
                state = sw_host_start;;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* end of request line */
        case sw_almost_done:
            r->request_end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
        }
    }

    b->pos = p;
    r->state = state;

    return NGX_AGAIN;

done:

    b->pos = p + 1;

    if (r->request_end == NULL) {
        r->request_end = p;
    }

    r->state = sw_start;


    return NGX_OK;
}



struct
ngx_http_parse_uri(ngx_uri *r){
     u_char  *p, ch;
     enum {
         sw_start = 0,
         sw_after_slash_in_uri,
         sw_check_uri,
         sw_uri
     } state;

     state = sw_start;

     for (p = r->uri_start; p != r->uri_end; p++) {

         ch = *p;

         switch (state) {

         case sw_start:

             if (ch != '/') {
                 return NGX_ERROR;
             }

             state = sw_after_slash_in_uri;
             break;

         /* check "/.", "//", "%", and "\" (Win32) in URI */
         case sw_after_slash_in_uri:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 state = sw_check_uri;
                 break;
             }

             switch (ch) {
             case ' ':
                 r->space_in_uri = 1;
                 state = sw_check_uri;
                 break;
             case '.':
                 r->complex_uri = 1;
                 state = sw_uri;
                 break;
             case '%':
                 r->quoted_uri = 1;
                 state = sw_uri;
                 break;
             case '/':
                 r->complex_uri = 1;
                 state = sw_uri;
                 break;
 #if (NGX_WIN32)
             case '\\':
                 r->complex_uri = 1;
                 state = sw_uri;
                 break;
 #endif
             case '?':
                 r->args_start = p + 1;
                 state = sw_uri;
                 break;
             case '#':
                 r->complex_uri = 1;
                 state = sw_uri;
                 break;
             case '+':
                 r->plus_in_uri = 1;
                 break;
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
 #if (NGX_WIN32)
                 if (r->uri_ext == p) {
                     r->complex_uri = 1;
                     state = sw_uri;
                     break;
                 }
 #endif
                 r->uri_ext = NULL;
                 state = sw_after_slash_in_uri;
                 break;
             case '.':
                 r->uri_ext = p + 1;
                 break;
             case ' ':
                 r->space_in_uri = 1;
                 break;
 #if (NGX_WIN32)
             case '\\':
                 r->complex_uri = 1;
                 state = sw_after_slash_in_uri;
                 break;
 #endif
             case '%':
                 r->quoted_uri = 1;
                 state = sw_uri;
                 break;
             case '?':
                 r->args_start = p + 1;
                 state = sw_uri;
                 break;
             case '#':
                 r->complex_uri = 1;
                 state = sw_uri;
                 break;
             case '+':
                 r->plus_in_uri = 1;
                 break;
             }
             break;

         /* URI */
         case sw_uri:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 break;
             }

             switch (ch) {
             case ' ':
                 r->space_in_uri = 1;
                 break;
             case '#':
                 r->complex_uri = 1;
                 break;
             }
             break;
         }
     }

     return NGX_OK;
 }


 ngx_int_t
 ngx_http_parse_complex_uri(ngx_http_request_t *r, ngx_uint_t merge_slashes)
 {
     u_char  c, ch, decoded, *p, *u;
     enum {
         sw_usual = 0,
         sw_slash,
         sw_dot,
         sw_dot_dot,
         sw_quoted,
         sw_quoted_second
     } state, quoted_state;

 #if (NGX_SUPPRESS_WARN)
     decoded = '\0';
     quoted_state = sw_usual;
 #endif

     state = sw_usual;
     p = r->uri_start;
     u = r->uri.data;
     r->uri_ext = NULL;
     r->args_start = NULL;

     ch = *p++;

     while (p <= r->uri_end) {

         /*
          * we use "ch = *p++" inside the cycle, but this operation is safe,
          * because after the URI there is always at least one character:
          * the line feed
          */

         ngx_log_debug3(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                        "s:%d in:'%Xd:%c'", state, ch, ch);

         switch (state) {

         case sw_usual:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 *u++ = ch;
                 ch = *p++;
                 break;
             }

             switch (ch) {
 #if (NGX_WIN32)
             case '\\':
                 if (u - 2 >= r->uri.data
                     && *(u - 1) == '.' && *(u - 2) != '.')
                 {
                     u--;
                 }

                 r->uri_ext = NULL;

                 if (p == r->uri_start + r->uri.len) {

                     /*
                      * we omit the last "\" to cause redirect because
                      * the browsers do not treat "\" as "/" in relative URL path
                      */

                     break;
                 }

                 state = sw_slash;
                 *u++ = '/';
                 break;
 #endif
             case '/':
 #if (NGX_WIN32)
                 if (u - 2 >= r->uri.data
                     && *(u - 1) == '.' && *(u - 2) != '.')
                 {
                     u--;
                 }
 #endif
                 r->uri_ext = NULL;
                 state = sw_slash;
                 *u++ = ch;
                 break;
             case '%':
                 quoted_state = state;
                 state = sw_quoted;
                 break;
             case '?':
                 r->args_start = p;
                 goto args;
             case '#':
                 goto done;
             case '.':
                 r->uri_ext = u + 1;
                 *u++ = ch;
                 break;
             case '+':
                 r->plus_in_uri = 1;
                 /* fall through */
             default:
                 *u++ = ch;
                 break;
             }

             ch = *p++;
             break;

         case sw_slash:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 state = sw_usual;
                 *u++ = ch;
                 ch = *p++;
                 break;
             }

             switch (ch) {
 #if (NGX_WIN32)
             case '\\':
                 break;
 #endif
             case '/':
                 if (!merge_slashes) {
                     *u++ = ch;
                 }
                 break;
             case '.':
                 state = sw_dot;
                 *u++ = ch;
                 break;
             case '%':
                 quoted_state = state;
                 state = sw_quoted;
                 break;
             case '?':
                 r->args_start = p;
                 goto args;
             case '#':
                 goto done;
             case '+':
                 r->plus_in_uri = 1;
             default:
                 state = sw_usual;
                 *u++ = ch;
                 break;
             }

             ch = *p++;
             break;

         case sw_dot:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 state = sw_usual;
                 *u++ = ch;
                 ch = *p++;
                 break;
             }

             switch (ch) {
 #if (NGX_WIN32)
             case '\\':
 #endif
             case '/':
                 state = sw_slash;
                 u--;
                 break;
             case '.':
                 state = sw_dot_dot;
                 *u++ = ch;
                 break;
             case '%':
                 quoted_state = state;
                 state = sw_quoted;
                 break;
             case '?':
                 r->args_start = p;
                 goto args;
             case '#':
                 goto done;
             case '+':
                 r->plus_in_uri = 1;
             default:
                 state = sw_usual;
                 *u++ = ch;
                 break;
             }

             ch = *p++;
             break;

         case sw_dot_dot:

             if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                 state = sw_usual;
                 *u++ = ch;
                 ch = *p++;
                 break;
             }

             switch (ch) {
 #if (NGX_WIN32)
             case '\\':
 #endif
             case '/':
                 state = sw_slash;
                 u -= 5;
                 for ( ;; ) {
                     if (u < r->uri.data) {
                         return NGX_HTTP_PARSE_INVALID_REQUEST;
                     }
                     if (*u == '/') {
                         u++;
                         break;
                     }
                     u--;
                 }
                 break;
             case '%':
                 quoted_state = state;
                 state = sw_quoted;
                 break;
             case '?':
                 r->args_start = p;
                 goto args;
             case '#':
                 goto done;
             case '+':
                 r->plus_in_uri = 1;
             default:
                 state = sw_usual;
                 *u++ = ch;
                 break;
             }

             ch = *p++;
             break;

         case sw_quoted:
             r->quoted_uri = 1;

             if (ch >= '0' && ch <= '9') {
                 decoded = (u_char) (ch - '0');
                 state = sw_quoted_second;
                 ch = *p++;
                 break;
             }

             c = (u_char) (ch | 0x20);
             if (c >= 'a' && c <= 'f') {
                 decoded = (u_char) (c - 'a' + 10);
                 state = sw_quoted_second;
                 ch = *p++;
                 break;
             }

             return NGX_HTTP_PARSE_INVALID_REQUEST;

         case sw_quoted_second:
             if (ch >= '0' && ch <= '9') {
                 ch = (u_char) ((decoded << 4) + ch - '0');

                 if (ch == '%' || ch == '#') {
                     state = sw_usual;
                     *u++ = ch;
                     ch = *p++;
                     break;

                 } else if (ch == '\0') {
                     return NGX_HTTP_PARSE_INVALID_REQUEST;
                 }

                 state = quoted_state;
                 break;
             }

             c = (u_char) (ch | 0x20);
             if (c >= 'a' && c <= 'f') {
                 ch = (u_char) ((decoded << 4) + c - 'a' + 10);

                 if (ch == '?') {
                     state = sw_usual;
                     *u++ = ch;
                     ch = *p++;
                     break;

                 } else if (ch == '+') {
                     r->plus_in_uri = 1;
                 }

                 state = quoted_state;
                 break;
             }

             return NGX_HTTP_PARSE_INVALID_REQUEST;
         }
     }

 done:

     r->uri.len = u - r->uri.data;

     if (r->uri_ext) {
         r->exten.len = u - r->uri_ext;
         r->exten.data = r->uri_ext;
     }

     r->uri_ext = NULL;

     return NGX_OK;

 args:

     while (p < r->uri_end) {
         if (*p++ != '#') {
             continue;
         }

         r->args.len = p - 1 - r->args_start;
         r->args.data = r->args_start;
         r->args_start = NULL;

         break;
     }

     r->uri.len = u - r->uri.data;

     if (r->uri_ext) {
         r->exten.len = u - r->uri_ext;
         r->exten.data = r->uri_ext;
     }

     r->uri_ext = NULL;

     return NGX_OK;
 }

