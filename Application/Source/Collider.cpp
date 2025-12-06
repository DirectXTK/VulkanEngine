#include "Collider.h"
#include "Application.h"
#define TILESIZE 0.04f
	bool ColliderSystem::IsCollided(const Float2& fPos,const Float2& fSize,const Float2& sPos,const Float2& sSize){
		if(fPos.x + (fSize.x*2) <sPos.x ||
			fPos.x > sPos.x + (sSize.x*2))
			return false;
		if(fPos.y + (fSize.y*2) <sPos.y ||
			fPos.y > sPos.y + (sSize.y*2))
			return false;
		return true;
	}

	bool ColliderSystem::DefaultCollisionFunction( Float2& fPos, Float2& fSize, Float2& sPos, Float2& sSize){
		if(!IsCollided(fPos,fSize,sPos,sSize))
			return false;
		
		Float2 pos1 = fPos;
		Float2 pos2 = sPos;
		
		Float2 Dis = {pos1.x-pos2.x,pos1.y-pos2.y};

		if(Dis.x ==0.0f || Dis.y ==0.0f){
			pos1.x += 0.001f*Core::RandomInt32(-1,1);
			pos1.y += 0.001f*Core::RandomInt32(-1,1);

		}

		float Dir = (float)Core::RandomInt32(-1,1);
		float Dir2 = (float)Core::RandomInt32(-1,1);

		pos1.x +=(Dis.x*0.366f);
		pos1.y +=(Dis.y*0.366f);

		pos2.x -=(Dis.x*0.366f);
		pos2.y -=(Dis.y*0.366f);
		
		fPos = pos1;
		sPos = pos2;

		return true;
	}
	void ColliderSystem::RunCollisionsAsync(char* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset){
		//uses threads per 200 obj or the max.
		uint32_t threadCount = std::min((objCount/200)+1,(uint64_t)Core::GetCPUThreadCount());
		uint64_t perThreadobjCount =  uint64_t(objCount/(uint64_t)threadCount);
		uint64_t offset{};
		std::vector<std::thread*> threads{};
		threads.resize(threadCount);
		if(isCollidedOffset != -1)
			for(uint64_t i=0;i < objCount;i++){
				bool* isCollided = (bool*)&objData[(i*stride)+isCollidedOffset];
				*isCollided = false;
			}

		for(uint32_t i =0 ;i < threadCount;i++){

			if(i+1 ==threadCount){
				perThreadobjCount = objCount-(perThreadobjCount*i);
				threads[i] = new std::thread(ColliderSystem::CollisionAsync,objData+offset,posOffset,sizeOffset,perThreadobjCount,i,perThreadobjCount,stride,isCollidedOffset);
			}else
				threads[i] = new std::thread(ColliderSystem::CollisionAsync,objData+offset,posOffset,sizeOffset,perThreadobjCount,i,objCount-(i*perThreadobjCount),stride,isCollidedOffset);
			offset +=perThreadobjCount*stride;

		}
		for(uint32_t i=0 ;i < threadCount;i++){
			threads[i]->join();
			delete threads[i];
		}

	}
	void ColliderSystem::CollisionAsync(char* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t thisThreadsObjCount,uint16_t threadIndex,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset){
		Float2* fPos{};
		Float2* fSize{};
		bool* fIsCollided{};

		Float2* sPos{};
		Float2* sSize{};
		bool* sIsCollided{};
		for(uint64_t i=0 ;i < thisThreadsObjCount-1;i++){
			fPos = (Float2*)&objData[(i*stride)+posOffset];
			fSize = (Float2*)&objData[(i*stride)+sizeOffset];
			for(uint64_t j=i+1;j < objCount;j++){

			sPos = (Float2*)&objData[(j*stride)+posOffset];
			sSize = (Float2*)&objData[(j*stride)+sizeOffset];

			bool ret = ColliderSystem::DefaultCollisionFunction(*fPos,*fSize,*sPos,*sSize);

			if(ret &&isCollidedOffset != -1){
				fIsCollided = (bool*)&objData[(i*stride)+isCollidedOffset];
				sIsCollided = (bool*)&objData[(j*stride)+isCollidedOffset];

				*sIsCollided =true;
				*fIsCollided = true; 
			}

			}
		}

	}

namespace Core{
	
}




