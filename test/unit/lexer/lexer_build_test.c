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

void basic(void) {
    data = "build: [[\"foo\"]]";
    expected = "CmdListFld(build) LBRACKET LBRACKET String(\"foo\") RBRACKET RBRACKET";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);

    data = "build: [[foo:var]]";
    expected = "CmdListFld(build) LBRACKET LBRACKET Varident(foo:var) RBRACKET RBRACKET";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);
}

void dune(void) {
    data = "build: [[\"dune\" \"build\" \"-p\" name \"-j\" jobs]]";
    expected = "CmdListFld(build) LBRACKET"
        " LBRACKET String(\"dune\") "
        "String(\"build\") String(\"-p\") "
        "Ident(name) String(\"-j\") "
        "Ident(jobs) "
        "RBRACKET RBRACKET";
    actual = opam_lex_string(data);
    CHECK_RESULT(expected, actual);
}

void dbg(void) {
    //data = "name: \"foopkg\"\nversion: \"1.0.0\"\n  build: [ [foo] { \"bar\" | baz } ]";

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
    /* RUN_TEST(basic); */
    RUN_TEST(dune);
    /* RUN_TEST(dbg); */

    int rc = UNITY_END();
    if (fflush(stdout) == EOF) {
        /* Handle error */
    }
    return rc;
}
