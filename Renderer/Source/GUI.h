#pragma once
#include "AppCore.h"
#include "Event.h"
#include "GUIStyles.h"
#define GUI_HEADER
class Application;
class FontSystem;
class GUIRenderer
{
public:
	GUIRenderer(Application* app, bool SaveState);
	void BeginGUI();

	void Panel(const std::string& ID, Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle = 0, bool Dragable = false);



	bool Button(const std::string& ID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size, MouseCodes mousecode = MouseCodes::LEFT, GUUID TextureHandle = 0, bool SaveState = true, bool Dragable = false, bool** IsPressed = nullptr);


	void Text(const std::string& strID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size);

	void Slider(const std::string& strID, float* number,const Float2& Position,const  Float4& color, const Float2& Size,const  float SlideAmount,const  Float2& MinMax,const  uint32_t DecimalPlaces=3);
	void Slider(const std::string& strID, int* number,const  Float2& Position,const Float4& color ,const Float2& Size,const  float SlideAmount=0.01f);

	void Quad(const Float2& position,const Float2& size,const Float4& color,GUUID textureID=0);

	//returns true if checkbox is currently checked
	bool CheckBox(const std::string& id,const Float2& position,const Float2& size,const Float4& color,GUUID customCheckBoxTexture = 0);

	void InputText(const char* ID, char* Buffer, uint64_t BufferSize, Float2 Position, Float2 Size,bool scrollable = false);

	void EndPanel();

	void PushStyle(const GUI::Style& style,void* Data);

	void PopStyle();

	void SetFontSize(uint32_t Size);
	void SetFont(const std::string& strID);
	void SetFont(GUUID id);

	uint32_t GetFontSize();

	void EndGUI();

	~GUIRenderer();
protected:
	void OnEvent(Event& event);
	void OnKeyBoardEvent(KeyBoardEvent& event);
	void OnMouseEvent(MouseEvent& event);
	void OnInputTextEvent(InputTextEvent& event);
private:
	void DrawBorder(const Float2& Position, const Float2& Size, const Float4& BorderColor, const Float4& BackGroundColor,float BorderWidth);
	void ReapplyStyles(const GUI::Style& removedStyle);

	void ApplyCurrentStyles( Float2& position, Float2& size, Float4& color,GUUID id =0);

	void UpdateLineStarts(std::vector<uint64_t>& lineStarts,const char* buffer,uint64_t bufferSize,float lineSizeX);
	friend Application;

	Application* m_Application{};
	struct PanelData {
		Float2 Position{};
		Float2 Size{};
		GUUID ID{};
		PanelData* pParent{};
		Float2 Offset{};
	};
	struct ButtonData {
		bool IsPressed{ false };
		Float2 DragPosition{};
		float LastClicked{};
	};
	struct SliderData {
		bool IsClicked{false};
		float MousePosChange{};
	};
	struct InputTextData{
		int64_t ScrollYIndex{};
		//shows which line starts where.
		std::vector<uint64_t> LineStarts{};
	};
	struct CheckBoxData{
		bool IsClicked{false};
	};
	std::unordered_map<std::string, ButtonData> m_Buttons{};
	std::unordered_map<std::string, PanelData> m_Panels{};
	std::unordered_map<std::string, SliderData> m_Sliders{};
	std::unordered_map<std::string, CheckBoxData> m_CheckBoxes{};
	std::unordered_map<GUUID, InputTextData> m_InputTextData{};

	bool m_InputTextInputEvent{false};

	
	//std::unordered_map<uint32_t, ButtonData> m_ButtonIDs{};
	std::unordered_map<GUUID, PanelData> m_PanelIDs{};
	
	//styles 
	struct StyleContainer {
		//heap allocated
		GUI::Style StyleType{};
		void* StyleData{};
	};
	std::vector<std::stack<StyleContainer>> m_StylesInQueue{};
	GUI::Style m_LastAddedStyle{};


	//Scroll 0 means no scrolling happend.
	float m_Scroll{};

	//Clicked object.
	GUUID m_SelectedObjID{0};
	//Currently selected obj.
	GUUID m_CurrentlySelectedObject{0};
	Float2* m_PickBufferData{};

	uint32_t m_PanelDepth{};
	PanelData* m_CurrenPanelParent{};
	uint32_t m_CurrentPanel{};
	GUUID m_DraggedPanel{ 0 };
	//uint32_t m_CurrentButton{};
	//If true when saves position of the buttons
	bool m_SaveState{};
	//consts
	//in milliseconds
	const float m_ButtonClickCooldowm{50.f};

	FontSystem* m_FontSystem{};
};

