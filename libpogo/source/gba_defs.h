#ifndef GBA_DEFS_H
#define GBA_DEFS_H

#define OAM_COLOR_256           0x00002000  // Select 256 colors


#define VRAM_BASE ((uint16 *)0x06000000)

#define OBJ_VRAM ((uint16 *)0x06014000)
#define OAM_BASE ((uint16 *)0x07000000)

#define BG_PALRAM ((uint16*)0x05000000)
#define OBJ_PALRAM ((uint16*)0x05000200)

#define SETW(adr, val) (*((volatile uint16*)adr) = val)
#define SETL(adr, val) (*((volatile uint32*)adr) = val)
#define GETW(adr) (*(volatile uint16 *)adr)

#define BUTTON_MASK             0x030f      // Button
#define PLUS_KEY_MASK           0x00f0      // +Control Pad
#define ALL_KEY_MASK            0x03ff      // All Key

#define A_BUTTON                0x0001      // A Button
#define B_BUTTON                0x0002      // B Button
#define SELECT_BUTTON           0x0004      // SELECT
#define START_BUTTON            0x0008      // START
#define R_KEY                   0x0010      // Right Button
#define L_KEY                   0x0020      // Left Button
#define U_KEY                   0x0040      // Up Button
#define D_KEY                   0x0080      // Down Button
#define R_BUTTON                0x0100      // R Button
#define L_BUTTON                0x0200      // L Button
#define KEY_IF_ENABLE           0x4000      // Interrupt Request Enable
#define KEY_OR_INTR             0x0000      // Normal Key Interrupt
#define KEY_AND_INTR            0x8000      // AND Key Interrupt

/*------------------------------------------------------------------*/
/*                      Memory Map                                  */
/*------------------------------------------------------------------*/

#define BOOT_ROM                0x00000000          // Boot ROM
#define BOOT_ROM_END            (BOOT_ROM + 0x4000)
#define EX_WRAM                 0x02000000          // CPU External Work RAM
#define EX_WRAM_END             (EX_WRAM + 0x40000)
#define CPU_WRAM                0x03000000          // CPU Internal Work RAM
#define CPU_WRAM_END            (CPU_WRAM + 0x8000)
#define WRAM                     EX_WRAM            // Entire Work RAM
#define WRAM_END                 CPU_WRAM_END
#define SOUND_AREA_ADDR_BUF     (CPU_WRAM_END - 0x10)//Sound Driver Work Address
#define INTR_CHECK_BUF          (CPU_WRAM_END - 0x8)// Interrupt Check
#define SOFT_RESET_DIRECT_BUF   (CPU_WRAM_END - 0x6)// SoftReset() Specify Return to
#define INTR_VECTOR_BUF         (CPU_WRAM_END - 0x4)// Interrupt Branch Address
#define REG_BASE                0x04000000          // Registers
#define REG_END                 (REG_BASE +  0x300)
#define PLTT                    0x05000000          // Palette RAM
#define PLTT_END                (PLTT +      0x400)
#define BG_PLTT                 (PLTT +        0x0) // BG Palette RAM
#define BG_PLTT_END             (PLTT +      0x200)
#define OBJ_PLTT                (PLTT +      0x200) // OBJ Palette RAM
#define OBJ_PLTT_END            (PLTT +      0x400)
#define VRAM                    0x06000000          // VRAM
#define VRAM_END                (VRAM +    0x18000)
#define BG_VRAM                 (VRAM +        0x0) // BG Character/Screen RAM
#define BG_BITMAP0_VRAM         (VRAM +        0x0) // BG Bitmap 0 RAM
#define BG_BITMAP1_VRAM         (VRAM +     0xa000) // BG Bitmap 1 RAM
#define OBJ_MODE0_VRAM          (VRAM +    0x10000) // OBJ Character RAM
#define OBJ_MODE1_VRAM          (VRAM +    0x10000)
#define OBJ_MODE2_VRAM          (VRAM +    0x10000)
#define OBJ_MODE3_VRAM          (VRAM +    0x14000)
#define OBJ_MODE4_VRAM          (VRAM +    0x14000)
#define OBJ_MODE5_VRAM          (VRAM +    0x14000)
#define OBJ_VRAM_END            (VRAM +    0x18000)
#define OAM                     0x07000000          // OAM
#define OAM_END                 (OAM +       0x400)

#define ROM_BANK0               0x08000000          // ROM Bank 0
#define ROM_BANK0_END           0x0a000000
#define ROM_BANK1               0x0a000000          // ROM Bank 1
#define ROM_BANK1_END           0x0c000000
#define ROM_BANK2               0x0c000000          // ROM Bank 2
#define ROM_BANK2_END           0x0e000000
#define RAM_BANK                0x0c000000          // RAM Bank
#define RAM_BANK_END            0x10000000

#define FLASH_1M                0x09fe0000          // 1M Flash Memory
#define FLASH_1M_END            0x0a000000

/*------------------------------------------------------------------*/
/*                      Memory Size                                 */
/*------------------------------------------------------------------*/

#define BOOT_ROM_SIZE           0x4000              // Boot ROM
#define EX_WRAM_SIZE            0x40000             // CPU External Work RAM
#define CPU_WRAM_SIZE           0x8000              // CPU Internal Work RAM
#define WRAM_SIZE               0x48000             // Entire Work RAM
#define PLTT_SIZE               (2*256*2)           // Palette RAM
#define BG_PLTT_SIZE            (2*256)             // BG Palette RAM
#define OBJ_PLTT_SIZE           (2*256)             // OBJ Palette RAM
#define VRAM_SIZE               0x18000             // VRAM
#define BG_MODE0_VRAM_SIZE      0x10000             // BG Character/Screen
#define BG_MODE1_VRAM_SIZE      0x10000
#define BG_MODE2_VRAM_SIZE      0x10000
#define BG_MODE3_VRAM_SIZE      0x14000             // BG Bitmap
#define BG_MODE4_VRAM_SIZE      0x0a000
#define BG_MODE5_VRAM_SIZE      0x0a000
#define OBJ_MODE0_VRAM_SIZE     0x08000             // OBJ Character
#define OBJ_MODE1_VRAM_SIZE     0x08000
#define OBJ_MODE2_VRAM_SIZE     0x08000
#define OBJ_MODE3_VRAM_SIZE     0x04000
#define OBJ_MODE4_VRAM_SIZE     0x04000
#define OBJ_MODE5_VRAM_SIZE     0x04000
#define OAM_SIZE                (8*128)             // OAM

#define ROM_BANK_SIZE           0x02000000          // ROM Bank
#define ROM_BANK0_SIZE          0x02000000          // ROM Bank 0
#define ROM_BANK1_SIZE          0x02000000          // ROM Bank 1
#define ROM_BANK2_SIZE          0x02000000          // ROM Bank 2
#define RAM_BANK_SIZE           0x02000000          // RAM Bank

#define FLASH_1M_SIZE           0x20000             // 1M Flash Memory

/*------------------------------------------------------------------*/
/*                      Register Address                            */
/*------------------------------------------------------------------*/

#define REG_IME         (REG_BASE + 0x208)  // Interrupt Master Enable
#define REG_IE          (REG_BASE + 0x200)  // Interrupt Enable
#define REG_IF          (REG_BASE + 0x202)  // Interrupt Request

#define REG_WAITCNT     (REG_BASE + 0x204)  // Game Pak Wait Control

#define REG_DISPCNT     (REG_BASE + 0x0)    // Display Control

