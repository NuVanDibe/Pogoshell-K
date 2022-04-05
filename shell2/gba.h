// gba.h

#ifndef GBA_HEADER
#define GBA_HEADER


#define TRUE 1
#define FALSE 0
#define BOOL int

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef unsigned char byte;
typedef unsigned short hword;
typedef unsigned long word;

// Volatile equivalents
typedef volatile unsigned int       vu32;
typedef volatile signed   int       vs32;
typedef volatile unsigned short int vu16;
typedef volatile signed   short int vs16;
typedef volatile unsigned char       vu8;
typedef volatile signed   char       vs8;

#define OAMmem         (vu32*)0x7000000
#define VideoBuffer    (vu16*)0x6000000
//#define OAMdata		   (vu16*)0x6100000
#define BGPaletteMem   (vu16*)0x5000000
#define OBJPaletteMem  (vu16*)0x5000200

//u32* OAMmem  		      =(u32*)0x7000000;
//u16* VideoBuffer 			=(u16*)0x6000000;
//u16* OAMData			=(u16*)0x6010000;
//u16* BGPaletteMem 	  	=(u16*)0x5000000;
//u16* OBJPaletteMem 	 	=(u16*)0x5000200;


		
#define REG_INTERUPT   *(vu32*)0x3007FFC
#define REG_DISPCNT    *(vu32*)0x4000000
#define REG_DISPCNT_L  *(vu16*)0x4000000
#define REG_DISPCNT_H  *(vu16*)0x4000002
#define REG_DISPSTAT   *(vu16*)0x4000004
#define REG_VCOUNT     *(vu16*)0x4000006
#define REG_BG0CNT     *(vu16*)0x4000008
#define REG_BG1CNT     *(vu16*)0x400000A
#define REG_BG2CNT     *(vu16*)0x400000C
#define REG_BG3CNT     *(vu16*)0x400000E
#define REG_BG0HOFS    *(vu16*)0x4000010
#define REG_BG0VOFS    *(vu16*)0x4000012
#define REG_BG1HOFS    *(vu16*)0x4000014
#define REG_BG1VOFS    *(vu16*)0x4000016
#define REG_BG2HOFS    *(vu16*)0x4000018
#define REG_BG2VOFS    *(vu16*)0x400001A
#define REG_BG3HOFS    *(vu16*)0x400001C
#define REG_BG3VOFS    *(vu16*)0x400001E
#define REG_BG2PA      *(vu16*)0x4000020
#define REG_BG2PB      *(vu16*)0x4000022
#define REG_BG2PC      *(vu16*)0x4000024
#define REG_BG2PD      *(vu16*)0x4000026
#define REG_BG2X       *(vu32*)0x4000028
#define REG_BG2X_L     *(vu16*)0x4000028
#define REG_BG2X_H     *(vu16*)0x400002A
#define REG_BG2Y       *(vu32*)0x400002C
#define REG_BG2Y_L     *(vu16*)0x400002C
#define REG_BG2Y_H     *(vu16*)0x400002E
#define REG_BG3PA      *(vu16*)0x4000030
#define REG_BG3PB      *(vu16*)0x4000032
#define REG_BG3PC      *(vu16*)0x4000034
#define REG_BG3PD      *(vu16*)0x4000036
#define REG_BG3X       *(vu32*)0x4000038
#define REG_BG3X_L     *(vu16*)0x4000038
#define REG_BG3X_H     *(vu16*)0x400003A
#define REG_BG3Y       *(vu32*)0x400003C
#define REG_BG3Y_L     *(vu16*)0x400003C
#define REG_BG3Y_H     *(vu16*)0x400003E
#define REG_WIN0H      *(vu16*)0x4000040
#define REG_WIN1H      *(vu16*)0x4000042
#define REG_WIN0V      *(vu16*)0x4000044
#define REG_WIN1V      *(vu16*)0x4000046
#define REG_WININ      *(vu16*)0x4000048
#define REG_WINOUT     *(vu16*)0x400004A
#define REG_MOSAIC     *(vu32*)0x400004C
#define REG_MOSAIC_L   *(vu32*)0x400004C
#define REG_MOSAIC_H   *(vu32*)0x400004E
#define REG_BLDMOD     *(vu16*)0x4000050
#define REG_COLEV      *(vu16*)0x4000052
#define REG_COLEY      *(vu16*)0x4000054

