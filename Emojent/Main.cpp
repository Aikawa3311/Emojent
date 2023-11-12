# include <Siv3D.hpp>
# include "AssetRegister.h"
# include "GlobalSetting.h"
# include "Stage.h"
#include "StageTest.h"
#include "StageSelect.h"
#include "Stage_Demo.h"

void Main()
{
	Window::Resize(1280, 720);

	// アセットの読み込み
	AssetRegister::Regist();

	Window::SetTitle(U"Emojent");

	// 2D 物理演算のワールド
	// P2World world;

	// シーンマネージャー
	App scene_manager;
	// scene_manager.add<PhysicsElements::Stage_Tutorial1>(SceneState::Stage_Tutorial1);
	// scene_manager.add<PhysicsElements::Stage_Tutorial2>(SceneState::Stage_Tutorial2);
	// scene_manager.add<PhysicsElements::Stage_Tutorial3>(SceneState::Stage_Tutorial3);
	// scene_manager.add<PhysicsElements::Stage_Stage1>(SceneState::Stage_Stage1);
	// scene_manager.add<PhysicsElements::Stage_Stage2>(SceneState::Stage_Stage2);
	// scene_manager.add<PhysicsElements::Stage_Stage3>(SceneState::Stage_Stage3);
	// scene_manager.add<PhysicsElements::Stage_Demo>(SceneState::Stage_Demo);
	// scene_manager.add<StageSelect>(SceneState::StageSelect);
	// scene_manager.add<PhysicsElements::Stage_Tutorial1>(Global::stage_scene_id_offset + 0);
	// scene_manager.add<PhysicsElements::Stage_Tutorial2>(Global::stage_scene_id_offset + 1);
	// scene_manager.add<PhysicsElements::Stage_Tutorial3>(Global::stage_scene_id_offset + 2);
	// scene_manager.add<PhysicsElements::Stage_Stage1>(Global::stage_scene_id_offset + 3);
	// scene_manager.add<PhysicsElements::Stage_Stage2>(Global::stage_scene_id_offset + 4);
	// scene_manager.add<PhysicsElements::Stage_Stage3>(Global::stage_scene_id_offset + 5);

	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 0);
	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 1);
	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 2);
	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 3);
	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 4);
	scene_manager.add<PhysicsElements::Stage>(Global::stage_scene_id_offset + 5);
	scene_manager.add<StageSelect>(Global::stage_select_scene_id);

	scene_manager.get()->stage_num = Global::stage_scene_id_offset + 0;
	scene_manager.init(Global::stage_scene_id_offset + 0, 0.0s);

	// scene_manager.init(SceneState::Stage_Demo, 0.0s);
	// scene_manager.changeScene(SceneState::StageTest, 0.0s);

	// BGM
	/*AudioAsset(U"bgm").setLoop(true);
	AudioAsset(U"bgm").setVolume(0.2);
	AudioAsset(U"bgm").play();*/
	AudioAsset(U"bgm").setLoop(true);
	AudioAsset(U"bgm").setVolume(0.4);
	AudioAsset(U"bgm").play();

	// Font font(30, U"font/MangabeyRegular.otf");

	while (System::Update())
	{
		ClearPrint();
		// Print << U"{}"_fmt(Profiler::FPS());
		// Print << Cursor::Pos();
		/*Print << Geometry2D::IsClockwise(v0, v1, v2);
		Print << Geometry2D::IsClockwise(v3, v1, v2);*/

		// FontAsset(U"Bold40")(U"{}"_fmt(Profiler::FPS())).draw(Point(10, 10), Palette::White);

		if (not scene_manager.update()) {
			break;
		}

		// Scene::Rect().draw(Palette::White);
		// font(U"RANK").draw(Arg::center = Vec2(400, 290), { Palette::Black, 1.0 });
	}
}

