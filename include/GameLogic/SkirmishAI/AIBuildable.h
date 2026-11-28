// AIBuildable.h: something the skirmish AI can ask to have built (Team derives
// from it at +4). SIZE 0x2C (Team's members resume at +0x30).
// Target facts (code RVAs, table VAs):
//  - ctor 0x0055B048 stores table 0x00C6B900, zeroes +4 (Real) and +8/+0xC/+0x10,
//    builds two four-byte STLport containers at +0x14 and +0x1C (ctor
//    0x004EC36C), zeroes +0x18 (Real), +0x21 and +0x24, and sets +0x20 and +0x28
//    to 1. Virtual dtor 0x0055B0CC (deleting 0x0055B166).
//  - table 0x00C6B900, 17 slots; Team's table 0x00C1AEB8 at Team +4 overrides
//    slots 0 (thunk 0x003A370F), 2 (0x0039D840), 13 (Team::getLocation
//    0x0039D7DB) and 14..16 (folded 0x0047A699 `return false`, 0x005CB9FA
//    `return true`, 0x005DAA6C `return 0`, each taking one argument but 14).
//  - slot 2: the base 0x0055B0B9 returns the larger of +0x18 and +4; Team's
//    override returns its prototype's production priority as a Real, so the
//    slot is a priority getter (provisional name).
//  - slot 3 addNeed (map name) inserts its argument into the +0x14 container;
//    slot 6 registerWithBuilder (map name) looks the owner up through the
//    skirmish AI manager (VA 0x00DFEEF8); slot 7 0x005F69CE stores +0x10;
//    slot 8 0x003A36D3 returns slot 9 / slot 10 for its argument; slots 9 and
//    10 return constants (VA 0x00BBAEAC, 0x00BBB8D8); slot 12 0x0055AED6 takes
//    (Xfer *, one more pointer); slots 13..16 are pure.
// Names without a map or string source are role labels (vslotN as in
// AIUpdate.h) until a caller names them.
#pragma once
#ifndef BFME2_AIBUILDABLE_H
#define BFME2_AIBUILDABLE_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/Coord3D.h"

class Xfer;

class AIBuildable
{
public:
	AIBuildable();							// 0x0055B048
	virtual ~AIBuildable();						// slot 0, 0x0055B0CC
	virtual void vslot1(Int);					// slot 1, 0x0055AFA3
	virtual Real getPriority() const;				// slot 2, 0x0055B0B9 (provisional name)
	virtual void addNeed(Int need);					// slot 3, 0x0055B146
	virtual Bool vslot4() const;					// slot 4, folded `return false` 0x0047A699
	virtual void vslot5();						// slot 5, 0x0055B228
	virtual void registerWithBuilder(void *builder, Bool);		// slot 6, 0x0055AD91
	virtual void vslot7(Int value);					// slot 7, 0x005F69CE (stores +0x10)
	virtual Real vslot8(Int);					// slot 8, 0x003A36D3
	virtual Real vslot9(Int);					// slot 9, 0x003A36F7
	virtual Real vslot10(Int);					// slot 10, 0x003A3700
	virtual Bool vslot11() const;					// slot 11, folded `return false` 0x0047A699
	virtual void vslot12(Xfer *xfer, void *);			// slot 12, 0x0055AED6
	void failBuild(Int);
	virtual Coord3D getLocation() const = 0;			// slot 13 (Team's override has the map name)
	virtual Bool vslot14() const = 0;				// slot 14
	virtual Bool vslot15(Int) = 0;					// slot 15
	virtual Int vslot16(Int) = 0;					// slot 16

protected:
	Real m_unk0x04;					// +0x04 slot 5 copies it to +0x18
	Int m_unk0x08;					// +0x08
	Int m_unk0x0C;					// +0x0C
	Int m_unk0x10;					// +0x10 slot 7 sets it
	UnsignedByte m_unk0x14[4];			// +0x14 STLport container (ctor 0x004EC36C); addNeed inserts here
	Real m_unk0x18;					// +0x18
	UnsignedByte m_unk0x1C[4];			// +0x1C STLport container (ctor 0x004EC36C)
	Bool m_unk0x20;					// +0x20 1 at construction
	Bool m_unk0x21;					// +0x21
	Int m_unk0x24;					// +0x24
	Bool m_unk0x28;					// +0x28 1 at construction
};
// SIZE 0x2C
typedef char AIBuildableSize[(sizeof(AIBuildable) == 0x2C) ? 1 : -1];

#endif
