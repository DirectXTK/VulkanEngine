#include "Camera.h"
        void Camera2D::Init(Float2 Position,Float2 Scale){

                m_Position = Position;
                m_Scale =Scale;
                RecalculateMatrix();

        }
        void Camera2D::RecalculateMatrix(){



            glm::vec3 foward{0.0f, 0.0f, 1.0f};
            glm::vec3 Centre(0.0f, 0.0f,0.0f);
            glm::mat4 Scale = glm::scale(glm::mat4(1.0),glm::vec3(m_Scale.x,m_Scale.y,0.0f));

            glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
            glm::mat4 view = glm::lookAtLH(glm::vec3(m_Position.x, m_Position.y, 0.0f), glm::vec3(m_Position.x, m_Position.y, 0.0f) + foward, glm::vec3(0.0f, 1.0f, 0.0f));
            //proj = glm::scale(proj, glm::vec3(m_Scale.x, m_Scale.y, 1.0f));
           view = glm::translate(glm::mat4(1.0f), -Centre) * Scale * glm::translate(glm::mat4(1.0f), Centre)* view;
            m_ViewProj = view * proj;
        }
