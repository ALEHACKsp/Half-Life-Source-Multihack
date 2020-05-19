#include "pch.h"
#include "hack.h"

namespace Hack
{
	namespace Data
	{
		//Menu
		bool bInitialized = false;
		bool bShowMenu = false;
		bool bBunnyhop = false;
		bool bTriggerbot = false;
		bool bEspSnaplines = false;
		int iSnaplineThickness = 2;
		bool bEspName = false;
		int iNameSize = FONT_SIZE;
		bool bEspLimitedRange = false;
		bool bEnableEntityFilter = false;
		int iNameDist = NAME_DIST;
		bool bEspDist = false;
		bool bGodmode = false;
		bool bNoClip = false;
		bool bOneHitKills = false;
		bool bInfiniteHealth = false;
		bool bInfiniteArmor = false;
		bool bNoAmmoDecrease = false;
		bool bCustomCrosshair = false;
		bool bTeleport = false;
		bool bTeleportPending = false;
		bool bShowNameColor = false;
		bool bShowSnaplineColor = false;
		bool bShowCrosshairSettings = false;
		bool bFovChanger = false;
		bool bServerCheats = false;
		bool bTimescaleChanger = false;
		char strNameFont[MAX_PATH] = FONT_NAME;
		float flNewTimescale = DEFAULT_TIMESCALE;
		int iNewFov = DEFAULT_FOV;
		flVec3 vTeleportPos = { 0, 0, 0 };
		float flEspRange = 750;
		bool bGravityChanger = false;
		float flNewGravity = DEFAULT_GRAVITY;
		DrawColor SnaplineColor = { 255, 0, 0, 255 };
		DrawColor NameColor = { 255, 125, 0, 255 };
		Crosshair crosshair = { { 0, 0 }, { 125, 255, 0, 255 }, 2, 2, 10 };
		Window wnd;
		ImFont* imgui_font;

		//Game Data
		mem_t client;
		mem_t engine;
		mem_t server;
		mem_t fDecreaseHealthAddr;
		mem_t fDecreaseAmmoAddr;
		mem_t fDecreaseAmmoShotgunAddr;
		mem_t fDecreaseAmmoOthersAddr;
		mem_t fReloadIncreaseClipAmmoAddr;
		mem_t fReloadIncreaseClipAmmoShotgunAddr;
		mem_t fReloadDecreaseAmmo;
		bool* CheckPlayerState;
		bool* Loading;
		HLEntity* LocalPlayer;
		HLEntity* Entity;
		mem_t EntityList;
		std::vector<char*> EntityStringFilter = { BLACKLISTED_ENTITIES };
		UINT KeyHook[0xFE];
		ViewMatrix vMatrix;
		HLClientData* ClientData;
		DWORD* ForceJump;
		DWORD* ForceAttack;
		DWORD* EnableCrosshair;
		DWORD* SvCheats;
		float* Gravity;
		float* Timescale;
	}
}

mem_t DecHealthJumpAddr;
mem_t DecAmmoJumpAddr;
mem_t DecAmmoShotgunJumpAddr;
mem_t DecAmmoOthersJumpAddr;
mem_t ReloadIncClipAmmoJumpAddr;
mem_t ReloadIncClipAmmoShotgunJumpAddr;
mem_t ReloadDecAmmoJumpAddr;

