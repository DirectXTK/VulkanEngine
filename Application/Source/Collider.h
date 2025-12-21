#pragma once
#include "AppCore.h"

#define VULKAN_ENGINE_COLLISION_ON

class Collider;

class ColliderSystem{
	public:
	//returns true if collided.
	static bool IsCollided(const Float2& fPos,const Float2& fSize,const Float2& sPos,const Float2& sSize);
	//return true if collided and updates positions of obj.	
	static bool DefaultCollisionFunction( Float2& fPos, Float2& fSize, Float2& sPos, Float2& sSize);
	//runs on multipleThreads.
	void RunCollisionsAsync(char* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset);
	private:
	static void CollisionAsync(char* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t thisThreadsObjCount,uint16_t threadIndex,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset);

};
namespace Core{
}

