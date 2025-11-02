#include "Layer.h"
#include "AssetManager.h"
#include "Renderer.h"
void Layer::Init()
{
	
}

Layer::Layer(std::string Name): m_LayerName(Name)
{
}


Layer* LayerController::CreateLayer(Layer* layer)
{
	m_Layers.push_back(layer);
	layer->OnCreate();
	return layer;
}
void LayerController::RenderLayers(double deltaTime){
	for(uint64_t i=0;i < m_Layers.size();i++){
		m_Layers[i]->OnRender(deltaTime);
	}
}
void LayerController::UpdateLayers(double deltatime)
{
	for (size_t i = 0; i < m_Layers.size(); i++) {
		m_Layers[i]->OnUpdate((float)deltatime);
	}
}

void LayerController::UpdateGUILayers()
{
	for (size_t i = 0; i < m_Layers.size(); i++) {
		m_Layers[i]->OnGUI();
	}
}

void LayerController::DestroyLayers()
{
	for (size_t i = 0; i < m_Layers.size(); i++) {
		m_Layers[i]->OnDestroy();
		delete m_Layers[i];
	}
}
