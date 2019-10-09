
local c=require("sigstackdump.c")
local api = {}
function api.setsignal (sig)
    c.signal(sig)
end
function api.setdump(pf)
    _signal_stack_dump_print_func=pf
end
--default printf
_signal_stack_dump_print_func=print
return api



