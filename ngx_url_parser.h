#ifndef NGX_URL_PARSER
#define NGX_URL_PARSER
#define DEBUG 1

typedef enum {
    sw_schema = 0,
    sw_schema_slash,
    sw_schema_slash_slash,
    sw_auth_user,
    sw_auth_pass,
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
    const char * uri_start;
    const char * uri_end;
    const char * uri_ext;
    const char * args_start;
    const char * fragment_start;
    const char * port_end;

    int complex_uri;
    int plus_in_uri;
    int quoted_uri;
    sw_state state;

} ngx_http_url_meta ;

typedef struct {
    char * schema;
    char * host;
    char * path;
    char * query;
    char * fragment;
    char * port;

} ngx_http_url;

#define NGX_URL_INVALID -1
#define NGX_URL_OK 1

int
ngx_url_parser(ngx_http_url *r, const char *b);
#endif
