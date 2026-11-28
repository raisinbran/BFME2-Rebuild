// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// W3DRoadBuffer.h: the draw buffer for all the roads in a scene (Zero Hour's W3DRoadBuffer.h, John Ahlquist).
// Target facts (retail game.dat): TRoadPt is 0x20 bytes (loc +0, top +8, bottom +0x10, count +0x18, last +0x1C, multi
// +0x1D, isAngled +0x1E, isJoin +0x1F: updateCounts 0x000D5149); RoadSegment is 0xBC bytes (pt1 +0, pt2 +0x20, curve
// radius +0x40, type +0x44, scale +0x48, width in texture +0x4C, unique id +0x50, visible +0x54, vertex count +0x58, vertex
// pointer +0x5C, index count +0x60, index pointer +0x64, info +0x68 (0x44 bytes), bounds sphere +0xAC; stride 0xBC in
// visibilityChanged 0x000D477C); RoadType is 0x24 bytes (BFME 2 inserts a dword before the unique id: unique id +0x18,
// stacking order +0x20; adjustStacking 0x000D4941). W3DRoadBuffer: road types +0, roads +4, road count +8, initialized +0xC,
// map +0x10, lights iterator +0x14, then the sizes the constructor 0x000DC568 stores (segments +0x34 = 500, vertices +0x38 =
// 1000, indices +0x3C = 2000, types +0x40 = 8) and the update flag +0x4C. freeRoadBuffers clears the +0x18 texture asset.
#pragma once
#ifndef BFME2_W3DROADBUFFER_H
#define BFME2_W3DROADBUFFER_H

#include "Common/GameType.h"
#include "Common/IRegion2D.h"
#include "WW3D2/TextureAsset.h"
#include "WWMath/Vector2.h"
#include "WWMath/SphereClass.h"
#include "WWMath/wwinline.h"

class WorldHeightMap;
class RefRenderObjListIterator;
class CameraClass;
class TextureClass;
class VertexBufferClass;
class DX8IndexBufferClass;
struct VertexFormatXYZDUV1;

enum { bottomLeft = 0, bottomRight = 1, topLeft = 2, topRight = 3, NUM_CORNERS = 4 };
#define MAX_LINKS 6
#define DEFAULT_ROAD_SCALE (8.0f)
#define MIN_ROAD_SEGMENT (0.25f)

struct TRoadPt
{
	Vector2 loc;		// +0x00
	Vector2 top;		// +0x08
	Vector2 bottom;		// +0x10
	int count;			// +0x18
	bool last;			// +0x1C
	bool multi;			// +0x1D
	bool isAngled;		// +0x1E
	bool isJoin;		// +0x1F
	TRoadPt() { loc.X = 0.0f; loc.Y = 0.0f; top.X = 0.0f; top.Y = 0.0f; bottom.X = 0.0f; bottom.Y = 0.0f; count = 0; last = false; multi = false; isAngled = false; isJoin = false; }	// the RoadSegment constructor 0x000D6C35 zeroes both points member by member
};

struct TRoadSegInfo
{
	Vector2 loc;
	Vector2 roadNormal;
	Vector2 roadVector;
	Vector2 corners[NUM_CORNERS];
	Real uOffset;
	Real vOffset;
	Real scale;
};

// The individual data for a road segment.
enum { MAX_SEG_VERTEX = 500, MAX_SEG_INDEX = 2000 };
enum TCorner
{
	SEGMENT,
	CURVE,
	TEE,
	FOUR_WAY,
	THREE_WAY_Y,
	THREE_WAY_H,
	THREE_WAY_H_FLIP,
	ALPHA_JOIN,
	NUM_JOINS
};

class RoadSegment
{
public:
	TRoadPt m_pt1;				// +0x00 drawing location pt1 to pt2.
	TRoadPt m_pt2;				// +0x20 drawing location to.
	Real m_curveRadius;			// +0x40 radius if it is a curve.
	TCorner m_type;				// +0x44 segment, or curve, or intersection.
	Real m_scale;				// +0x48
	Real m_widthInTexture;		// +0x4C width of the road in the texture.
	int m_uniqueID;				// +0x50 road type.
	bool m_visible;				// +0x54
protected:
	int m_numVertex;			// +0x58
	VertexFormatXYZDUV1 *m_vb;	// +0x5C
	int m_numIndex;				// +0x60
	unsigned short *m_ib;		// +0x64
	TRoadSegInfo m_info;		// +0x68
	SphereClass m_bounds;		// +0xAC
public:
	RoadSegment();						// 0x000D6C35
	~RoadSegment();
	void SetVertexBuffer(VertexFormatXYZDUV1 *vb, int numVertex);
	void SetIndexBuffer(unsigned short *ib, int numIndex);
	int GetIndices(unsigned short *destination_ib, int numToCopy, int offset);
	void GetRoadSegInfo(TRoadSegInfo *pInfo);
	void updateSegLighting();
};
// SIZE 0xBC
typedef char RoadSegmentSize[(sizeof(RoadSegment) == 0xBC) ? 1 : -1];

class RoadType
{
public:
	RoadType();

