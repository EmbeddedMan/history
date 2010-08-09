#include "main.h"

// *** code *****************************************************************

#if BASIC

// the last word of each flash bank is the generation number
#define GENERATION(p)  *(int *)((p)+BASIC_LARGE_PAGE_SIZE-sizeof(int))

#define PAGE_SIZE(p)  (((p) == FLASH_CODE1_PAGE || (p) == FLASH_CODE2_PAGE) ? BASIC_LARGE_PAGE_SIZE : BASIC_SMALL_PAGE_SIZE)

// we always pick the newer flash bank
#define FLASH_CODE_PAGE  ((GENERATION(FLASH_CODE1_PAGE)+1 > GENERATION(FLASH_CODE2_PAGE)+1) ? FLASH_CODE1_PAGE : FLASH_CODE2_PAGE)

static
void
check_line(byte *page, struct line *line)
{
    assert(line->line_number != 0xffffffff);
    if (line->line_number) {
        assert(line->length > 0 && line->length <= BASIC_BYTECODE_SIZE);
    } else {
        assert(line->length == 0);
    }
    assert(line->size == ROUNDUP(line->length, sizeof(int)) + LINESIZE);

    if (page) {
        assert((byte *)line >= page && (byte *)line+line->size <= page+PAGE_SIZE(page));
    }
}

static
struct line *
find_first_line_in_page(byte *page)
{
    struct line *line;

    line = (struct line *)page;
    check_line(page, line);
    return line;
}

static
struct line *
find_next_line_in_page(byte *page, struct line *line)
{
    struct line *next;

    // if we're about to go past the end...
    if (! line->line_number) {
        return NULL;
    }

    check_line(page, line);
    next = (struct line *)((byte *)line + line->size);
    check_line(page, next);
    assert(next->line_number > line->line_number || ! next->line_number);
    return next;
}

static
struct line *
find_exact_line_in_page(byte *page, int line_number)
{
    struct line *line;

    for (line = find_first_line_in_page(page); line; line = find_next_line_in_page(page, line)) {
        if (line->line_number == line_number) {
            return line;
        }
    }
    return NULL;
}

static
struct line *
find_following_line_in_page(byte *page, int line_number)
{
    struct line *line;

    for (line = find_first_line_in_page(page); line; line = find_next_line_in_page(page, line)) {
        if (line->line_number > line_number || ! line->line_number) {
            return line;
        }
    }
    return NULL;
}

static
void
delete_line_in_page(byte *page, struct line *line)
{
    int shrink;
    struct line *next;

    check_line(page, line);
    assert(line->line_number);

    next = (struct line *)((byte *)line + line->size);

    memmove((byte *)line, (byte *)next, page+PAGE_SIZE(page)-(byte *)next);
    shrink = (byte *)next - (byte *)line;
    memset(page+PAGE_SIZE(page)-shrink, 0, shrink);

    // verify
    find_exact_line_in_page(page, 0);
}

static
bool
insert_line_in_page(byte *page, int line_number, int length, byte *bytecode)
{
    int room;
    int grow;
    struct line *eop;
    struct line *line;

    assert(line_number);
    assert(length);
    assert(page == RAM_CODE_PAGE);

    // delete this line if it exists
    line = find_exact_line_in_page(page, line_number);
    if (line) {
        delete_line_in_page(page, line);
    }

    // find the eop
    eop = find_exact_line_in_page(page, 0);
    assert(eop && ! eop->line_number && eop->length == 0 && eop->size == LINESIZE);
    room = page+PAGE_SIZE(page) - ((byte *)eop+eop->size);

    // check for available memory
    grow = LINESIZE+ROUNDUP(length, sizeof(int));
    if (grow > room) {
        return false;
    }

    // shift the following lines down
    line = find_following_line_in_page(page, line_number);
    assert(line);
    assert(((byte *)line+grow) + (((byte *)eop+eop->size) - (byte *)line) <= page+PAGE_SIZE(page));
    memmove((byte *)line+grow, (byte *)line, ((byte *)eop+eop->size) - (byte *)line);

    // insert the new line
    line->line_number = line_number;
    line->size = grow;
    line->length = length;
    memcpy(line->bytecode, bytecode, length);

    // verify
    find_exact_line_in_page(page, 0);
    
    return true;
}

