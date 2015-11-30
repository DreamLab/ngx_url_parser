#include "gtest/gtest.h"
#include "../ngx_url_parser.h"


TEST(ngx_url_parser, IncorrectUrl) {
    const char * str = "https:;";
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

TEST(ngx_url_parser, CorrectUrl) {
    const char * str = "http://mkaciuba.pl";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);
    ngx_url_free(&url);
    ASSERT_EQ(NGX_URL_OK, status);
}

TEST(ngx_url_parser, UrlWithPort) {
    const char * str = "http://mkaciuba.pl:443";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "443");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}
TEST(ngx_url_parser, UrlWithPort2) {
    const char * str = "http://mkaciuba.pl:4";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "4");
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPortAndPath) {
    const char * str = "http://mkaciuba.pl:443/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, "443");
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPath) {
    const char * str = "http://mkaciuba.pl/";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery) {
    const char * str = "http://mkaciuba.pl/?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery2) {
    const char * str = "http://mkaciuba.pl/d/b/?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "http");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/d/b/");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndQuery3) {
    const char * str = "https://mkaciuba.pl/d/b?a=2&b[]=1";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/d/b");
    ASSERT_STREQ(url.query, "a=2&b[]=1");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithPathAndFragment) {
    const char * str = "https://mkaciuba.pl/#test";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, "test");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithQuery) {
    const char * str = "https://mkaciuba.pl?aa";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, "aa");
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithQuery2) {
    const char * str = "https://mkaciuba.pl?";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.query, "");
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithFragment) {
    const char * str = "https://mkaciuba.pl#aa";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.query, NULL);
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, NULL);
    ASSERT_STREQ(url.fragment, "aa");
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithRandomQuery) {
    const char * str = "https://mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.userpass, NULL);

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithUserPass) {

    const char * str = "https://marcin:a@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.userpass, "marcin:a");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithUserPassNoPass) {

    const char * str = "https://marcin:@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.userpass, "marcin:");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithUserPassNoUser) {

    const char * str = "https://:pw@mkaciuba.pl/?sdfsdf?sdfsdfsd";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, NULL);
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, NULL);
    ASSERT_STREQ(url.query, "sdfsdf?sdfsdfsd");
    ASSERT_STREQ(url.userpass, ":pw");

    ngx_url_free(&url);
}

TEST(ngx_url_parser, UrlWithAllPramtert) {

    const char * str = "https://a:pw@mkaciuba.pl:1/?cbf#abc";
    ngx_http_url url;
    int status = ngx_url_parser(&url, str);

    ASSERT_EQ(NGX_URL_OK, status);
    ASSERT_STREQ(url.host, "mkaciuba.pl");
    ASSERT_STREQ(url.schema, "https");
    ASSERT_STREQ(url.port, "1");
    ASSERT_STREQ(url.path, "/");
    ASSERT_STREQ(url.fragment, "abc");
    ASSERT_STREQ(url.query, "cbf");
    ASSERT_STREQ(url.userpass, "a:pw");

    ngx_url_free(&url);
}