#define REG_STAT        (REG_BASE + 0x4)    // Status
#define REG_VCOUNT      (REG_BASE + 0x6)    // V Counter

#define REG_MOSAIC      (REG_BASE + 0x4c)   // Mosaic Size

#define REG_BLDCNT      (REG_BASE + 0x50)   // Blend Mode Control
#define REG_BLDALPHA    (REG_BASE + 0x52)   // Semi-transparent Parameter
#define REG_BLDY        (REG_BASE + 0x54)   // Brightness Change Parameter

#define REG_WINCNT      (REG_BASE + 0x40)   // Window Control
#define REG_WIN0H       (REG_BASE + 0x40)   // Window 0 Horizontal Area
#define REG_WIN1H       (REG_BASE + 0x42)   // Window 1 Horizontal Area
#define REG_WIN0V       (REG_BASE + 0x44)   // Window 0 Vertical Area
#define REG_WIN1V       (REG_BASE + 0x46)   // WIndow 1 Vertical Area
#define REG_WININ       (REG_BASE + 0x48)   // Internal Window Control
#define REG_WIN0        (REG_BASE + 0x48)   // Window 0 Control
#define REG_WIN1        (REG_BASE + 0x49)   // Window 1 Control
#define REG_WINOUT      (REG_BASE + 0x4a)   // External Window Control
#define REG_OBJWIN      (REG_BASE + 0x4b)   // OBJ Window Control


#define REG_BGCNT       (REG_BASE + 0x8)    // BG Control
#define REG_BG0CNT      (REG_BASE + 0x8)    // BG 0 Control
#define REG_BG1CNT      (REG_BASE + 0xa)    // BG 1 Control
#define REG_BG2CNT      (REG_BASE + 0xc)    // BG 2 Control
#define REG_BG3CNT      (REG_BASE + 0xe)    // BG 3 Control
#define REG_BGOFS       (REG_BASE + 0x10)   // BG Offset
#define REG_BG0HOFS     (REG_BASE + 0x10)   // BG 0 H Offset
#define REG_BG0VOFS     (REG_BASE + 0x12)   // BG 0 V Offset
#define REG_BG1HOFS     (REG_BASE + 0x14)   // BG 1 H Offset
#define REG_BG1VOFS     (REG_BASE + 0x16)   // BG 1 V Offset
#define REG_BG2HOFS     (REG_BASE + 0x18)   // BG 2 H Offset
#define REG_BG2VOFS     (REG_BASE + 0x1a)   // BG 2 V Offset
#define REG_BG3HOFS     (REG_BASE + 0x1c)   // BG 3 H Offset
#define REG_BG3VOFS     (REG_BASE + 0x1e)   // BG 3 V Offset
#define REG_BG2AFFINE   (REG_BASE + 0x20)   // BG 2 Affin Transformation Parameters
#define REG_BG2PA       (REG_BASE + 0x20)   // BG 2 Line Direction X Coordinate Difference
#define REG_BG2PB       (REG_BASE + 0x22)   // BG 3 Vertical Direction X Coordinate Difference
#define REG_BG2PC       (REG_BASE + 0x24)   // BG 2 Line Direction Y coordinate Difference
#define REG_BG2PD       (REG_BASE + 0x26)   // BG 2 Vertical Direction Y Coordinate Difference
#define REG_BG2X        (REG_BASE + 0x28)   // BG 2 Start X Coordinate
#define REG_BG2X_L      (REG_BASE + 0x28)
#define REG_BG2X_H      (REG_BASE + 0x2a)
#define REG_BG2Y        (REG_BASE + 0x2c)   // BG 2 Start Y Coordinate
#define REG_BG2Y_L      (REG_BASE + 0x2c)
#define REG_BG2Y_H      (REG_BASE + 0x2e)
#define REG_BG3AFFINE   (REG_BASE + 0x30)   // BG 3 Affin Transformation Parameters
#define REG_BG3PA       (REG_BASE + 0x30)   // BG 3 Line Direction X Coordinate Difference
#define REG_BG3PB       (REG_BASE + 0x32)   // BG 3 Vertical Direction X Coordinate Difference
#define REG_BG3PC       (REG_BASE + 0x34)   // BG 3 Line Direction Y Coordinate Difference
#define REG_BG3PD       (REG_BASE + 0x36)   // BG 3 Vertical Direction Y Coordinate Difference
#define REG_BG3X        (REG_BASE + 0x38)   // BG 3 Start X Coordinate
#define REG_BG3X_L      (REG_BASE + 0x38)
#define REG_BG3X_H      (REG_BASE + 0x3a)
#define REG_BG3Y        (REG_BASE + 0x3c)   // BG 3 Start Y Coordinate
#define REG_BG3Y_L      (REG_BASE + 0x3c)
#define REG_BG3Y_H      (REG_BASE + 0x3e)


#define REG_SOUNDCNT    (REG_BASE + 0x80)   // Sound Control
#define REG_SOUNDCNT_L  (REG_BASE + 0x80)
#define REG_SOUNDCNT_H  (REG_BASE + 0x82)
#define REG_SOUNDCNT_X  (REG_BASE + 0x84)

#define REG_SOUNDBIAS   (REG_BASE + 0x88)   // Sound BIAS

#define REG_SOUND1CNT   (REG_BASE + 0x60)   // Sound 1 Control
#define REG_SOUND1CNT_L (REG_BASE + 0x60)
#define REG_SOUND1CNT_H (REG_BASE + 0x62)
#define REG_SOUND1CNT_X (REG_BASE + 0x64)
#define REG_SOUND2CNT   (REG_BASE + 0x68)   // Sound 2 Control
#define REG_SOUND2CNT_L (REG_BASE + 0x68)
#define REG_SOUND2CNT_H (REG_BASE + 0x6c)
#define REG_SOUND3CNT   (REG_BASE + 0x70)   // Sound 3 Control
#define REG_SOUND3CNT_L (REG_BASE + 0x70)
#define REG_SOUND3CNT_H (REG_BASE + 0x72)
#define REG_SOUND3CNT_X (REG_BASE + 0x74)
#define REG_SOUND4CNT   (REG_BASE + 0x78)   // Sound 4 Control
#define REG_SOUND4CNT_L (REG_BASE + 0x78)
#define REG_SOUND4CNT_H (REG_BASE + 0x7c)

#define REG_WAVE_RAM    (REG_BASE + 0x90)   // Sound 3 Waveform RAM
#define REG_WAVE_RAM0   (REG_BASE + 0x90)
#define REG_WAVE_RAM0_L (REG_BASE + 0x90)
#define REG_WAVE_RAM0_H (REG_BASE + 0x92)
#define REG_WAVE_RAM1   (REG_BASE + 0x94)
#define REG_WAVE_RAM1_L (REG_BASE + 0x94)
#define REG_WAVE_RAM1_H (REG_BASE + 0x96)
#define REG_WAVE_RAM2   (REG_BASE + 0x98)
#define REG_WAVE_RAM2_L (REG_BASE + 0x98)
#define REG_WAVE_RAM2_H (REG_BASE + 0x9a)
#define REG_WAVE_RAM3   (REG_BASE + 0x9c)
#define REG_WAVE_RAM3_L (REG_BASE + 0x9c)
#define REG_WAVE_RAM3_H (REG_BASE + 0x9e)

#define REG_FIFO        (REG_BASE + 0xa0)   // Sound FIFO
#define REG_FIFO_A      (REG_BASE + 0xa0)
#define REG_FIFO_A_L    (REG_BASE + 0xa0)
#define REG_FIFO_A_H    (REG_BASE + 0xa2)
#define REG_FIFO_B      (REG_BASE + 0xa4)
#define REG_FIFO_B_L    (REG_BASE + 0xa4)
#define REG_FIFO_B_H    (REG_BASE + 0xa6)


