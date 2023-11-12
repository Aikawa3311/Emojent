#include "stdafx.h"
#include "Stage_Stage3.h"


void PhysicsElements::Stage_Stage3::init()
{
	// シーン関連
	stage_num = 5;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Mediumaquamarine.lerp(Palette::Black, 0.5);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// goal->set_score_time(Score::stage1);

	create_stage();
}

void PhysicsElements::Stage_Stage3::create_stage()
{
	// 重力
	world->setGravity(Vec2{ 0, 980 });
	P2BodyID id;

	// 左壁
	wall->addRect(Vec2(0, -32), Vec2(32, 800));

	// 右壁
	wall->addRect(Vec2(1248, -32), Vec2(32, 800));

	// 天井
	wall->addRect(Vec2(0, -32), Vec2(1280, 64));
	// 隅
	wall->addTriangle(Vec2(32, 32), Vec2(0, 0), Vec2(0, 96), Vec2(96, 0));

	// 地面
	// wall->addRect(Vec2(0, 704), Vec2(672, 32));
	// 危険な地面
	id = wall->addRect(Vec2(192, 672), Vec2(480, 64));
	wall->change_filter(id, WallFilterType::Dangerous);
	id = wall->addRect(Vec2(704, 672), Vec2(544, 64));
	wall->change_filter(id, WallFilterType::Dangerous);

	// 高台
	wall->addRect(Vec2(32, 320), Vec2(160, 32));
	// 高台の下の地面、ゴール
	wall->addRect(Vec2(32, 576), Vec2(160, 160));

	// 上から伸びる柱
	wall->addRect(Vec2(416, 32), Vec2(32, 352));
	// 円
	wall->addCircle(Vec2(432, 416), 16);
	// 隅
	wall->addTriangle(Vec2(416, 32), Vec2(0, 0), Vec2(-96, 0), Vec2(0, 96));
	wall->addTriangle(Vec2(448, 32), Vec2(0, 0), Vec2(0, 96), Vec2(96, 0));

	// 下に伸びる柱
	wall->addRect(Vec2(672, 416), Vec2(32, 416));
	// 円
	wall->addCircle(Vec2(688, 384), 16);
	// 隅
	id = wall->addTriangle(Vec2(672, 672), Vec2(0, 0), Vec2(0, -96), Vec2(-96, 0));
	wall->change_filter(id, WallFilterType::Dangerous);
	id = wall->addTriangle(Vec2(704, 672), Vec2(0, 0), Vec2(96, 0), Vec2(0, -96));
	wall->change_filter(id, WallFilterType::Dangerous);

	// 上から伸びる柱
	wall->addRect(Vec2(960, 32), Vec2(32, 352));
	// 円
	wall->addCircle(Vec2(976, 416), 16);
	// 隅
	wall->addTriangle(Vec2(960, 32), Vec2(0, 0), Vec2(-96, 0), Vec2(0, 96));
	wall->addTriangle(Vec2(992, 32), Vec2(0, 0), Vec2(0, 96), Vec2(96, 0));

	// 左下・右下・右上隅
	id = wall->addTriangle(Vec2(192, 672), Vec2(0, 0), Vec2(96, 0), Vec2(0, -96));
	wall->change_filter(id, WallFilterType::Dangerous);
	id = wall->addTriangle(Vec2(1248, 672), Vec2(0, 0), Vec2(0, -96), Vec2(-96, 0));
	wall->change_filter(id, WallFilterType::Dangerous);
	wall->addTriangle(Vec2(1248, 32), Vec2(0, 0), Vec2(-96, 0), Vec2(0, 96));

	// 施錠壁（トグル）
	id = wall->addRect(Vec2(160, 352), Vec2(32, 224));
	gun_targets->add_gun_target(id, Vec2(1120, 128), WallFilterType::Ignore, WallFilterType::Normal);
	// wall_toggle_dangerous.emplace_back(id);

	// プレイヤースタート地点
	start_pos = Vec2(96, 295);
	player->reset(start_pos);

	// ゴール
	goal->set_pos(Vec2(96, 551));

	// スタート
	stopwatch.start();
}

void PhysicsElements::Stage_Stage3::draw_mid_layer() const
{

}

void PhysicsElements::Stage_Stage3::change_next_scene()
{
}

PhysicsElements::Stage_Stage3::Stage_Stage3(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Stage3::Update()
{
	update_pipeline();
	Print << stage_col1;
	Print << stage_col2;
}

void PhysicsElements::Stage_Stage3::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Stage3::update()
//{
//}
//
//void PhysicsElements::Stage_Stage3::draw() const
//{
//}
