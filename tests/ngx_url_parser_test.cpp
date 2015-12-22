#include "gtest/gtest.h"

extern "C" {
    #include "../ngx_url_parser.h"
}

TEST(ngx_url_parser, SW_SCHEMA) {
    const char * str = "818181";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_INVALID, status);
}

TEST(ngx_url_parser, SW_SCHEMA_PATH) {
    const char * str = "/818181";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);

    ASSERT_STREQ(url.path, "/818181");
    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA_INVALID) {
    const char * str = "81:8181";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA_INVALID2) {
    const char * str = ":8181";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA_INVALID3){
    const char * str = ":8181/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA_SLASH_INVALID){
    const char * str = "htt:8";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA_SLASH_INVALID2){
    const char * str = "htt://";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_HOST_IP_INVALID){
    const char * str = "htt://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_HOST_IPv6){
    const char * str = "htt://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_STREQ(url.host, "[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]");
    ASSERT_STREQ(url.schema, "htt");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_EQ(NGX_URL_OK, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_HOST_IPv6_PATH){
    const char * str = "http://[::192.9.5.5]/ipng";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.path, "/ipng");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_HOST_INVALID_IP) {
    const char * str = "http://[::192.#9.5.5]/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_PORT_IPv6){
    const char * str = "http://[::192.9.5.5]:80";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, USER_PASS){
    const char * str = "http://user:pass@[::192.9.5.5]:80";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.userpass, "user:pass");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_AFTER_SLASH_IN_URI_DOT){
    const char * str = "http://[::192.9.5.5]:80/.";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.userpass, NULL);
    ASSERT_STREQ(url.path, "/.");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_AFTER_SLASH_IN_URI_PERCENT){
    const char * str = "http://[::192.9.5.5]:80/%";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.userpass, NULL);
    ASSERT_STREQ(url.path, "/%");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_AFTER_SLASH_IN_URI_SLASH){
    const char * str = "http://[::192.9.5.5]:80///";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.userpass, NULL);
    ASSERT_STREQ(url.path, "///");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_AFTER_SLASH_IN_URI_SLASH_FRAMGENT){
    const char * str = "http://[::192.9.5.5]:80///#fragment";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.host, "[::192.9.5.5]");
    ASSERT_STREQ(url.port, "80");
    ASSERT_STREQ(url.userpass, NULL);
    ASSERT_STREQ(url.path, "///");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, "fragment");

    ngx_url_free(&url);
}
