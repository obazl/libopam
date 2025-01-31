#include <stdlib.h>

#include "liblogc.h"
#include "unity.h"
#include "lexer_test.h"

char *errmsg = "";

char *data, *actual, *expected;

#define DEBUG_LEVEL debug_opamc_test
extern int  DEBUG_LEVEL;
#define TRACE_FLAG trace_opamc_test
extern bool TRACE_FLAG;

extern int  debug_opamc_lexis;
extern bool trace_opamc_lexis;

#define CHECK_RESULT(expected, actual) \
    if (actual == NULL) {                            \
        TEST_FAIL_MESSAGE("Lexical error");          \
    } else {                                         \
        TEST_ASSERT_EQUAL_STRING(expected, actual);  \
        free(actual);                                \
    }

void setUp(void) {
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
}

void tearDown(void) {
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
}

void string_flds(void) {
    data = "name: \"foo\"";
    expected = "FldName(name) String(\"foo\")";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }
}

void flags(void) {
    data = "flags: [ verbose plugin]";
    expected = "abc";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }
}

// build: [ [ <term> { <filter> } ... ] { <filter> } ... ]
// <term> ::= <string> | <varident>
// <varident> e.g. a+b+c:foo
// "Package variables have the form package-name:var-name"
// for example foo:installed is a boolean variable
// that can be used to check if package foo is installed
void varident(void) {
    /* data = "build: [ [ _:bar ] ]"; */
    // e.g. _:name, _:version
    //data = "build: [ [ foo:installed ] ]";
    data = "build: [ [ foo+bar ] ]";
    expected = "abc";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }
}

/* Version = "\"" (Identchar | "." | "~")+ "\""; */
/* Identchar = Letter | Digit | "_" | "-" | "+" ; */
void fvf_versions(void) {
    data = "{ \"a+b\" }";
    expected = "LBRACE VERSION(\"a+b\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "{ \"a_b-c+d.e~f\" }";
    expected = "LBRACE VERSION(\"a_b-c+d.e~f\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    /* hideous but legal: */
    data = "{ \"_-+.~\" }";
    expected = "LBRACE VERSION(\"_-+.~\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    /* singletons also permitted */
    data = "{ \"_\" }";
    expected = "LBRACE VERSION(\"_\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "{ \"-\" }";
    expected = "LBRACE VERSION(\"-\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "{ \"+\" }";
    expected = "LBRACE VERSION(\"+\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "{ \".\" }";
    expected = "LBRACE VERSION(\".\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "{ \"~\" }";
    expected = "LBRACE VERSION(\"~\") RBRACE";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);
}

void fvf_simple(void) {
    data = "depopts: { & = \"foo+bar\" }";
    /* data = "depopts: { & = \"3.1.2\" }"; */
    //data = "{ >= \"3.1.2\" }";
    expected = "LBRACE RELOP(>=) String(\"3.1.2\") RBRACE";
    actual = opam_lex_string(data);
    if (actual == NULL) {
        TEST_FAIL_MESSAGE("Lexical error");
    } else {
        TEST_ASSERT_EQUAL_STRING(expected, actual);
        free(actual);
    }

    /* data = "{ = \"3.1.2\" }"; */
    /* expected = "LBRACE RELOP(=) String(\"3.1.2\") RBRACE"; */
    /* actual = opam_lex_string(data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, actual); */
    /* free(actual); */

    /* data = "{ ! = \"3.1.2\" }"; */
    /* expected = "LBRACE BANG RELOP='=' String=\"3.1.2\" RBRACE"; */
    /* actual = opam_lex_string(data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, actual); */
    /* free(actual); */

    /* data = "{ ? = \"3.1.2\" }"; */
    /* expected = "LBRACE QMARK RELOP='=' String=\"3.1.2\" RBRACE"; */
    /* actual = opam_lex_string(data); */
    /* if (actual == NULL) { */
    /*     TEST_FAIL(); */
    /* } else { */
    /*     TEST_ASSERT_EQUAL_STRING(expected, actual); */
    /*     free(actual); */
    /* } */

    /* data = "{ ( = \"3.1.2\" ) }"; */
    /* expected = "LBRACE LPAREN RELOP='=' String=\"3.1.2\" RPAREN RBRACE"; */
    /* actual = opam_lex_string(data); */
    /* TEST_ASSERT_EQUAL_STRING(expected, actual); */
    /* free(actual); */
}

void fvf_logop(void) {
    data = " depends: { = \"1.2.3\" }";
    /* data = "{ >= \"3.12\" & build }"; */
    expected = "LBRACE RELOP='>=' String=\"3.1.2\" LOGOP='&' RPAREN RBRACE";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void dbg(void) {
    //data = "name: \"foopkg\"\nversion: \"1.0.0\"\n  build: [ [foo] { \"bar\" | baz } ]";

    /* data = " \"foo\" { \"bar\" }"; */
    /*ok data = " \"foo\" { build }"; */
    /*ok? data = " \"foo\" { true }"; */
    //data = " \"foo\" { 32 }";
    /* data = " \"foo\" { \"3.2\" & build }"; */
    //data = " \"foo\" { >=\"3.12\" & build }";
    /* data = "build  : [ [\"dune\" \"subst\"] {pinned}"; */
    //data = "build: [[foo] { 32 }]";
    data = "build: [ [foo] { a } ]";
    expected = "";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    initialize(argc, argv, "lexis");
    LOG_DEBUG(0, "beginning lexer test", "");

    UNITY_BEGIN();

    /* **************** */
    RUN_TEST(string_flds);
    /* RUN_TEST(flags); */
    /* RUN_TEST(varident); */
    /* RUN_TEST(fvf_versions); */

    /* RUN_TEST(fvf_simple); */
    /* RUN_TEST(fvf_logop); */
    /* RUN_TEST(fpf); */

    /* RUN_TEST(dbg); */

    int rc = UNITY_END();
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
    return rc;
}
