#include "parser_internal.h"

typedef struct {
  uint8_t *str;
  uint8_t len;
} HToken;



static HParseResult* parse_token(void *env, HParseState *state) {
  HToken *t = (HToken*)env;
  for (int i=0; i<t->len; ++i) {
    uint8_t chr = (uint8_t)h_read_bits(&state->input_stream, 8, false);
    if (t->str[i] != chr) {
      return NULL;
    }
  }
  HParsedToken *tok = a_new(HParsedToken, 1);
  tok->token_type = TT_BYTES; tok->bytes.token = t->str; tok->bytes.len = t->len;
  return make_result(state, tok);
}

const const HParserVtable token_vt = {
  .parse = parse_token,
};

const HParser* h_token(const uint8_t *str, const size_t len) { 
  HToken *t = g_new(HToken, 1);
  t->str = (uint8_t*)str, t->len = len;
  HParser *ret = g_new(HParser, 1);
  ret->vtable = &token_vt;
  ret->env = t;
  return (const HParser*)ret;
}
