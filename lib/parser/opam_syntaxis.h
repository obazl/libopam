/* This file was automatically generated.  Do not edit! */
#undef INTERFACE
int ParseFallback(int iToken);
typedef union opam_token_u opam_token_u;
#include <stdbool.h>
#include "utarray.h"
typedef struct opam_cmd_s opam_cmd_s;
typedef struct opam_arg_s opam_arg_s;
union opam_token_u {
    char c;
    int i;
    char *s;
    bool boolval;
    UT_array *cmds;
    struct opam_cmd_s *cmd;
    UT_array *args;
    struct opam_arg_s *arg;
    char *filter;
};
#define ParseTOKENTYPE  union opam_token_u 
typedef struct opam_parse_state_s opam_parse_state_s;
#define ParseARG_PDECL , struct opam_parse_state_s *opam_parse_state 
void Parse(void *yyp,int yymajor,ParseTOKENTYPE yyminor ParseARG_PDECL);
#include "liblogc.h"
#if defined(__linux__)
#include <linux/limits.h>
#endif
#if !(defined(__linux__))
#include <limits.h>
#endif
#include "uthash.h"
#include "utstring.h"
extern int DEBUG_LEVEL;
extern int DEBUG_LEVEL;
extern int DEBUG_LEVEL;
extern int DEBUG_LEVEL;
#define LOG_TRACE(lvl, fmt, ...) if (DEBUG_LEVEL>lvl) log_trace(fmt, __VA_ARGS__)
#define CRESET "\033[0m"
#define RED "\033[0;31m"
#define LOG_ERROR(lvl, fmt, ...) if (DEBUG_LEVEL>lvl) log_error(fmt, __VA_ARGS__)
#include <stdio.h>
extern UT_icd opam_arg_icd;
extern UT_icd opam_cmd_icd;
struct opam_cmd_s {
    UT_array *args;  /* array of opam_arg_s */
    char *filter;
};
#if defined(YYCOVERAGE)
int ParseCoverage(FILE *out);
#endif
extern struct opam_arg_s *p;
#if defined(YYTRACKMAXSTACKDEPTH)
int ParseStackPeak(void *p);
#endif
#if !defined(Parse_ENGINEALWAYSONSTACK)
void ParseFree(void *p,void(*freeProc)(void *));
#endif
void ParseFinalize(void *p);
#if !defined(YYMALLOCARGTYPE)
#define YYMALLOCARGTYPE size_t
#endif
#define ParseCTX_PDECL
#if !defined(Parse_ENGINEALWAYSONSTACK)
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)ParseCTX_PDECL);
#endif
void ParseInit(void *yypRawParser ParseCTX_PDECL);
#if !defined(NDEBUG)
void ParseTrace(FILE *TraceFILE,char *zTracePrompt);
#endif
#define ParseCTX_STORE
#define ParseCTX_FETCH
#define ParseCTX_PARAM
#define ParseCTX_SDECL
#define ParseARG_STORE yypParser->opam_parse_state =opam_parse_state ;
#define ParseARG_FETCH  struct opam_parse_state_s *opam_parse_state =yypParser->opam_parse_state ;
#define ParseARG_PARAM ,opam_parse_state 
typedef struct opam_lexer_s opam_lexer_s;
typedef struct opam_package_s opam_package_s;
struct opam_parse_state_s {
    struct opam_lexer_s *lexer;
    struct opam_package_s *pkg;
};
#define ParseARG_SDECL  struct opam_parse_state_s *opam_parse_state ;
typedef struct opam_binding_s opam_binding_s;
struct opam_package_s {
    const char *src;
    struct opam_binding_s *bindings; /* uthash table */
};
enum phrase_type_e {
    BINDING_BOOL,
    BINDING_BUILD,
    BINDING_DEPENDS,
    BINDING_INT,
    BINDING_STRING,
    BINDING_STRINGLIST,
};
typedef enum phrase_type_e phrase_type_e;
struct opam_binding_s {
    char *name;                    /* key */
    // WARNING: if we use 'int t', then the enum above will not be
    // included by makeheaders.
    enum phrase_type_e t;
    void *val;
    UT_hash_handle hh; /* makes this structure hashable */
};
#define VERSION                        71
#define Varident                       70
#define URL                            69
#define TRUE                           68
#define TQ                             67
#define TERM_VARIDENT                  66
#define TERM_STRING                    65
#define TERM                           64
#define TAGS                           63
#define SYNOPSIS                       62
#define SUBSTS                         61
#define String3                        60
#define String                         59
#define SQ                             58
#define SETENV                         57
#define RUNTEST                        56
#define RPAREN                         55
#define RELOP                          54
#define REMOVE                         53
#define RBRACKET                       52
#define RBRACE                         51
#define QMARK                          50
#define POST_MESSAGES                  49
#define PKGNAME                        48
#define PIN_DEPENDS                    47
#define PATCHES                        46
#define PACKAGE                        45
#define OPAM_VERSION                   44
#define MESSAGES                       43
#define MAINTAINER                     42
#define LPAREN                         41
#define LogOp                          40
#define LICENSE                        39
#define LBRACKET                       38
#define LBRACE                         37
#define Int                            36
#define INSTALL                        35
#define IDENTCHAR                      34
#define IDENT                          33
#define HOMEPAGE                       32
#define FVF_LOGOP                      31
#define FldName                        30
#define FLAGS                          29
#define FLAG                           28
#define FEATURES                       27
#define FALSE                          26
#define EXTRA_SOURCE                   25
#define EXTRA_FILES                    24
#define ERROR                          23
#define EQ                             22
#define DQ                             21
#define DOC                            20
#define DEV_REPO                       19
#define DESCRIPTION                    18
#define DEPOPTS                        17
#define DEPEXTS                        16
#define DEPENDS                        15
#define CONFLICT_CLASS                 14
#define CONFLICTS                      13
#define COMMA                          12
#define COLON                          11
#define CmdListFld                     10
#define BUILD_ENV                       9
#define BUILD_DOC                       8
#define BUILD                           7
#define Bool                            6
#define BUG_REPORTS                     5
#define BANG                            4
#define AVAILABLE                       3
#define AUTHORS                         2
#define AMP                             1
#define LOG_DEBUG(lvl, fmt, ...) if (DEBUG_LEVEL>lvl) log_debug(fmt, __VA_ARGS__)
struct opam_arg_s {
    int  t;  // 0=string, 1=var, 2=filter?
    char *val;
};
extern bool TRACE_FLAG;
extern bool TRACE_FLAG;
extern bool TRACE_FLAG;
extern bool TRACE_FLAG;
#define INTERFACE 0
extern int line;
struct opam_lexer_s {
    const char *src;
    const char *start;
    int        line;
    int        pos; /* from start of line */
    const char *tok;
    const char *cursor;
    const char *limit;
    const char *marker;
    int mode;                   /* i.e. start condition */
};
