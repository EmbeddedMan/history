// *** run2.c *********************************************************
// this file implements private extensions to the stickos bytecode
// execution engine.

#include "main.h"

bool run2_scroll;

// this function executes a private bytecode statement.
bool  // end
run2_bytecode_code(byte code, byte *bytecode, int length)
{
#if ! _WIN32
#pragma unused(bytecode, length)
#endif

#if BADGE_BOARD
    int r;
    int c;
    bool boo;
#endif
    bool end;
    int index;
    
    end = false;

    index = 0;

    switch (code) {
#if BADGE_BOARD
        case code_scroll:
            cw7bug++;  // CW7 BUG
            
#if ! _WIN32
            while (! jm_scroll_ready()) {
                // see if the sleep switch was pressed
                basic_poll();
            }
#endif
            
            run2_scroll = true;
            boo = run_bytecode_code(code_print, run_condition, bytecode, length);
            run2_scroll = false;
            
            return boo;

        case code_set:
        case code_clear:
            index += run_evaluate(bytecode+index, length-index, &r);
            
            assert(bytecode[index] == code_comma);
            index++;
            
            index += run_evaluate(bytecode+index, length-index, &c);
            
            if (run_condition) {
#if ! _WIN32
                if (code == code_set) {
                    jm_set(r, c);
                } else {
                    jm_clear(r, c);
                }
#endif
            }
            break;
#endif

        default:
            assert(0);
            break;
    }

    assert(index == length);
    return end;

//XXX_SKIP_XXX:
    stop();
    return false;
}