	TextureClass *m_roadTexture;		// +0x00 roads texture
	VertexBufferClass *m_vertexRoad;	// +0x04 road vertex buffer.
	DX8IndexBufferClass *m_indexRoad;	// +0x08 indices defining the triangles for the road drawing.
	int m_numRoadVertices;				// +0x0C number of vertices used in m_vertexRoad.
	int m_numRoadIndices;				// +0x10 number of indices used in m_indexRoad.
	int m_unk0x14;						// +0x14 BFME 2's insertion before the unique id (role not recovered)
	int m_uniqueID;						// +0x18 ID of the road type in INI.
	bool m_isAutoLoaded;				// +0x1C
	int m_stackingOrder;				// +0x20 order in the drawing. 0 drawn first, then 1 and so on.

	int getStacking() { return m_stackingOrder; }
	void setStacking(int order) { m_stackingOrder = order; }
	int getUniqueID() { return m_uniqueID; }

};
// SIZE 0x24
typedef char RoadTypeSize[(sizeof(RoadType) == 0x24) ? 1 : -1];

// W3DRoadBuffer: draw buffer for the roads.
class W3DRoadBuffer
{
	friend class BaseHeightMapRenderObjClass;

public:
	void loadRoads();										// Loads the roads from the map objects list.
	void updateLighting();									// Updates the diffuse lighting in the buffers.

protected:
	RoadType *m_roadTypes;			// +0x00
	RoadSegment *m_roads;			// +0x04 the road buffer. All roads are stored here.
	int m_numRoads;					// +0x08 number of roads used in m_roads.
	bool m_initialized;				// +0x0C true if the subsystem initialized.
	WorldHeightMap *m_map;			// +0x10 pointer to the height map data.
	RefRenderObjListIterator *m_lightsIterator;	// +0x14 lighting iterator.
	TextureAsset m_roadTexture;		// +0x18, freeRoadBuffers clears the asset handle at 0x000D778C.
	unsigned char m_unk0x1C[0x18];	// +0x1C..+0x33, roles not recovered.
	int m_maxRoadSegments;			// +0x34 size of m_roads.
	int m_maxRoadVertex;			// +0x38 size of m_vertexRoad.
	int m_maxRoadIndex;				// +0x3C size of m_indexRoad.
	int m_maxRoadTypes;				// +0x40 size of m_roadTypes.
	int m_curNumRoadVertices;		// +0x44 number of vertices used in the current road type.
	int m_curNumRoadIndices;		// +0x48 number of indices used in the current road type.
	bool m_updateBuffers;			// +0x4C if true, update the vertex buffers.
	bool m_unk0x4D;					// +0x4D set to true by the constructor

	void addMapObjects(void);
	void adjustStacking(int topUniqueID, int bottomUniqueID);
	void insertCrossTypeJoins(void);
	void insertTee(Vector2 loc, int index1, Real scale);
	bool insertY(Vector2 loc, int index1, Real scale);
	int findCrossTypeJoinVector(Vector2 loc, Vector2 *joinVector, int uniqueID);
	void miter(int ndx1, int ndx2);
	void insert4Way(Vector2 loc, int index1, Real scale);
	void insertCurveSegmentAt(int ndx1, int ndx2);
	void moveRoadSegTo(int fromNdx, int toNdx);
	void checkLinkAfter(int ndx);
	void checkLinkBefore(int ndx);
	void flipTheRoad(RoadSegment *pRoad) { TRoadPt tmp = pRoad->m_pt1; pRoad->m_pt1 = pRoad->m_pt2; pRoad->m_pt2 = tmp; }	// flips the loc1 and loc2 info
	void updateCounts(RoadSegment *pRoad);
	void offset3Way(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, Vector2 teeVector, Real widthInTexture);
	void offsetH(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, Vector2 teeVector, Bool flip, Bool mirror, Real widthInTexture);
	void offsetY(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, Vector2 loc, Vector2 upVector, Real widthInTexture);
	void offset4Way(TRoadPt *pc1, TRoadPt *pc2, TRoadPt *pc3, TRoadPt *pr3, TRoadPt *pc4, Vector2 loc, Vector2 alignVector, Real widthInTexture);
	void rotateAbout(Vector2 *ptP, Vector2 center, Real angle);
	void updateCountsAndFlags(void);
	void insertCurveSegments(void);
	void insertTeeIntersections(void);
	void preloadRoadsInVertexAndIndexBuffers(void);			// Fills the index and vertex buffers for drawing.
	void preloadRoadSegment(RoadSegment *pRoad);
	void loadCurve(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, Real scale);
	void loadFloatSection(RoadSegment *pRoad, Vector2 loc, Vector2 roadVector, Real halfHeight, Real left, Real right, Real uOffset, Real vOffset, Real scale);
	void loadFloat4PtSection(RoadSegment *pRoad, Vector2 loc, Vector2 roadNormal, Vector2 roadVector, Vector2 *cornersP, Real uOffset, Real vOffset, Real uScale, Real vScale);
	void loadTee(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, bool is4way, Real scale);
	void loadY(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, Real scale);
	void loadAlphaJoin(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, Real scale);
	void loadH(RoadSegment *pRoad, Vector2 loc1, Vector2 loc2, bool flip, Real scale);
	bool visibilityChanged(const IRegion2D &bounds);		// Returns true if some roads are now visible that weren't, or vice versa.

public:
	void clearAllRoads(void);								// Empties the road buffer.
};

#endif
