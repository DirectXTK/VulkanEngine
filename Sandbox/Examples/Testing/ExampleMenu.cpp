#include "ExampleMenu.h"
#include "AnimationTestingLayer.h"
#include "GUITesting.h"
#include "AnimationTestingLayer.h"
#include "CollisionTestingLayer.h"
#include "SerializationTesting.h"
#include "FontTestingLayer.h"
#include "MultiThreadingLayer.h"
ExampleMenu::ExampleMenu(): Layer("ExampleMenu")
{
}

void ExampleMenu::OnCreate()
{
}

void ExampleMenu::OnUpdate(double DeltaTime)
{
}
void ExampleMenu::OnRender(double deltime){

}
void ExampleMenu::OnDestroy()
{
}

void ExampleMenu::OnGUI()
{
    GUIRenderer* gui = Application::GetGUIRenderer();
    gui->Panel("Panel",{0.0f,0.0f},{0.4f,0.4f,0.4f,0.8f},{0.5f,0.5f},0,true);

    if(gui->Button("GUITesting","GUITESTING",{0.0f,0.85f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<GUITestingLayer>();
    }
    else if(gui->Button("AnimationTestingLayer","ANIMATION",{0.0f,0.55f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<AnimationTestingLayer>();
    }
     else if(gui->Button("CollisionLayer","CollisionLayer",{0.0f,0.3f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<CollisionLayer>();
    }
     else if(gui->Button("SerializationTestingLayer","SerializationTestingLayer",{0.0f,0.05f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<SerializationTestingLayer>();
    }
    else if(gui->Button("FontTestingLayer","FontTestingLayer",{0.0f,-0.2f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<FontTestingLayer>();
    } else if(gui->Button("MultiThreadingLayer","MultiThreadingLayer",{0.0f,-0.45f},{1.0f,1.0f,1.0f,1.0f},{0.15f,0.1f})){
        TransitionLayer<MultiThreadingLayer>();
    }


    gui->EndPanel();
}
