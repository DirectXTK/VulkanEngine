#include "Layer.h"
#include "AssetManager.h"
#include "Application.h"
Layer::Layer(std::string Name): m_LayerName(Name)
{
}


Layer* LayerController::CreateLayer(Layer* layer)
{
	m_Layers.push_back(layer);
	layer->OnCreate();
	layer->SetController(this);
	return layer;
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
			Core::Log("QueueThisDestruction");
		}
	}
	Core::Log("Layer not found{RemoveLayer}");
}
void LayerController::TransitionLayers(){
	for(uint32_t i=0;i < m_QueuedTransitioningLayers.size();i++){
		TransitionData& data = m_QueuedTransitioningLayers.back();
		int32_t index{-1};

		for(uint32_t j=0;j < m_Layers.size();j++){
			if(m_Layers[i] == data.Initial)
			{
				index =i;
				break;
			}
		}
		if(index ==-1)
			Core::Log("Failed to transition layer{TransitionLayers()}");
		else{
			data.Transitioned->SetController(data.Initial->m_Controller);
			data.Initial->OnDestroy();
			m_Layers[index] = std::move(data.Transitioned);
			m_Layers[index]->OnCreate();
		}
		m_QueuedTransitioningLayers.pop();
	}
}
