#include "stdafx.h"
#include "Stage_Stage2.h"


void PhysicsElements::Stage_Stage2::init()
{
	// シーン関連
	stage_scene_state = SceneState::Stage_Stage2;
	stage_num = 4;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Purple.lerp(Palette::Gray, 0.5);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// goal->set_score_time(Score::stage1);

	create_stage();
}

void PhysicsElements::Stage_Stage2::create_stage()
{
	// 重力
	world->setGravity(Vec2{ 0, 980 });
	P2BodyID id;

	// 左壁
	wall->addRect(Vec2(0, -32), Vec2(32, 800));

	// 右壁
	wall->addRect(Vec2(1248, -32), Vec2(32, 800));
	// wall->addRect(Vec2(1088, 32), Vec2(196, 160));
	wall->addTriangle(Vec2(1088, 32), Vec2(0, 0), Vec2(196, 160), Vec2(196, 0));

	// 天井
	wall->addRect(Vec2(0, -32), Vec2(1280, 96));

	// 地面
	wall->addRect(Vec2(0, 672), Vec2(672, 64));

	// スタート地点の天井
	// wall->addRect(Vec2(0, 0), Vec2(288, 288));
	wall->addRect(Vec2(32, 256), Vec2(384, 32));
	wall->addRect(Vec2(32, 192), Vec2(480, 64));
	wall->addRect(Vec2(32, 64), Vec2(96, 128));
	// 上から伸びる柱
	wall->addRect(Vec2(352, 288), Vec2(32, 160));
	wall->addCircle(Vec2(368, 448), 16);

	// 坂
	wall->addTriangle(Vec2(384, 256), Vec2(0, 0), Vec2(0, 192), Vec2(128, 0));
	// wall->addTriangle(Vec2(448, 672), Vec2(0, 0), Vec2(128, 0), Vec2(128, -192));
	wall->addTriangle(Vec2(512, 576), Vec2(0, 0), Vec2(64, 0), Vec2(64, -96));

	// 危険な床（トグル）
	id = wall->addRect(Vec2(576, 480), Vec2(672, 64));
	wall->change_filter(id, WallFilterType::Dangerous);
	gun_targets->add_gun_target(id, Vec2(192, 128), WallFilterType::Normal, WallFilterType::Dangerous);
	wall_toggle_dangerous.emplace_back(id);

	// 危険な床の下の壁
	wall->addRect(Vec2(672, 576), Vec2(672, 160));
	wall->addRect(Vec2(576, 544), Vec2(672, 32));

	// 円
	wall->addCircle(Vec2(896, 196), 16);

	// 右下の屋根
	wall->addRect(Vec2(1088, 352), Vec2(32, 32));
	id = wall->addRect(Vec2(1120, 352), Vec2(128, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 施錠壁（トグル）
	id = wall->addRect(Vec2(512, 576), Vec2(32, 96));
	items->item_add(id, WallFilterType::Ignore, Vec2(1180, 430), U"emoji_key");
	wall_toggle_ignore.emplace_back(id);
	
	// プレイヤースタート地点
	start_pos = Vec2(160, 647);
	player->reset(start_pos);

	// ゴール
	goal->set_pos(Vec2(610, 647));

	// スタート
	stopwatch.start();
}

void PhysicsElements::Stage_Stage2::draw_mid_layer() const
{

}

void PhysicsElements::Stage_Stage2::change_next_scene()
{
	changeScene(SceneState::Stage_Stage3, 500);
}

PhysicsElements::Stage_Stage2::Stage_Stage2(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Stage2::Update()
{
	update_pipeline();

}

void PhysicsElements::Stage_Stage2::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Stage2::update()
//{
//}
//
//void PhysicsElements::Stage_Stage2::draw() const
//{
//}
