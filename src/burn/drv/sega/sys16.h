#include "tiles_generic.h"
#include "m68000_intf.h"
#include "z80_intf.h"
#include "burn_ym2151.h"
#include "burn_ym2203.h"
#include "burn_ym2413.h"
#include "burn_ym2612.h"
#include "rf5c68.h"
#include "burn_gun.h"
#include "bitswap.h"
#include "genesis_vid.h"
#include "8255ppi.h"
#include "burn_shift.h"
#include "mcs51.h"
#include "resnet.h"
#include "msm6295.h"

#define SYS16_ROM_PROG			1
#define SYS16_ROM_TILES			2
#define SYS16_ROM_SPRITES		3
#define SYS16_ROM_Z80PROG		4
#define SYS16_ROM_KEY			5
#define SYS16_ROM_7751PROG		6
#define SYS16_ROM_7751DATA		7
#define SYS16_ROM_UPD7759DATA	8
#define SYS16_ROM_PROG2			9
#define SYS16_ROM_ROAD			10
#define SYS16_ROM_PCMDATA		11
#define SYS16_ROM_Z80PROG2		12
#define SYS16_ROM_Z80PROG3		13
#define SYS16_ROM_Z80PROG4		14
#define SYS16_ROM_PCM2DATA		15
#define SYS16_ROM_PROM 			16
#define SYS16_ROM_PROG3			17
#define SYS16_ROM_SPRITES2		18
#define SYS16_ROM_RF5C68DATA	19
#define SYS16_ROM_I8751			20
#define SYS16_ROM_MSM6295		21
#define SYS16_ROM_TILES_20000	22

// sys16_run.cpp
extern UINT8 System16InputPort0[8];
extern UINT8 System16InputPort1[8];
extern UINT8 System16InputPort2[8];
extern UINT8 System16InputPort3[8];
extern UINT8 System16InputPort4[8];
extern UINT8 System16InputPort5[8];
extern UINT8 System16InputPort6[8];
extern UINT8 System16Gear;
extern INT16 System16AnalogPort0;
extern INT16 System16AnalogPort1;
extern INT16 System16AnalogPort2;
extern INT16 System16AnalogPort3;
extern INT16 System16AnalogPort4;
extern INT16 System16AnalogPort5;
extern INT16 System16AnalogSelect;
extern UINT8 System16Dip[3];
extern UINT8 System16Input[7];
extern UINT8 System16Reset;
extern UINT8 System16MCUData;
extern UINT8 *System16Rom;
extern UINT8 *System16Code;
extern UINT8 *System16Rom2;
extern UINT8 *System16Z80Rom;
extern UINT8 *System16Z80Code;
extern UINT8 *System16UPD7759Data;
extern UINT8 *System16PCMData;
extern UINT8 *System16RF5C68Data;
extern UINT8 *System16I8751Rom;
extern UINT8 *System16Prom;
extern UINT8 *System16Key;
extern UINT8 *System16Ram;
extern UINT8 *System16ExtraRam;
extern UINT8 *System16Z80Ram;
extern UINT8 *System16TempGfx;
extern UINT8 *System16PriorityMap;
extern UINT8 *System16TileRam;
extern UINT8 *System16TextRam;
extern UINT8 *System16TextRam;
extern UINT8 *System16SpriteRam;
extern UINT8 *System16SpriteRamBuff;
extern UINT8 *System16SpriteRam2;
extern UINT8 *System16RotateRam;
extern UINT8 *System16RotateRamBuff;
extern UINT8 *System16PaletteRam;
extern UINT8 *System16RoadRam;
extern UINT8 *System16RoadRamBuff;
extern UINT32 *System16Palette;
extern UINT8 *System16Tiles;
extern UINT8 *System16Sprites;
extern UINT8 *System16Sprites2;
extern UINT8 *System16Roads;
extern UINT32 System16NumTiles;
extern UINT32 System16RomSize;
extern UINT32 System16Rom2Size;
extern UINT32 System16TileRomSize;
extern UINT32 System16SpriteRomSize;
extern UINT32 System16Sprite2RomSize;
extern UINT32 System16RoadRomSize;
extern UINT32 System16Z80RomSize;
extern UINT32 System16Z80Rom2Num;
extern UINT32 System16PCMDataSize;
extern UINT32 System16PCMDataSizePreAllocate;
extern UINT32 System16ExtraRamSize;
extern UINT32 System16SpriteRamSize;
extern UINT32 System16SpriteRam2Size;
extern UINT32 System16RotateRamSize;
extern UINT32 System16UPD7759DataSize;
extern UINT32 System16I8751RomNum;
extern UINT32 System16MSM6295RomNum;

