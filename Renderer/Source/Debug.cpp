#include "Debug.h"
namespace Debug{

    void ValidateDrawBatch(std::vector<Texture*>& textures){
        uint32_t ErrorCount{};
        uint32_t WarningCount{};
        for(uint32_t i =0;i < textures.size();i++){
                Core::Log("ValidationBatchStart");

             if(!textures[i]->GetImage()){
                 Core::Log(ErrorType::Error,"Image is nullptr");
                ErrorCount++;
             }
             if(!textures[i]->GetImageView()){
                   Core::Log(ErrorType::Error,"Image view is nullptr");
                ErrorCount++;
             }
               if(!textures[i]->GetSampler()){
                   Core::Log(ErrorType::Error,"Sampler is nullptr");
                ErrorCount++;
             }
                Core::Log("ValidationBatchEnd with ",ErrorCount," Errors and ",WarningCount," Warnings");
        }


    }



}