#include "GUITesting.h"
#include "Serializer.h"

GUITestingLayer::GUITestingLayer() : Layer("GUITestingLayer")
{

}
void GUITestingLayer::OnCreate() {

   

}
void GUITestingLayer::OnUpdate(double deltatime)
{
   

}

void GUITestingLayer::OnGUI()
{
	if (m_App->m_GUIRenderer->Button({ 0.0f,0.0f }, { 1.0f,1.0f,0.0f,1.0f }, { 0.1f,0.1f },MouseCodes::LEFT,false)) {
		std::cout << "Pressed\n";
	}
}

void GUITestingLayer::OnDestroy()
{
}



