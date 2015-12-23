/*
* Tests for invalid urls
*/

#include "gtest/gtest.h"

extern "C" {
    #include "../ngx_url_parser.h"
}

TEST(ngx_url_parser, SW_HOST_INVALID_IP) {
    const char * str = "http://[::192.#9.5.5]/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, SW_SCHEMA) {
    const char * str = "818181";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_INVALID, status);
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

TEST(ngx_url_parser, SW_CHECK_URI_PERC_INVALID){
    const char * str = "http://example.com%%";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, IncorrectUrl) {
    const char * str = "https:;";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_INVALID, status);
}

TEST(ngx_url_parser, IncorrectUrlNoHost) {
    const char * str = "https:///a";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_INVALID, status);
}

TEST(ngx_url_parser, IncorrectUrl2) {
    const char * str = "";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_INVALID, status);
}

TEST(ngx_url_parser, IncorrectUrl3NoPassNoUser) {
    const char * str = "http://:ssafs/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);
    ngx_url_free(&url);
}

TEST(ngx_url_parser, IncorrectUrl3NoPort) {
    const char * str = "http://ssafs:/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);
    ngx_url_free(&url);
}

TEST(ngx_url_parser, IncorrectUrl4NoPort) {
    const char * str = "http://ssafs:";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ASSERT_EQ(NGX_URL_INVALID, status);
    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlInvalidTwoAt) {

    const char * str = "https://user:password@@mkaciuba.pl:555/path/?query#fragment";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_INVALID, status);

    ngx_url_free(&url);
}

