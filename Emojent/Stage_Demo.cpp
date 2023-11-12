#include "stdafx.h"
#include "Stage_Demo.h"


void PhysicsElements::Stage_Demo::init()
{
	// シーン関連
	goal->set_stage_num(getData().stage_num + Global::stage_scene_id_offset);

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

void PhysicsElements::Stage_Demo::create_stage()
{
	load_stage_toml(U"stage/stage_1.toml");

	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// スタート
	stopwatch.start();
}

void PhysicsElements::Stage_Demo::change_next_scene()
{
}

PhysicsElements::Stage_Demo::Stage_Demo(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Demo::Update()
{
	update_pipeline();
}

void PhysicsElements::Stage_Demo::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Demo::update()
//{
//}
//
//void PhysicsElements::Stage_Demo::draw() const
//{
//}
