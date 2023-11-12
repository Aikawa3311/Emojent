#include "stdafx.h"
#include "Stage_Tutorial2.h"


void PhysicsElements::Stage_Tutorial2::init()
{
	// シーン関連
	stage_num = 1;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Deeppink.lerp(Palette::Gray, 0.4);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	create_stage();
}

void PhysicsElements::Stage_Tutorial2::create_stage()
{
	// 重力
	world->setGravity(Vec2{ 0, 980 });

	// プレイヤースタート地点
	start_pos = Vec2(128, 296);
	player->reset(start_pos);

	// 左壁
	wall->addRect(Vec2(0, -32), Vec2(32, 800));

	// 右壁
	wall->addRect(Vec2(1248, -32), Vec2(32, 800));

	// 天井
	wall->addRect(Vec2(0, -56), Vec2(1280, 96));

	// 左側の高台
	wall->addRect(Vec2(0, 320), Vec2(288, 96));
	// 高台の下
	wall->addRect(Vec2(0, 416), Vec2(160, 384));
	wall->addRect(Vec2(160, 416), Vec2(288, 64));

	// 危険な床
	P2BodyID id = wall->addRect(Vec2(288, 352), Vec2(160, 64));
	wall->change_filter(id, WallFilterType::Dangerous);

	// 危険な床の隣（トグル）
	wall->addRect(Vec2(448, 320), Vec2(32, 160));
	id = wall->addRect(Vec2(480, 320), Vec2(160, 96));
	items->item_add(id, WallFilterType::Ignore, Vec2(1206, 342), U"emoji_key");
	// wall_toggle_ignore.emplace_back(id);
	wall->addRect(Vec2(448, 0), Vec2(192, 128));

	// スロープとその先の地面
	wall->addTriangle(Vec2(640, 320), Vec2(0, 0), Vec2(0, 64), Vec2(160, 64));
	wall->addTriangle(Vec2(640, 40), Vec2(0, 0), Vec2(0, 88), Vec2(160, 0));
	wall->addRect(Vec2(640, 384), SizeF(160, 32));
	wall->addRect(Vec2(800, 384), Vec2(64, 32));

	// 危険な床（トグル）
	id = wall->addRect(Vec2(864, 384), Vec2(384, 32));
	wall->change_filter(id, WallFilterType::Dangerous);
	gun_targets->add_gun_target(id, Vec2(1200, 96), WallFilterType::Normal, WallFilterType::Dangerous);
	// wall_toggle_dangerous.emplace_back(id);

	// アイテム上の屋根
	wall->addRect(Vec2(1168, 256), Vec2(80, 32));

	// 床
	wall->addRect(Vec2(128, 656), Vec2(512, 128));

	// 壁（トグル）
	id = wall->addRect(Vec2(352, 478), Vec2(32, 178));
	items->item_add(id, WallFilterType::Ignore, Vec2(1120, 530), U"emoji_key");
	// wall_toggle_ignore.emplace_back(id);

	// 危険な床
	id = wall->addRect(Vec2(640, 688), Vec2(416, 32));
	wall->change_filter(id, WallFilterType::Dangerous);

	// 右下高台
	wall->addRect(Vec2(1056, 608), Vec2(224, 224));
	wall->addRect(Vec2(1216, 416), Vec2(64, 192));

	// wall->addCircle(Vec2(800, 400), 20);

	// 施錠壁と解除アイテム
	// P2BodyID id = wall->addRect(Vec2(416, 32), Vec2(32, 160));
	// wall_toggle_ignore.emplace_back(id);
	// items->item_add(id, WallFilterType::Ignore, Vec2(480, -32), U"emoji_key");

	// ゴール
	goal->set_pos(Vec2(260, 631));

	// スタート
	stopwatch.start();
}

//void PhysicsElements::Stage_Tutorial2::draw_mid_layer() const
//{
//	// 説明用
//	// 操作説明1
//	/*Vec2 const& ppos = player->get_pos();
//	double a = 0;
//	if (ppos.x < 500 && ppos.y < 300) {
//		a = 1.0;
//	}
//	else {
//		a = 0.9;
//	}
//	RoundRect(Vec2(94, 94), Vec2(266, 80), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
//	FontAsset(U"RoundedMgenplus28")(U"点滅している壁に").draw(Vec2(106, 98), { Palette::Steelblue, a });
//	FontAsset(U"RoundedMgenplus28")(U"当たるとミス").draw(Vec2(106, 130), { Palette::Steelblue, a });
//
//	if (ppos.x > 500 && ppos.y < 300) {
//		a = 1.0;
//	}
//	else {
//		a = 0.9;
//	}
//	RoundRect(Vec2(800, 128), Vec2(332, 152), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
//	FontAsset(U"RoundedMgenplus28")(U"左クリックで発砲").draw(Vec2(810, 130), { Palette::Steelblue, a });
//	GunTarget::draw_pos(Vec2(840, 200), false);
//	FontAsset(U"RoundedMgenplus28")(U"に弾を当てると").draw(Vec2(880, 180), { Palette::Steelblue, a });
//	FontAsset(U"RoundedMgenplus28")(U"壁の状態が切り替わる").draw(Vec2(810, 230), { Palette::Steelblue, a });
//
//	if (ppos.y > 332) {
//		a = 1.0;
//	}
//	else {
//		a = 0.9;
//	}
//	RoundRect(Vec2(664, 576), Vec2(352, 90), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
//	FontAsset(U"RoundedMgenplus28")(U"発砲の反動で\n逆方向に速度を得られる").draw(Vec2(674, 578), { Palette::Steelblue, a });
//	Circle(Vec2(830, 420), 20).drawFrame(8, { Palette::Red, 0.6 });
//	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(695, 510), AlphaF(0.5));
//	Line(Vec2(695, 510), Vec2(830, 420)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.6 });
//	TextureAsset(U"emoji_gun").resized(45).rotated(Math::QuarterPi).drawAt(Vec2(665, 470), AlphaF(0.6));*/
//
//	// Line(Vec2(640, 180).movedBy(25, 25), Vec2(640, 180).movedBy(50, 50)).drawArrow(10, SizeF(20, 20), { Palette::Darkorange, 0.6 });
//}

void PhysicsElements::Stage_Tutorial2::change_next_scene()
{
	changeScene(getData().stage_num + 1, 500);
	++getData().stage_num;
}

PhysicsElements::Stage_Tutorial2::Stage_Tutorial2(InitData const& init_data)
	: Stage(init_data)
{
	init();
}

void PhysicsElements::Stage_Tutorial2::Update()
{
	update_pipeline();

}

void PhysicsElements::Stage_Tutorial2::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage_Tutorial2::update()
//{
//}
//
//void PhysicsElements::Stage_Tutorial2::draw() const
//{
//}
