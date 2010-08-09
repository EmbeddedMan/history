#define VERSION  "1.21j"

#if ! MCF51JM128
#define DEMO  1  // 1 enables DEMO board USB power and dtin3 LED
#endif

#define DEBUG  0  // 0 is fastest but riskiest (removes all assert()s)
#define FAST  1  // 1 disables expensive check functions
#if PICTOCRYPT
#define SECURE  1  // 1 sets flash security
#else
#define SECURE  0  // 1 sets flash security
#endif
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale

#if ! _WIN32
#define SHRINK  0  // turn on for building debug code
#endif

