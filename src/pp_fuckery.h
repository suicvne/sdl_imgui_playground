#ifndef __PP_FUCKERY__
#define __PP_FUCKERY__

// Macro Hell

#define SIMPLE_FUNCTION(rval_, func_, action_) \
    rval_ func_()       { return action_();     }
#define DECLARE_FUNCTION(rval_, func_, action_, seq_...) \
    rval_ func_(seq_)   { return action_(seq_); }






#endif // __PP_FUCKERY__