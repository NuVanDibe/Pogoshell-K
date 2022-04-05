
#define IN_IWRAM __attribute__ ((section (".iwram")))
extern unsigned int __FarFunction (unsigned int (*ptr)(), ...);  // Reference to routine in crt0.S
extern void __FarProcedure (void (*ptr)(), ...);  // Reference to routine in crt0.S

extern void executeCart(unsigned int a, unsigned int b, unsigned int c) IN_IWRAM;
extern void setRamStart(unsigned int a) IN_IWRAM;


/* Return name of cart/manufacturer/driver as a string */
char *fcGetCartName(void)
{
	return "EZ2/3";
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

/*
Set which bank SRAM is mapped to. For standard carts this routine is called
with either 0x0, 0x10000, 0x20000 or 0x30000 for the 4 possible banks. This
bank is expected to read & writable at adress 0x0E000000 after this call

Returns resulting offset, negative if error
*/
int fcSetSRamStart(int offset)
{
	__FarProcedure(setRamStart, offset/0x1000);
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
	offset = ((offset & 0x03FFFFFF)>>15);	
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
