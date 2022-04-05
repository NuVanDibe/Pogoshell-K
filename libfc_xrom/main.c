
#define IN_IWRAM __attribute__ ((section (".iwram")))
extern unsigned int __FarFunction (unsigned int (*ptr)(), ...);  // Reference to routine in crt0.S
extern void __FarProcedure (void (*ptr)(), ...);  // Reference to routine in crt0.S

extern void executeCart(unsigned int a, unsigned int b, unsigned int c) IN_IWRAM;

typedef unsigned short uint16;

/* VBoy and Mappy debug console output */
static void dprint(const char *sz)
{
	asm volatile(
	"mov r2, %0\n"
	"ldr r0, =0xc0ded00d\n"
	"mov r1, #0\n"
	"and r0, r0, r0\n"
  :
  :
  "r" (sz) :
  "r0", "r1", "r2");
}

// Dummy array to force Pogoshell to grow to 128KByte instead of 64KByte
// so that following bins will be aligned correctly for XRom
//const unsigned char dummy[65536] = { 0 };

/* Return name of cart/manufacturer/driver as a string */
char *fcGetCartName(void)
{
	return "XROM";
}

/*
Return support for the present cart by this driver
Bitwise OR of the following flags:

CART_SUPPORTED       1 (Only clear this bit if inserted cart is known to be incompatible)
ROMSWITCH_SUPPORT    2 (Can swith romstart)
SRAMSWICH_SUPPORT    4 (Can switch SRAM banks)
RTC_SUPPORT          8 (Handles RTC internally, not standard Pokemon)
RTC_SET_SUPPORT     16 (Can set the RTC)

*/
unsigned int fcGetSupport(void)
{
	return 7;
}


/*
Return the size of the cart in MBit
*/
int fcGetCartSize(void)
{
	return 0;
}

/*
Return the size of the SRAM in KBit
*/
int fcGetSRamSize(void)
{
	return 2*1024;
}

static volatile uint16 *ptr = (uint16*)0x09000000;

/*
Set which bank SRAM is mapped to. For standard carts this routine is called
with either 0x0, 0x10000, 0x20000 or 0x30000 for the 4 possible banks. This
bank is expected to read & writable at adress 0x0E000000 after this call

Returns resulting offset, negative if error
*/
int fcSetSRamStart(int offset)
{
	dprint("Setting RAMSTART\n");
	*ptr = 0x8000;
	*ptr = (offset>>16) << 11;
	return offset & 0xFFFF0000;
}

/*
Start a ROM located at a certain offset in the cart, and then jump to adress
jump = 0 means normal softreset to romstart.
Use pointer to own code in RAM to switch without execute.
Normally doest return unless error:
Error codes:
-1 Unsupported romstart position
*/
int fcExecuteRom(unsigned int offset, unsigned int jump)
{
	offset &= 0x07FFFFFF;
	__FarProcedure(executeCart, jump, offset);
	return 0;
}


/* RTC time is expressed in "standardized" seconds since 00:00 Jan 1st 2000,
   meaning each month is assumed to have 31 days.
*/


/* Get the realtime clock if available */
int fcGetRTC(void)
{
	return 0;
}

/* Set the realtime clock if available */
void fcSetRTC(int v)
{
}
