#include <windows.h>
#include "main.h"

extern int isatty(int);

bool cpustick_prompt = true;
bool ftdi_echo = true;

void
delay(int ms)
{
    Sleep(isatty(0)?(ms):(ms)/10);
}

void
flash_erase_pages(uint32 *addr, uint32 npages)
{
    memset(addr, 0xff, npages*FLASH_PAGE_SIZE);
}

void
flash_write_words(uint32 *addr, uint32 *data, uint32 nwords)
{
    memcpy(addr, data, nwords*sizeof(int));
}

void
flash_upgrade(void)
{
}

void
clone(bool and_run)
{
}

#define LINE_INPUT_SIZE  72

void
ftdi_command_error(int offset)
{
    int i;
    char buffer[2+LINE_INPUT_SIZE+1];

    assert(offset < LINE_INPUT_SIZE);

    offset += 2;  // prompt -- revisit, this is decided in cpustick.c!

    if (offset >= 10) {
        strcpy(buffer, "error -");
        for (i = 7; i < offset; i++) {
            buffer[i] = ' ';
        }
        buffer[i++] = '^';
        assert(i < sizeof(buffer));
        buffer[i] = '\0';
    } else {
        for (i = 0; i < offset; i++) {
            buffer[i] = ' ';
        }
        buffer[i++] = '^';
        assert(i < sizeof(buffer));
        buffer[i] = '\0';
        strcat(buffer, " - error");
    }
    printf("%s\n", buffer);
}

static
BOOL
ctrlc(int event)
{
    if (event) {
        return 0;
    } else {
        stop();
        return 1;
    }
}

int
main(int argc, char **argv)
{
    int i;
    char text[2*LINE_INPUT_SIZE];

    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlc, true);

    flash_erase_pages((uint32 *)FLASH_CODE1_PAGE, BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);
    flash_erase_pages((uint32 *)FLASH_CODE2_PAGE, BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);
    for (i = 0; i < BASIC_STORES; i++) {
        flash_erase_pages((uint32 *)FLASH_STORE_PAGE(i), BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);
    }
    flash_erase_pages((uint32 *)FLASH_CATALOG_PAGE, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);
    flash_erase_pages((uint32 *)FLASH_PARAM1_PAGE, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);
    flash_erase_pages((uint32 *)FLASH_PARAM2_PAGE, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);

    basic_initialize();

    if (argc == 1) {
        basic_run("help about");
    }
    for (;;) {
        if (isatty(0) && cpustick_prompt) {
            write(1, "> ", 2);
        }
        if (! gets(text)) {
            break;
        }
        text[LINE_INPUT_SIZE-1] = '\0';
        basic_run(text);
    }
    return 0;
}
