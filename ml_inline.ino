
#include "config.h"




#define ML_SYNTH_INLINE_DECLARATION
#include <ml_inline.h>
#undef ML_SYNTH_INLINE_DECLARATION

#define ML_SYNTH_INLINE_DEFINITION
#include <ml_inline.h>
#ifdef OLED_OSC_DISP_ENABLED
#define ML_SCOPE_OLED
#include <ml_scope_oled_inline.h>
#endif
#undef ML_SYNTH_INLINE_DEFINITION