#define REG_SOUND1CNT_L  *(vu16*)0x4000060
#define REG_SOUND1CNT_H  *(vu16*)0x4000062
#define REG_SOUND1CNT_X  *(vu16*)0x4000064
#define REG_SOUND2CNT_L  *(vu16*)0x4000068
#define REG_SOUND2CNT_H  *(vu16*)0x400006C
#define REG_SOUND3CNT_L  *(vu16*)0x4000070
#define REG_SOUND3CNT_H  *(vu16*)0x4000072
#define REG_SOUND3CNT_X  *(vu16*)0x4000074
#define REG_SOUND4CNT_L  *(vu16*)0x4000078
#define REG_SOUND4CNT_H  *(vu16*)0x400007C
#define REG_SOUNDCNT_L   *(vu16*)0x4000080
#define REG_SOUNDCNT_H   *(vu16*)0x4000082
#define REG_SOUNDCNT_X   *(vu16*)0x4000084

#define REG_SGBIAS     *(vu16*)0x4000088

#define WAVE_RAM0      *(vu32*)0x4000090
#define WAVE_RAM0_L    *(vu16*)0x4000090
#define WAVE_RAM0_H    *(vu16*)0x4000092
#define WAVE_RAM1      *(vu32*)0x4000094
#define WAVE_RAM1_L    *(vu16*)0x4000094
#define WAVE_RAM1_H    *(vu16*)0x4000096
#define WAVE_RAM2      *(vu32*)0x4000098
#define WAVE_RAM2_L    *(vu16*)0x4000098
#define WAVE_RAM2_H    *(vu16*)0x400009A
#define WAVE_RAM3      *(vu32*)0x400009C
#define WAVE_RAM3_L    *(vu16*)0x400009C
#define WAVE_RAM3_H    *(vu16*)0x400009E

