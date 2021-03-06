#pragma warning( disable : 4244 )
#include "overlay.h"
#include <string>
#include <filesystem>
#include <thread>
#include <algorithm>
#include <iterator>
#include "imgui/unknownController.h"
#include "imgui/FontAwesome.h"
#include "imgui/IconsFontAwesome.h"
#include "imgui/Comfortaa-Regular.h"
#include "imgui/Comfortaa-Bold.h"
#include "imgui/imgui_memory_editor.h"
#include <map>
#include "defs.h"
#include <string>

// set up these variables here in case we need them later
HWND overlayWindow;
IDirect3D9Ex* p_Object;
IDirect3DDevice9Ex* p_Device;
D3DPRESENT_PARAMETERS p_Params;
ID3DXLine* p_Line;
ID3DXFont* pFont;
ID3DXFont* pModuleFont;
ID3DXFont* pESPFont;
RECT rc;
MARGINS margin = { -1 };
ImFont* titleFont;
ImFont* bigFont;
MSG message;
IDirect3DTexture9* unknownCtrl;
float oldDelay = 0;

// winprochandler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;

	case WM_DESTROY:
		overlay::DirectXShutdown();
		PostQuitMessage(0);
		return 0;
		break;

	default:
		return DefWindowProcW(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void HandleMessages(std::wstring lClassName, std::wstring lWindowName)
{
	// create the window and set window attributes
	overlayWindow = CreateWindowExW(/*WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED*/NULL, lClassName.c_str(), lWindowName.c_str(), WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, 0, 0);
	SetWindowLongA(overlayWindow, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	DwmExtendFrameIntoClientArea(overlayWindow, &margin);
	ShowWindow(overlayWindow, SW_SHOW);
	UpdateWindow(overlayWindow);

	while (true)
	{
		if (PeekMessageW(&message, overlayWindow, 0, 0, PM_REMOVE))
		{
			DispatchMessageW(&message);
			TranslateMessage(&message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == globals.hWnd)
		{
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(overlayWindow, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		Sleep(1);
	}
}

bool overlay::InitWindow()
{
	// create a window class
	std::string r1 = Util::RandomString(16);
	std::string r2 = Util::RandomString(16);

	std::wstring lClassName = std::wstring(r1.begin(), r1.end());
	std::wstring lWindowName = std::wstring(r2.begin(), r2.end());

	WNDCLASSEXW wClass;
	wClass.cbSize = sizeof(WNDCLASSEXW);
	wClass.style = 0;
	wClass.lpfnWndProc = WinProc;
	wClass.cbClsExtra = NULL;
	wClass.cbWndExtra = NULL;
	wClass.hInstance = nullptr;
	wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wClass.hCursor = LoadCursor(0, IDC_ARROW);
	wClass.hbrBackground = nullptr;
	wClass.lpszMenuName = lWindowName.c_str();
	wClass.lpszClassName = lClassName.c_str();
	wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

	if (!RegisterClassExW(&wClass))
		return false;

	std::thread msgThread(HandleMessages, lClassName, lWindowName);
	msgThread.detach();

	return true;
}

bool overlay::DirectXInit()
{
	// initialize directx
	while (!overlayWindow) {}

	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		return false;

	GetClientRect(globals.hWnd, &rc);

	globals.windowWH = { rc.right - rc.left, rc.bottom - rc.top };

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = overlayWindow;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = globals.windowWH.x;
	p_Params.BackBufferHeight = globals.windowWH.y;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, overlayWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		return false;

	D3DXCreateLine(p_Device, &p_Line);

	D3DXCreateFont(p_Device, 50, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma", &pFont); // Create the font used for text rendering
	D3DXCreateFont(p_Device, 14, 0, FW_BOLD, 10, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Tahoma", &pESPFont); // Create the font used for esp rendering
	//D3DXCreateFontW
	if (FAILED(D3DXCreateTextureFromFileInMemory(p_Device, unknownController, sizeof(unknownController), &unknownCtrl)))
		return false;

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	// add default font and merge fontawesome icons
	io.Fonts->AddFontFromMemoryTTF(comfortaaRegular, sizeof(comfortaaRegular), 13.f);

	// you can uncomment this if you want to load the icon font/bold font
	//static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	//ImFontConfig icons_config;
	//icons_config.MergeMode = true;
	//icons_config.PixelSnapH = true;
	//icons_config.GlyphOffset = ImVec2(0.f, 2.f);
	//io.Fonts->AddFontFromMemoryTTF(font_awesome_data, sizeof(font_awesome_data), 16.f, &icons_config, icons_ranges); // icons font

	//// add title font and merge fontawesome icons
	//ImFontConfig titleConfig;
	//titleConfig.GlyphOffset = ImVec2(0.f, -2.f);
	//titleFont = io.Fonts->AddFontFromMemoryTTF(comfortaaBold, sizeof(comfortaaBold), 19.f, &titleConfig); // bold font

	//io.Fonts->AddFontFromMemoryTTF(font_awesome_data, sizeof(font_awesome_data), 21.f, &icons_config, icons_ranges);

	//// add big font and merge fontawesome icons
	//bigFont = io.Fonts->AddFontFromMemoryTTF(comfortaaRegular, sizeof(comfortaaRegular), 16.f);

	//icons_config.GlyphOffset = ImVec2(0.f, 1.f);
	//io.Fonts->AddFontFromMemoryTTF(font_awesome_data, sizeof(font_awesome_data), 18.f, &icons_config, icons_ranges);

	ImGui_ImplWin32_Init(overlayWindow);
	ImGui_ImplDX9_Init(p_Device);

	return true;
}

void InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}

ImU8 customRead(const ImU8* data, size_t offset) {
	return Driver.rpm<ImU8>(globals.testPtr + offset);
}

void customWrite(ImU8* data, size_t offset, ImU8 d) {
	Driver.wpm<ImU8>(globals.testPtr + offset, d);
}

void DrawImGui()
{
	// pass input to imgui
	InputHandler();

	// create an imgui frame
	
	// set the right window flags
	if (globals.menuActive)
		SetWindowLongA(overlayWindow, GWL_EXSTYLE, (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	else
		SetWindowLongA(overlayWindow, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));

	if (globals.menuActive)
	{
		static MemoryEditor mem_edit_1;
		char data[0x10000];
		size_t data_size = 0x10000;
		mem_edit_1.ReadFn = customRead;
		mem_edit_1.WriteFn = customWrite;
		mem_edit_1.DrawWindow(xorstr_("Memory Editor"), data, data_size, 0);

		ImGui::SetNextWindowSize(ImVec2(820, 700));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		/* -------------------------------------- Main Window Start ------------------------------------ */
		if (ImGui::Begin(xorstr_("FlowHook"), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize )) {

			static short tab = 0;

			if (ImGui::Button(xorstr_("Aimbot"), ImVec2(265, 25)))
				tab = 0;

			ImGui::SameLine();

			if (ImGui::Button(xorstr_("Visuals"), ImVec2(265, 25)))
				tab = 1;

			ImGui::SameLine();

			if (ImGui::Button(xorstr_("Misc"), ImVec2(265, 25)))
				tab = 2;


			/*===================Aimbot==================*/
			if (tab == 0){
				ImGui::Text(xorstr_("Aimbot"));
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::Checkbox(xorstr_("Aimbot"), &globals.aimbot);
				ImGui::Checkbox(xorstr_("Visible check"), &globals.aimbotVisibleCheck);
				ImGui::Checkbox(xorstr_("Prediction"), &globals.Prediction);
				ImGui::Checkbox(xorstr_("Fov"), &globals.fov);
				ImGui::SameLine();
				ImGui::ColorEdit4(xorstr_("color"), (float*)&globals.fovCircleColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
				ImGui::SliderFloat(xorstr_("Smoothness"), &globals.aimbotSmoothness, 0, 100);
				ImGui::SliderFloat(xorstr_("FOV"), &globals.aimbotFOV, 1, 400);
				ImGui::Combo(xorstr_("Bone"), &globals.aimbotOffset, globals.Bonez, IM_ARRAYSIZE(globals.Bonez));
				ImGui::PopID();
				ImGui::Combo(xorstr_("Hotkey"), &globals.aimbotKeyID, globals.aimbotKeys, IM_ARRAYSIZE(globals.aimbotKeys));
				ImGui::PopID();
			}


			/*===================visual==================*/
			if (tab == 1) {   
				static short section = 0;

				if (ImGui::Button(xorstr_("Esp"), ImVec2(265, 20)))
					section = 0;

				ImGui::SameLine();

				if (ImGui::Button(xorstr_("Player Glow"), ImVec2(265, 20)))
					section = 1;

				ImGui::SameLine();

				if (ImGui::Button(xorstr_("SkinChanger"), ImVec2(265, 20)))
					section = 2;


				if (section == 0) {

					ImGui::Text(xorstr_("Box Esp"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("Box ESP"), &globals.Boxesp);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("ESP Color | Visible"), (float*)&globals.espColorVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("ESP Color | Invisible"), (float*)&globals.espColorInvisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::Combo(xorstr_("Type"), &globals.Boxnum, globals.Boxtype, IM_ARRAYSIZE(globals.Boxtype));
					ImGui::PopID();

					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("Fill Boxes"), &globals.fillBox);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("ESP Fill Color | Visible"), (float*)&globals.espFillColorVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("ESP Fill Color | Invisible"), (float*)&globals.espFillColorInvisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);

					ImGui::Spacing();
					ImGui::Text(xorstr_("Head Esp"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("Head Circle"), &globals.Circleesp);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Head Circle Color | Visible"), (float*)&globals.headCircleColorVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Head Circle Color | Invisible"), (float*)&globals.headCircleColorInvisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SliderInt(xorstr_("ESP Thickness"), &globals.espThickness, 1, 5, "%dpx");

					ImGui::Spacing();
					ImGui::Text(xorstr_("SnapLine"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("SnapLine"), &globals.Snapeline);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Line Color | Visible"), (float*)&globals.SnapColorv, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Line Color | Invisible"), (float*)&globals.SnapColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);

					ImGui::Combo(xorstr_("Select"), &globals.SnapSelect, globals.SnapSel, IM_ARRAYSIZE(globals.SnapSel));
					ImGui::PopID();

					ImGui::SliderFloat(xorstr_("SnapDist"), &globals.SnaplineDist, 0.f, 1000.f, "%.1fm");

					ImGui::Spacing();
					ImGui::Text(xorstr_("Esp Stuff"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::ColorEdit4(xorstr_("Text Color | Visible"), (float*)&globals.textColorVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::ColorEdit4(xorstr_("Text Color | Invisible"), (float*)&globals.textColorInvisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SliderFloat(xorstr_("Max Box Distance"), &globals.maxBoxDistance, 0.f, 1000.f, "%.1fm");
					ImGui::SliderFloat(xorstr_("Max Text Distance"), &globals.maxTextDistance, 0.f, 1000.f, "%.1fm");
					ImGui::SliderFloat(xorstr_("Max Head Circle Distance"), &globals.maxHeadCircleDistance, 0.f, 1000.f, "%.1fm");

					ImGui::Spacing();
					ImGui::Text(xorstr_("Healthbar"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Combo(xorstr_("Type"), &globals.Healthnum, globals.Healthtype, IM_ARRAYSIZE(globals.Healthtype));
					ImGui::PopID();
					ImGui::SliderFloat(xorstr_("Max Healthbars Circle Distance"), &globals.maxHealthDistance, 0.f, 1000.f, "%.1fm");
					ImGui::SliderFloat(xorstr_("Max Head Line Distance"), &globals.maxHeadLineDistance, 0.f, 1000.f, "%.1fm");
				}

				if (section == 1) { //playerglow
					ImGui::Separator();
					ImGui::Text(xorstr_("Enemy player"));
					ImGui::Checkbox(xorstr_("Glow"), &globals.playerGlow);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Visible"), (float*)&globals.playerGlowColorv, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("Invisible"), (float*)&globals.playerGlowColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::Checkbox(xorstr_("Outline"), &globals.playerOutline);
					ImGui::Combo(xorstr_("Type"), &globals.playerGlowStyle, globals.playerGlowStyles, IM_ARRAYSIZE(globals.playerGlowStyles));
					ImGui::PopID();
					ImGui::InputInt("Player | Custom", &globals.CustomPlayerGlowtypes, 0.f);
					ImGui::SliderFloat(xorstr_("Glow Brightness"), &globals.PlayerGlowBrightness, 0.f, 100.f, "%.1f%%");
					ImGui::SliderFloat(xorstr_("Player Transperancy"), &globals.PlayerTransperancy, 0.f, 100.f, "%.1f%%");
					ImGui::Separator();
					ImGui::Text(xorstr_("local player"));

					ImGui::Checkbox(xorstr_("localGlow"), &globals.localplayerGlow);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("color"), (float*)&globals.localplayerGlowColorv, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::Checkbox(xorstr_("Outline"), &globals.localplayerOutline);
					ImGui::Combo(xorstr_("Type"), &globals.localplayerGlowStyle, globals.localplayerGlowStyles, IM_ARRAYSIZE(globals.localplayerGlowStyles));
					ImGui::PopID();
					ImGui::InputInt("Local | Custom", &globals.CustomLocalGlowtypes, 0.f);
					ImGui::SliderFloat(xorstr_("Glow Brightness"), &globals.localPlayerGlowBrightness, 0.f, 100.f, "%.1f%%");
					ImGui::SliderFloat(xorstr_("LocalPlayer Transperancy"), &globals.LocalTransperancy, 0.f, 100.f, "%.1f%%");

					ImGui::Text(xorstr_("Item Glow"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("Item Glow"), &globals.itemGlow);
					ImGui::ColorEdit3(xorstr_("Color"), (float*)&globals.rareColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip);

					ImGui::Text(xorstr_("Viewmodel glow"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox(xorstr_("ViewModel Glow"), &globals.viewmodelGlow);
					ImGui::SameLine();
					ImGui::ColorEdit4(xorstr_("color"), (float*)&globals.viewmodelColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);
					ImGui::Checkbox(xorstr_("Outline"), &globals.viewmodelOutline);
					ImGui::InputInt(xorstr_("ViewModel | Custom"), &globals.viewmodelGlowType, 0.f);
					ImGui::SliderFloat(xorstr_("Glow Brightness"), &globals.viewmodelBrightness, 0.f, 100.f, "%.1f%%");
					ImGui::SliderFloat(xorstr_("ViewModel Transperancy"), &globals.ViewModelTransperancy, 0.f, 100.f, "%.1f%%");

				}

				if (section == 2) {  //skin
					ImGui::Text(xorstr_("SkinChanger | LocalPlayer"));
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::InputInt("SkinID", &globals.Skin, 0.f);
				}
			}

			/*===================Misc==================*/
			if (tab == 2) {

				ImGui::Text(xorstr_("Misc"));
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::Checkbox(xorstr_("Rapid Fire"), &globals.rapidFire);
				ImGui::SliderFloat(xorstr_("Rapid Fire Speed"), &globals.rapidFireSpeed, 1.f, 2.f);

				ImGui::Spacing();
				ImGui::Checkbox(xorstr_("Third Person"), &globals.thirdPerson);
				ImGui::Checkbox(xorstr_("Radar hack"), &globals.Minimap);
				ImGui::Checkbox(xorstr_("Crosshair"), &globals.Crosshair);
				ImGui::SameLine();
				ImGui::ColorEdit4(xorstr_("Color"), (float*)&globals.CrosshairColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar);

				ImGui::Spacing();
				ImGui::Checkbox(xorstr_("Recoil Control"), &globals.rcs);
				ImGui::SliderFloat(xorstr_("RCS Intensity"), &globals.rcsIntensity, 0.f, 100.f, "%.1f%%");
			}
		}
		ImGui::End();
	}
}

// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/client_class.h
struct ClientClass
{
	uint64_t pCreateFn;
	uint64_t pCreateEventFn;
	uint64_t pNetworkName;
	uint64_t pRecvTable;
	uint64_t pNext;
	uint32_t ClassID;
	uint32_t ClassSize;
};

// https://github.com/CasualX/apexbot/blob/d6dfbf40b5025e6676fe75177a39ddd285285160/src/state.cpp#L104
bool getClassNameX(uintptr_t ent, std::string& className)
{
	uintptr_t clientNetworkableVTable = Driver.rpm<uintptr_t>(ent + 8 * 3);
	if (!clientNetworkableVTable)
		return false;

	uintptr_t getClientClass = Driver.rpm<uintptr_t>(clientNetworkableVTable + 8 * 3);
	if (!getClientClass)
		return false;

	uint32_t disp = Driver.rpm<uint32_t>(getClientClass + 3);
	if (!disp) {
		return false;
	}
	const uint64_t client_class_ptr = getClientClass + disp + 7;
	// Get the ClientClass instance itself.
	ClientClass clientClass = Driver.rpm<ClientClass>(client_class_ptr);

	// Finally grab some bytes to be interpreted as a nul terminated c-string.
	className = Driver.ReadString(clientClass.pNetworkName, 128);
	if (className.length() == 128)
		className = xorstr_("Unknown");
	return true;
}


void overlay::Render()
{
	static ImDrawList* drawList;

	while (!GetAsyncKeyState(VK_END))
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		DrawImGui();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::Begin(xorstr_("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

		drawList = ImGui::GetWindowDrawList();

		p_Device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.f, 0);
		p_Device->BeginScene();
		ImGuiIO& io = ImGui::GetIO();

		// Raw directx drawing
		if (Player::IsPlayer(globals.localPlayer))
		{
			// get view matrix every frame
			globals.viewMatrix = Driver.rpm<viewMatrix_t>(Driver.rpm<uintptr_t>(globals.viewRenderer + OFFSET_MATRIX));

			int index = 0; // we need the index for the visibility check
			// loop through the entity list
			for (uintptr_t& player : Player::GetPlayers())
			{
				bool visible = Player::IsVisible(player, index);
				index++;

				if (!Player::IsValidEnemy(player)) continue; // check if its a valid enemy

				vec3 targetHead = Player::GetBonePos(player, 8); // get head position
				vec2 targetHeadScreen;
				if (!Util::WorldToScreen(targetHead, targetHeadScreen)) continue; // convert to screen coordinates

				vec3 targetBody = Driver.rpm<vec3>(player + OFFSET_ORIGIN); // get vecOrigin
				vec2 targetBodyScreen;
				if (!Util::WorldToScreen(targetBody, targetBodyScreen)) continue; // convert to screen coordinates
				
				vec3 targetNeck = Player::GetBonePos(player, 7); // get neck bone
				vec2 targetNeckScreen;
				if (!Util::WorldToScreen(targetNeck, targetNeckScreen)) continue; // convert to screen coordinates

				if (globals.esp)
				{
					// calculate the stuff needed for the boxes
					float height = abs(abs(targetHeadScreen.y) - abs(targetBodyScreen.y));
					float width = height / 2.f;
					float middle = targetBodyScreen.x - (width / 2.f);
					float dist = 0.f;
					// If we're alive calculate it based off the localPlayer, however if we're dead use the player we're spectating
					if (!Player::IsDead(globals.localPlayer)) {
						dist = Driver.rpm<vec3>(globals.localPlayer + OFFSET_ORIGIN).DistTo(targetHead);
					}
					else
					{
						// Getting observer is aids
						int observingIndex = Driver.rpm<int>(globals.localPlayer + OFFSET_OBSERVING_TARGET);
						uintptr_t observingPlayer = Driver.rpm<uintptr_t>(globals.moduleBase + OFFSET_ENTITYLIST + (observingIndex << 5));
						dist = Driver.rpm<vec3>(observingPlayer + OFFSET_ORIGIN).DistTo(targetHead);
					}
					float distM = Util::ToMeters(dist);
					float neckHeadDistance = targetHeadScreen.Dist2D(targetNeckScreen);

					D3DCOLOR espColor;
					D3DCOLOR espFillColor;

					// get the appropriate colors
					if (visible)
					{
						espColor = Util::Vec4toARGB(globals.espColorVisible);
						espFillColor = Util::Vec4toARGB(globals.espFillColorVisible);
					}
					else
					{
						espColor = Util::Vec4toARGB(globals.espColorInvisible);
						espFillColor = Util::Vec4toARGB(globals.espFillColorInvisible);
					}

					// call the function to draw the box
					if (globals.Boxesp)
					Modules::DrawBoxESP(middle, targetHeadScreen.y, width, height, globals.espThickness, espFillColor, espColor);

					if (distM <= globals.maxHealthDistance)
					{
						D3DCOLOR healthcol;
						D3DCOLOR shieldcol;

						shieldcol = Util::Vec4toARGB(globals.shieldcol);
						healthcol = Util::Vec4toARGB(globals.healthcol);

						int Health = Driver.rpm<int>(player + OFFSET_HEALTH);
						int shield = Driver.rpm<int>(player + OFFSET_SHIELD);

						std::string healthnu = std::to_string(Health);
						std::string shieldnu = std::to_string(shield);

						// call the function to draw the healthbars
						switch (globals.Healthnum) {
						case 0: // bars
							Modules::DrawHealthbars(middle, targetHeadScreen.y, width, height, 4, Driver.rpm<int>(player + OFFSET_HEALTH), Driver.rpm<int>(player + OFFSET_SHIELD), Driver.rpm<int>(player + OFFSET_ARMOR_TYPE));
							break;
						case 1: // numbers
							DrawShadowString(shieldnu.c_str(), targetBodyScreen.x - 35, targetHeadScreen.y + 5, 1, shieldcol, ARGB(255, 0, 0, 0), pESPFont);
							DrawShadowString(healthnu.c_str(), targetBodyScreen.x - 35, targetHeadScreen.y - 5, 1, healthcol, ARGB(255, 0, 0, 0), pESPFont);
							break;
						}
					}

					if (distM <= globals.maxTextDistance)
					{
						D3DCOLOR textColor;

						// get the appropriate colors
						if (visible)
							textColor = Util::Vec4toARGB(globals.textColorVisible);
						else
							textColor = Util::Vec4toARGB(globals.textColorInvisible);

						// draw text
						DrawShadowString(Player::GetName(player).c_str(), middle, targetHeadScreen.y - 15, 1, textColor, ARGB(255, 0, 0, 0), pESPFont);
						std::string distance = std::to_string(distM);
						distance = distance.substr(0, distance.find('.')) + "m";
						DrawShadowString(distance.c_str(), middle, targetBodyScreen.y + 1, 1, textColor, ARGB(255, 0, 0, 0), pESPFont);
					}

					if (distM <= globals.maxHeadCircleDistance)
					{
						D3DCOLOR circleColor;

						// get the appropriate colors
						if (visible)
							circleColor = Util::Vec4toARGB(globals.headCircleColorVisible);
						else
							circleColor = Util::Vec4toARGB(globals.headCircleColorInvisible);

						// TODO: This needs to be fixed or removed, personally I don't care about it. Somethimes the circle breaks and becomes very large. I don't know why but I don't really care.
						if(globals.Circleesp)
						DrawCircle(targetHeadScreen.x, targetHeadScreen.y, neckHeadDistance, 1.f, 20.f, circleColor); // we need the distance from head to neck so we can size it depending on distance
						
					}

					if (distM <= globals.SnaplineDist) {
						
						if (globals.Snapeline) {
							
							D3DCOLOR Color;
							if (visible)
								Color = Util::Vec4toARGB(globals.SnapColorv);
							else
								Color = Util::Vec4toARGB(globals.SnapColor);

							switch (globals.SnapSelect) {
							case 0: // "top
								DrawLine(io.DisplaySize.x / 2, 0, targetHeadScreen.x, targetHeadScreen.y, 1, Color);
								break;
							case 1: // "crosshair
								DrawLine(io.DisplaySize.x / 2, io.DisplaySize.y / 2, targetHeadScreen.x, targetHeadScreen.y, 1, Color);
								break;
							case 2: // "bottom
								DrawLine(io.DisplaySize.x / 2, io.DisplaySize.y, targetHeadScreen.x, targetHeadScreen.y, 1, Color);
								break;
							}
						}
					}
				}
			}

		}

		if (globals.fov) {
			DrawCircle(io.DisplaySize.x / 2, io.DisplaySize.y / 2, globals.aimbotFOV, 1.f, 100, Util::Vec4toARGB(globals.fovCircleColor));
		}

		if (globals.Crosshair) {
			DrawLine(io.DisplaySize.x / 2 - 10, io.DisplaySize.y / 2, io.DisplaySize.x / 2 + 10, io.DisplaySize.y / 2, 1, Util::Vec4toARGB(globals.CrosshairColor)); //hewtee
			DrawLine(io.DisplaySize.x / 2, io.DisplaySize.y / 2 - 10, io.DisplaySize.x / 2, io.DisplaySize.y / 2 + 10, 1, Util::Vec4toARGB(globals.CrosshairColor)); //boso
		}
		int seenTypes[50] = {};
		int seenIndex = 0;
		for (size_t i = 0; i < globals.entityList.size() && false; i++) {
			uintptr_t ent = globals.entityList[i];
			if (ent == NULL) continue;

			vec3 entPos = Driver.rpm<vec3>(ent + OFFSET_ORIGIN);
			vec2 w2sEntPos;
			Util::WorldToScreen(entPos, w2sEntPos);
			if (w2sEntPos.y <= 0.f)
				continue;
			
			std::string className;
			getClassNameX(ent, className);

			if (className == "CPropSurvival") { // TODO: Enable when getting item IDs
				int scriptID = Driver.rpm<int>(ent + OFFSET_SCRIPT_ID);
				DrawShadowString(std::to_string(scriptID).c_str(), w2sEntPos.x, w2sEntPos.y, 2, ARGB(255, 255, 255, 255), ARGB(255, 0, 0, 0), pESPFont);
			}

			if (
				// Firing Range
				className == xorstr_("ScriptMoverLightweight") ||
				className == xorstr_("CPropSurvival") ||
				className == xorstr_("CBaseEntity") ||
				className == xorstr_("CDynamicProp") ||
				className == xorstr_("CWeaponX") || // Weapon in players hands
				className == xorstr_("CPlayerWaypoint") ||
				className == xorstr_("CBaseAnimating") ||
				className == xorstr_("CGrappleHook") ||
				className == xorstr_("CScriptNetData_SNDC_PLAYER_GLOBAL") || // wtf are these
				className == xorstr_("CScriptNetData_SNDC_PLAYER_EXCLUSIVE") ||
				className == xorstr_("CInfoTarget") ||
				className == xorstr_("CAI_BaseNPC") ||
				className == xorstr_("CZiplineEnd") ||
				className == xorstr_("CZipline") ||
				className == xorstr_("CAmbientGeneric") ||
				className == xorstr_("CSkyCamera") ||
				className == xorstr_("CParticleSystem") ||
				className == xorstr_("CPointCamera") ||
				className == xorstr_("CCascadeLight") ||
				className == xorstr_("CTriggerNoZipline") ||
				className == xorstr_("CEntityLinkPage") ||
				className == xorstr_("CImportantOnEntSound") ||
				className == xorstr_("Titan_Cockpit") ||
				className == xorstr_("CPredictedFirstPersonProxy") ||
				className == xorstr_("CTeam") ||
				className == xorstr_("CBaseViewModel") ||
				className == xorstr_("CWorld") ||
				className == xorstr_("CFirstPersonProxy") ||
				className == xorstr_("CEntityDissolve") ||
				className == xorstr_("CPhysicsProp") ||
				className == xorstr_("CGlobalNonRewinding") ||
				className == xorstr_("CColorCorrection") ||
				className == xorstr_("CScriptNetDataGlobal") ||
				className == xorstr_("CScriptNetDataGlobalNonRewind") ||
				className == xorstr_("CBaseGrenade") || // All grenades(Thermite, Frag, Arc Star)
				className == xorstr_("CRopeKeyframe") || // Exists while using Pathfinder ultimate
				className == xorstr_("CScriptProp") ||
				className == xorstr_("CScriptTraceVolume") ||
				className == xorstr_("CScriptMover") ||
				className == xorstr_("CInfoPlacementHelper") ||
				className == xorstr_("CCrossbowBolt") || // All(?) bullets
				className == xorstr_("CStatusEffectPlugin") ||
				className == xorstr_("CMissile") || // Missiles from ultimates(While in air)
				className == xorstr_("CVortexSphere") ||
				className == xorstr_("CPlayer") ||
				// World's Edge
				className == xorstr_("CPropDoor") || // Door
				className == xorstr_("CTriggerSlip") ||
				className == xorstr_("CTriggerNoGrapple") ||
				className == xorstr_("CDeathBoxProp") || // Death Box
				className == xorstr_("CLootRoller") ||
				className == xorstr_("CTriggerNoObjectPlacement") ||
				className == xorstr_("CBoneFollower") ||
				className == xorstr_("CBaseTrigger") ||
				className == xorstr_("CScriptMoverTrainNode") ||
				className == xorstr_("CScriptNetData_SNDC_DEATH_BOX") ||
				className == xorstr_("CFuncBrush") ||
				className == xorstr_("CLootGrabber") ||
				className == xorstr_("CTriggerCylinderHeavy") ||
				className == xorstr_("CTriggerUpdraft") ||
				className == xorstr_("CDynamicPropLightweight") ||
				className == xorstr_("CShieldProp") ||
				className == xorstr_("CProjectile") ||
				className == xorstr_("CNPC_Dropship") || // Respawn Ship(?)
				// Kings Canyon
				className == xorstr_("CFuncBrushLightweight") || // Appeared in Selection Menu
				className == xorstr_("CBreakableProp") ||
				className == xorstr_("CPlayerDecoy") ||
				className == xorstr_("CPlayerVehicle") || // Not sure how this existed on Kings Canyon
				className == xorstr_("CTurret") ||
				className == xorstr_("CKnockBack") ||
				className == xorstr_("CZiplinePhysics") ||
				className == xorstr_("CScriptNetData_SNDC_PLAYER_EXCLUSIVE_EXPANDED")
			) continue;

			//printf("%s\n", className.c_str());

			DrawShadowString(className.c_str(), w2sEntPos.x, w2sEntPos.y, 2, ARGB(255, 255, 255, 255), ARGB(255, 0, 0, 0), pESPFont);
		}
		/*
		 * CPropSurvival
		 * CDynamicProp
		 * CParticleSystem(DT_ParticleSystem)
		 * CAI_BaseNPC
		 * CWeaponX
		 * ScriptMoverLightweight - Targets that get shot down
		 * CBaseEntity
		 * CZiplineEnd
		 * CZipline
		*/
		drawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		p_Device->EndScene();
		p_Device->PresentEx(0, 0, 0, 0, 0);
	}
}

void overlay::DirectXShutdown()
{
	// destroy everything
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	p_Object->Release();
	p_Device->Release();
	pFont->Release();
	pESPFont->Release();
	p_Line->Release();

	DestroyWindow(overlayWindow);
}
/*
void overlay::UpdateWindowPos()
{
	RECT oldRC = rc;
	while (!GetAsyncKeyState(VK_END))
	{
		// update the window position every 100 ms
		globals.hWnd = FindWindowA(NULL, xorstr_("Apex Legends"));

		if (globals.hWnd)
		{
			GetWindowRect(globals.hWnd, &rc);
			globals.windowWH = { rc.right - rc.left, rc.bottom - rc.top };

			DWORD dwStyle = GetWindowLong(globals.hWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				rc.top += 32;
				globals.windowWH.y -= 39;
			}

			MoveWindow(overlayWindow, rc.left, rc.top, globals.windowWH.x, globals.windowWH.y, true);
		}

		Sleep(100);
	}
}*/
