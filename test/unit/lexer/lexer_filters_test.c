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

void filters_simple(void) {
    /* varident | string | int | bool */

    data = "foo:installed";
    expected = "Varident(foo:installed)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "\"foo\"";
    expected = "String(\"foo\")";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "32";
    expected = "Int(32)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "true";
    expected = "Bool(true)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void filters_logop(void) {
    /* <filter> ::= <filter> <logop> <filter> | ... */

    data = "foo:installed | bar:installed";
    expected = "Varident(foo:installed) LogOp(|) Varident(bar:installed)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "(a:foo | b:foo)";
    expected = "LPAREN Varident(a:foo) LogOp(|) Varident(b:foo) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "(a:foo | b:foo)";
    expected = "LPAREN Varident(a:foo) LogOp(|) Varident(b:foo) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "(a:foo & b:bar)";
    expected = "LPAREN Varident(a:foo) LogOp(&) Varident(b:bar) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "(a:foo & b:bar) | c:baz";
    expected = "LPAREN Varident(a:foo) LogOp(&) Varident(b:bar) RPAREN LogOp(|) Varident(c:baz)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "(a:foo & b:bar) | (c:baz & d:burf)";
    expected = "LPAREN Varident(a:foo) LogOp(&) Varident(b:bar) RPAREN LogOp(|) LPAREN Varident(c:baz) LogOp(&) Varident(d:burf) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void filters_unary_op_simple(void) {
    /* <filter> ::= ... | "!" <filter> | "?" <filter> | ... */

    data = "! foo:installed";
    expected = "BANG Varident(foo:installed)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "? foo:installed";
    expected = "QMARK Varident(foo:installed)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "! \"foo\"";
    expected = "BANG String(\"foo\")";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "? \"foo\"";
    expected = "QMARK String(\"foo\")";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "!32";
    expected = "BANG Int(32)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "?32";
    expected = "QMARK Int(32)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "!true";
    expected = "BANG Bool(true)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = "?true";
    expected = "QMARK Bool(true)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void filters_unary_op_compound(void) {
    data = " !(a:foo | b:foo)";
    expected = "BANG LPAREN Varident(a:foo) LogOp(|) Varident(b:foo) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = " ?(a:foo | b:foo)";
    expected = "QMARK LPAREN Varident(a:foo) LogOp(|) Varident(b:foo) RPAREN";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = " ?a:foo | ?b:foo";
    expected = "QMARK Varident(a:foo) LogOp(|) QMARK Varident(b:foo)";
    actual = opam_lex_string(data);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    data = " ?a:foo | (?b:foo & \"c\")";
    expected = "QMARK Varident(a:foo) LogOp(|) LPAREN QMARK Varident(b:foo) LogOp(&) String(\"c\") RPAREN";
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
    /* RUN_TEST(filters_simple); */
    /* RUN_TEST(filters_logop); */
    /* RUN_TEST(filters_unary_op_simple); */
    RUN_TEST(filters_unary_op_compound);

    /* RUN_TEST(dbg); */

    int rc = UNITY_END();
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
    return rc;
}
