#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "utstring.h"
#include "liblogc.h"

#include "opam_lexer.h"

#define DEBUG_LEVEL debug_opamc_lexis
int  DEBUG_LEVEL;
#define TRACE_FLAG trace_opamc_lexis
bool TRACE_FLAG;

#if EXPORT_INTERFACE
#define TOKEN_NAME(x) (char*)#x
#endif

#if defined(LEXDEBUG)
EXPORT char *opam_token_names[256] = {
    [AMP]      = TOKEN_NAME(AMP),
    [AUTHORS]      = TOKEN_NAME(AUTHORS),
    [AVAILABLE]      = TOKEN_NAME(AVAILABLE),
    [BANG]      = TOKEN_NAME(BANG),
    [Bool]      = TOKEN_NAME(Bool),
    [BUG_REPORTS]  = TOKEN_NAME(BUG_REPORTS),
    [BUILD]      = TOKEN_NAME(BUILD),
    [BUILD_DOC]      = TOKEN_NAME(BUILD_DOC),
    [BUILD_ENV]      = TOKEN_NAME(BUILD_ENV),
    [CmdListFld]  = TOKEN_NAME(CmdListFld),
    [COLON]        = TOKEN_NAME(COLON),
    [COMMA]        = TOKEN_NAME(COMMA),
    [CONFLICTS]      = TOKEN_NAME(CONFLICTS),
    [CONFLICT_CLASS]      = TOKEN_NAME(CONFLICT_CLASS),
    [DEPENDS]      = TOKEN_NAME(DEPENDS),
    [DEPEXTS]      = TOKEN_NAME(DEPEXTS),
    [DEPOPTS]      = TOKEN_NAME(DEPOPTS),
    [DESCRIPTION]      = TOKEN_NAME(DESCRIPTION),
    [DEV_REPO]      = TOKEN_NAME(DEV_REPO),
    [DOC]          = TOKEN_NAME(DOC),
    [DQ]           = TOKEN_NAME(DQ),
    [EQ]           = TOKEN_NAME(EQ),
    [ERROR]        = TOKEN_NAME(ERROR),
    [EXTRA_FILES]  = TOKEN_NAME(extra_files),
    [EXTRA_SOURCE] = TOKEN_NAME(extra_source),
    [FALSE]        = TOKEN_NAME(FALSE),
    [FEATURES]     = TOKEN_NAME(FEATURES),
    [FLAGS]        = TOKEN_NAME(FLAGS),
    [FLAG]         = TOKEN_NAME(FLAG),
    [FldName]      = TOKEN_NAME(FldName),
    /* [FVF_LOGOP]      = TOKEN_NAME(FVF_LOGOP), */
    [HOMEPAGE]     = TOKEN_NAME(homepage),
    [Ident]        = TOKEN_NAME(Ident),
    [IDENTCHAR]    = TOKEN_NAME(IDENTCHAR),
    [INSTALL]      = TOKEN_NAME(INSTALL),
    [Int]          = TOKEN_NAME(Int),
    [LBRACE]      = TOKEN_NAME(LBRACE),
    [LBRACKET]      = TOKEN_NAME(LBRACKET),
    [LICENSE]      = TOKEN_NAME(LICENSE),
    [LogOp]      = TOKEN_NAME(LogOp),
    [LPAREN]       = TOKEN_NAME(LPAREN),
    [MAINTAINER]   = TOKEN_NAME(MAINTAINER),
    [MESSAGES]      = TOKEN_NAME(MESSAGES),
    [OPAM_VERSION] = TOKEN_NAME(OPAM_VERSION),
    [PACKAGE]      = TOKEN_NAME(PACKAGE),
    [PATCHES]      = TOKEN_NAME(PATCHES),
    [PIN_DEPENDS]      = TOKEN_NAME(PIN_DEPENDS),
    [PkgName]      = TOKEN_NAME(PkgName),
    [POST_MESSAGES]      = TOKEN_NAME(POST_MESSAGES),
    [QMARK]      = TOKEN_NAME(QMARK),
    [RBRACE]      = TOKEN_NAME(RBRACE),
    [RBRACKET]      = TOKEN_NAME(RBRACKET),
    [RelOp]      = TOKEN_NAME(RelOp),
    [REMOVE]      = TOKEN_NAME(REMOVE),
    [RPAREN]       = TOKEN_NAME(RPAREN),
    [RUNTEST]      = TOKEN_NAME(RUN_TEST),
    [SETENV]      = TOKEN_NAME(SETENV),
    [SQ]      = TOKEN_NAME(SQ),
    [String]       = TOKEN_NAME(String),
    [String3]       = TOKEN_NAME(String3),
    [SUBSTS]      = TOKEN_NAME(SUBSTS),
    [SYNOPSIS]      = TOKEN_NAME(SYNOPSIS),
    [TAGS]      = TOKEN_NAME(TAGS),
    [TERM]      = TOKEN_NAME(TERM),
    [TERM_STRING]      = TOKEN_NAME(TERM_STRING),
    [TERM_VARIDENT]      = TOKEN_NAME(TERM_VARIDENT),
    [TQ]         = TOKEN_NAME(TQ),
    [TRUE]         = TOKEN_NAME(TRUE),
    [URL]      = TOKEN_NAME(URL),
    [Varident]     = TOKEN_NAME(Varident),
    [VERSION]      = TOKEN_NAME(VERSION),
    NULL
};
#endif

