/*
 * AgbMain & other functions (core.c)
 * Part of libpogo, a c-library replacent for GBA
 * Programmed by Jonas Minnberg (Sasq)
 *
 * DESCRIPTION
 * Entrypoint and GBA specific stuff for pogo applications
 * Somewhat missnamed
 *
 **/
#include "core.h"
#include "gba_defs.h"
#include "rtc.h"

#include "cartlib.h"

int cartSupported;

extern void device_doirq(void);


volatile int frame_counter;

/* Assembly functions */
extern void executeCart(uint32 a, uint32 b, uint32 c) IN_IWRAM;
extern void setRamStart(uint32 a) IN_IWRAM;
extern void haltCpu(void) IN_IWRAM;

void Reset(void)
{
	SETW(REG_IME, 1);
	SETW(REG_IE, 0);
	SETW(REG_IF, 0);
#ifdef CARTLIB
	fcExecuteRom(0, 0x8C);
#else
	__FarProcedure(executeCart, 0x8C, 0, 0);
#endif
}

extern void getSema(volatile uint32 *sema);
extern void relSema(volatile uint32 *sema);
extern uint32 trySema(volatile uint32 *sema);

void get_sema(volatile uint32 *sema)
{
	__FarProcedure(getSema, sema);
}

void rel_sema(volatile uint32 *sema)
{
	__FarProcedure(relSema, sema);
}

uint32 try_sema(volatile uint32 *sema)
{
	return __FarFunction(trySema, sema);
}

//volatile int satan = 0;

static int current_bank = -1;

void set_ram_start(int i)
{
	if(current_bank != i)
	{
		if(i >= 0)
#ifdef CARTLIB
			fcSetSRamStart(i * 0x10000);
#else
			__FarProcedure(setRamStart, i);
#endif
		current_bank = i;
	}
}

/* Must be called by IRQ-handler (in crt0.S) */
volatile void InterruptProcess(void)
{
	int i = GETW(REG_IF);
	if(i & V_COUNT_INTR_FLAG)
		frame_counter++;
	device_doirq();
	SETW(REG_IF, 0xFFFF);
}

/* Halt CPU */
void Halt(void)
{
	__FarProcedure(haltCpu);
}

/* VBoy and Mappy debug console output */
void dprint(const char *sz)
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

extern void core_init(void);
extern void device_init(void);

int start_keys = 0;

/* Startup code (core init) */
volatile void __gccmain(void)
{
	int i;
	int keys = 0x3FF;

	SETW(REG_DISPCNT, DISP_LCDC_OFF);
	SETL(REG_BG2Y, 0);
	SETL(REG_BG2X, 0);
	SETL(REG_BG3Y, 0);
	SETL(REG_BG3X, 0);
	/* Turn off all sprites */
	for(i=0; i<128; i++) {
		OAM_BASE[i*4] = OAM_OBJ_OFF;
	}

	/* Make sure sound is off */
	SETW(REG_SOUNDCNT_X, 0);

	keys = GETW(REG_KEYINPUT);
	keys = GETW(REG_KEYINPUT);
	keys = GETW(REG_KEYINPUT);

	keys = (~keys) & 0x3FF;

	start_keys = keys;

	while(keys)
	{
		keys = GETW(REG_KEYINPUT);
		keys = (~keys) & 0x3FF;
	}

	device_init();

	SETW(REG_IE, V_COUNT_INTR_FLAG);
	SETW(REG_STAT, 0x28);
	SETW(REG_SOUNDBIAS, 0x0000);
	//dprint("core_init\n");
	//reset_io();
}

int main(int argc, char **argv);

void AgbMain(void)
{
	int i = 0;
	char *args[8];

	//dprint("agbmain\n");

	__gccmain();

	uchar *p = (uchar *)(0x02000000 + 255 * 1024 + 8);
	uint32 *p2 = (uint32 *)(0x02000000 + 255 * 1024);

	//_dprintf("val is %x from %p\n", p2[0], p2);

	/* Check for commandline arguments in WRAM */
	if(p2[0] == 0xFAB0BABE)
	{
		*p2 = 0;
		for(i=0; i<p2[1]; i++)
		{
			args[i] = p;
			//_dprintf("Arg %d is %s\n", i, p);
			p += (strlen(p)+1);
		}
	}
	// WARNING! FUCKS UP SRAM ON PLUGINS THAT HAS THEIR OWN SRAM HANDLING!
	//else
	//	set_ram_start(0);

#ifdef CARTLIB
	cartSupported = fcGetSupport();
#endif

	//rtc_enable();
	//rtc_check();

	//rtc_get(args);

	main(i, args);

	/* "Reset" back to shell, if any */
	SETW(REG_IME, 1);
	SETW(REG_IE, 0);
	SETW(REG_IF, 0);

	{
		void (*f)(void) = (void*)0x8000000;
		f();
	}
/*#ifdef CARTLIB
	fcExecuteRom(0, 0);
#else
	__FarProcedure(executeCart, 0, 0, 0);
#endif*/
}
