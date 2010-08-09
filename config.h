#if PICTOCRYPT
#define VERSION  "1.1"
#else
#define VERSION  "1.07"
#endif

#define DEMO  1  // 1 enables DEMO board USB power and dtin3 LED
#define DEBUG  1  // 0 is fastest but riskiest (removes all assert()s)
#define RELEASE  1  // 1 disables expensive check functions
#if PICTOCRYPT
#define SECURE  1  // 1 sets flash security
#else
#define SECURE  0  // 1 sets flash security
#endif
#define EXTRACT  0  // 1 uses extracted headers rather than Freescale