#define REG_DMA0        (REG_BASE + 0xb0)   // DMA 0
#define REG_DMA0SAD     (REG_BASE + 0xb0)   // (Internal Memory -> 
                                            // Internal Memory)
#define REG_DMA0SAD_L   (REG_BASE + 0xb0)
#define REG_DMA0SAD_H   (REG_BASE + 0xb2)
#define REG_DMA0DAD     (REG_BASE + 0xb4)
#define REG_DMA0DAD_L   (REG_BASE + 0xb4)
#define REG_DMA0DAD_H   (REG_BASE + 0xb6)
#define REG_DMA0CNT     (REG_BASE + 0xb8)
#define REG_DMA0CNT_L   (REG_BASE + 0xb8)
#define REG_DMA0CNT_H   (REG_BASE + 0xba)
#define REG_DMA1        (REG_BASE + 0xbc)   // DMA 1
#define REG_DMA1SAD     (REG_BASE + 0xbc)   // (Internal Memory -> Sound 
                                            // FIFO/Internal Memory)
#define REG_DMA1SAD_L   (REG_BASE + 0xbc)
#define REG_DMA1SAD_H   (REG_BASE + 0xbe)
#define REG_DMA1DAD     (REG_BASE + 0xc0)
#define REG_DMA1DAD_L   (REG_BASE + 0xc0)
#define REG_DMA1DAD_H   (REG_BASE + 0xc2)
#define REG_DMA1CNT     (REG_BASE + 0xc4)
#define REG_DMA1CNT_L   (REG_BASE + 0xc4)
#define REG_DMA1CNT_H   (REG_BASE + 0xc6)
#define REG_DMA2        (REG_BASE + 0xc8)   // DMA 2
#define REG_DMA2SAD     (REG_BASE + 0xc8)   // (Internal Memory -> Sound 
                                            // FIFO/Internal Memory)
#define REG_DMA2SAD_L   (REG_BASE + 0xc8)
#define REG_DMA2SAD_H   (REG_BASE + 0xca)
#define REG_DMA2DAD     (REG_BASE + 0xcc)
#define REG_DMA2DAD_L   (REG_BASE + 0xcc)
#define REG_DMA2DAD_H   (REG_BASE + 0xce)
#define REG_DMA2CNT     (REG_BASE + 0xd0)
#define REG_DMA2CNT_L   (REG_BASE + 0xd0)
#define REG_DMA2CNT_H   (REG_BASE + 0xd2)
#define REG_DMA3        (REG_BASE + 0xd4)   // DMA 3
#define REG_DMA3SAD     (REG_BASE + 0xd4)   // (Game Pak/Internal Memory <--> 
                                            // Internal Memory)
#define REG_DMA3SAD_L   (REG_BASE + 0xd4)
#define REG_DMA3SAD_H   (REG_BASE + 0xd6)
#define REG_DMA3DAD     (REG_BASE + 0xd8)
#define REG_DMA3DAD_L   (REG_BASE + 0xd8)
#define REG_DMA3DAD_H   (REG_BASE + 0xda)
#define REG_DMA3CNT     (REG_BASE + 0xdc)
#define REG_DMA3CNT_L   (REG_BASE + 0xdc)
#define REG_DMA3CNT_H   (REG_BASE + 0xde)


#define REG_TM0CNT      (REG_BASE + 0x100)  // Timer 0
#define REG_TM0CNT_L    (REG_BASE + 0x100)
#define REG_TM0CNT_H    (REG_BASE + 0x102)
#define REG_TM1CNT      (REG_BASE + 0x104)  // Timer 1
#define REG_TM1CNT_L    (REG_BASE + 0x104)
#define REG_TM1CNT_H    (REG_BASE + 0x106)
#define REG_TM2CNT      (REG_BASE + 0x108)  // Timer 2
#define REG_TM2CNT_L    (REG_BASE + 0x108)
#define REG_TM2CNT_H    (REG_BASE + 0x10a)
#define REG_TM3CNT      (REG_BASE + 0x10c)  // Timer 3
#define REG_TM3CNT_L    (REG_BASE + 0x10c)
#define REG_TM3CNT_H    (REG_BASE + 0x10e)


#define REG_RCNT        (REG_BASE + 0x134)  // General Input/Output Control

#define REG_SIOCNT      (REG_BASE + 0x128)  // Serial Communication Control
#define REG_SIODATA8    (REG_BASE + 0x12a)  // 8bit Serial Communication Data
#define REG_SIODATA32   (REG_BASE + 0x120)  // 32bit Serial Communication Data
#define REG_SIOMLT_SEND (REG_BASE + 0x12a)  // Multi-play SIO Send Data
#define REG_SIOMLT_RECV (REG_BASE + 0x120)  // Multi-play SIO Receive Data
#define REG_SIOMULTI0   (REG_BASE + 0x120)
#define REG_SIOMULTI1   (REG_BASE + 0x122)
#define REG_SIOMULTI2   (REG_BASE + 0x124)
#define REG_SIOMULTI3   (REG_BASE + 0x126)

#define REG_JOYCNT      (REG_BASE + 0x140)  // JOY Bus Communication Control
#define REG_JOYSTAT     (REG_BASE + 0x158)  // JOY Bus Status
#define REG_JOY_RECV    (REG_BASE + 0x150)  // JOY Bus Receive Data
#define REG_JOY_RECV_L  (REG_BASE + 0x150)
#define REG_JOY_RECV_H  (REG_BASE + 0x152)
#define REG_JOY_TRANS   (REG_BASE + 0x154)  // JOY Bus Send Data
#define REG_JOY_TRANS_L (REG_BASE + 0x154)
#define REG_JOY_TRANS_H (REG_BASE + 0x156)


#define REG_KEYINPUT    (REG_BASE + 0x130)  // Key Input
#define REG_KEYCNT      (REG_BASE + 0x132)  // Key Control



#define LCD_WIDTH               240         // Screen Width
#define LCD_HEIGHT              160         // Screen Height

#define SYSTEM_CLOCK           (16 * 1024 * 1024)   // System Clock


/*------------------------------------------------------------------*/
/*                Register & RAM Reset System Call                  */
/*------------------------------------------------------------------*/

#define RESET_ALL_FLAG          0xff        // Reset all

#define RESET_ALL_REG_FLAG      0xe0        // Reset all registers

#define RESET_REG_FLAG          0x80        // Reset registers 
                                            // (other than stated below)
#define RESET_REG_SOUND_FLAG    0x40        // Reset sound register
#define RESET_REG_SIO_FLAG      0x20        // Reset SIO register

#define RESET_ALL_RAM_FLAG      0x1f        // Clear the entire RAM

#define RESET_EX_WRAM_FLAG      0x01        // Clear CPU external RAM
#define RESET_CPU_WRAM_FLAG     0x02        // Clear CPU internal RAM
#define RESET_PLTT_FLAG         0x04        // Clear palette
#define RESET_VRAM_FLAG         0x08        // Clear VRAM
#define RESET_OAM_FLAG          0x10        // Clear OAM


/*------------------------------------------------------------------*/
/*                    BG Screen Data                                */
/*------------------------------------------------------------------*/

#define BG_SC_CHAR_NO_MASK      0x03ff      // Character No
#define BG_SC_FLIP_MASK         0x0c00      // Flip
#define BG_SC_PLTT_MASK         0xf000      // Color Palette No