void Hack::DrawMenu(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("ESP"))
	{
		ImGui::Checkbox("ESP Snaplines", &Data::bEspSnaplines);
		ImGui::Checkbox("ESP Name", &Data::bEspName);
		ImGui::Checkbox("Enable Entity Filter", &Data::bEnableEntityFilter);
		ImGui::Checkbox("ESP Limited Range", &Data::bEspLimitedRange);
		if (Data::bEspLimitedRange)
		{
			ImGui::SliderFloat("ESP Range", &Data::flEspRange, MIN_ESP_RANGE, MAX_ESP_RANGE, "%1.f");
		}

		ImGui::Checkbox("Show Snapline Settings", &Data::bShowSnaplineColor);
		if (Data::bShowSnaplineColor)
		{
			ImGui::SliderInt("ESP Snaline Thickness", &Data::iSnaplineThickness, ESP_THICKNESS_MIN, ESP_THICKNESS_MAX);
			ImGui::SliderInt("ESP Snapline R", &Data::SnaplineColor.r, 0, 255);
			ImGui::SliderInt("ESP Snapline G", &Data::SnaplineColor.g, 0, 255);
			ImGui::SliderInt("ESP Snapline B", &Data::SnaplineColor.b, 0, 255);
			ImGui::SliderInt("ESP Snapline A", &Data::SnaplineColor.a, 0, 255);
		}

		ImGui::Checkbox("Show Name Settings", &Data::bShowNameColor);
		if (Data::bShowNameColor)
		{
			ImGui::Checkbox("ESP Dist", &Data::bEspDist);

			if (ImGui::InputText("Font Name", Data::strNameFont, sizeof(Data::strNameFont)) && D3D9::dxFont)
			{
				D3D9::dxFont->Release();
				D3DXCreateFontA(pDevice, Data::iNameSize, 0, FW_REGULAR, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Data::strNameFont, &D3D9::dxFont);
			}

			if (ImGui::SliderInt("Font Size", &Data::iNameSize, FONT_MIN_SIZE, FONT_MAX_SIZE) && D3D9::dxFont)
			{
				D3D9::dxFont->Release();
				D3DXCreateFontA(pDevice, Data::iNameSize, 0, FW_REGULAR, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Data::strNameFont, &D3D9::dxFont);
			}

			ImGui::SliderInt("Name Distance", &Data::iNameDist, NAME_DIST_MIN, NAME_DIST_MAX);

			ImGui::SliderInt("ESP Name R", &Data::NameColor.r, 0, 255);
			ImGui::SliderInt("ESP Name G", &Data::NameColor.g, 0, 255);
			ImGui::SliderInt("ESP Name B", &Data::NameColor.b, 0, 255);
			ImGui::SliderInt("ESP Name A", &Data::NameColor.a, 0, 255);
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("CROSSHAIR"))
	{
		ImGui::Checkbox("Enable Custom Crosshair", &Data::bCustomCrosshair);
		ImGui::Checkbox("Show Crosshair Settings", &Data::bShowCrosshairSettings);
		if (Data::bShowCrosshairSettings)
		{
			ImGui::SliderInt("Crosshair Thickness", &Data::crosshair.Thickness, 0, Data::wnd.GetWidth());
			ImGui::SliderInt("Crosshair Size", &Data::crosshair.Size, 0, Data::wnd.GetWidth());
			ImGui::SliderInt("Crosshair Gap", &Data::crosshair.Gap, 0, Data::wnd.GetWidth());
			ImGui::SliderInt("Crosshair R", &Data::crosshair.Color.r, 0, 255);
			ImGui::SliderInt("Crosshair G", &Data::crosshair.Color.g, 0, 255);
			ImGui::SliderInt("Crosshair B", &Data::crosshair.Color.b, 0, 255);
			ImGui::SliderInt("Crosshair A", &Data::crosshair.Color.a, 0, 255);
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("CHEATS"))
	{
		ImGui::Checkbox("Bunnyhop", &Data::bBunnyhop);
		ImGui::Checkbox("Triggerbot", &Data::bTriggerbot);
		ImGui::Checkbox("Infinite Health", &Data::bInfiniteHealth);
		ImGui::Checkbox("Infinite Armor", &Data::bInfiniteArmor);
		if (ImGui::Checkbox("Infinite Ammo", &Data::bNoAmmoDecrease)) Hack::HookDecreaseAmmo();
		ImGui::Checkbox("Noclip", &Data::bNoClip);
		if (ImGui::Checkbox("Godmode", &Data::bGodmode)) Hack::HookDecreaseHealth();
		if (ImGui::Checkbox("One Hit Kills", &Data::bOneHitKills)) Hack::HookDecreaseHealth();
		ImGui::Checkbox("Server Cheats", &Data::bServerCheats);
		ImGui::Checkbox("Gravity Changer", &Data::bGravityChanger);
		if (Data::bGravityChanger) ImGui::SliderFloat("Gravity", &Data::flNewGravity, MIN_GRAVITY, MAX_GRAVITY, "%10.f");
		ImGui::Checkbox("Timescale Changer", &Data::bTimescaleChanger);
		if(Data::bTimescaleChanger) ImGui::SliderFloat("Timescale", &Data::flNewTimescale, MIN_TIMESCALE, MAX_TIMESCALE, "%1.f");
		ImGui::Checkbox("FOV Changer", &Data::bFovChanger);
		if(Data::bFovChanger) ImGui::SliderInt("FOV", &Data::iNewFov, MIN_FOV, MAX_FOV);
		ImGui::Checkbox("Teleport", &Data::bTeleport);
		if (Data::bTeleport)
		{
			ImGui::SliderFloat("X", &Data::vTeleportPos.x, MIN_POS, MAX_POS, "%.1f");
			ImGui::SliderFloat("Y", &Data::vTeleportPos.y, MIN_POS, MAX_POS, "%.1f");
			ImGui::SliderFloat("Z", &Data::vTeleportPos.z, MIN_POS, MAX_POS, "%.1f");
			if (ImGui::Button("Force Teleport")) Data::bTeleportPending = true;
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("ABOUT"))
	{
		ImGui::TextColored(COLOR_ORANGE_TXT, "This multihack was made by rdbo");
		ImGui::TextColored(COLOR_BLUE_TXT, "https://github.com/rdbo");
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void Hack::Run(LPDIRECT3DDEVICE9 pDevice)
{
	if (!Data::bInitialized)
	{
		Data::client = Memory::In::GetModuleAddress(CLIENT_MODULE);
		Data::engine = Memory::In::GetModuleAddress(ENGINE_MODULE);
		Data::server = Memory::In::GetModuleAddress(SERVER_MODULE);
		if (!Data::client || !Data::engine || !Data::server) return;
		Data::ForceJump = (DWORD*)(Data::client + HLS::Offsets::Client::dwJump);
		Data::ForceAttack = (DWORD*)(Data::client + HLS::Offsets::Client::dwAttack);
		Data::SvCheats = (DWORD*)(Data::engine + HLS::Offsets::Engine::dwServerCheats);
		Data::Gravity = (float*)(Data::server + HLS::Offsets::Server::flGravity);
		Data::Timescale = (float*)(Data::engine + HLS::Offsets::Engine::flTimescale);
		Data::CheckPlayerState = (bool*)(Data::client + HLS::Offsets::Client::bCheckPlayerState);
		Data::Loading = (bool*)(Data::engine + HLS::Offsets::Engine::bLoading);
		Data::EnableCrosshair = (DWORD*)(Data::client + HLS::Offsets::Client::bEnableCrosshair);
		Data::fDecreaseHealthAddr = (Data::server + HLS::Offsets::Server::fDecreaseHealth);
		Data::fDecreaseAmmoAddr = (Data::server + HLS::Offsets::Server::fDecreaseAmmo);
		Data::fDecreaseAmmoShotgunAddr = (Data::server + HLS::Offsets::Server::fDecreaseAmmoShotgun);
		Data::fDecreaseAmmoOthersAddr = (Data::server + HLS::Offsets::Server::fDecreaseAmmoOthers);
		Data::fReloadIncreaseClipAmmoAddr = (Data::server + HLS::Offsets::Server::fReloadIncreaseClipAmmo);
		Data::fReloadIncreaseClipAmmoShotgunAddr = (Data::server + HLS::Offsets::Server::fReloadIncreaseClipAmmoShotgun);
		Data::fReloadDecreaseAmmo = (Data::server + HLS::Offsets::Server::fReloadDecreaseAmmo);
		DecHealthJumpAddr = Data::fDecreaseHealthAddr + DECREASE_HEALTH_HOOK_LENGTH;
		DecAmmoJumpAddr = Data::fDecreaseAmmoAddr + DECREASE_AMMO_HOOK_LENGTH;
		DecAmmoShotgunJumpAddr = Data::fDecreaseAmmoShotgunAddr + DECREASE_AMMO_SHOTGUN_HOOK_LENGTH;
		DecAmmoOthersJumpAddr = Data::fDecreaseAmmoOthersAddr + DECREASE_AMMO_OTHERS_HOOK_LENGTH;
		ReloadIncClipAmmoJumpAddr = Data::fReloadIncreaseClipAmmoAddr + RELOAD_INCREASE_CLIP_AMMO_HOOK_LENGTH;
		ReloadIncClipAmmoShotgunJumpAddr = Data::fReloadIncreaseClipAmmoShotgunAddr + RELOAD_INCREASE_CLIP_AMMO_SHOTGUN_HOOK_LENGTH;
		ReloadDecAmmoJumpAddr = Data::fReloadDecreaseAmmo + RELOAD_DECREASE_AMMO_HOOK_LENGTH;

		D3DXCreateFontA(pDevice, Data::iNameSize, 0, FW_REGULAR, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Data::strNameFont, &D3D9::dxFont);
		Data::bInitialized = true;
	}

	GetClientRect(D3D9::hWnd, &Data::wnd);
	Data::crosshair.Position.x = Data::wnd.left + Data::wnd.GetWidth() / 2;
	Data::crosshair.Position.y = Data::wnd.top + Data::wnd.GetHeight() / 2;

	if (!*Data::CheckPlayerState || *Data::Loading) return;
	Data::LocalPlayer = *(HLEntity**)(Data::server + HLS::Offsets::Server::dwLocalPlayer);
	if (!Data::LocalPlayer) return;
	Data::ClientData = *(HLClientData**)(Data::client + HLS::Offsets::Client::dwClientData);
	if (!Data::ClientData) return;

	ServerCheatsChecker();
	InfiniteHealth();
	InfiniteArmor();
	Bunnyhop();
	Triggerbot();
	Teleport();
	NoClip();
	FovChanger();
	GravityChanger();
	TimescaleChanger();
	ServerCheats();

	Data::EntityList = (mem_t)(Data::server + HLS::Offsets::Server::dwEntityList);
	Data::vMatrix = *(ViewMatrix*)(Data::engine + HLS::Offsets::Engine::dwViewMatrix);

	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (!*Data::CheckPlayerState || *Data::Loading) break;
		Data::Entity = *(HLEntity**)(Data::EntityList + i * HLS::Offsets::Server::Entity::dwLoopDistance);
		if (*Data::CheckPlayerState && !*Data::Loading && Data::Entity && (mem_t)Data::Entity != (mem_t)Data::LocalPlayer && !Data::Entity->Dormant && Data::Entity->Health > 0 && FilterEntity(Data::Entity))
		{
			iVec2 Ent2DPos;
			if (D3D9::WorldToScreen(Data::Entity->Position, Ent2DPos, Data::vMatrix.Matrix, Data::wnd.GetWidth(), Data::wnd.GetHeight()))
			{
				if (!Data::bEspLimitedRange || (Data::bEspLimitedRange && Data::flEspRange >= D3D9::Get3DDistance(Data::LocalPlayer->Position, Data::Entity->Position)))
				{
					ESP_Snaplines(Data::Entity, Ent2DPos, pDevice, Data::wnd);
					ESP_Name(Data::Entity, Ent2DPos, pDevice);
				}
			}
		}
	}

	DrawCrosshair(Data::crosshair, pDevice);
}

void Hack::DrawCrosshair(Crosshair xhair, LPDIRECT3DDEVICE9 pDevice)
{
	if (Data::bCustomCrosshair)
	{
		if (*Data::EnableCrosshair == 1)
			*Data::EnableCrosshair = 0;
		//Left
		D3D9::DrawLine({ xhair.Position.x - xhair.Gap - xhair.Size, xhair.Position.y }, { xhair.Position.x - xhair.Gap, xhair.Position.y }, xhair.Thickness, xhair.Color.GetColorD3D(), pDevice);
		//Right
		D3D9::DrawLine({ xhair.Position.x + xhair.Gap, xhair.Position.y }, { xhair.Position.x + xhair.Gap + xhair.Size, xhair.Position.y }, xhair.Thickness, xhair.Color.GetColorD3D(), pDevice);
		//Top
		D3D9::DrawLine({ xhair.Position.x, xhair.Position.y - xhair.Gap - xhair.Size }, { xhair.Position.x, xhair.Position.y - xhair.Gap }, xhair.Thickness, xhair.Color.GetColorD3D(), pDevice);
		//Bottom
		D3D9::DrawLine({ xhair.Position.x, xhair.Position.y + xhair.Gap }, { xhair.Position.x, xhair.Position.y + xhair.Gap + xhair.Size }, xhair.Thickness, xhair.Color.GetColorD3D(), pDevice);
	}

	else if(*Data::EnableCrosshair == 0)
		*Data::EnableCrosshair = 1;
}

void Hack::Bunnyhop()
{
	if (Data::bBunnyhop && Data::KeyHook[KEY_BHOP] == WM_KEYDOWN && Data::ClientData && Data::ClientData->Flags & (1 << 0))
	{
		*Data::ForceJump = 6;
	}
}

void Hack::ServerCheats()
{
	if (Data::bServerCheats && !*Data::SvCheats)
	{
		*Data::SvCheats = 1;
	}

	else if (!Data::bServerCheats && *Data::SvCheats)
	{
		*Data::SvCheats = 0;
	}
}

void Hack::Triggerbot()
{
	if (Data::bTriggerbot && Data::ClientData->OnTarget)
	{
		*Data::ForceAttack = STATE_ON;
	}
}

void Hack::NoClip()
{
	if (Data::bNoClip && Data::LocalPlayer->NoClipState != NOCLIP_STATE_ON)
	{
		Data::LocalPlayer->NoClipState = NOCLIP_STATE_ON;
	}

	else if (!Data::bNoClip && Data::LocalPlayer->NoClipState != NOCLIP_STATE_OFF)
	{
		Data::LocalPlayer->NoClipState = NOCLIP_STATE_OFF;
	}
}

void Hack::Teleport()
{
	if (Data::bTeleportPending)
	{
		Data::LocalPlayer->Position.x = Data::vTeleportPos.x;
		Data::LocalPlayer->Position.y = Data::vTeleportPos.y;
		Data::LocalPlayer->Position.z = Data::vTeleportPos.z;
		Data::bTeleportPending = false;
	}

	if(!Data::bShowMenu && !Data::bTeleportPending)
	{
		Data::vTeleportPos.x = Data::LocalPlayer->Position.x;
		Data::vTeleportPos.y = Data::LocalPlayer->Position.y;
		Data::vTeleportPos.z = Data::LocalPlayer->Position.z;
	}
}

void Hack::ServerCheatsChecker()
{
	if (!*Data::SvCheats && Data::bTimescaleChanger)
		Data::bServerCheats = true;
}

void Hack::FovChanger()
{
	if (Data::bFovChanger && Data::ClientData->FieldOfView != Data::iNewFov)
	{
		Data::ClientData->FieldOfView = Data::iNewFov;
	}

	else if(!Data::bFovChanger && Data::ClientData->FieldOfView != DEFAULT_FOV)
	{
		Data::ClientData->FieldOfView = DEFAULT_FOV;
	}
}

void Hack::GravityChanger()
{
	if (Data::bGravityChanger && *Data::Gravity != Data::flNewGravity)
	{
		*Data::Gravity = Data::flNewGravity;
	}

	else if (!Data::bGravityChanger && *Data::Gravity != DEFAULT_GRAVITY)
	{
		*Data::Gravity = DEFAULT_GRAVITY;
	}
}

void Hack::TimescaleChanger()
{
	if (Data::bTimescaleChanger && *Data::Timescale != Data::flNewTimescale)
	{
		*Data::Timescale = Data::flNewTimescale;
	}

	else if (!Data::bTimescaleChanger && *Data::Timescale != DEFAULT_TIMESCALE)
	{
		*Data::Timescale = DEFAULT_TIMESCALE;
	}
}

void Hack::InfiniteHealth()
{
	if (Data::bInfiniteHealth && *Data::CheckPlayerState && Data::LocalPlayer && Data::LocalPlayer->Health < MAX_VALUE)
	{
		Data::LocalPlayer->Health = MAX_VALUE;
	}
}

void Hack::InfiniteArmor()
{
	if (Data::bInfiniteArmor && *Data::CheckPlayerState && Data::LocalPlayer && Data::LocalPlayer->Armor < MAX_VALUE)
	{
		Data::LocalPlayer->Armor = MAX_VALUE;
	}
}

__declspec(naked)
void Hack::hkDecreaseHealth()
{
	__asm
	{
		push esi
		sub esi, HLS::Offsets::Server::Entity::dwHealth //Go to base address
		add esi, HLS::Offsets::Server::Entity::dwEntityTypeId //Get entity type
		cmp [esi], LOCALPLAYER_TYPE_ID //Check If the entity is the localplayer
		jne enemy
		jmp player

		enemy:
		pop esi
		cmp Data::bOneHitKills, 1
		jne originalcode
		mov edi, 0
		jmp originalcode

		player:
		pop esi
		cmp Data::bGodmode, 1
		jne originalcode
		mov edi, MAX_VALUE
		jmp originalcode

		originalcode: //Stolen bytes
		mov[esi], edi //Set Health
		pop edi
		mov eax, 00000001
		jmp DecHealthJumpAddr
	}
}

void Hack::HookDecreaseHealth()
{
	bool bEnable = Data::bGodmode || Data::bOneHitKills;
	if (bEnable && Data::fDecreaseHealthAddr)
		Memory::In::Hook::Detour((byte_t*)Data::fDecreaseHealthAddr, (byte_t*)Hack::hkDecreaseHealth, DECREASE_HEALTH_HOOK_LENGTH);
	else
		Memory::In::Hook::Restore(Data::fDecreaseHealthAddr);
}

void Hack::HookDecreaseAmmo()
{
	if (Data::bNoAmmoDecrease && Data::fDecreaseAmmoAddr && Data::fDecreaseAmmoShotgunAddr && Data::fDecreaseAmmoOthersAddr)
	{
		Memory::In::Hook::Detour((byte_t*)Data::fDecreaseAmmoAddr, (byte_t*)Hack::hkDecreaseAmmo, DECREASE_AMMO_HOOK_LENGTH);
		Memory::In::Hook::Detour((byte_t*)Data::fDecreaseAmmoShotgunAddr, (byte_t*)Hack::hkDecreaseAmmoShotgun, DECREASE_AMMO_SHOTGUN_HOOK_LENGTH);
		Memory::In::Hook::Detour((byte_t*)Data::fDecreaseAmmoOthersAddr, (byte_t*)Hack::hkDecreaseAmmoOthers, DECREASE_AMMO_OTHERS_HOOK_LENGTH);
		Memory::In::Hook::Detour((byte_t*)Data::fReloadIncreaseClipAmmoAddr, (byte_t*)Hack::hkReloadIncreaseClipAmmo, RELOAD_INCREASE_CLIP_AMMO_HOOK_LENGTH);
		Memory::In::Hook::Detour((byte_t*)Data::fReloadIncreaseClipAmmoShotgunAddr, (byte_t*)Hack::hkReloadIncreaseClipAmmoShotgun, RELOAD_INCREASE_CLIP_AMMO_SHOTGUN_HOOK_LENGTH);
		//Memory::In::Hook::Detour((byte_t*)Data::fReloadDecreaseAmmoAddr, (byte_t*)Hack::hkReloadDecreaseAmmo, RELOAD_DECREASE_AMMO_HOOK_LENGTH);
	}

	else
	{
		Memory::In::Hook::Restore(Data::fDecreaseAmmoAddr);
		Memory::In::Hook::Restore(Data::fDecreaseAmmoShotgunAddr);
		Memory::In::Hook::Restore(Data::fDecreaseAmmoOthersAddr);
		Memory::In::Hook::Restore(Data::fReloadIncreaseClipAmmoAddr);
		Memory::In::Hook::Restore(Data::fReloadIncreaseClipAmmoShotgunAddr);
		//Memory::In::Hook::Restore(Data::fReloadDecreaseAmmoAddr);
	}
}

__declspec(naked)
void Hack::hkDecreaseAmmo()
{
	__asm
	{
		cmp ebx, MAX_VALUE
		je exit
		mov ebx, MAX_VALUE
		jmp originalcode

		originalcode:
		mov[esi], ebx
		mov eax, edi
		pop edi

		exit:
		jmp DecAmmoJumpAddr
	}
}

__declspec(naked)
void Hack::hkDecreaseAmmoShotgun()
{
	__asm
	{
		mov edi, MAX_VALUE
		jmp originalcode

		originalcode:
		mov[esi], edi
		pop edi
		mov eax, esi

		exit:
		jmp DecAmmoShotgunJumpAddr
	}
}

__declspec(naked)
void Hack::hkDecreaseAmmoOthers()
{
	__asm
	{
		mov ebx, MAX_VALUE

		originalcode:
		mov[esi], ebx
		pop edi
		pop esi
		pop ebx

		exit:
		jmp DecAmmoOthersJumpAddr
	}
}

__declspec(naked)
void Hack::hkReloadIncreaseClipAmmo()
{
	__asm
	{
		mov edi, MAX_VALUE
		jmp originalcode

		originalcode:
		mov[esi], edi
		pop edi
		mov eax, esi

		exit:
		jmp ReloadIncClipAmmoJumpAddr;
	}
}

__declspec(naked)
void Hack::hkReloadIncreaseClipAmmoShotgun()
{
	__asm
	{
		mov eax, MAX_VALUE
		jmp originalcode

		originalcode:
		//mov[ebx + 000004AC], eax
		push ebx
		add ebx, 0x4AC
		mov [ebx], eax
		pop ebx

		exit:
		jmp ReloadIncClipAmmoShotgunJumpAddr
	}
}

__declspec(naked)
void Hack::hkReloadDecreaseAmmo()
{
	__asm
	{
		mov ebx, MAX_VALUE
		jmp originalcode

		originalcode:
		mov[esi], ebx
		pop edi
		pop esi
		pop ebx

		exit:
		jmp ReloadDecAmmoJumpAddr
	}
}

bool Hack::FilterEntity(HLEntity* ent)
{
	if (!Data::bEnableEntityFilter) return true;
	size_t size = Data::EntityStringFilter.size();
	bool check = true;
	check &= Data::Entity->TypeId != 0;
	for (size_t i = 0; i < size; i++)
	{
		if (!check) break;
		char* blstr = Data::EntityStringFilter.at(i);
		check &= !!strcmp(blstr, ent->EntityTypeStr);
	}

	return check;
}

void Hack::ESP_Snaplines(HLEntity* ent, iVec2 Ent2DPos, LPDIRECT3DDEVICE9 pDevice, Window wnd)
{
	if (Data::bEspSnaplines)
	{
		int wWidth = wnd.GetWidth();
		int wHeight = wnd.GetHeight();
		iVec2 screenCoords = { wWidth / 2, wHeight };

		D3D9::DrawLine(screenCoords, Ent2DPos, Data::iSnaplineThickness, Data::SnaplineColor.GetColorD3D(), pDevice);
	}
}

void Hack::ESP_Name(HLEntity* ent, iVec2 Ent2DPos, LPDIRECT3DDEVICE9 pDevice)
{
	if (Data::bEspName && D3D9::dxFont)
	{
		std::string str = ent->EntityTypeStr;

		if (Data::bEspDist)
		{
			float dist = D3D9::Get3DDistance(Data::LocalPlayer->Position, ent->Position);
			char dist_buf[128];
			snprintf(dist_buf, sizeof(dist_buf), "%.2f", dist);

			str += "[";
			str += dist_buf;
			str += "M";
			str += "]";
		}

		D3D9::DrawString(str.c_str(), Ent2DPos.x, Ent2DPos.y, Data::iNameDist, Data::NameColor.GetColorD3D(), D3D9::dxFont);
	}
}