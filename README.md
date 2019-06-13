# What is FRAY? [![Build Status](https://www.travis-ci.org/PsiLupan/FRAY.svg?branch=master)](https://www.travis-ci.org/PsiLupan/FRAY)
FRAY is a intended to be a recompilable version of the Super Smash Bros. Melee NTSC 1.02 DOL (or DOLphin executable). The term "fray" itself is a synonym to "melee," as I prefer not to infringe on "brawl" or the original game's name.

Unlike other projects, FRAY does not currently intend to integrate a mix of ASM and C, unless it becomes necessary for certain functions like Dolphin SDK or HAL library components that are largely optimized for ASM.

## Is there a Patcher to drop in these functions to the existing game?

Not at present, though if someone has or is willing to create one, I'm open to it. 

I've looked at Hanafuda (https://github.com/AxioDL/hanafuda) and besides the LLVM toolchain crashing outright with an undecipherable exception, it'd take too much work for me to get it working for me.

## What is the current progress?
The codebase fluctuates quite often between compilable and non-compilable due to my regular contribution of bad/broken code. A rough estimate of progress around the codebase itself is that HAL's sysdolphin library is nearly recreated and the boot code, with a skip directly to the Title Screen scene, is close to rendering the Title.

## How much different from the original game will this be?
FRAY does not presently attempt to reimplement playing the movie files, creating/manipulating memory card data, the Tournament system, or implementing the game's debug menu. 

The current decision to not implement those features was largely based on scope and the fact they're not reused portions of code throughout the game. Likewise, modders regularly remove these sections when attempting to free DOL space for mods. If they come back, I'd prefer to implement many "area-specific" features as REL files, which are essentially the GameCube/Wii equivalent of DLL files that can be loaded and unloaded at-will.

## How can I contribute?
### For reverse engineers without C experience
Updating any document available at https://smashboards.com/threads/melee-hacks-and-you-new-hackers-start-here-in-the-op.247119/ is the biggest help for anyone not able to contribute code directly.

### For C programmers without reversing experience
You're welcome to submit Pull Requests to correct any issues you see. There's currently not contributing guidelines for code formatting, so as long as it's readable I'll typically accept it or provide feedback otherwise. At present, many of the issues in the codebase are known (bad pointer de-refs, lack of header impls, etc..), so I prefer contribution over criticism.

### For anyone looking to get reverse engineering experience
I highly recommend using the NSA's Ghidra tool, as it can provide pseudo-code for PPC assembly and offers many of the features that IDA does without the cost. Likewise, IDA has several failings with the Gamecube's PowerPC instruction set regarding handling of certain instructions, which can lead to really, really bad decompilations.
