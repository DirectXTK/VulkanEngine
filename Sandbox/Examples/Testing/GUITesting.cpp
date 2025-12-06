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
void GUITestingLayer::OnRender(double deltime){

}
void GUITestingLayer::OnEvent(Event& event){
	if(event.GetEventType() == EventType::MOUSE){
		OnMouseEvent((MouseEvent&)event);
	}
	if(event.GetEventType() == EventType::KEYBOARD){
		OnKeyBoardEvent((KeyBoardEvent&)event);
	}
}
void GUITestingLayer::OnKeyBoardEvent(KeyBoardEvent& event){

}
void GUITestingLayer::OnMouseEvent(MouseEvent& event){


}
void GUITestingLayer::OnGUI()
{
	GUIRenderer* guiRenderer = Application::GetGUIRenderer();
	


	guiRenderer->Panel("Panel", {0.0f,-0.9f}, {1.0f,1.0f,1.0f,1.0f}, {1.0f,0.1f}, 0, true);

	//guiRenderer->Button("Start Button","", { 0.0f,0.5f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.5f,0.5f },MouseCodes::LEFT,0,false);
	guiRenderer->CheckBox("test",{-0.8f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	guiRenderer->CheckBox("test1",{-0.6f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});
	guiRenderer->CheckBox("test2",{-0.4f,0.0f},{0.10f,0.50f},{1.0f,1.0f,1.0f,1.0f});

	guiRenderer->EndPanel();
}

void GUITestingLayer::OnDestroy()
{
}