#define BG_SC_CHAR_NO_SHIFT     0
#define BG_SC_FLIP_SHIFT        10
#define BG_SC_PLTT_SHIFT        12

#define BG_SC_H_FLIP            0x0400      // H Flip
#define BG_SC_V_FLIP            0x0800      // V Flip

/*------------------------------------------------------------------*/
/*                        Affine Transformation                     */
/*------------------------------------------------------------------*/

#define SIN_SHIFT               14
#define AFFINE_DIV_SHIFT        8
#define AFFINE_REG_SHIFT        8
#define AFFINE_CAL_SHIFT        (SIN_SHIFT + AFFINE_DIV_SHIFT - AFFINE_REG_SHIFT)

/*------------------------------------------------------------------*/
/*                        OAM Data                                  */
/*------------------------------------------------------------------*/

#define OAM_V_POS_MASK          0x000000ff  // OBJ X Coordinate
#define OAM_H_POS_MASK          0x01ff0000  // OBJ Y Coordinate
#define OAM_SHAPE_MASK          0x0000c000  // OBJ shape
#define OAM_SIZE_MASK           0xc0000000  // OBJ size
#define OAM_OBJ_MODE_MASK       0x00000c00  // OBJ Mode
#define OAM_AFFINE_MODE_MASK    0x00000300  // Affine Mode
#define OAM_AFFINE_NO_MASK      0x3e000000  // Affine Parameter No
#define OAM_FLIP_MASK           0x30000000  // Flip
#define OAM_CHAR_NO_MASK        0x000003ff  // Character No
#define OAM_PRIORITY_MASK       0x00000c00  // BG Relative Priority
#define OAM_PLTT_MASK           0x0000f000  // Color Palette No
#define OAM_AFFINE_PARAM_MASK   0xffff0000  // Affine Parameter

#define OAM_V_POS_SHIFT         0
#define OAM_H_POS_SHIFT         16
#define OAM_SHAPE_SHIFT         14
#define OAM_SIZE_SHIFT          30
#define OAM_AFFINE_MODE_SHIFT   8
#define OAM_OBJ_MODE_SHIFT      10
#define OAM_AFFINE_NO_SHIFT     25
#define OAM_FLIP_SHIFT          28
#define OAM_CHAR_NO_SHIFT       0
#define OAM_PRIORITY_SHIFT      10
#define OAM_PLTT_SHIFT          12
#define OAM_AFFINE_PARAM_SHIFT  16

#define OAM_OBJ_ON              0x00000000  // OBJ ON
#define OAM_OBJ_OFF             0x00000200  // OBJ OFF
#define OAM_OBJ_NORMAL          0x00000000  // OBJ Normal Mode
#define OAM_OBJ_BLEND           0x00000400  // OBJ Semi-transparent Mode
#define OAM_OBJ_WINDOW          0x00000800  // OBJ Window Mode
#define OAM_AFFINE_NONE         0x00000000  // Affine Invalid Mode
#define OAM_AFFINE_NORMAL       0x00000100  // Affine Normal Mode
#define OAM_AFFINE_TWICE        0x00000300  // Affine Double-size Mode
#define OAM_AFFINE_ERASE        0x00000200  // Affine non-display Mode
#define OAM_MOS_ON              0x00001000  // Mosaic ON
#define OAM_MOS_OFF             0x00000000  // Mosaic OFF
#define OAM_COLOR_16            0x00000000  // Select 16
#define OAM_COLOR_256           0x00002000  // Select 256 colors
#define OAM_H_FLIP              0x10000000  // H Flip
#define OAM_V_FLIP              0x20000000  // V Flip
#define OAM_SQUARE              0x00000000  // Square OBJ
#define OAM_H_RECTANGLE         0x00004000  // Horizontally-oriented rectangle OBJ
#define OAM_V_RECTANGLE         0x00008000  // Vertically-oriented rectangle OBJ
#define OAM_SIZE_NO_0           0x00000000  // OBJ size No 0
#define OAM_SIZE_NO_1           0x40000000  // OBJ size No 1
#define OAM_SIZE_NO_2           0x80000000  // OBJ size No 2
#define OAM_SIZE_NO_3           0xc0000000  // OBJ size No 3
#define OAM_SIZE_8x8            0x00000000  // OBJ 8 x 8 dot
#define OAM_SIZE_16x16          0x40000000  // OBJ 16 x 16 dot
#define OAM_SIZE_32x32          0x80000000  // OBJ 32 x 32 dot
#define OAM_SIZE_64x64          0xc0000000  // OBJ 64 x 64 dot
#define OAM_SIZE_16x8           0x00004000  // OBJ 16 x 8 dot
#define OAM_SIZE_32x8           0x40004000  // OBJ 32 x 8 dot
#define OAM_SIZE_32x16          0x80004000  // OBJ 32 x 16 dot
#define OAM_SIZE_64x32          0xc0004000  // OBJ 64 x 32 dot
#define OAM_SIZE_8x16           0x00008000  // OBJ 8 x 16 dot
#define OAM_SIZE_8x32           0x40008000  // OBJ 8 x 32 dot
#define OAM_SIZE_16x32          0x80008000  // OBJ 16 x 32 dot
#define OAM_SIZE_32x64          0xc0008000  // OBJ 32 x 64 dot


/*------------------------------------------------------------------*/
/*                      Color Palette Data                          */
/*------------------------------------------------------------------*/

#define PLTT_RED_MASK           0x001f      // Red
#define PLTT_GREEN_MASK         0x03e0      // Green
#define PLTT_BLUE_MASK          0x7c00      // Blue

#define PLTT_RED_SHIFT          0
#define PLTT_GREEN_SHIFT        5
#define PLTT_BLUE_SHIFT         10


/********************************************************************/
/*              AGB Register Control Bit                            */
/********************************************************************/

/*------------------------------------------------------------------*/
/*                  Program Status Register                         */
/*------------------------------------------------------------------*/

#define PSR_CPU_MODE_MASK       0x1f        // CPU Mode

#define PSR_USER_MODE           0x10        // User
#define PSR_FIQ_MODE            0x11        // FIQ
#define PSR_IRQ_MODE            0x12        // IRQ
#define PSR_SVC_MODE            0x13        // Supervisor
#define PSR_ABORT_MODE          0x17        // Abort (Prefetch/Data)
#define PSR_UNDEF_MODE          0x1b        // Undefined Command
#define PSR_SYS_MODE            0x1f        // System

#define PSR_THUMB_STATE         0x20        // THUMB State
#define PSR_FIQ_DISABLE         0x40        // FIQ Disable
#define PSR_IRQ_DISABLE         0x80        // IRQ Disable
#define PSR_IRQ_FIQ_DISABLE     0xc0        // IRQ & FIQ Disable

#define PSR_V_FLAG              0x10000000  // Overflow
#define PSR_C_FLAG              0x20000000  // Carry/Borrow/Extend
#define PSR_Z_FLAG              0x40000000  // Zero
#define PSR_N_FLAG              0x80000000  // Negative/Less Than


/*------------------------------------------------------------------*/
/*                  Interrupt Request/ Enable Flag                  */
/*------------------------------------------------------------------*/