extern INT32 System16SoundMute; // hangon hw
extern UINT8 System16VideoControl;
extern INT32 System16SoundLatch;
extern bool System16BTileAlt;
extern bool Shangon;
extern bool Hangon;
extern bool AlienSyndrome;
extern bool HammerAway;
extern bool Lockonph;
extern bool AltbeastMode;
extern bool ThndrbldMode;
extern bool TturfMode;
extern bool System16Z80Enable;
extern bool System1668KEnable;

extern INT32 System16YM2413IRQInterval;

extern bool System16HasGears;

extern INT32 s16a_update_after_vblank;

extern INT32 nSystem16CyclesDone[4]; 
extern UINT32 System16ClockSpeed;
extern UINT32 System16Z80ClockSpeed;

extern UINT8* System16I8751InitialConfig;

typedef void (*Sim8751)();
extern Sim8751 Simulate8751;

typedef void (*System16Map68K)();
extern System16Map68K System16Map68KDo;

typedef void (*System16MapZ80)();
extern System16MapZ80 System16MapZ80Do;

typedef INT32 (*System16CustomLoadRom)();
extern System16CustomLoadRom System16CustomLoadRomDo;

typedef INT32 (*System16CustomDecryptOpCode)();
extern System16CustomDecryptOpCode System16CustomDecryptOpCodeDo;

typedef UINT8 (*System16ProcessAnalogControls)(UINT16 value);
extern System16ProcessAnalogControls System16ProcessAnalogControlsDo;

typedef void (*System16MakeAnalogInputs)();
extern System16MakeAnalogInputs System16MakeAnalogInputsDo;

UINT8 __fastcall System16PPIZ80PortRead(UINT16 a);
void __fastcall System16Z80PortWrite(UINT16 a, UINT8 d);

INT32 System16LoadRoms(bool bLoad);

INT32 CustomLoadRom20000();
INT32 CustomLoadRom40000();

UINT16 System16MultiplyChipRead(INT32 which, INT32 offset);
void System16MultiplyChipWrite(INT32 which, INT32 offset, UINT16 data);
UINT16 System16CompareTimerChipRead(INT32 which, INT32 offset);
void System16CompareTimerChipWrite(INT32 which, INT32 offset, UINT16 data);
UINT16 System16DivideChipRead(INT32 which, INT32 offset);
void System16DivideChipWrite(INT32 which, INT32 offset, UINT16 data);

INT32 System16Init();
INT32 System16Exit();
INT32 System16AFrame();
INT32 System16BFrame();
INT32 System18Frame();
INT32 HangonFrame();
INT32 HangonYM2203Frame();
INT32 OutrunFrame();
INT32 XBoardFrame();
INT32 XBoardFrameGPRider();
INT32 YBoardFrame();
INT32 System16Scan(INT32 nAction, INT32 *pnMin);
void sys16_sync_mcu();

