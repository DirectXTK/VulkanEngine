#pragma once
#include "AppCore.h"
struct ChunkNode {

};
struct Chunk {
	void Init(uint32_t ChunkSize, Float2 Offset) {
		Size = ChunkSize;
		ChunkOffset = Offset;
	}
	Float2 ChunkOffset{};

	//true means blocked
	bool* PathingData{};
	uint32_t Size{};

};
class ChunkSystem
{
public:
	ChunkSystem(uint32_t WidthInChunks,uint32_t HeightInChunks);

	Chunk* GetChunk(uint32_t Index) { return &m_Chunks[Index]; }
private:

#define CHUNKSIZE 50*50

	ChunkNode m_Nodes[CHUNKSIZE];

	Chunk* m_Chunks{};
	uint32_t m_ChunkCount{};
};