#define V_BLANK_INTR_FLAG       0x0001      // V Blank Interrupt
#define H_BLANK_INTR_FLAG       0x0002      // H Blank Interrupt
#define V_COUNT_INTR_FLAG       0x0004      // V Counter Corresponding Interrupt
#define TIMER0_INTR_FLAG        0x0008      // Timer 0 Interrupt
#define TIMER1_INTR_FLAG        0x0010      // Timer 1 Interrupt
#define TIMER2_INTR_FLAG        0x0020      // Timer 2 Interrupt
#define TIMER3_INTR_FLAG        0x0040      // Timer 3 Interrupt
#define SIO_INTR_FLAG           0x0080      // Serial Communication Interrupt
#define DMA0_INTR_FLAG          0x0100      // DMA0 Interrupt
#define DMA1_INTR_FLAG          0x0200      // DMA1 Interrupt
#define DMA2_INTR_FLAG          0x0400      // DMA2 Interrupt
#define DMA3_INTR_FLAG          0x0800      // DMA3 Interrupt
#define KEY_INTR_FLAG           0x1000      // Key Interrupt
#define CASSETTE_INTR_FLAG      0x2000      // Game Pak Interrupt


/*------------------------------------------------------------------*/
/*                      Display Control                             */
/*------------------------------------------------------------------*/

#define DISP_BG_MODE_MASK       0x0007      // BG Mode
#define DISP_ON_MASK            0x1f00      // OBJ BG ON
#define DISP_WIN_MASK           0x6000      // Window ON

#define DISP_BG_MODE_SHIFT      0
#define DISP_ON_SHIFT           8
#define DISP_WIN_SHIFT          13

#define DISP_MODE_0             0x0000      // BG Mode 0
#define DISP_MODE_1             0x0001      // BG Mode 1
#define DISP_MODE_2             0x0002      // BG Mode 2
#define DISP_MODE_3             0x0003      // BG Mode 3
#define DISP_MODE_4             0x0004      // BG Mode 4
#define DISP_MODE_5             0x0005      // BG Mode 5
#define DISP_BMP_FRAME_NO       0x0010      // Bitmap Mode Display Frame
#define DISP_OBJ_HOFF           0x0020      // OBJ Processing in H Blank OFF
#define DISP_OBJ_CHAR_2D_MAP    0x0000      // OBJ Character Data 2D Mapping
#define DISP_OBJ_CHAR_1D_MAP    0x0040      // OBJ Character Data 1D Mapping
#define DISP_LCDC_OFF           0x0080      // LCDC OFF
#define DISP_BG0_ON             0x0100      // BG0 ON
#define DISP_BG1_ON             0x0200      // BG1 ON
#define DISP_BG2_ON             0x0400      // BG2 ON
#define DISP_BG3_ON             0x0800      // BG3 ON
#define DISP_BG_ALL_ON          0x0f00      // All BG ON
#define DISP_OBJ_ON             0x1000      // OBJ ON
#define DISP_OBJ_BG_ALL_ON      0x1f00      // All OBJ/BG ON
#define DISP_WIN0_ON            0x2000      // Window 0 ON
#define DISP_WIN1_ON            0x4000      // Window 1 ON
#define DISP_WIN01_ON           0x6000      // Window 0,1 ON
#define DISP_OBJWIN_ON          0x8000      // OBJ Window ON
#define DISP_WIN_ALL_ON         0xe000      // All Window ON
#define DISP_ALL_ON             0x7f00      // All ON

/*------------------------------------------------------------------*/
/*                      Status Control                              */
/*------------------------------------------------------------------*/

#define STAT_VCOUNT_CMP_MASK    0xff00      // V Counter Compare Value

#define STAT_VCOUNT_CMP_SHIFT   8

#define STAT_V_BLANK            0x0001      // During V Blank
#define STAT_H_BLANK            0x0002      // During H Blank
#define STAT_V_COUNT            0x0004      // Matching with V Counter
#define STAT_V_BLANK_IF_ENABLE  0x0008      // V Blank Interrupt Request, Enable
#define STAT_H_BLANK_IF_ENABLE  0x0010      // H Blank Interrupt Request, Enable
#define STAT_V_COUNT_IF_ENABLE  0x0020      // V Counter Corresponding 
                                            // Interrupt Request, Enable

/*------------------------------------------------------------------*/
/*                      BG Control                                  */
/*------------------------------------------------------------------*/

#define BG_PRIORITY_MASK        0x0003      // BG Priority
#define BG_CHAR_BASE_MASK       0x000c      // Character Base Address
#define BG_SCREEN_BASE_MASK     0x1f00      // Screen Base Address
#define BG_SCREEN_SIZE_MASK     0xc000      // Screen Size (256/512)

#define BG_PRIORITY_SHIFT       0
#define BG_CHAR_BASE_SHIFT      2
#define BG_SCREEN_BASE_SHIFT    8
#define BG_SCREEN_SIZE_SHIFT    14

#define BG_PRIORITY_0           0x0000      // BG 0 Priority
#define BG_PRIORITY_1           0x0001      // BG 1 Priority
#define BG_PRIORITY_2           0x0002      // BG 2 Priority
#define BG_PRIORITY_3           0x0003      // BG 3 Priority
#define BG_MOS_ON               0x0040      // Mosaic ON
#define BG_MOS_OFF              0x0000      // Mosaic OFF
#define BG_COLOR_16             0x0000      // Select 16 colors
#define BG_COLOR_256            0x0080      // Select 256 colors
#define BG_LOOP_ON              0x2000      // Loop ON
#define BG_LOOP_OFF             0x0000      // Loop OFF
#define BG_SCREEN_SIZE_0        0x0000      // Screen Size (256x256)
#define BG_SCREEN_SIZE_1        0x4000      // Screen Size (512x256)
#define BG_SCREEN_SIZE_2        0x8000      // Screen Size (256x512)
#define BG_SCREEN_SIZE_3        0xc000      // Screen Size (512x512)

/*------------------------------------------------------------------*/
/*                      Window Control                              */
/*------------------------------------------------------------------*/

#define WIN_END_POS_MASK        0x00ff      // Window End Position
#define WIN_START_POS_MASK      0xff00      // Window Start Position

#define WIN_END_POS_SHIFT       0
#define WIN_START_POS_SHIFT     8

#define WIN_BG0_ON              0x0001      // BG0 ON
#define WIN_BG1_ON              0x0002      // BG1 ON
#define WIN_BG2_ON              0x0004      // BG2 ON
#define WIN_BG3_ON              0x0008      // BG3 ON
#define WIN_OBJ_ON              0x0010      // OBJ ON
#define WIN_BLEND_ON            0x0020      // Blend ON
#define WIN_ALL_ON              0x003f      // All ON

/*------------------------------------------------------------------*/
/*                      Mosaic Control                              */
/*------------------------------------------------------------------*/

#define MOS_H_SIZE_MASK         0x000f      // Mosaic Height
#define MOS_V_SIZE_MASK         0x00f0      // Mosaic Width
#define MOS_BG_H_SIZE_MASK      0x000f      // BG Mosaic Height
#define MOS_BG_V_SIZE_MASK      0x00f0      // BG Mosaic Width
#define MOS_OBJ_H_SIZE_MASK     0x000f      // OBJ Mosaic Height
#define MOS_OBJ_V_SIZE_MASK     0x00f0      // OBJ Mosaic Width

#define MOS_H_SIZE_SHIFT        0
#define MOS_V_SIZE_SHIFT        4
#define MOS_BG_H_SIZE_SHIFT     0
#define MOS_BG_V_SIZE_SHIFT     4
#define MOS_OBJ_H_SIZE_SHIFT    8
#define MOS_OBJ_V_SIZE_SHIFT    12

/*------------------------------------------------------------------*/
/*                      Blend Control                               */
/*------------------------------------------------------------------*/