struct line *
code_next_line(IN bool deleted_ok, IN OUT int *line_number)
{
    struct line *line;
    struct line *ram_line;
    struct line *flash_line;

    do {
        ram_line = find_following_line_in_page(RAM_CODE_PAGE, *line_number);
        assert(ram_line);
        flash_line = find_following_line_in_page(FLASH_CODE_PAGE, *line_number);
        assert(flash_line);

        if (! ram_line->line_number && ! flash_line->line_number) {
            return NULL;
        }

        if (! ram_line->line_number) {
            line = flash_line;
        } else if (! flash_line->line_number) {
            line = ram_line;
        } else if (flash_line->line_number < ram_line->line_number) {
            line = flash_line;
        } else {
            line = ram_line;
        }

        *line_number = line->line_number;
    } while (! deleted_ok && line->length == 1 && line->bytecode[0] == code_deleted);

    return line;
}

int
code_sub_line(byte *sub_name)
{
    int line_number;
    struct line *line;

    line_number = 0;
    for (;;) {
        line = code_next_line(false, &line_number);
        if (line) {
            if (line->bytecode[0] == code_sub && ! strcmp((char *)line->bytecode+1, (char *)sub_name)) {
                return line->line_number;
            }
        } else {
            return 0;
        }
    }
}

void
code_insert(int line_number, char *text_in, IN int text_offset)
{
    bool boo;
    int length;
    int syntax_error;
    char text[BASIC_LINE_SIZE];
    byte bytecode[BASIC_BYTECODE_SIZE];

    syntax_error = -1;

    if (text_in) {
        strcpy(text, text_in);
        if (! parse_line(text, &length, bytecode, &syntax_error)) {
            ftdi_command_error(text_offset + syntax_error);
            return;
        }
    } else {
        bytecode[0] = code_deleted;
        length = 1;
    }

    boo = insert_line_in_page(RAM_CODE_PAGE, line_number, length, bytecode);
    if (! boo) {
        printf("auto save\n");
        code_save(0);
        boo = insert_line_in_page(RAM_CODE_PAGE, line_number, length, bytecode);
        if (! boo) {
            printf("out of code ram\n");
        }
    }
}

void
code_edit(int line_number_in)
{
    int n;
    int line_number;
    struct line *line;
    char text[BASIC_LINE_SIZE+10];  // REVISIT -- line number size?
    
    line_number = line_number_in-1;
    line = code_next_line(false, &line_number);
    if (! line || line_number != line_number_in) {
        return;
    }
    
    n = sprintf(text, "%d ", line_number);
    unparse_bytecode(line->bytecode, line->length, text+n);
#if ! _WIN32
    ftdi_edit(text);
    cpustick_edit = true;
#endif
}

void
code_list(int start_line_number, int end_line_number)
{
    int indent;
    int line_number;
    struct line *line;
    char text[BASIC_LINE_SIZE+30/*2*MAX_SCOPES*/];  // REVISIT -- line number size?

    indent = 0;
    line_number = 0;
    for (;;) {
        line = code_next_line(false, &line_number);
        if (line) {
            if (line->bytecode[0] == code_endif || line->bytecode[0] == code_else || line->bytecode[0] == code_elseif ||
                line->bytecode[0] == code_endwhile || line->bytecode[0] == code_next ||
                line->bytecode[0] == code_endsub) {
                if (indent) {
                    indent--;
                } else {
                    printf("missing block begins?\n");
                }
            }
            if (line_number >= start_line_number && (! end_line_number || line_number <= end_line_number)) {
                memset(text, ' ', indent*2);
                unparse_bytecode(line->bytecode, line->length, text+indent*2);
                printf("%4d %s\n", line_number, text);
            }
            if (end_line_number && line_number > end_line_number) {
                break;
            }
            if (line->bytecode[0] == code_if || line->bytecode[0] == code_else || line->bytecode[0] == code_elseif ||
                line->bytecode[0] == code_while || line->bytecode[0] == code_for ||
                line->bytecode[0] == code_sub) {
                indent++;
            }
        } else {
            if (indent) {
                printf("missing block ends?\n");
            }
            printf("end\n");
            break;
        }
    }
}

