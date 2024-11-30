#include "ChunkSystem.h"

ChunkSystem::ChunkSystem(uint32_t Width,uint32_t Height)
{
	m_ChunkCount = Width*Height;
	m_Chunks = new Chunk[m_ChunkCount];
	uint32_t ChunkSize = 50 ;
	float NodeSize = 0.02f;

	for (uint32_t y = 0; y < Height; y++) {
		for (uint32_t x = 0; x < Width; x++) {

			m_Chunks[x + (y * Width)].Init(ChunkSize, { ChunkSize * NodeSize * x  ,  ChunkSize * NodeSize * y });
		}
	}
}