#define BLD_PIXEL_MASK          0x003f      // Select Pixel
#define BLD_1ST_PIXEL_MASK      0x003f      // Select 1st Pixel
#define BLD_MODE_MASK           0x00c0      // Blend Mode
#define BLD_2ND_PIXEL_MASK      0x3f00      // Select 2nd Pixel

#define BLD_1ST_PIXEL_SHIFT     0
#define BLD_MODE_SHIFT          6
#define BLD_2ND_PIXEL_SHIFT     8

#define BLD_BG0                 0x0001      // Select BG0 Pixel
#define BLD_BG1                 0x0002      // Select BG1 Pixel
#define BLD_BG2                 0x0004      // Select BG2 Pixel
#define BLD_BG3                 0x0008      // Select BG3 Pixel
#define BLD_OBJ                 0x0010      // Select OBJ Pixel
#define BLD_BD                  0x0020      // Select Background Color Pixel
#define BLD_ALL                 0x003f      // Select All Pixels
#define BLD_BG0_1ST             0x0001      // Select BG0 1st Pixel
#define BLD_BG1_1ST             0x0002      // Select BG1 1st Pixel
#define BLD_BG2_1ST             0x0004      // Select BG2 1st Pixel
#define BLD_BG3_1ST             0x0008      // Select BG3 1st Pixel
#define BLD_OBJ_1ST             0x0010      // Select OBJ 1st Pixel
#define BLD_BD_1ST              0x0020      // Select Background Color 1st Pixel
#define BLD_1ST_ALL             0x003f      // Select All 1st Pixel
#define BLD_NORMAL_MODE         0x0000      // Normal Mode
#define BLD_A_BLEND_MODE        0x0040      // Semi-transparent Mode
#define BLD_UP_MODE             0x0080      // Brightness UP Mode
#define BLD_DOWN_MODE           0x00c0      // Brightness DOWN Mode
#define BLD_BG0_2ND             0x0100      // Select BG0 2nd Pixel
#define BLD_BG1_2ND             0x0200      // Select BG1 2nd Pixel
#define BLD_BG2_2ND             0x0400      // Select BG2 2nd Pixel
#define BLD_BG3_2ND             0x0800      // Select BG3 2nd Pixel
#define BLD_OBJ_2ND             0x1000      // Select OBJ 2nd Pixel
#define BLD_BD_2ND              0x2000      // Select Background color 2nd Pixel
#define BLD_2ND_ALL             0x3f00      // Select All 2nd Pixel


#define BLD_A_MASK              0x001f      // Blend Variable A
#define BLD_B_MASK              0x1f00      // Blend Variable B
#define BLD_Y_MASK              0x001f      // Blend Variable Y

#define BLD_A_SHIFT             16
#define BLD_B_SHIFT             24
#define BLD_Y_SHIFT             0


/*------------------------------------------------------------------*/
/*                      Sound Control                               */
/*------------------------------------------------------------------*/

#define SOUND_SO1_LEVEL_MASK    0x03        // S01 Output Level
#define SOUND_SO2_LEVEL_MASK    0x30        // S02 Output Level
#define SOUND_DMG_SO1_ON_MASK   0x0f        // DMG Compatible Sound->S01 ON
#define SOUND_DMG_SO2_ON_MASK   0xf0        // DMG Compatible Sound->S02 ON

#define SOUND_SWEEP_SHIFT_MASK  0x03        // Sweep Shift Number
#define SOUND_SWEEP_TIME_MASK   0x30        // Sweep Time
#define SOUND_COUNTS_MASK       0x3f        // Sound Length (Count Number)
#define SOUND_DUTY_MASK         0xc0        // Waveform Duty
#define SOUND_ENV_STEPS_MASK    0x03        // Envelope Step Number
#define SOUND_ENV_INIT_MASK     0xf0        // Envelope Initial-Value
#define SOUND_FREQUENCY_MASK    0x03ff      // Frequency
#define SOUND_FREQUENCY_L_MASK  0xff
#define SOUND_FREQUENCY_H_MASK  0x03

#define SOUND_3_COUNTS_MASK     0xff        // Sound 3 Length (Count Number)
#define SOUND_3_LEVEL_MASK      0xe0        //    Output Level

#define SOUND_4_PRESCALER_MASK  0x03        // Sound 4 Prescaler Select
#define SOUND_4_POLYSHIFT_MASK  0xf0        // Polynomical Counter Shift Number


#define SOUND_SO1_LEVEL_SHIFT   0
#define SOUND_SO2_LEVEL_SHIFT   4
#define SOUND_DMG_SO1_ON_SHIFT  0
#define SOUND_DMG_SO2_ON_SHIFT  4

#define SOUND_SWEEP_SHIFT_SHIFT 0
#define SOUND_SWEEP_TIME_SHIFT  4
#define SOUND_COUNTS_SHIFT      0
#define SOUND_DUTY_SHIFT        6
#define SOUND_ENV_STEPS_SHIFT   0
#define SOUND_ENV_INIT_SHIFT    4
#define SOUND_FREQUENCY_SHIFT   0
#define SOUND_FREQUENCY_L_SHIFT 0
#define SOUND_FREQUENCY_H_SHIFT 8

#define SOUND_3_COUNTS_SHIFT    0
#define SOUND_3_LEVEL_SHIFT     5

#define SOUND_4_PRESCALER_SHIFT 0
#define SOUND_4_POLYSHIFT_SHIFT 4


#define SOUND_1_ON              0x01        // Sound 1 ON
#define SOUND_2_ON              0x02        // Sound 2 ON
#define SOUND_3_ON              0x04        // Sound 3 ON
#define SOUND_4_ON              0x08        // Sound 4 ON
#define SOUND_DMG_ON            0x80        // DMG Compatible Sound ON

#define SOUND_1_SO1_ON          0x01        // Sound 1 -> S01 ON
#define SOUND_2_SO1_ON          0x02        // Sound 2 -> S01 ON
#define SOUND_3_SO1_ON          0x04        // Sound 3 -> S01 ON
#define SOUND_4_SO1_ON          0x08        // Sound 4 -> S01 ON
#define SOUND_1_SO2_ON          0x10        // Sound 1 -> S02 ON
#define SOUND_2_SO2_ON          0x20        // Sound 2 -> S02 ON
#define SOUND_3_SO2_ON          0x40        // Sound 3 -> S02 ON
#define SOUND_4_SO2_ON          0x80        // Sound 4 -> S02 ON
#define SOUND_DMG_ALL_SO1_ON    0x0f        // DMG Compatible Sound -> S01 ON
#define SOUND_DMG_ALL_SO2_ON    0xf0        // DMG Compatible Sound -> S02 ON
#define SOUND_DMG_ALL_SO_ON     0xff        // DMG Compatible Sound -> S01/2 ON
#define SOUND_VIN_SO1_ON        0x80        // Vin -> S01 ON
#define SOUND_VIN_SO2_ON        0x08        // Vin -> S02 ON
#define SOUND_VIN_SO_ON         0x08        // Vin -> S01/2 ON

