#pragma once
#include "RendCore.h"

class Camera2D{
    public:
        void Init(Float2 Position,Float2 Scale);

        glm::mat4 GetViewProj(){return m_ViewProj;}

        Float2 GetPosition(){return m_Position;}
        Float2 GetScale(){return m_Scale;}
       
         void SetScale(Float2 Scale){m_Scale = Scale;RecalculateMatrix();}
          void SetPosition(Float2 Position){ m_Position= Position;RecalculateMatrix();}
    private:
        void RecalculateMatrix();
        glm::mat4 m_View{};
        glm::mat4 m_Proj{};
        glm::mat4 m_ViewProj{};

        Float2 m_Position{};
        Float2 m_Scale{};

};