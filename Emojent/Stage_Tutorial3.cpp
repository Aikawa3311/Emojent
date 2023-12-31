﻿#include "stdafx.h"
#include "Stage_Tutorial3.h"


void PhysicsElements::Stage_Tutorial3::init()
{
	// シーン関連
	// stage_scene_state = SceneState::Stage_Tutorial3;
	stage_num = 2;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Blue.lerp(Palette::Gray, 0.6);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	create_stage();
}

void PhysicsElements::Stage_Tutorial3::create_stage()
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
	wall->addRect(Vec2(0, 32), Vec2(576, 64));

	// 高台
	wall->addRect(Vec2(32, 480), Vec2(128, 64));
	wall->addRect(Vec2(32, 544), Vec2(32, 288));
	// wall->addRect(Vec2(160, 544), Vec2(64, 288));
	wall->addTriangle(Vec2(160, 480), Vec2(0, 0), Vec2(0, 64), Vec2(64, 64));

	// 四角
	id = wall->addRect(Vec2(320, 256), Vec2(32, 32));
	wall->change_filter(id, WallFilterType::Dangerous);
	// wall->addTriangle(Vec2(320, 256), Vec2(32, 0), Vec2(0, 32), Vec2(32, 32));
	// wall->addTriangle(Vec2(320, 288), Vec2(0, 0), Vec2(32, 32), Vec2(32, 0));
	// 上に伸びる柱
	wall->addRect(Vec2(352, 192), Vec2(32, 128));
	wall->addRect(Vec2(576, 32), Vec2(32, 288));

	// プラットフォーム
	id = wall->addRect(Vec2(384, 320), Vec2(192, 4));
	wall->change_filter(id, WallFilterType::Platform);
	id = wall->addRect(Vec2(384, 192), Vec2(192, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 錠前壁（トグル）
	id = wall->addRect(Vec2(576, 320), Vec2(32, 256));
	items->item_add(id, WallFilterType::Ignore, Vec2(480, 256), U"emoji_key");
	// wall_toggle_ignore.emplace_back(id);

	// 地面
	wall->addRect(Vec2(576, 576), Vec2(128, 160));
	// id = wall->addRect(Vec2(704, 640), Vec2(384, 4));
	// wall->change_filter(id, WallFilterType::Platform);
	wall->addRect(Vec2(0, 704), Vec2(1280, 32));
	wall->addRect(Vec2(0, 640), Vec2(1280, 64));
	// wall->addRect(Vec2(1088, 640), Vec2(160, 96));

	// 低め天井
	wall->addRect(Vec2(608, 32), Vec2(160, 128));

	// 丸
	id = wall->addCircle(Vec2(928, 384), 24);
	wall->change_filter(id, WallFilterType::Dangerous);

	// 丸の上に伸びる柱
	wall->addRect(Vec2(960, 160), Vec2(32, 192));

	// プラットフォーム
	id = wall->addRect(Vec2(992, 256), Vec2(256, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 錠前壁（トグル）
	id = wall->addRect(Vec2(192, 544), Vec2(32, 96));
	items->item_add(id, WallFilterType::Ignore, Vec2(1120, 160), U"emoji_key");
	// wall_toggle_ignore.emplace_back(id);

	// プレイヤースタート地点
	start_pos = Vec2(96, 455);
	player->reset(start_pos);

	// ゴール
	goal->set_pos(Vec2(96, 615));

	// スタート
	stopwatch.start();
}

void PhysicsElements::Stage_Tutorial3::change_next_scene()
{
	changeScene(getData().stage_num + 1, 500);
	++getData().stage_num;
}

PhysicsElements::Stage_Tutorial3::Stage_Tutorial3(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Tutorial3::Update()
{
	update_pipeline();

}

void PhysicsElements::Stage_Tutorial3::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Tutorial3::update()
//{
//}
//
//void PhysicsElements::Stage_Tutorial3::draw() const
//{
//}
