[![Build Status](https://travis-ci.org/Aldor007/ngx_url_parser.svg)](https://travis-ci.org/Aldor007/ngx_url_parser)

ngx_url_parser
==============
Yet another url parser, written in C.

This repository contains url parser extracted from source code of NGINX. There are some changes in code it but concept is same as in NGINX.

Usage:
----
Include ngx_url_parser.h and then
```C
const char * str = "https://user:password@mkaciuba.pl:555/path/?query#fragment";
// structure in with result will be stored
ngx_http_url url;

// run parser
int status = ngx_url_parser(&url, str);
if (status != NGX_URL_OK) {
    printf("Error processing url!\n");
    return 1;
}

printf("Url = %s\n", str);
printf("\nParse status %d", status);
printf("\n Schema = %s", url.schema);
printf("\n Host = %s", url.host);
printf("\n Port = %s", url.port);
printf("\n Path = %s", url.path);
printf("\n Query = %s", url.query);
printf("\n Fragment = %s", url.fragment);
printf("\n UserPass = %s", url.userpass);
printf("\n");

// free memory
ngx_url_free(&url);
```

