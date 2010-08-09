#include "ipport.h"
#include "netbuf.h"
#include "net.h"

extern unsigned char disable_autorun;
extern unsigned char far __heap_addr[], __heap_size[];
extern int TCPTV_MSL;

volatile unsigned long cticks = 0;

int main (void)
{
    int i;
    extern struct net netstatic[1];  /* actual net structs */
    
   	splx(0);

   /* The C lib already has a small heap in the program image (which
    * is why the printf already works) but we need more. So we
    * take what's left of the 16 megs on the card.
    */
   mheap_init((char *)__heap_addr, (long)__heap_size);  /* start, length */

#ifdef NPDEBUG
   printf("\nHeap size = %d bytes\n", (long)__heap_size );
#endif

   port_prep = prep_evb;

   netstatic[0].mib.ifDescr = (u_char *)"Fast Ethernet Controller";

    // if we don't have an IP address...
#if STICKOS
    i = var_get_flash(FLASH_IPADDRESS);
#else
    i = 0;
#endif
    if (disable_autorun || ! i || i == -1) {
        // use dhcp
        powerup_config_flags = 1;
    } else {
        // use the ip address
        netstatic[0].n_ipaddr = i;
    }

   /* We set the station's Ethernet physical (MAC) address
    * from the address already in use by dBUG. This prevents
    * ARP problems on the development server. Production systems
    * usually read this from flash or eprom.
    */

#ifdef NPDEBUG
   dprintf("\netheraddr = %02X:%02X:%02X:%02X:%02X:%02X\n\n",
            mac_addr_fec[0], mac_addr_fec[1], mac_addr_fec[2],
            mac_addr_fec[3], mac_addr_fec[4], mac_addr_fec[5]);
#endif

   /* Heap memory saving trick - reduce the time a TCP socket
    * will linger in CLOSE_WAIT state. For systems with limited
    * heap space and a busy web server, this makes a big difference.
    */
   TCPTV_MSL = 1;    /* set low max seg lifetime default */

#ifdef NPDEBUG
   printf("Calling netmain()...\n");
#endif
   netmain();     /* Start and run net tasks, no return. */
   return 0;
}

void
exit(int code)
{
   printf("Exit, code %d. Push RESET button now.\n", code);
   splx(7);
   while (1)
      asm { halt };
}

void
dtrap()
{
   printf("dtrap\n");
   exit(0);
}

static int crits = 0;
static int sysint;

void
ENTER_CRIT_SECTION(void * p)
{
   if(crits++ == 0)
   {
      sysint = splx(7);
   }
}

void
EXIT_CRIT_SECTION(void * p)
{
   if(--crits == 0)
   {
      splx(sysint);
   }
   if(crits < 0)
   {
      printf("negative crit section ct(%d)\n", crits);
      dtrap();
   }
}

/********************************************************************/