#define SOUND_A_SO1_ON          0x01        // Direct Sound A -> S01 ON
#define SOUND_A_SO2_ON          0x02        // Direct Sound A -> S02 ON
#define SOUND_B_SO1_ON          0x10        // Direct Sound B -> S01 ON
#define SOUND_B_SO2_ON          0x20        // Direct Sound B -> S02 ON
#define SOUND_A_ALL_SO_ON       0x03        // Direct Sound A -> S01/2 ON
#define SOUND_B_ALL_SO_ON       0x30        // Direct Sound B -> S01/2 ON
#define SOUND_DIRECT_ALL_SO1_ON 0x11        // Direct Sound A/B -> S01 ON
#define SOUND_DIRECT_ALL_SO2_ON 0x22        // Direct Sound A/B -> S02 ON
#define SOUND_DIRECT_ALL_SO_ON  0x33        // Direct Sound A/B -> S01/2 ON
#define SOUND_A_TIMER_0         0x00        // Timer 0 -> Direct Sound A
#define SOUND_A_TIMER_1         0x04        // Timer 1 -> Direct Sound A
#define SOUND_B_TIMER_0         0x00        // Timer 0 -> Direct Sound B
#define SOUND_B_TIMER_1         0x40        // Timer 1 -> Direct Sound B
#define SOUND_A_FIFO_RESET      0x08        // Direct Sound A FIFO Reset
#define SOUND_B_FIFO_RESET      0x80        // Direct Sound B FIFO Reset

#define SOUND_DMG_MIX_1_4       0x00        // DMG Compatible Sound Mix Ratio 1/4
#define SOUND_DMG_MIX_2_4       0x01        //                        2/4
#define SOUND_DMG_MIX_FULL      0x02        //                        Full Range
#define SOUND_A_MIX_1_2         0x00        // Direct Sound A Mix Ratio 1/2
#define SOUND_A_MIX_FULL        0x04        //                        Full Range
#define SOUND_B_MIX_1_2         0x00        // Direct Sound B Mix Ratio 1/2
#define SOUND_B_MIX_FULL        0x08        //                        Full Range
#define SOUND_ALL_MIX_FULL      0x0e        // All Sound Mix Ratio Full Range


#define SOUND_INIT_ON           0x80        // Initial ON
#define SOUND_INIT_OFF          0x00        // Initial ON
#define SOUND_COUNTER_ON        0x40        // Counter ON
#define SOUND_COUNTER_OFF       0x00        // Counter OFF

#define SOUND_SWEEP_UP          0x00        // Sweep Addition
#define SOUND_SWEEP_DOWN        0x08        //          Subtraction
#define SOUND_DUTY_1_8          0x00        // Waveform Duty 1/8
#define SOUND_DUTY_2_8          0x40        //                  2/8
#define SOUND_DUTY_4_8          0x80        //                  4/8
#define SOUND_DUTY_6_8          0xc0        //                  6/8
#define SOUND_ENV_UP            0x08        // Envelope UP
#define SOUND_ENV_DOWN          0x00        //              DOWN

#define SOUND_3_LOCAL_ON        0x80        // Sound 3 ON
#define SOUND_3_WAVE_BANK_0     0x00        // Sound 3 Waveform RAM Bank 0
#define SOUND_3_WAVE_BANK_1     0x40        //                       Bank 1
#define SOUND_3_WAVE_CONNECT    0x20        // Sound 3 Waveform RAM Connect
#define SOUND_3_WAVE_DISCONNECT 0x00        //                       Disconnect
#define SOUND_3_LEVEL_0_4       0x00        // Sound 3 Output None
#define SOUND_3_LEVEL_FULL      0x20        //                 4/4
#define SOUND_3_LEVEL_2_4       0x40        //                 2/4
#define SOUND_3_LEVEL_1_4       0x60        //                 1/4
#define SOUND_3_LEVEL_3_4       0x80        //                 3/4

#define SOUND_4_POLYSTEP_15     0x00        // Sound 4 Polynomical Counter 15 step
#define SOUND_4_POLYSTEP_7      0x08        //                              7 step


/*------------------------------------------------------------------*/
/*                      SIO Control                                 */
/*------------------------------------------------------------------*/

#define SIO_MODE_MASK           0x3000      // Communication mode
#define SIO_BAUD_RATE_MASK      0x0003      // Baud rate
#define SIO_ID_NO_MASK          0x0030      // Communication ID

#define SIO_MODE_SHIFT          12
#define SIO_BAUD_RATE_SHIFT     0
#define SIO_ID_NO_SHIFT         4

#define SIO_8BIT_MODE           0x0000      // Normal 8-bit communication mode
#define SIO_32BIT_MODE          0x1000      // Normal 32-bit communication mode
#define SIO_MULTI_MODE          0x2000      // Multi-play communication mode
#define SIO_UART_MODE           0x3000      // UART communication mode

#define SIO_SCK_OUT             0x0000      // Select external clock
#define SIO_SCK_IN              0x0001      // Select internal clock
#define SIO_IN_SCK_256K         0x0000      // Select internal clock 256KHz
#define SIO_IN_SCK_2M           0x0002      //                  Select 2MHz
#define SIO_ACK_RECV            0x0004      // Request transfer
#define SIO_ACK_SEND            0x0008      // Enable transfer
#define SIO_9600_BPS            0x0000      // Baud rate 9600 bps
#define SIO_38400_BPS           0x0001      //          38400 bps
#define SIO_57600_BPS           0x0002      //          57600 bps
#define SIO_115200_BPS          0x0003      //        115200 bps
#define SIO_MULTI_CONNECT       0x0004      // Connecting multi-play communication
#define SIO_MULTI_DISCONNECT    0x0000      //                  Disconnect
#define SIO_MULTI_PARENT        0x0008      // Multi-play communication  Connect master
#define SIO_MULTI_CHILD         0x0000      //                  Connect slave
#define SIO_MULTI_SI            0x0004      // Multi-play communication SI terminal
#define SIO_MULTI_SD            0x0008      //                  SD terminal
#define SIO_MULTI_BUSY          0x0080      // Multi-play communicating
#define SIO_CTS_ENABLE          0x0004      // Enable UART send enable signal
#define SIO_UART_7BIT           0x0000      // UART communication data length 7 bit
#define SIO_UART_8BIT           0x0080      //                       8 bit
#define SIO_ERROR               0x0040      // Detect error
#define SIO_START               0x0080      // Start transfer
#define SIO_ENABLE              0x0080      // Enable SIO
#define SIO_FIFO_ENABLE         0x0100      // Enable FIFO
#define SIO_PARITY_ENABLE       0x0200      // Enable parity
#define SIO_PARITY_EVEN         0x0000      // Even parity
#define SIO_PARITY_ODD          0x0008      // Odd parity
#define SIO_TRANS_ENABLE        0x0400      // Enable transmitter
#define SIO_TRANS_DATA_FULL     0x0010      // Transmitted data full 
#define SIO_RECV_ENABLE         0x0800      // Enable receiver
#define SIO_RECV_DATA_EMPTY     0x0020      // No data received
#define SIO_IF_ENABLE           0x4000      // Enable interrupt request

/*------------------------------------------------------------------*/
/*                      JOY Bus Control                             */
/*------------------------------------------------------------------*/

#define JOY_IF_RESET            0x01        // Interrupt request of JOY bus reset 
#define JOY_IF_RECV             0x02        // Interrupt request of JOY bus receive completion
#define JOY_IF_SEND             0x04        // Interrupt request of JOY bus send completion
#define JOY_IF_ENABLE           0x40        // Enable interrupt request

/*------------------------------------------------------------------*/
/*                      JOY Bus Status                              */
/*------------------------------------------------------------------*/

#define JSTAT_FLAGS_MASK        0x30        // General flag

#define JSTAT_FLAGS_SHIFT       4

#define JSTAT_RECV              0x02        // Receive status
#define JSTAT_SEND              0x08        // Send status