// d_sys16a.cpp
void System16APPI0WritePortA(UINT8 data);
void System16APPI0WritePortB(UINT8 data);
void System16APPI0WritePortC(UINT8 data);
UINT16 __fastcall System16AReadWord(UINT32 a);
UINT8 __fastcall System16AReadByte(UINT32 a);
void __fastcall System16AWriteWord(UINT32 a, UINT16 d);
void __fastcall System16AWriteByte(UINT32 a, UINT8 d);
UINT8 System16A_I8751ReadPort(INT32 port);
void System16A_I8751WritePort(INT32 port, UINT8 data);

// d_sys16b.cpp

// d_sys18.cpp
UINT8 system18_io_chip_r(UINT32 offset);
void system18_io_chip_w(UINT32 offset, UINT16 d);
void System18GfxBankWrite(UINT32 offset, UINT16 d);
void HamawayGfxBankWrite(UINT32 offset, UINT16 d);

// d_hangon.cpp
void HangonPPI0WritePortA(UINT8 data);
void HangonPPI0WritePortB(UINT8 data);
void HangonPPI0WritePortC(UINT8 data);
UINT8 HangonPPI1ReadPortC();
void HangonPPI1WritePortA(UINT8 data);
UINT16 __fastcall HangonReadWord(UINT32 a);
UINT8 __fastcall HangonReadByte(UINT32 a);
void __fastcall HangonWriteWord(UINT32 a, UINT16 d);
void __fastcall HangonWriteByte(UINT32 a, UINT8 d);
UINT8 Hangon_I8751ReadPort(INT32 port);
void Hangon_I8751WritePort(INT32 port, UINT8 data);

// d_outrun.cpp
void OutrunPPI0WritePortC(UINT8 data);
UINT16 System16RoadControlRead(UINT32 a);
void System16RoadControlWrite(UINT32 offset, UINT16 d);
UINT16 __fastcall Outrun2ReadWord(UINT32 a);
UINT8 __fastcall Outrun2ReadByte(UINT32 a);
void __fastcall Outrun2WriteWord(UINT32 a, UINT16 d);
void __fastcall Outrun2WriteByte(UINT32 a, UINT8 d);

// d_xbrd.cpp
UINT16 __fastcall XBoardReadWord(UINT32 a);
UINT8 __fastcall XBoardReadByte(UINT32 a);
void __fastcall XBoardWriteWord(UINT32 a, UINT16 d);
void __fastcall XBoardWriteByte(UINT32 a, UINT8 d);
UINT16 __fastcall XBoard2ReadWord(UINT32 a);
UINT8 __fastcall XBoard2ReadByte(UINT32 a);
void __fastcall XBoard2WriteWord(UINT32 a, UINT16 d);
void __fastcall XBoard2WriteByte(UINT32 a, UINT8 d);

// d_ybrd.cpp
UINT16 __fastcall YBoardReadWord(UINT32 a);
UINT8 __fastcall YBoardReadByte(UINT32 a);
void __fastcall YBoardWriteWord(UINT32 a, UINT16 d);
void __fastcall YBoardWriteByte(UINT32 a, UINT8 d);
UINT16 __fastcall YBoard2ReadWord(UINT32 a);
void __fastcall YBoard2WriteWord(UINT32 a, UINT16 d);
UINT16 __fastcall YBoard3ReadWord(UINT32 a);
UINT8 __fastcall YBoard3ReadByte(UINT32 a);
void __fastcall YBoard3WriteWord(UINT32 a, UINT16 d);
extern INT32 Pdrift_analog_adder;
extern INT32 Pdrift_analog_target;