void
code_delete(int start_line_number, int end_line_number)
{
    int line_number;
    struct line *line;

    if (! start_line_number && ! end_line_number) {
        return;
    }

    line_number = start_line_number?start_line_number-1:0;
    for (;;) {
        line = code_next_line(false, &line_number);
        if (line) {
            if (end_line_number && line_number > end_line_number) {
                break;
            }
            code_insert(line_number, NULL, 0);
        } else {
            break;
        }
    }
}

static byte *alternate_flash_code_page;
static int alternate_flash_code_page_offset;

static
void
code_erase_alternate(void)
{
    // determine the alternate flash page
    assert(FLASH_CODE_PAGE == FLASH_CODE1_PAGE || FLASH_CODE_PAGE == FLASH_CODE2_PAGE);
    alternate_flash_code_page = (FLASH_CODE_PAGE == FLASH_CODE1_PAGE) ? FLASH_CODE2_PAGE : FLASH_CODE1_PAGE;
    alternate_flash_code_page_offset = 0;

    // erase the alternate flash page
    assert(BASIC_LARGE_PAGE_SIZE >= FLASH_PAGE_SIZE && ! (BASIC_LARGE_PAGE_SIZE%FLASH_PAGE_SIZE));
    flash_erase_pages((uint32 *)alternate_flash_code_page, BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);
}

static
bool
code_append_line_to_alternate(const struct line *line)
{
    int size;
    int nwords;

    size = line->size;

    // if this is not the special end line...
    if (line->line_number) {
        // we always leave room for the special end line
        if ((int)(alternate_flash_code_page_offset+size) > (int)(BASIC_LARGE_PAGE_SIZE-sizeof(int)-LINESIZE)) {
            return false;
        }
    }

    assert(! (size%sizeof(uint32)));
    nwords = size/sizeof(uint32);

    // copy ram or flash to flash
    assert(FLASH_CODE_PAGE != alternate_flash_code_page);
    assert(nwords && ! (alternate_flash_code_page_offset%sizeof(uint32)));
    flash_write_words((uint32 *)(alternate_flash_code_page+alternate_flash_code_page_offset), (uint32 *)line, nwords);

    alternate_flash_code_page_offset += size;
    assert(! (alternate_flash_code_page_offset%sizeof(uint32)));
    
    return true;
}

const static struct line empty = { 0, LINESIZE, 0 };

static
void
code_promote_alternate(void)
{
    bool boo;
    int generation;

    assert(FLASH_CODE_PAGE != alternate_flash_code_page);

    // append an end line to the alternate flash bank
    boo = code_append_line_to_alternate(&empty);
    assert(boo);

    // update the generation of the alternate page, to make it primary!
    generation = GENERATION(FLASH_CODE_PAGE)+1;
    flash_write_words((uint32 *)(alternate_flash_code_page+BASIC_LARGE_PAGE_SIZE-sizeof(int)), (uint32 *)&generation, 1);

    assert(FLASH_CODE_PAGE == alternate_flash_code_page);
    assert(GENERATION(FLASH_CODE1_PAGE) != GENERATION(FLASH_CODE2_PAGE));

    delay(500);  // this always takes a while!
}