#define REG_SGFIF0A    *(vu32*)0x40000A0
#define REG_SGFIFOA_L  *(vu16*)0x40000A0
#define REG_SGFIFOA_H  *(vu16*)0x40000A2
#define REG_SGFIFOB    *(vu32*)0x40000A4
#define REG_SGFIFOB_L  *(vu16*)0x40000A4
#define REG_SGFIFOB_H  *(vu16*)0x40000A6
#define REG_DM0SAD     *(vu32*)0x40000B0
#define REG_DM0SAD_L   *(vu16*)0x40000B0
#define REG_DM0SAD_H   *(vu16*)0x40000B2
#define REG_DM0DAD     *(vu32*)0x40000B4
#define REG_DM0DAD_L   *(vu16*)0x40000B4
#define REG_DM0DAD_H   *(vu16*)0x40000B6
#define REG_DM0CNT     *(vu32*)0x40000B8
#define REG_DM0CNT_L   *(vu16*)0x40000B8
#define REG_DM0CNT_H   *(vu16*)0x40000BA
#define REG_DM1SAD     *(vu32*)0x40000BC
#define REG_DM1SAD_L   *(vu16*)0x40000BC
#define REG_DM1SAD_H   *(vu16*)0x40000BE
#define REG_DM1DAD     *(vu32*)0x40000C0
#define REG_DM1DAD_L   *(vu16*)0x40000C0
#define REG_DM1DAD_H   *(vu16*)0x40000C2
#define REG_DM1CNT     *(vu32*)0x40000C4
#define REG_DM1CNT_L   *(vu16*)0x40000C4
#define REG_DM1CNT_H   *(vu16*)0x40000C6
#define REG_DM2SAD     *(vu32*)0x40000C8
#define REG_DM2SAD_L   *(vu16*)0x40000C8
#define REG_DM2SAD_H   *(vu16*)0x40000CA
#define REG_DM2DAD     *(vu32*)0x40000CC
#define REG_DM2DAD_L   *(vu16*)0x40000CC
#define REG_DM2DAD_H   *(vu16*)0x40000CE
#define REG_DM2CNT     *(vu32*)0x40000D0
#define REG_DM2CNT_L   *(vu16*)0x40000D0
#define REG_DM2CNT_H   *(vu16*)0x40000D2
#define REG_DM3SAD     *(vu32*)0x40000D4
#define REG_DM3SAD_L   *(vu16*)0x40000D4
#define REG_DM3SAD_H   *(vu16*)0x40000D6
#define REG_DM3DAD     *(vu32*)0x40000D8
#define REG_DM3DAD_L   *(vu16*)0x40000D8
#define REG_DM3DAD_H   *(vu16*)0x40000DA
#define REG_DM3CNT     *(vu32*)0x40000DC
#define REG_DM3CNT_L   *(vu16*)0x40000DC
#define REG_DM3CNT_H   *(vu16*)0x40000DE
#define REG_TM0D       *(vu16*)0x4000100
#define REG_TM0CNT     *(vu16*)0x4000102
#define REG_TM1D       *(vu16*)0x4000104
#define REG_TM1CNT     *(vu16*)0x4000106
#define REG_TM2D       *(vu16*)0x4000108
#define REG_TM2CNT     *(vu16*)0x400010A
#define REG_TM3D       *(vu16*)0x400010C
#define REG_TM3CNT     *(vu16*)0x400010E
#define REG_SCD0       *(vu16*)0x4000120
#define REG_SCD1       *(vu16*)0x4000122
#define REG_SCD2       *(vu16*)0x4000124
#define REG_SCD3       *(vu16*)0x4000126
#define REG_SCCNT      *(vu32*)0x4000128
#define REG_SCCNT_L    *(vu16*)0x4000128
#define REG_SCCNT_H    *(vu16*)0x400012A
#define REG_KEYINPUT   *(vu16*)0x4000130
#define REG_KEYCNT     *(vu16*)0x4000132
#define REG_R          *(vu16*)0x4000134
#define REG_HS_CTRL    *(vu16*)0x4000140
#define REG_JOYRE      *(vu32*)0x4000150
#define REG_JOYRE_L    *(vu16*)0x4000150
#define REG_JOYRE_H    *(vu16*)0x4000152
#define REG_JOYTR      *(vu32*)0x4000154
#define REG_JOYTR_L    *(vu16*)0x4000154
#define REG_JOYTR_H    *(vu16*)0x4000156
#define REG_JSTAT      *(vu32*)0x4000158
#define REG_JSTAT_L    *(vu16*)0x4000158
#define REG_JSTAT_H    *(vu16*)0x400015A
#define REG_IE         *(vu16*)0x4000200
#define REG_IF         *(vu16*)0x4000202
#define REG_WSCNT      *(vu16*)0x4000204
#define REG_IME        *(vu16*)0x4000208
#define REG_PAUSE      *(vu16*)0x4000300

// DMA control register values

// Destination address increment types. Possible values are:
//    VAL_DMACNT_DEST_INC_IW - Increment and writeback
//    VAL_DMACNT_DEST_INC_DW - Decrement and writeback
//    VAL_DMACNT_DEST_INC_NC - No change
//    VAL_DMACNT_DEST_INC_IN - Increment, no writeback
#define VAL_DMACNT_DEST_INC_IW 0x00000000
#define VAL_DMACNT_DEST_INC_DW 0x00200000
#define VAL_DMACNT_DEST_INC_NC 0x00400000
#define VAL_DMACNT_DEST_INC_IN 0x00600000

// Source address increment types. Possible values are:
//    VAL_DMACNT_SRC_INC_IW - Increment and writeback
//    VAL_DMACNT_SRC_INC_DW - Decrement and writeback
//    VAL_DMACNT_SRC_INC_NC - No change
#define VAL_DMACNT_SRC_INC_IW  0x00000000
#define VAL_DMACNT_SRC_INC_DW  0x00800000
#define VAL_DMACNT_SRC_INC_NC  0x01000000

// Repeat flag - if true, DMA will repeat (for hblank/vblank triggered DMA only)
#define VAL_DMACNT_REPEAT      0x02000000

