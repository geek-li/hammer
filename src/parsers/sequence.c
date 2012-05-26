#include "parser_internal.h"

typedef struct {
  size_t len;
  const HParser **p_array;
} HSequence;

static HParseResult* parse_sequence(void *env, HParseState *state) {
  HSequence *s = (HSequence*)env;
  HCountedArray *seq = h_carray_new_sized(state->arena, (s->len > 0) ? s->len : 4);
  for (size_t i=0; i<s->len; ++i) {
    HParseResult *tmp = h_do_parse(s->p_array[i], state);
    // if the interim parse fails, the whole thing fails
    if (NULL == tmp) {
      return NULL;
    } else {
      if (tmp->ast)
	h_carray_append(seq, (void*)tmp->ast);
    }
  }
  HParsedToken *tok = a_new(HParsedToken, 1);
  tok->token_type = TT_SEQUENCE; tok->seq = seq;
  return make_result(state, tok);
}

static const HParserVtable sequence_vt = {
  .parse = parse_sequence,
};

const HParser* h_sequence(const HParser *p, ...) {
  va_list ap;
  size_t len = 0;
  const HParser *arg;
  va_start(ap, p);
  do {
    len++;
    arg = va_arg(ap, const HParser *);
  } while (arg);
  va_end(ap);
  HSequence *s = g_new(HSequence, 1);
  s->p_array = g_new(const HParser *, len);

  va_start(ap, p);
  s->p_array[0] = p;
  for (size_t i = 1; i < len; i++) {
    s->p_array[i] = va_arg(ap, const HParser *);
  } while (arg);
  va_end(ap);

  s->len = len;
  HParser *ret = g_new(HParser, 1);
  ret->vtable = &sequence_vt; ret->env = (void*)s;
  return ret;
}
