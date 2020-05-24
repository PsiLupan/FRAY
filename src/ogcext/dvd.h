#ifndef _DVD_H_
#define _DVD_H_

#include <gctypes.h>
#include <ogc/dvd.h>

#define T_FILE 0
#define T_DIR 1

//Taken from Swiss-GC
typedef struct {
	u8  ConsoleID;		//G = Gamecube, R = Wii.
	u8  GamecodeA;		//2 Ascii letters to indicate the GameID.
	u8  GamecodeB;		//2 Ascii letters to indicate the GameID.
	u8	CountryCode;	//J=JAP . P=PAL . E=USA . D=OoT MasterQuest
	u8	MakerCodeA;		//Eg 08 = Sega etc.
	u8  MakerCodeB;
	u8	DiscID;
	u8	Version;
	u8	AudioStreaming; //01 = Enable it. 00 = Don't
	u8 	StreamBufSize;	//For the AudioEnable. (always 00?)
	u8	unused_1[18];	
	u32 DVDMagicWord;	//0xC2339F3D
	char GameName[992];	//String Name of Game, rarely > 32 chars
	u32 DMonitorOffset;	//offset of debug monitor (dh.bin)?
	u32	DMonitorLoadAd;	//addr(?) to load debug monitor?
	u8	unused_2[24];	
	u32 DOLOffset;		//offset of main executable DOL (bootfile)
	u32 FSTOffset;		//offset of the FST ("fst.bin")
	u32	FSTSize;		//size of FST
	u32	MaxFSTSize;		//maximum size of FST (usually same as FSTSize)*
	u32 UserPos;		//user position(?)
	u32 UserLength;		//user length(?)
	u32 unknown_1;		//(?)
	u32 unused_3;
	u32 DMonitorSize;
	u32 SimMemSize;
	u32 ArgOffset;
	u32 DebugFlag;
	u32 TrackLocation;
	u32 TrackSize;
	u32 RegionCode;
	u32 unknown_2;
	// *Multiple DVDs must use it, to properly reside all FSTs.
} DiskHeader __attribute__((aligned(32)));

typedef struct _FSTEntry {
    u8 filetype : 1; //00 - Flags (0: File, 1: Directory)
    u8 offset[3]; //01-03 - Pointer to name in String Table
    u32 addr;
    u32 len;
} FSTEntry;

void DVDInit(void);
BOOL DVD_CheckDisk(void);
BOOL DVDFastOpen(s32, dvdfileinfo*);
BOOL DVDClose(dvdfileinfo*);
s32 DVDConvertFilenameToEntrynum(const char*);
s32 DVDConvertPathToEntrynum(char*);

#endif