// sys16_gfx.cpp
extern INT32 System16VideoEnable;
extern INT32 System16AVideoEnableDelayed;
extern INT32 System18VdpEnable;
extern INT32 System18VdpMixing;
extern INT32 System16ScreenFlip;
extern INT32 System16ScreenFlipXoffs;
extern INT32 System16ScreenFlipYoffs;
extern INT32 System16SpriteShadow;
extern INT32 System16SpriteXOffset;
extern INT32 System16SpriteBanks[16];
extern INT32 System16TileBanks[8];
extern INT32 System16OldTileBanks[8];
extern INT32 System16Page[4];
extern INT32 System16OldPage[4];
extern UINT8 BootlegFgPage[4];
extern UINT8 BootlegBgPage[4];
extern INT32 System16ScrollX[4];
extern INT32 System16ScrollY[4];
extern INT32 System16ColScroll;
extern INT32 System16RowScroll;
extern INT32 System16RoadControl;
extern INT32 System16RoadColorOffset1;
extern INT32 System16RoadColorOffset2;
extern INT32 System16RoadColorOffset3;
extern INT32 System16RoadXOffset;
extern INT32 System16RoadPriority;
extern INT32 System16PaletteEntries;
extern INT32 System16SpritePalOffset;
extern INT32 System16TilemapColorOffset;
extern INT32 System16TileBankSize;
extern INT32 System16RecalcBgTileMap;
extern INT32 System16RecalcBgAltTileMap;
extern INT32 System16RecalcFgTileMap;
extern INT32 System16RecalcFgAltTileMap;
extern INT32 System16CreateOpaqueTileMaps;
extern INT32 System16IgnoreVideoEnable;

extern bool bSystem16BootlegRender;

extern UINT16 *pTempDraw;

void System16PaletteInit();
void System16PaletteExit();

void System16GfxScan(INT32 nAction);
void System16Decode8x8Tiles(UINT8 *pTile, INT32 Num, INT32 offs1, INT32 offs2, INT32 offs3);
void OutrunDecodeRoad();
void HangonDecodeRoad();
void System16ATileMapsInit(INT32 bOpaque);
void System16BTileMapsInit(INT32 bOpaque);
void System16TileMapsExit();
INT32 System16ARender();
INT32 System16BRender();
INT32 System16BootlegRender();
INT32 System16BAltRender();
void System16ATileByteWrite(UINT32 Offset, UINT8 d);
void System16ATileWordWrite(UINT32 Offset, UINT16 d);
void System16BTileByteWrite(UINT32 Offset, UINT8 d);
void System16BTileWordWrite(UINT32 Offset, UINT16 d);
INT32 System18Render();
INT32 HangonRender();
INT32 HangonAltRender();
INT32 OutrunRender();
INT32 ShangonRender();
INT32 XBoardRender();
INT32 YBoardRender();

// fd1089.cpp
void FD1089Decrypt();

// sys16_fd1094.cpp
extern UINT16* fd1094_userregion;

void fd1094_driver_init(INT32 nCPU);
void fd1094_machine_init();
void fd1094_exit();
void fd1094_scan(INT32 nAction);

// genesis_vid.cpp

// sega_315_5195.cpp
extern bool LaserGhost;

typedef UINT8 (*sega_315_5195_custom_io)(UINT32);
extern sega_315_5195_custom_io sega_315_5195_custom_io_do;

typedef void (*sega_315_5195_custom_io_write)(UINT32, UINT8);
extern sega_315_5195_custom_io_write sega_315_5195_custom_io_write_do;

UINT8 sega_315_5195_io_read(UINT32 offset);
void sega_315_5195_io_write(UINT32 offset, UINT8 d);
UINT8 __fastcall sega_315_5195_read_byte(UINT32 a);
UINT16 __fastcall sega_315_5195_read_word(UINT32 a);
void __fastcall sega_315_5195_write_byte(UINT32 a, UINT8 d);
void __fastcall sega_315_5195_write_word(UINT32 a, UINT16 d);
UINT8 sega_315_5195_i8751_read_port(INT32 port);
void sega_315_5195_i8751_write_port(INT32 port, UINT8 data);

void sega_315_5195_reset();
void sega_315_5195_configure_explicit(UINT8 *map_data);
void sega_315_5195_init();
void sega_315_5195_exit();
INT32 sega_315_5195_scan(INT32 nAction);
