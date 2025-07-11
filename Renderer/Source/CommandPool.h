#pragma once
#include "RendCore.h"
class CommandPool{
    public:
        void Init(VkDevice device,uint32_t QueueIndex);
        VkCommandPool GetCommandPool(){return m_CommandPool;}

        ~CommandPool();
    private:
        VkDevice m_Device{};
        VkCommandPool m_CommandPool{};
};