// Size - copy 32-bit or 16-bit values
#define VAL_DMACNT_SIZE_32     0x04000000
#define VAL_DMACNT_SIZE_16     0x00000000

// Start mode. Possible values are:
//   VAL_DMACNT_START_NOW - Start immediately
//   VAL_DMACNT_START_VBL - Start on vblank
//   VAL_DMACNT_START_HBL - Start on hblank
//   VAL_DMACNT_START_SND - Start on FIFO-empty requests (DMA1/2) or at start of rendering line (DMA3)
#define VAL_DMACNT_START_NOW   0x00000000
#define VAL_DMACNT_START_VBL   0x10000000
#define VAL_DMACNT_START_HBL   0x20000000
#define VAL_DMACNT_START_SND   0x30000000

// If set, generate an interrupt on DMA completion
#define VAL_DMACNT_IRQ         0x40000000

// If set, enable DMA
#define VAL_DMACNT_ENABLE      0x80000000


//Key Definitions
#define KEY_A 1
#define KEY_B 2
#define KEY_SELECT 4
#define KEY_START 8
#define KEY_RIGHT 16
#define KEY_LEFT 32
#define KEY_UP 64
#define KEY_DOWN 128
#define KEY_R 256
#define KEY_L 512

#define DISP_VBLANK	1
#define DISP_VBLANK_INT_EN	8

	//this converts a color value to 15 bit BGR value used by GBA
#define RGB(r,g,b)  (((r>>3))+((g>>3)<<5)+((b>>3)<<10))

#define SCREENWIDTH 240  //width of the screen in pixels
#define SCREENHEIGHT 160 //height


///REG_DISPCNT Defines////
#define BG0_ENABLE		0x100 
#define BG1_ENABLE		0x200
#define BG2_ENABLE		0x400
#define BG3_ENABLE		0x800
#define OBJ_ENABLE		0x1000
#define WIN0_ENABLE		0x2000
#define WIN1_ENABLE		0x4000
#define WINOBJ_ENABLE	0x8000

#define OBJ_MAP_1D      0x40
#define OBJ_MAP_2D      0x0
#define BACK_BUFFER     0x10

#define MODE0			0x0
#define MODE1			0x1
#define MODE2			0x2
#define MODE3			0x3
#define MODE4			0x4
#define MODE5			0x5


#define INT_VBLANK		0x0001
#define INT_HBLANK		0x0002
#define INT_DMA0		0x0100
#define INT_DMA1		0x0200
#define INT_DMA2		0x0400
#define INT_DMA3		0x0800
#define INT_VCOUNT		0x0004
#define INT_TIMER0		0x0008
#define INT_TIMER1		0x0010
#define INT_TIMER2		0x0020
#define INT_TIMER3		0x0040
#define INT_KEYBOARD	0x1000
#define INT_CART		0x2000
#define INT_COMMS		0x0080

//Atribute0 stuff
#define ROTATION_FLAG 		0x100
#define SIZE_DOUBLE			0x200
#define MODE_NORMAL     	0x0
#define MODE_TRANSPERANT	0x400
#define MODE_WINDOWED		0x800
#define MOSAIC				0x1000
#define COLOR_16			0x0000
#define COLOR_256			0x2000
#define SQUARE			0x0
#define TALL			0x4000
#define WIDE			0x8000

//Atribute1 stuff
#define ROTDATA(n)		((n) << 9)
#define HORIZONTAL_FLIP		0x1000
#define VERTICAL_FLIP		0x2000
#define SIZE_8			0x0
#define SIZE_16			0x4000
#define SIZE_32			0x8000
#define SIZE_64			0xC000

//atribute2 stuff

#define PRIORITY(n)		((n) << 10)
#define PALETTE(n)		((n) << 12)


// timers
#define TIME_FREQ_SYSTEM 0x0
#define TIME_FREQ_64 0x1
#define TIME_FREQ_256 0x2
#define TIME_FREQ_1024 0x3
#define TIME_OVERFLOW 0x4
#define TIME_ENABLE 0x80
#define TIME_IRQ_ENABLE 0x40


#endif

