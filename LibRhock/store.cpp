#include <terminal.h>
#include <rhock/print.h>
#include <rhock/memory.h>
#include <rhock/vm.h>
#include <rhock/obj.h>
#include <rhock/store.h>
#include <flash_write.h>

#define PAGE_SIZE   1024
#define PAGES       10
#define FLASH_SIZE  (PAGES*PAGE_SIZE)

// Allocate the appropriate zone in the flash
const char rhock_progs[FLASH_SIZE] 
__attribute__((section("rhock_progs"), aligned(PAGE_SIZE))) = {0};

const uint32_t rhock_store_page_size()
{
    return PAGE_SIZE;
}

const uint32_t rhock_store_pages()
{
    return PAGES;
}

const uint8_t *rhock_store_page_get(uint32_t n)
{
    return (const uint8_t*)&rhock_progs[n*PAGE_SIZE];
}

void rhock_store_page_write(uint32_t n, uint8_t *data)
{
    flash_write((uint32_t)rhock_store_page_get(n), data, PAGE_SIZE);
}

TERMINAL_COMMAND(store, "Shows the store status")
{
    uint32_t k = 0;
    uint32_t used_pages = 0;
    struct rhock_obj obj;
    terminal_io()->print("Store address: ");
    terminal_io()->println((uint32_t)rhock_progs);
    
    while (k < rhock_store_pages()) {
        terminal_io()->print("Page ");
        terminal_io()->print(k);
        terminal_io()->print(": ");
        if (!rhock_store_page_is_free(k)) {
            uint32_t size = rhock_obj_guess_size(rhock_store_page_get(k));
            uint32_t pages = rhock_store_page_round(size);
            rhock_obj_load(&obj, rhock_store_page_get(k), size);
            terminal_io()->print(" program #");
            terminal_io()->print(obj.id);
            terminal_io()->print(", size=");
            terminal_io()->print(size);
            terminal_io()->print(" (");
            terminal_io()->print(pages);
            terminal_io()->println(" pages)");
            if (rhock_obj_crc(&obj) != obj.crc16) {
                terminal_io()->println("BAD CHECKSUM");
            }

            for (int i=0; i<100; i++) {
                terminal_io()->print((int)(*(rhock_store_page_get(k)+i)));
                terminal_io()->print(" ");
            }
                terminal_io()->println();
            
            used_pages += pages;
            k += pages;
        } else {
            terminal_io()->println(" free");
            k++;
        }
    }
    terminal_io()->println();
    terminal_io()->print("Used pages: ");
    terminal_io()->print(used_pages);
    terminal_io()->print("/");
    terminal_io()->println(rhock_store_pages());
}

#if 0
TERMINAL_COMMAND(test, "Loads a dummy program")
{
    if (argc == 0) {
        terminal_io()->println("Usage: test [n]");
    } else {
        // Minimum void binary
        char dummy[38] = {0x00, 0x00, 0x00, 0x00, 0x31, 0xf1, 0x00, 0x26, 
            0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };
        dummy[3] = atoi(argv[0]);

        struct rhock_obj tmp;
        rhock_obj_load(&tmp, (uint8_t*)dummy, 38);
        uint16_t crc = rhock_obj_crc(&tmp);
        dummy[4] = (crc>>8)&0xff;
        dummy[5] = (crc>>0)&0xff;

        terminal_io()->print("id=");
        terminal_io()->println((int)dummy[3]);
        terminal_io()->print("crc16=");
        terminal_io()->println(rhock_obj_crc(&tmp));

        int r = rhock_store_alloc(sizeof(dummy));
        terminal_io()->print("Result: ");
        terminal_io()->println(r);
        //terminal_io()->print("Page: ");
        //terminal_io()->println(current_page);

        if (r) {
            r = rhock_store_append(dummy, 38);
            terminal_io()->print("Append result: ");
            terminal_io()->println(r);
            
            if (r) {
                r = rhock_store_load();
                terminal_io()->print("Load result: ");
                terminal_io()->println(r);
            }
        }
    }
}
#endif
