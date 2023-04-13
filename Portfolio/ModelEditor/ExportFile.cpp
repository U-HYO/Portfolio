#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"

void ExportFile::Init()
{
	Tank();
	Tower();
	Airplane();
	Kachujin();
	Weapons();
	Paladin();
	//Miki();
}

void ExportFile::Release()
{
}

void ExportFile::Tank()
{
	Converter* conv = Debug_new Converter();

	conv->ReadFile(L"Tank/Tank.fbx");
	conv->ExportMesh(L"Tank/Tank");
	conv->ExportMaterial(L"Tank/Tank");
	SAFE_DELETE(conv);
}

void ExportFile::Tower()
{
	Converter* conv = Debug_new Converter();
	conv->ReadFile(L"Tower/Tower.fbx");
	conv->ExportMesh(L"Tower/Tower");
	conv->ExportMaterial(L"Tower/Tower");
	SAFE_DELETE(conv);
}

void ExportFile::Airplane()
{
	Converter* conv = Debug_new Converter();
	conv->ReadFile(L"B787/Airplane.fbx");
	conv->ExportMesh(L"B787/Airplane");
	conv->ExportMaterial(L"B787/Airplane");
	SAFE_DELETE(conv);
}

void ExportFile::Kachujin()
{
	Converter* conv = Debug_new Converter();

	conv->ReadFile(L"Kachujin/Kachujin.fbx");
	conv->ExportMesh(L"Kachujin/Kachujin");
	conv->ExportMaterial(L"Kachujin/Kachujin");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Idle.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Idle");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Walk.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Walk");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Run.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Run");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Kachujin/Sword And Shield Slash.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Slash");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Kachujin/Bboy Uprock.fbx");
	conv->ExportAnimClip(0, L"Kachujin/Uprock");
	SAFE_DELETE(conv);
}

void ExportFile::Weapons()
{
	std::vector<std::wstring> names;
	names.push_back(L"Cutter.fbx");
	names.push_back(L"Cutter2.fbx");
	names.push_back(L"Dagger_epic.fbx");
	names.push_back(L"Dagger_small.fbx");
	names.push_back(L"Katana.fbx");
	names.push_back(L"LongArrow.obj");
	names.push_back(L"LongBow.obj");
	names.push_back(L"Rapier.fbx");
	names.push_back(L"Sword.fbx");
	names.push_back(L"Sword2.fbx");
	names.push_back(L"Sword_epic.fbx");

	for (std::wstring name : names)
	{
		Converter* conv = Debug_new Converter();
		conv->ReadFile(L"Weapon/" + name);


		String::Replace(&name, L".fbx", L"");
		String::Replace(&name, L".obj", L"");

		conv->ExportMesh(L"Weapon/" + name);
		conv->ExportMaterial(L"Weapon/" + name);

		SAFE_DELETE(conv);
	}
}

void ExportFile::Paladin()
{
	Converter* conv = Debug_new Converter();

	conv->ReadFile(L"Paladin/Paladin.fbx");
	conv->ExportMesh(L"Paladin/Paladin");
	conv->ExportMaterial(L"Paladin/Paladin");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/BackWalk.fbx");
	conv->ExportAnimClip(0, L"Paladin/BackWalk");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Block.fbx");
	conv->ExportAnimClip(0, L"Paladin/Block");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Block_Idle.fbx");
	conv->ExportAnimClip(0, L"Paladin/Block_Idle");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Idle.fbx");
	conv->ExportAnimClip(0, L"Paladin/Idle");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Idle2.fbx");
	conv->ExportAnimClip(0, L"Paladin/Idle2");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Rolling.fbx");
	conv->ExportAnimClip(0, L"Paladin/Rolling");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Run.fbx");
	conv->ExportAnimClip(0, L"Paladin/Run");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/SideWalk.fbx");
	conv->ExportAnimClip(0, L"Paladin/SideWalk");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Turn.fbx");
	conv->ExportAnimClip(0, L"Paladin/Turn");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/UnBlock.fbx");
	conv->ExportAnimClip(0, L"Paladin/UnBlock");
	SAFE_DELETE(conv);

	conv = Debug_new Converter();
	conv->ReadFile(L"Paladin/Walk.fbx");
	conv->ExportAnimClip(0, L"Paladin/Walk");
	SAFE_DELETE(conv);


}

void ExportFile::Miki()
{
	Converter* conv = Debug_new Converter();

	conv->ReadFile(L"Miki/Miki.pmx");
	conv->ExportMesh(L"Miki/Miki");
	conv->ExportMaterial(L"Miki/Miki");
	SAFE_DELETE(conv);
}
