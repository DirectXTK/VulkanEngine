#include "Camera.h"
        void Camera2D::Init(Float2 Position,Float2 Scale){

                m_Position = Position;
                m_Scale =Scale;
                RecalculateMatrix();

        }
        void Camera2D::RecalculateMatrix(){




            m_ViewProj = glm::ortho(-1.0f*m_Scale.x,1.0f*m_Scale.x,-1.0f*m_Scale.y,1.0f*m_Scale.y,0.0f,1.0f)*glm::inverse(glm::translate(glm::mat4(1.0f),glm::vec3(m_Position.x,m_Position.y,0.0f)));
        }
