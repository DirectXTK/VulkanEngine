#pragma once
#include "AppCore.h"
#include "Event.h"
class Renderer;
class AssetManager;
class LayerController;
class Layer{
public:
    virtual void OnUpdate(double deltatime)=0;
    virtual void OnRender(double deltaTime)=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnEvent(Event& event){}
    virtual void OnGUI()=0;

    std::string& GetLayerName(){return m_LayerName;}
    template<typename T>
    void TransitionLayer();

    virtual ~Layer() =default;
protected:
    friend LayerController;
    Layer(std::string name);
    void SetController(LayerController* controller);

    LayerController* m_Controller{};
    std::string m_LayerName{};
public:
};
class LayerController{
    public:
       

        void UpdateLayers(double deltatime);
        void RenderLayers(double deltaTime);
        void UpdateGUILayers();
        void DestroyLayers();
        void OnEvent(Event& event);
        uint64_t GetLayerCount(){return m_Layers.size();}

        template<typename LAYER>
        LAYER* GetLayer()const{
            for(uint32_t i =0;i < m_Layers.size();i++){
                LAYER* ret = dynamic_cast<LAYER*>(m_Layers[i]);
                if(ret)
                    return ret;
            }
            Core::Log(ErrorType::FatalError,"There is no such type created{GetLayer()}");
            return nullptr;
        }
        enum class QueueType{
            NONE,CREATE,TRANSITION,REMOVE,
        };
        struct TransitionData{
            TransitionData(){}
            TransitionData(Layer* initial,Layer* transitioned,const QueueType& type): Initial(initial),Transitioned(transitioned),Type(type){}
            Layer* Initial{};
            Layer* Transitioned{};
            QueueType Type{};
        };
        void QueueCreateLayer(Layer* layer){m_CommandQueue.emplace(layer,nullptr,QueueType::CREATE);}
        template<typename REMOVEL>
        void QueueRemoveLayer(){m_CommandQueue.emplace(GetLayer<REMOVEL>(),nullptr,QueueType::REMOVE);}
        void QueueTransition(Layer* initial,Layer* transitioned){m_CommandQueue.emplace(initial,transitioned,QueueType::TRANSITION);}
        void RunQueue();
    private:

    void RemoveLayer(Layer* layer);


    std::queue<TransitionData> m_CommandQueue{};
    std::vector<Layer*> m_Layers{};
};
template<typename T>
void Layer::TransitionLayer(){
    m_Controller->QueueTransition(this,new T());
}