bool is_empty(const char *s)
{
  while (*s) {
    if (!isspace(*s))
      return false;
    s++;
  }
  return true;
}

int token_type;

EXPORT int opam_get_next_token(struct opam_lexer_s *lexer,
                               union opam_token_u *otok,
                               UT_string *accum)
{
    token_type = get_next_opam_token(lexer, otok);
    if (token_type < 0) {

        switch(token_type) {
        case ERR_VARIDENT:
            fprintf(stderr, RED "ERROR" CRESET
                    " Bad varident: %s\n", otok->s);
            break;
        default:
            LOG_ERROR(0, "Other error: %d", token_type);
        }
        const char *ptr = lexer->tok;
        if (lexer->line > 0) {
            while (*ptr != '\n') ptr--;
            ptr++;
        } else {
            ptr = lexer->start;
        }
        int len; char buf[4];
        len = snprintf(buf, sizeof(buf), "%d", lexer->line);
        len += snprintf(buf, sizeof(buf), "%d", lexer->pos);
        fprintf(stderr, RED "ERROR" CRESET
                " line %d, pos %d: \"%s\"\n",
                lexer->line, lexer->pos, ptr);
        fprintf(stderr, RED "%*s\n" CRESET,
                lexer->pos+21 + len, "^");
        goto result;
    }

    if (token_type == 0) {      /* finished */
        goto result;
    }

#if defined(LEXDEBUG)
    LOG_DEBUG(1, "mode: %d; token type: %d: %s",
              lexer->mode,
              token_type,
              opam_token_names[token_type]);

    utstring_printf(accum, "%s", opam_token_names[token_type]);

    switch(token_type) {
    case Bool:
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case FldName:
        LOG_DEBUG(0, "\tFldName %s", (char*)otok->s);
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case CmdListFld:
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case FLAG:
        LOG_DEBUG(0, "\ts: %s", (char*)otok->s);
        utstring_printf(accum, "(\"%s\") ", (char*)otok->s);
        break;
    case VERSION:
        LOG_DEBUG(0, "\ts: %s", (char*)otok->s);
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case DESCRIPTION:
    case OPAM_VERSION:
    case PkgName:
    case String:
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case String3:
    case SYNOPSIS:
    case TERM:
    case TERM_STRING:
    case TERM_VARIDENT:
    case Ident:
    case Varident:
        /* LOG_DEBUG(0, "\ts: %s", (char*)otok->s); */
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    case Int:
        utstring_printf(accum, "(%d) ", (int)otok->i);
        break;
    case LogOp:
    case RelOp:
        utstring_printf(accum, "(%s) ", (char*)otok->s);
        break;
    default:
        utstring_printf(accum, " ");
    }
#endif
 result:
    return token_type;
}

EXPORT void opam_lex_file(char *fname)
{
    TRACE_ENTRY;
    log_set_quiet(false);

    /* UT_array *token_list; */
    /* utarray_new(token_list, &node_icd); */
/* nodelist: UT_array of node_s */
// sealark_nodes.c:
/* EXPORT UT_icd node_icd = {sizeof(struct node_s), NULL, sealark_alias_node, sealark_node_free}; */


    /* log_info("_lex_file: %s", fname); */
    FILE *f;

    f = fopen(fname, "r");
    if (f == NULL) {
        perror(fname);
        log_error("fopen failure for %s", fname);
        /* log_error("Value of errno: %d", errnum); */
        /* log_error("fopen error %s", strerror( errnum )); */
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    const size_t fsize = (size_t) ftell(f);
    if (fsize == 0) {
        fclose(f);
        errno = -1;
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_SET);
    char *buffer = (char*) malloc(fsize + 1);
    size_t read_ct = fread(buffer, 1, fsize, f);
    (void)read_ct; //FIXME: error check
    buffer[fsize] = 0;
    fclose(f);

    if (is_empty(buffer)) {
        fclose(f);
        errno = -2;
        exit(EXIT_FAILURE);
    }

    struct opam_lexer_s * lexer = malloc(sizeof(struct opam_lexer_s));
    opam_lexer_init(fname, lexer, buffer);
    /* struct bf_lexer_s * lexer = malloc(sizeof(struct bf_lexer_s)); */
    /* lexer_init(fname, lexer, buffer); */

    int tok;
    union opam_token_u otok; // = malloc(sizeof(union opam_token));


    log_set_quiet(false);
    log_set_level(LOG_TRACE);
    /* log_info("starting lex"); */

    while ( (tok = get_next_opam_token(lexer, &otok)) != 0 ) {
#if defined(LEXDEBUG)
        LOG_DEBUG(0, "mode: %d; token type: %d: %s",
                  lexer->mode,
                  tok, opam_token_names[tok]);
        switch(tok) {
        case DESCRIPTION:
        /* case FILTER: */
        case FldName:
        case LogOp:
        case OPAM_VERSION:
        case PkgName:
        case RelOp:
        case String:
        case String3:
        case SYNOPSIS:
        case TERM:
        case TERM_STRING:
        case TERM_VARIDENT:
        case Varident:
        case VERSION:
            LOG_DEBUG(0, "\ts: %s", (char*)otok.s); break;
        /* default: */
        /*     LOG_DEBUG(0, "other: %d", tok); break; */
        }
#endif
        /* otok = malloc(sizeof(union opam_token)); */
    }
    LOG_TRACE(0, "opam_lexer: end of input", "");
    free(buffer);

    /* return token_list; */
    TRACE_EXIT;
}
