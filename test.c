#include <stdio.h>

#include "ngx_url_parser.h"

int main(int argc, char *argv[])
{

    const char * str = "http://mkaciuba.pl/b/?a=1#a";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    printf("\nParse status %d", status);
    printf("\n Schema = %s", url.schema);
    printf("\n Host = %s", url.host);
    printf("\n Path = %s", url.path);
    printf("\n Query = %s", url.query);
    printf("\n Fragment = %s", url.fragment);
    /* const char * i = url.url_start; */
    /* if (url.url_start) */
    /*     printf("url start %c\n", *(url.url_start)); */
    /* if (url.args_start != NULL) */
    /*     printf("%c", *url.args_start); */

    /* for (; i != url.schema_end; i++) */
    /*     printf("%c", *i); */
    return 0;
}
