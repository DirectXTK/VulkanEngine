#pragma once
#include "RendCore.h"
class CommandPool{
    public:
        void Init(VkDevice device,uint32_t QueueIndex);
        VkCommandPool GetCommandPool(){return m_CommandPool;}
    private:
        VkCommandPool m_CommandPool{};
};