#include "Layer.h"
#include "AssetManager.h"
#include "Application.h"
Layer::Layer(std::string Name): m_LayerName(Name)
{
}

void LayerController::RenderLayers(double deltaTime){
	for(uint64_t i=m_Layers.size();i > 0;i--){
		m_Layers[i-1]->OnRender(deltaTime);
	}
}
void LayerController::UpdateLayers(double deltatime)
{
	for (size_t i = m_Layers.size(); i > 0; i--) {
		m_Layers[i-1]->OnUpdate((float)deltatime);
	}
}

void LayerController::UpdateGUILayers()
{
	for (size_t i = m_Layers.size(); i > 0; i--) {
		m_Layers[i-1]->OnGUI();
	}
}
void LayerController::OnEvent(Event& event){

	for(size_t i =m_Layers.size();i > 0;i--){
		if(event.GetEventType() == EventType::EXPENDED)
			return;
		m_Layers[i-1]->OnEvent(event);
	}
}
void LayerController::DestroyLayers()
{
	for (size_t i = m_Layers.size(); i > 0; i--) {
		m_Layers[i-1]->OnDestroy();
		delete m_Layers[i-1];
	}
}
void Layer::SetController(LayerController* controller){
	m_Controller = controller;
}
void LayerController::RemoveLayer(Layer* layer){
	for(int64_t i=0 ;i < m_Layers.size();i++){
		if(m_Layers[i] == layer){
			m_Layers[i]->OnDestroy();
			delete m_Layers[i];
			m_Layers.erase(m_Layers.begin()+i);
		}
	}
	Core::Log("Layer not found{RemoveLayer}");
}
void LayerController::RunQueue(){
	if(m_CommandQueue.empty())
		return;
	//wait for rendering to complete
	if(m_CommandQueue.size() != 0){
		while(Application::GetRender()->GetReadyFrameCount() !=0){
			Core::Log("StillWaiting");
			Application::GetRenderer()->WaitForIdle();
		}
	}
	if(!m_CommandQueue.empty()){
		Application::GetRenderer()->WaitForIdle();
		Application::GetGUIRenderer()->ResetGUIData();
	}



	for(uint32_t i=0;i < m_CommandQueue.size();i++){
		TransitionData& data = m_CommandQueue.front();
		int32_t index{-1};
		if(data.Type == QueueType::TRANSITION){
		for(uint32_t j=0;j < m_Layers.size();j++){

			if(m_Layers[j] == data.Initial)
			{
				index =j;
				break;
			}
		}
		if(index ==-1){
			Core::Log("Failed to transition layer{TransitionLayers()} Layer count ",m_Layers.size()," ",(void*)data.Initial);
			delete data.Transitioned;
		}
		else{

			if(data.Initial&& data.Transitioned)
			{
			data.Transitioned->SetController(data.Initial->m_Controller);

			data.Initial->OnDestroy();
			delete data.Initial;
			m_Layers[index] = std::move(data.Transitioned);
			m_Layers[index]->OnCreate();


			}
		}
		}
		else if(data.Type == QueueType::CREATE){
			if(data.Initial)
			{
			m_Layers.push_back(data.Initial);
			
			data.Initial->SetController(this);
			data.Initial->OnCreate();

		}
		}
		else if(data.Type == QueueType::REMOVE){
			if(data.Initial)
			{
			RemoveLayer(data.Initial);
			}

		}
		m_CommandQueue.pop();
		

	}
	
}
