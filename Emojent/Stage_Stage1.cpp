#include "stdafx.h"
#include "Stage_Stage1.h"


void PhysicsElements::Stage_Stage1::init()
{
	// シーン関連
	stage_num = 3;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Yellow.lerp(Palette::Black, 0.5);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// goal->set_score_time(Score::stage1);

	create_stage();
}

void PhysicsElements::Stage_Stage1::create_stage()
{
	// 重力
	world->setGravity(Vec2{ 0, 980 });
	P2BodyID id;

	// 左壁
	wall->addRect(Vec2(0, -32), Vec2(32, 800));

	// 右壁
	wall->addRect(Vec2(1248, -32), Vec2(32, 800));
	wall->addRect(Vec2(1184, 32), Vec2(64, 352));

	// 天井
	wall->addRect(Vec2(0, -32), Vec2(1280, 64));

	// 地面
	wall->addRect(Vec2(0, 672), Vec2(576, 64));
	// wall->addRect(Vec2(1088, 640), Vec2(160, 96));

	// 高台
	wall->addRect(Vec2(32, 320), Vec2(128, 96));
	wall->addTriangle(Vec2(32, 416), Vec2(0, 0), Vec2(0, 128), Vec2(128, 0));
	// 高台に戻るためのプラットフォーム
	id = wall->addRect(Vec2(160, 416), Vec2(96, 4));
	wall->change_filter(id, WallFilterType::Platform);
	id = wall->addRect(Vec2(160, 544), Vec2(96, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 上に伸びる柱
	wall->addRect(Vec2(320, 32), Vec2(64, 128));
	wall->addRect(Vec2(736, 32), Vec2(64, 128));
	wall->addRect(Vec2(800, 128), Vec2(32, 32));

	// 危険な足場（トグル）
	id = wall->addRect(Vec2(576, 384), Vec2(672, 32));
	wall->change_filter(id, WallFilterType::Dangerous);
	gun_targets->add_gun_target(id, Vec2(1086, 96), WallFilterType::Normal, WallFilterType::Dangerous);
	// wall_toggle_dangerous.emplace_back(id);

	// プラットフォーム
	id = wall->addRect(Vec2(992, 320), Vec2(256, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 床側の足場
	wall->addRect(Vec2(416, 640), Vec2(160, 32));
	wall->addTriangle(Vec2(384, 640), Vec2(32, 0), Vec2(0, 32), Vec2(32, 32));

	// 錠前壁（トグル）
	id = wall->addRect(Vec2(1120, 416), Vec2(32, 160));
	items->item_add(id, WallFilterType::Ignore, Vec2(1088, 256), U"emoji_key");
	// wall_toggle_ignore.emplace_back(id);

	// ゴール足場
	wall->addRect(Vec2(1120, 576), Vec2(160, 160));

	// プレイヤースタート地点
	start_pos = Vec2(96, 298);
	player->reset(start_pos);

	// ゴール
	goal->set_pos(Vec2(1200, 551));

	// スタート
	stopwatch.start();
}

void PhysicsElements::Stage_Stage1::draw_mid_layer() const
{

}

void PhysicsElements::Stage_Stage1::change_next_scene()
{
	changeScene(getData().stage_num+1, 500);
	++getData().stage_num;
}

PhysicsElements::Stage_Stage1::Stage_Stage1(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Stage1::Update()
{
	update_pipeline();

}

void PhysicsElements::Stage_Stage1::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Stage1::update()
//{
//}
//
//void PhysicsElements::Stage_Stage1::draw() const
//{
//}
