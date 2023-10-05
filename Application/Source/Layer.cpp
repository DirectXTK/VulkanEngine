#include "Layer.h"

void Layer::SetSystem(Camera2D* camera, Renderer* renderer)
{
	m_Camera2 = camera;
	m_Renderer = renderer;
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

void LayerController::UpdateLayers()
{
	for (size_t i = 0; i < m_Layers.size(); i++) {
		m_Layers[i]->OnUpdate();
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