/*------------------------------------------------------------------*/
/*                      General Input/Output Control                */
/*------------------------------------------------------------------*/

#define R_SIO_MODE_MASTER_MASK  0xc000      // SIO mode master

#define R_SIO_MODE_MASTER_SHIFT 14

#define R_SIO_MASTER_MODE       0x0000      // SIO master mode
#define R_DIRECT_MODE           0x8000      // General input/output 
                                            // communication mode
#define R_JOY_MODE              0xc000      // JOY communication mode

#define R_SC                    0x0001      // Data
#define R_SD                    0x0002
#define R_SI                    0x0004
#define R_SO                    0x0008
#define R_SC_I_O                0x0010      // Select I/O
#define R_SD_I_O                0x0020
#define R_SI_I_O                0x0040
#define R_SO_I_O                0x0080
#define R_SC_IN                 0x0000      // Input setting
#define R_SD_IN                 0x0000
#define R_SI_IN                 0x0000
#define R_SO_IN                 0x0000
#define R_SC_OUT                0x0010      // Output setting
#define R_SD_OUT                0x0020
#define R_SI_OUT                0x0040
#define R_SO_OUT                0x0080
#define R_IF_ENABLE             0x0100      // Enable interrupt request


/*------------------------------------------------------------------*/
/*                      DMG Control                                 */
/*------------------------------------------------------------------*/

#define DMA_TIMMING_MASK        0x30000000  // Start Timing
#define DMA_COUNT_MASK          0x0000ffff  // Transmission Count

#define DMA_TIMMING_SHIFT       30
#define DMA_COUNT_SHIFT         0

#define DMA_ENABLE              0x80000000  // DMA Enable
#define DMA_IF_ENABLE           0x40000000  // Interrupt Request Enable
#define DMA_TIMMING_IMM         0x00000000  // Run Immediately
#define DMA_TIMMING_V_BLANK     0x10000000  // Run V Blank
#define DMA_TIMMING_H_BLANK     0x20000000  // Run H Blank
#define DMA_TIMMING_DISP        0x30000000  // Run Display
#define DMA_TIMMING_SOUND       0x30000000  // Run Sound FIFO Request
#define DMA_DREQ_ON             0x08000000  // Data Request Synchronize Mode ON
#define DMA_16BIT_BUS           0x00000000  // Select Bus Size 16Bit
#define DMA_32BIT_BUS           0x04000000  // Select Bus Size 32Bit
#define DMA_CONTINUOUS_ON       0x02000000  // Continuous Mode ON
#define DMA_SRC_INC             0x00000000  // Select Source Increment
#define DMA_SRC_DEC             0x00800000  // Select Source Decrement
#define DMA_SRC_FIX             0x01000000  // Select Source Fixed
#define DMA_DEST_INC            0x00000000  // Select Destination Increment
#define DMA_DEST_DEC            0x00200000  // Select Destination Decrement
#define DMA_DEST_FIX            0x00400000  // Select Destination Fixed
#define DMA_DEST_RELOAD         0x00600000  // Select Destination 
                                            // Increment/Reload

/*------------------------------------------------------------------*/
/*                      Timer Control                               */
/*------------------------------------------------------------------*/

#define TMR_PRESCALER_MASK      0x00030000  // Prescaler Clock

#define TMR_PRESCALER_SHIFT     16

#define TMR_PRESCALER_1CK       0x00000000  // Prescaler 1 Clock
#define TMR_PRESCALER_64CK      0x00010000  //            64 clocks
#define TMR_PRESCALER_256CK     0x00020000  //          256 clocks
#define TMR_PRESCALER_1024CK    0x00030000  //        1024 clocks
#define TMR_IF_ENABLE           0x00400000  // Interrupt Request Enable
#define TMR_ENABLE              0x00800000  // Run Timer


/*------------------------------------------------------------------*/
/*                          Key                                     */
/*------------------------------------------------------------------*/

#define BUTTON_MASK             0x030f      // Button
#define PLUS_KEY_MASK           0x00f0      // +Control Pad
#define ALL_KEY_MASK            0x03ff      // All Key

#define A_BUTTON                0x0001      // A Button
#define B_BUTTON                0x0002      // B Button
#define SELECT_BUTTON           0x0004      // SELECT
#define START_BUTTON            0x0008      // START
#define R_KEY                   0x0010      // Right Button
#define L_KEY                   0x0020      // Left Button
#define U_KEY                   0x0040      // Up Button
#define D_KEY                   0x0080      // Down Button
#define R_BUTTON                0x0100      // R Button
#define L_BUTTON                0x0200      // L Button
#define KEY_IF_ENABLE           0x4000      // Interrupt Request Enable
#define KEY_OR_INTR             0x0000      // Normal Key Interrupt
#define KEY_AND_INTR            0x8000      // AND Key Interrupt


/*------------------------------------------------------------------*/
/*                  Game Pak Wait Control                           */
/*------------------------------------------------------------------*/

#define CST_SRAM_4WAIT          0x0000      // SRAM 4 wait
#define CST_SRAM_3WAIT          0x0001      //          3 wait
#define CST_SRAM_2WAIT          0x0002      //          2 wait
#define CST_SRAM_8WAIT          0x0003      //          8 wait
#define CST_ROM0_1ST_4WAIT      0x0000      // ROM0 1st 4 wait
#define CST_ROM0_1ST_3WAIT      0x0004      //                 3 wait
#define CST_ROM0_1ST_2WAIT      0x0008      //                 2 wait
#define CST_ROM0_1ST_8WAIT      0x000c      //                 8 wait
#define CST_ROM0_2ND_2WAIT      0x0000      //          2nd 2 wait
#define CST_ROM0_2ND_1WAIT      0x0010      //                 1 wait
#define CST_ROM1_1ST_4WAIT      0x0000      // ROM 1 1st 4 wait
#define CST_ROM1_1ST_3WAIT      0x0020      //                 3 wait
#define CST_ROM1_1ST_2WAIT      0x0040      //                 2 wait
#define CST_ROM1_1ST_8WAIT      0x0060      //                 8 wait
#define CST_ROM1_2ND_4WAIT      0x0000      //          2nd 4 wait
#define CST_ROM1_2ND_1WAIT      0x0080      //                 1 wait
#define CST_ROM2_1ST_4WAIT      0x0000      // ROM 2 1st 4 wait
#define CST_ROM2_1ST_3WAIT      0x0100      //                 3 wait
#define CST_ROM2_1ST_2WAIT      0x0200      //                 2 wait
#define CST_ROM2_1ST_8WAIT      0x0300      //                 8 wait
#define CST_ROM2_2ND_8WAIT      0x0000      //          2nd 8 wait
#define CST_ROM2_2ND_1WAIT      0x0400      //                 1 wait

#define CST_PHI_OUT_NONE        0x0000      // terminal output clock fixed Lo
#define CST_PHI_OUT_4MCK        0x0800      //                4MHz
#define CST_PHI_OUT_8MCK        0x1000      //                8MHz
#define CST_PHI_OUT_16MCK       0x1800      //              16MHz
#define CST_PREFETCH_ENABLE     0x4000      // Enable prefetch buffer
#define CST_AGB                 0x0000      // AGB Game Pak
#define CST_CGB                 0x8000      // CGB Game Pak

#define IN_IWRAM __attribute__ ((section (".iwram")))
extern uint32 __FarFunction (uint32 (*ptr)(), ...);  // Reference to routine in crt0.S
extern void __FarProcedure (void (*ptr)(), ...);  // Reference to routine in crt0.S

#endif
