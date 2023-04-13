#include "stdafx.h"
#include "MapEditor.h"
#include "Enviroment/Brush.h"
#include "Utilities/Path.h"

void MapEditor::Init()
{
	heightMapFileDirectory = L"Terrain/";
	heightMapFileName = L"Gray2048.png";

	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetRotationDegree(20, 0, 0);
	Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA)->GetTransform()->SetPosition(126, 35, 39);
	((FreeCamera*)Context::Get()->GetCamera((UINT)CameraMode::FREE_CAMERA))->Speed(40, 2);
	    
	shader = Debug_new Shader(L"62_Water.fxo");
	shadow = Debug_new Shadow(shader, Vector3(128, 0, 128), 65);
	
	sky = Debug_new CubeSky(L"Environment/GrassCube1024.dds", shader);
	terrain = Debug_new Terrain(shader, L"../../_Textures/Terrain/Gray256.dds");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->Layer1(L"Terrain/Dirt.png");
	//brush = Debug_new Brush(shader, terrain);
	terrain->brush = Debug_new Brush(shader, terrain);
	UpdateDataMapFileList();
}

void MapEditor::Release()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(shadow);
	SAFE_DELETE(sky);
	//SAFE_DELETE(brush);
	SAFE_DELETE(terrain);
}

void MapEditor::Update()
{
	const float DISTANCE = 10.0f;
	static int corner = 3;
	ImGuiIO& io = ImGui::GetIO();
	if (corner != -1)
	{
		auto viewport = Context::Get()->GetViewport();
		//ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 window_pos =
			ImVec2((corner & 1) ? (viewport->GetPos().x + viewport->GetSize().x - DISTANCE) : (viewport->GetPos().x + DISTANCE),
			(corner & 2) ? (viewport->GetPos().y + viewport->GetSize().y - DISTANCE) : (viewport->GetPos().y + DISTANCE));
		ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		//ImGui::SetNextWindowViewport(viewport->ID);
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("System Info", NULL, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		std::string str = "";

		str = std::string("FrameRate : ") + std::to_string(ImGui::GetIO().Framerate);
		ImGui::Text(str.c_str());

		ImGui::Separator();
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			//if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();

	ImGui::Separator();

	/*std::string name = String::ToString(heightMapFileName);
	char* temp = Debug_new char[name.size() + 2];
	strcpy_s(temp, name.size() + 1, name.c_str());

	ImGui::InputText("HeightMap", temp, strlen(temp) + 1);*/

	std::string name = String::ToString(heightMapFileName);
	static char buf[32];
	strcpy_s(buf, name.size() + 1, name.c_str());
	ImGui::InputText("HeightMap", buf, IM_ARRAYSIZE(buf));
	heightMapFileName = String::ToWString(buf);

	//heightMapFileName = String::ToWString(temp);
	//delete[] temp;

	if (ImGui::Button("Open HeightMap") == true)
	{
		if (heightMapFileName.length() > 0)
		{
			std::wstring fileName = L"../../_Textures/" + heightMapFileDirectory + heightMapFileName;
			if (Path::ExistFile(fileName))
			{
				SAFE_DELETE(heightMapFile);
				heightMapFile = Debug_new Texture(heightMapFileDirectory + heightMapFileName);
			}
		}
	}
	

	const char* items[] = { "Red", "Green", "Blue", "Alpha" };
	ImGui::Combo("Channel", &selectedChannel, items, 4);

	if (ImGui::Button("Convert Map File"))
	{
		ToDataMapFile();

		UpdateDataMapFileList();
	}


	if (heightMapFile != NULL)
		ImGui::Image(heightMapFile->SRV(), ImVec2(128, 128));

	ImGui::Separator();
	if (ImGui::CollapsingHeader("Data Map List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < dataMapFileList.size(); i++)
		{
			if (ImGui::Button(String::ToString(dataMapFileList[i]).c_str(), ImVec2(200, 0)))
			{
				//SAFE_DELETE(brush);
				SAFE_DELETE(terrain);

				std::wstring path = dataMapFileList[i] + L".dds";
				terrain = Debug_new Terrain(shader, path);
				terrain->BaseMap(L"Terrain/Dirt3.png");
				terrain->Layer1(L"Terrain/Dirt.png");

				//brush = Debug_new Brush(shader, terrain);
			}
		}//for(i)
	}//ImGui::CollapsingHeader

	ImGui::Separator();

	if(ImGui::Button("Save to File"))
	{
		terrain->SaveHeightMap();
		UpdateDataMapFileList();
	}
	if (ImGui::Button("Load from File"))
	{
		FileSystem::OpenFileDialog([=](std::string path)
		{
			//SAFE_DELETE(brush);
			SAFE_DELETE(terrain);

			terrain = Debug_new Terrain(shader, FileSystem::ToWString(path), true);
		});
		terrain->BaseMap(L"Terrain/Dirt3.png");
		terrain->Layer1(L"Terrain/Dirt.png");
		//brush = Debug_new Brush(shader, terrain);
	}
	

	sky->Update();

	if (terrain != NULL)
	{
		terrain->Update();
		//brush->Update();
		//terrain->brush->Update();
	}

}

void MapEditor::PreRender()
{
	terrain->Pass(6);
	terrain->Render();

}

void MapEditor::Render()
{
	sky->Pass(4);
	sky->Render();

	if (terrain != NULL)
	{
		//brush->Render();
		terrain->brush->Render();
		terrain->Pass(6);
		terrain->Render();
	}
}

void MapEditor::ToDataMapFile()
{
	if (heightMapFile == NULL) return;

	ID3D11Texture2D* srcTexture = heightMapFile->GetTexture();
	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);


	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, NULL, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);

	UINT* pixels = Debug_new UINT[readDesc.Width * readDesc.Height];
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);


	UINT* heights = Debug_new UINT[readDesc.Width * readDesc.Height];
	for (UINT i = 0; i < readDesc.Width * readDesc.Height; i++)
	{
		UINT pixel = pixels[i];
		UINT result = 0;

		Channel channel = (Channel)selectedChannel;

		//UINT r = (0x000000FF & pixel) >> 0;
		//UINT g = (0x0000FF00 & pixel) >> 8;
		//UINT b = (0x00FF0000 & pixel) >> 16;
		//UINT a = (0xFF000000 & pixel) >> 24;

		switch (channel)
		{
			case Channel::Alpha: result = (0xFF000000 & pixel) >> 24; break;
			case Channel::Blue: result = (0x00FF0000 & pixel) >> 16; break;
			case Channel::Green: result = (0x0000FF00 & pixel) >> 8; break;
			case Channel::Red: result = (0x000000FF & pixel) >> 0; break;
		}

		heights[i] = result << 24;
	}


	ID3D11Texture2D* saveTexture;
	D3D11_TEXTURE2D_DESC saveDesc;
	ZeroMemory(&saveDesc, sizeof(D3D11_TEXTURE2D_DESC));
	saveDesc.Width = readDesc.Width;
	saveDesc.Height = readDesc.Height;
	saveDesc.ArraySize = 1;
	saveDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	saveDesc.MipLevels = 1;
	saveDesc.SampleDesc = readDesc.SampleDesc;
	saveDesc.Usage = D3D11_USAGE_STAGING;
	saveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA saveSubResource;
	saveSubResource.pSysMem = heights;
	saveSubResource.SysMemPitch = sizeof(UINT) * readDesc.Width;
	saveSubResource.SysMemSlicePitch = sizeof(UINT) * readDesc.Width * readDesc.Height;

	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, &saveSubResource, &saveTexture));


	std::wstring fileName = Path::GetFileNameWithoutExtension(heightMapFile->GetFile());
	fileName = L"../../_Textures/Terrain/" + fileName + L".dds";

	Check(D3DX11SaveTextureToFile(D3D::GetDC(), saveTexture, D3DX11_IFF_DDS, fileName.c_str()));

	SAFE_RELEASE(srcTexture);
	SAFE_RELEASE(readTexture);
	SAFE_RELEASE(saveTexture);
}

void MapEditor::UpdateDataMapFileList()
{
	dataMapFileList.clear();
	Path::GetFiles(&dataMapFileList, L"../../_Textures/Terrain/", L"*.dds", false);

	for (std::wstring& file : dataMapFileList)
		file = Path::GetFileNameWithoutExtension(file);
}