void
code_save(int renum)
{
    bool boo;
    int line_number;
    int line_renumber;
    struct line *line;
    struct line *ram_line;
    byte ram_line_buffer[sizeof(struct line)-VARIABLE+BASIC_BYTECODE_SIZE];

    // erase the alternate flash bank
    code_erase_alternate();

    // we'll build lines in ram
    ram_line = (struct line *)ram_line_buffer;

    // for all lines...
    boo = true;
    line_number = 0;
    line_renumber = 0;
    for (;;) {
        line = code_next_line(true, &line_number);
        if (! line) {
            break;
        }
        line_number = line->line_number;

        // if the line is not deleted...
        if (line->length != 1 || line->bytecode[0] != code_deleted) {
            // copy the line to ram
            assert(line->size <= sizeof(ram_line_buffer));
            memcpy(ram_line, line, line->size);

            // if we're renumbering lines...
            if (renum) {
                ram_line->line_number = renum+line_renumber;
                line_renumber += 10;
            }

            // append the line to the alternate flash bank
            boo = code_append_line_to_alternate(ram_line);
            if (! boo) {
                break;
            }
        }
    }
    
    // if we saved all lines successfully...
    if (boo) {
        // promote the alternate flash bank
        code_promote_alternate();

        // clear ram
        memset(RAM_CODE_PAGE, 0, BASIC_SMALL_PAGE_SIZE);
        *(struct line *)RAM_CODE_PAGE = empty;
    } else {
        printf("out of code flash\n");
    }
}

void
code_new(void)
{
    // erase the alternate flash bank
    code_erase_alternate();

    // promote the alternate flash bank
    code_promote_alternate();

    // clear ram
    memset(RAM_CODE_PAGE, 0, BASIC_SMALL_PAGE_SIZE);
    *(struct line *)RAM_CODE_PAGE = empty;

    // clear variables
    var_clear(true);
}

void
code_undo()
{
    // clear ram
    memset(RAM_CODE_PAGE, 0, BASIC_SMALL_PAGE_SIZE);
    *(struct line *)RAM_CODE_PAGE = empty;
}

void
code_mem(void)
{
    int n;
    struct line *eop;
    
    eop = find_exact_line_in_page(RAM_CODE_PAGE, 0);
    assert(eop);
    n = (byte *)eop+LINESIZE-RAM_CODE_PAGE;
    printf("%3d%% ram code bytes used%s\n", n*100/BASIC_SMALL_PAGE_SIZE, n>LINESIZE?" (unsaved changes!)":"");
    
    eop = find_exact_line_in_page(FLASH_CODE_PAGE, 0);
    assert(eop);
    n = (byte *)eop+LINESIZE-FLASH_CODE_PAGE;
    printf("%3d%% flash code bytes used\n", n*100/(BASIC_LARGE_PAGE_SIZE-sizeof(int)));
}

struct catalog {
    char name[BASIC_STORES][15];  // 14 character name
    int dummy;
};

void
code_store(char *name)
{
    int i;
    int e;
    int n;
    struct catalog temp;
    struct catalog *catalog;

    // update the primary flash
    code_save(0);

    if (! *name) {
        return;
    }

    memcpy(&temp, FLASH_CATALOG_PAGE, sizeof(temp));
    catalog = (struct catalog *)&temp;

    // look up the name in the catalog
    e = -1;
    for (i = 0; i < BASIC_STORES; i++) {
        if (catalog->name[i][0] == (char)0xff || catalog->name[i][0] == (char)0) {
            e = i;
            continue;
        }
        if (! strncmp(catalog->name[i], name, sizeof(catalog->name[i])-1)) {
            break;
        }
    }

    // if there is no space left...
    if (i == BASIC_STORES) {
        if (e == -1) {
            printf("out of storage\n");
            return;
        }
        n = e;
    } else {
        n = i;
    }

    // erase the store flash
    flash_erase_pages((uint32 *)FLASH_STORE_PAGE(n), BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);

    // if the name doesn't yet exist...
    if (i == BASIC_STORES) {
        // update the catalog
        strncpy(catalog->name[n], name, sizeof(catalog->name[n])-1);
        flash_erase_pages((uint32 *)FLASH_CATALOG_PAGE, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);
        flash_write_words((uint32 *)FLASH_CATALOG_PAGE, (uint32 *)&temp, sizeof(temp)/sizeof(uint32));
    }

    // copy the primary flash to the store flash
    flash_write_words((uint32 *)FLASH_STORE_PAGE(n), (uint32 *)FLASH_CODE_PAGE, BASIC_LARGE_PAGE_SIZE/sizeof(uint32));
}

