#include "liblogc.h"

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>

#if INTERFACE
#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif
#endif

#include <unistd.h>

#include "liblogc.h"
#include "utarray.h"
#include "utstring.h"
#include "utstring.h"

#include "opamlexer.h"
#include "lex_string_driver.h"

#define DEBUG_LEVEL debug_opamc_lexis
extern  int debug_opamc_lexis;
#define TRACE_FLAG trace_opamc_lexis
extern  bool trace_opamc_lexis;

/* ********************************** */
EXPORT char *opam_lex_string(char *s)
{
    TRACE_ENTRY_MSG("s: %s", s);
    struct opam_lexer_s * lexer = malloc(sizeof(struct opam_lexer_s));
    opam_lexer_init(NULL, lexer, s);

    int token_type;
    union opam_token_u otok; // = malloc(sizeof(union opam_token));

    UT_string *accum;
    utstring_new(accum);

    while ( (token_type = opam_get_next_token(lexer,
                                              &otok,
                                              accum)
             ) != 0 ) {
        if (token_type < 0) {
            utstring_free(accum);
            return NULL;
        }
    }
    /* LOG_DEBUG(0, "accum: %s", utstring_body(accum)); */
    /* LOG_TRACE(0, "opam_lexer: end of input", ""); */
    /* remove trailing space */
    char *result = strdup(utstring_body(accum));
    result[strlen(result) - 1] = '\0';
    utstring_free(accum);
    TRACE_EXIT;
    return result;
}

/* int main(int argc, char *argv[]) */
/* { */
/*     int opt; */
/*     debug_opamc_lexis = 1; */
/*     trace_opamc_lexis = true; */

/*     while ((opt = getopt(argc, argv, "hv")) != -1) { */
/*         switch (opt) { */
/*         /\* case 'f': *\/ */
/*         /\*     /\\* BUILD.bazel or BUILD file *\\/ *\/ */
/*         /\*     utstring_printf(opam_file, "%s", optarg); *\/ */
/*         /\*     break; *\/ */
/*         case 'h': */
/*             log_info("Help: "); */
/*             exit(EXIT_SUCCESS); */
/*         /\* default: *\/ */
/*         /\*     log_error("Usage: %s [-f] [opamfile]", argv[0]); *\/ */
/*         /\*     exit(EXIT_FAILURE); *\/ */
/*         } */
/*     } */

/*     /\* if (utstring_len(opam_file) == 0) { *\/ */
/*     /\*     log_error("-f <opamfile> must be provided."); *\/ */
/*     /\*     exit(EXIT_FAILURE); *\/ */
/*     /\* } *\/ */

/*     char *wd = getenv("BUILD_WORKING_DIRECTORY"); */
/*     if (wd) { */
/*         /\* we launched from bazel workspace, cd to launch dir *\/ */
/*         chdir(wd); */
/*     } */

/*     char *s = "\"ocaml\" {>= \"4.08.0\"}"; */
/*     opam_lex_string(s); */
/*     /\* UT_array *result = opam_lex_file(utstring_body(opam_file)); *\/ */

/*     /\* UT_array *result = sealark_lex_string("'hello'\n#cmt1\n"); *\/ */

/*     log_debug("main RESULT dump:"); */
/* } */
