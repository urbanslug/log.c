#include "../src/log.h"

int main(void)
{
    log_trace("Trace");
    log_debug("Debug");
    log_info("Info");
    log_warn("Warn");
    log_error("Error");
    log_fatal("Fatal");
    
    return 0;
}