void
code_load(char *name)
{
    int i;
    int generation;
    byte *code_page;
    struct catalog temp;
    struct catalog *catalog;

    // update the primary flash and clear ram
    code_save(0);

    memcpy(&temp, FLASH_CATALOG_PAGE, sizeof(temp));
    catalog = (struct catalog *)&temp;

    // look up the name in the catalog
    for (i = 0; i < BASIC_STORES; i++) {
        if (catalog->name[i][0] == (char)0xff || catalog->name[i][0] == (char)0) {
            continue;
        }
        if (! strncmp(catalog->name[i], name, sizeof(catalog->name[i])-1)) {
            break;
        }
    }

    // if the store name does not exist...
    if (i == BASIC_STORES) {
        printf("program '%s' not found\n", name);
        return;
    }
    
    code_page = FLASH_CODE_PAGE;
    generation = GENERATION(code_page);

    // erase the primary flash
    flash_erase_pages((uint32 *)code_page, BASIC_LARGE_PAGE_SIZE/FLASH_PAGE_SIZE);

    // copy the store flash to the primary flash (except the generation)
    flash_write_words((uint32 *)code_page, (uint32 *)FLASH_STORE_PAGE(i), BASIC_LARGE_PAGE_SIZE/sizeof(uint32)-1);
    
    // then restore the generation
    flash_write_words((uint32 *)(code_page+BASIC_LARGE_PAGE_SIZE-sizeof(int)), (uint32 *)&generation, 1);
}

void
code_dir()
{
    int i;
    struct catalog temp;
    struct catalog *catalog;

    memcpy(&temp, FLASH_CATALOG_PAGE, sizeof(temp));
    catalog = (struct catalog *)&temp;

    // look up the name in the catalog
    for (i = 0; i < BASIC_STORES; i++) {
        if (catalog->name[i][0] == (char)0xff || catalog->name[i][0] == (char)0) {
            continue;
        }
        printf("%s\n", catalog->name[i]);
    }
}

void
code_purge(char *name)
{
    int i;
    struct catalog temp;
    struct catalog *catalog;

    memcpy(&temp, FLASH_CATALOG_PAGE, sizeof(temp));
    catalog = (struct catalog *)&temp;

    // look up the name in the catalog
    for (i = 0; i < BASIC_STORES; i++) {
        if (catalog->name[i][0] == (char)0xff || catalog->name[i][0] == (char)0) {
            continue;
        }
        if (! strncmp(catalog->name[i], name, sizeof(catalog->name[i])-1)) {
            break;
        }
    }

    // if the store name does not exist...
    if (i == BASIC_STORES) {
        printf("program '%s' not found\n", name);
        return;
    }

    // update the catalog
    strncpy(catalog->name[i], "", sizeof(catalog->name[i])-1);
    flash_erase_pages((uint32 *)FLASH_CATALOG_PAGE, BASIC_SMALL_PAGE_SIZE/FLASH_PAGE_SIZE);
    flash_write_words((uint32 *)FLASH_CATALOG_PAGE, (uint32 *)&temp, sizeof(temp)/sizeof(uint32));

    delay(500);  // this always takes a while!
}

void
code_initialize(void)
{
    assert(! (FLASH_PAGE_SIZE%sizeof(uint32)));
    assert(! (BASIC_SMALL_PAGE_SIZE%FLASH_PAGE_SIZE));
    assert(! (BASIC_LARGE_PAGE_SIZE%FLASH_PAGE_SIZE));

#if ! _WIN32
    if (led_disable_autorun) {
        return;
    }
#endif

    // if this is our first boot since reflash...
    if (*(int *)FLASH_CODE_PAGE == 0xffffffff) {
        code_new();
        assert(find_first_line_in_page(FLASH_CODE_PAGE));
    } else {
        // clear ram
        memset(RAM_CODE_PAGE, 0, BASIC_SMALL_PAGE_SIZE);
        *(struct line *)RAM_CODE_PAGE = empty;
    }
}

#endif
