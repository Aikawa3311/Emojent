#include "stdafx.h"
#include "Stage_Tutorial1.h"

void PhysicsElements::Stage_Tutorial1::draw_mouse(Vec2 const& pos, double const r, double const margin, int32 click, Color const& col) const
{
	double s = 2;
	double t = r + margin;


	// 左クリック
	if (click == 1) {
		Circle(pos.movedBy(r, r), r).drawPie(0_deg, -90_deg, col);
	}
	Circle(pos.movedBy(r, r), r).drawArc(0_deg, -90_deg, 1, 1, col);
	Line(pos.movedBy(r, r), pos.movedBy(r, 0)).draw(2, col);
	Line(pos.movedBy(r, r), pos.movedBy(0, r)).draw(2, col);


	// 右クリック
	if (click == 2) {
		Circle(pos.movedBy(t, r), r).drawPie(0_deg, 90_deg, col);
	}
	Circle(pos.movedBy(t, r), r).drawArc(0_deg, 90_deg, 1, 1, col);
	Line(pos.movedBy(t, r), pos.movedBy(t + r, r)).draw(2, col);
	Line(pos.movedBy(t, r), pos.movedBy(t, 0)).draw(2, col);

	// 本体
	Line(pos.movedBy(0, t), pos.movedBy(t + r, t)).draw(2, col);
	Line(pos.movedBy(0, t), pos.movedBy(0, t+s)).draw(2, col);
	Line(pos.movedBy(t + r, t), pos.movedBy(t + r, t + s)).draw(2, col);
	Circle(pos.movedBy(r + margin/2, t+s), r+margin/2).drawArc(90_deg, 180_deg, 1, 1, col);
}

void PhysicsElements::Stage_Tutorial1::init()
{
	// シーン関連
	stage_scene_state = SceneState::Stage_Tutorial1;
	stage_num = 0;
	goal->set_stage_num(stage_num);

	// ステージの色
	stage_col1 = Palette::White.lerp(Palette::Gray, 0.4);
	stage_col2 = Palette::Forestgreen.lerp(Palette::Gray, 0.4);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// 銃は使えないようにしておく
	gun->set_freeze(true);
	gun->set_hyde_gun(true);

	// キーフレームの設定
	anim_keyframe.set(U"opening_transition_t", { 0.5s, 0.0 }, { 1.5s, 1.0 }, EaseOutQuad)
		.set(U"opening_title_t", { 1.5s, 0.0 }, { 2.0s, 1.0 }, EaseOutQuint)
		.set(U"opening_anykey_t", { 2.5s, 0.0 }, { 3.0s, 1.0 }, EaseInOutQuad);

	create_stage();
}

void PhysicsElements::Stage_Tutorial1::create_stage()
{
	//// 重力
	//world->setGravity(Vec2{ 0, 980 });

	//// プレイヤースタート地点
	//start_pos = Vec2(-384, 231);
	//player->reset(start_pos);

	//// 床
	//wall->addRect(Vec2(-640, 256), Vec2(1280, 300));

	//// 段差
	//wall->addRect(Vec2(96, 192), Vec2(544, 64));

	//// 左壁
	//wall->addRect(Vec2(-640, -400), Vec2(64, 800));

	//// 右壁
	//wall->addRect(Vec2(576, -400), Vec2(64, 800));

	//// 天井
	//wall->addRect(Vec2(-640, -416), Vec2(1280, 96));

	//// 低めの天井
	//wall->addRect(Vec2(-640, -416), Vec2(480, 224));

	//// プラットフォーム2
	//P2BodyID id = wall->addRect(Vec2(0, 80), Vec2(160, 4));
	//wall->change_filter(id, WallFilterType::Platform);

	//// 浮いてる足場
	//wall->addRect(Vec2(-160, -32), Vec2(96, 32));

	//// 屋根
	//wall->addRect(Vec2(416, 0), Vec2(160, 32));

	//// 施錠壁と解除アイテム
	//id = wall->addRect(Vec2(416, 32), Vec2(32, 160));
	//wall_toggle_ignore.emplace_back(id);
	//items->item_add(id, WallFilterType::Ignore, Vec2(480, -32), U"emoji_key");

	//// ゴール
	//goal->set_pos(Vec2(500, 167));

	//// タイトルを兼ねてるため特別処理、プレイヤーの動きをいったん止める
	//set_freeze(true);

	//// カメラを寄る
	//camera.jumpTo(start_pos, 4.5);

	//// スタート
	//anim_keyframe.start();

	// 重力
	world->setGravity(Vec2{ 0, 980 });

	// プレイヤースタート地点
	start_pos = Vec2(256, 591);
	player->reset(start_pos);

	// 床
	wall->addRect(Vec2(0, 616), Vec2(1280, 300));

	// 段差
	wall->addRect(Vec2(736, 552), Vec2(544, 64));

	// 左壁
	wall->addRect(Vec2(0, -40), Vec2(64, 800));

	// 右壁
	wall->addRect(Vec2(1206, -40), Vec2(96, 800));

	// 天井
	wall->addRect(Vec2(0, -56), Vec2(1280, 96));

	// 低めの天井
	wall->addRect(Vec2(0, -56), Vec2(480, 224));

	// プラットフォーム2
	P2BodyID id = wall->addRect(Vec2(576, 440), Vec2(224, 4));
	wall->change_filter(id, WallFilterType::Platform);

	// 浮いてる足場
	wall->addRect(Vec2(416, 328), Vec2(160, 32));

	// 屋根
	wall->addRect(Vec2(1056, 360), Vec2(160, 32));

	// 施錠壁と解除アイテム
	id = wall->addRect(Vec2(1056, 392), Vec2(32, 160));
	wall_toggle_ignore.emplace_back(id);
	items->item_add(id, WallFilterType::Ignore, Vec2(1120, 328), U"emoji_key");

	// ゴール
	goal->set_pos(Vec2(1140, 527));

	// タイトルを兼ねてるため特別処理、プレイヤーの動きをいったん止める
	set_freeze(true);

	// カメラを寄る
	camera.jumpTo(start_pos, 4.5);

	// スタート
	anim_keyframe.start();
}

void PhysicsElements::Stage_Tutorial1::draw_mid_layer() const
{
	// 光
	double length = 120;
	int32 n = 16;
	for (int32 i = 0; i < n; ++i) {
		Vec2 const p1 = start_pos + length * Vec2(Cos(((double)i / n) * Math::TwoPi), Sin(((double)i / n) * Math::TwoPi));
		Vec2 const p2 = start_pos + length * Vec2(Cos(((double)(i+1) / n) * Math::TwoPi), Sin(((double)(i+1) / n) * Math::TwoPi));
		Triangle(start_pos, p1, p2).draw(ColorF(1, 1, 1, 0.5), ColorF(1, 1, 1, 0), ColorF(1, 1, 1, 0));
	}

	length = 150;
	for (int32 i = 0; i < n; ++i) {
		if (i % 2 == 0) continue;
		Vec2 const p1 = start_pos + length * Vec2(Cos(((double)i / n - Scene::Time() * 0.04) * Math::TwoPi), Sin(((double)i / n - Scene::Time() * 0.04) * Math::TwoPi));
		Vec2 const p2 = start_pos + length * Vec2(Cos(((double)(i + 1) / n - Scene::Time() * 0.04) * Math::TwoPi), Sin(((double)(i + 1) / n - Scene::Time() * 0.04) * Math::TwoPi));
		Triangle(start_pos, p1, p2).draw(ColorF(1, 1, 1, 0.5), ColorF(1, 1, 1, 0.2), ColorF(1, 1, 1, 0.2));
	}

	// 説明用
	// 操作説明1
	Vec2 const& ppos = player->get_pos();
	double a = 0;
	if (ppos.x < 740 && ppos.y > 360) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(416, 460), Vec2(224, 140), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"操作").draw(Vec2(426, 466), { Palette::Steelblue, a });
	TextureAsset(U"icon_a_key").draw(Vec2(448, 552), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_left").draw(Vec2(480, 552), {Palette::Steelblue, a});
	TextureAsset(U"icon_d_key").draw(Vec2(576, 552), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_right").draw(Vec2(544, 552), {Palette::Steelblue, a});
	TextureAsset(U"icon_w_key").draw(Vec2(511, 486), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_up").draw(Vec2(512, 520), {Palette::Steelblue, a});

	// リトライ・ステージセレクトの説明
	a = 1.0;
	RoundRect(Vec2(64, 32), Vec2(320, 96), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Orangered, a });
	TextureAsset(U"icon_r_key").draw(Vec2(84, 46), { Palette::Orangered, a });
	FontAsset(U"RoundedMgenplus28")(U"リトライ").draw(Vec2(120, 40), { Palette::Orangered, a });
	TextureAsset(U"icon_t_key").draw(Vec2(84, 84), { Palette::Orangered, a });
	FontAsset(U"RoundedMgenplus28")(U"ステージセレクト").draw(Vec2(120, 78), { Palette::Orangered, a });

	/*FontAsset(U"MamelonHiRegular30")(U"操作").draw(Vec2(426, 466), { Palette::Steelblue, a });
	TextureAsset(U"icon_a_key").draw(Vec2(448, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_left").draw(Vec2(480, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_d_key").draw(Vec2(576, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_right").draw(Vec2(544, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_w_key").draw(Vec2(511, 486), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_up").draw(Vec2(512, 520), { Palette::Steelblue, a });*/

	// 鍵の説明
	if (ppos.x > 740 && ppos.y > 360) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(820, 580), Vec2(320, 128), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	// TextureAsset(U"emoji_face_crying").resized(30).draw(Vec2(200, 234), AlphaF(a));
	// TextureAsset(U"emoji_key").resized(30).draw(Vec2(200, 269), AlphaF(a));
	// TextureAsset(U"emoji_lock").resized(30).draw(Vec2(200, 300), AlphaF(a));
	// FontAsset(U"MamelonHiRegular30")(U"に触れるとクリア").draw(Vec2(230, 234), { Palette::Steelblue, a });
	// FontAsset(U"MamelonHiRegular30")(U"を取ると").draw(Vec2(230, 269), {Palette::Steelblue, a});
	// FontAsset(U"MamelonHiRegular30")(U"を開けられる").draw(Vec2(230, 300), {Palette::Steelblue, a});

	TextureAsset(U"emoji_face_crying").resized(30).draw(Vec2(840, 594), AlphaF(a));
	TextureAsset(U"emoji_key").resized(30).draw(Vec2(840, 629), AlphaF(a));
	TextureAsset(U"emoji_lock").resized(34).draw(Vec2(988, 625), AlphaF(a));
	FontAsset(U"RoundedMgenplus28")(U"に触れるとクリア").draw(Vec2(870, 588), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"を取ると").draw(Vec2(870, 623), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"を").draw(Vec2(1020, 623), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"開けられる").draw(Vec2(840, 658), { Palette::Steelblue, a });
	
	if (ppos.x > 440 && ppos.y < 360) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(800, 100), Vec2(350, 90), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	// draw_mouse(Vec2(150, -180), 12.0, 2.0, 2, {Palette::Steelblue, a});
	FontAsset(U"RoundedMgenplus28")(U"右クリックで壁に\nワイヤをつける・外す").draw(Vec2(820, 105), {Palette::Steelblue, a});
	// TextureAsset(U"icon_z_key").draw(Vec2(720, 240), {Palette::Steelblue, a});
	// FontAsset(U"MamelonHiRegular30")(U"でガイドの表示").draw(Vec2(750, 240), {Palette::Steelblue, a});
	// 例を表示
	Circle(Vec2(860, 40), 20).drawFrame(8, { Palette::Red, 0.6 });
	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(640, 180), AlphaF(0.6));
	Line(Vec2(640, 180), Vec2(860, 40)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.6 });
	Line(Vec2(640, 180).movedBy(25, 25), Vec2(640, 180).movedBy(50, 50)).drawArrow(10, SizeF(20, 20), {Palette::Darkorange, 0.6});

	// タイトル
	// FontAsset(U"RobotoSlab40")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, Palette::Black), start_pos.movedBy(0, -50), Palette::Lightgoldenrodyellow);
	double t = anim_keyframe[U"opening_title_t"];
	FontAsset(U"RobotoSlab40MSDF")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, { Palette::Black, t }), start_pos.movedBy(-70 * (1 - t), -50), { Palette::Lightgoldenrodyellow, t });

	/*
	// 説明用
	// 操作説明1
	Vec2 const& ppos = player->get_pos();
	double a = 0;
	if (ppos.x < 100 && ppos.y > 0) {
		a = 1.0;
	}
	else {
		a = 0.3;
	}
	RoundRect(Vec2(-224, 100), Vec2(224, 140), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"MamelonHiRegular30")(U"操作").draw(Vec2(-214, 106), { Palette::Steelblue, a });
	TextureAsset(U"icon_a_key").draw(Vec2(-192, 192), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_left").draw(Vec2(-160, 192), {Palette::Steelblue, a});
	TextureAsset(U"icon_d_key").draw(Vec2(-64, 192), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_right").draw(Vec2(-96, 192), {Palette::Steelblue, a});
	TextureAsset(U"icon_w_key").draw(Vec2(-129, 126), {Palette::Steelblue, a});
	TextureAsset(U"icon_arrow_up").draw(Vec2(-128, 160), {Palette::Steelblue, a});

	// 鍵の説明
	if (ppos.x > 100 && ppos.y > 0) {
		a = 1.0;
	}
	else {
		a = 0.3;
	}
	RoundRect(Vec2(180, 220), Vec2(320, 128), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	// TextureAsset(U"emoji_face_crying").resized(30).draw(Vec2(200, 234), AlphaF(a));
	// TextureAsset(U"emoji_key").resized(30).draw(Vec2(200, 269), AlphaF(a));
	// TextureAsset(U"emoji_lock").resized(30).draw(Vec2(200, 300), AlphaF(a));
	// FontAsset(U"MamelonHiRegular30")(U"に触れるとクリア").draw(Vec2(230, 234), { Palette::Steelblue, a });
	// FontAsset(U"MamelonHiRegular30")(U"を取ると").draw(Vec2(230, 269), {Palette::Steelblue, a});
	// FontAsset(U"MamelonHiRegular30")(U"を開けられる").draw(Vec2(230, 300), {Palette::Steelblue, a});

	TextureAsset(U"emoji_face_crying").resized(30).draw(Vec2(200, 234), AlphaF(a));
	TextureAsset(U"emoji_key").resized(30).draw(Vec2(200, 269), AlphaF(a));
	TextureAsset(U"emoji_lock").resized(34).draw(Vec2(348, 265), AlphaF(a));
	FontAsset(U"MamelonHiRegular30")(U"に触れるとクリア").draw(Vec2(230, 234), { Palette::Steelblue, a });
	FontAsset(U"MamelonHiRegular30")(U"を取ると").draw(Vec2(230, 269), { Palette::Steelblue, a });
	FontAsset(U"MamelonHiRegular30")(U"を").draw(Vec2(380, 269), { Palette::Steelblue, a });
	FontAsset(U"MamelonHiRegular30")(U"開けられる").draw(Vec2(200, 300), { Palette::Steelblue, a });

	if (ppos.x > -200 && ppos.y < 0) {
		a = 1.0;
	}
	else {
		a = 0.3;
	}
	RoundRect(Vec2(90, -200), Vec2(310, 128), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	// draw_mouse(Vec2(150, -180), 12.0, 2.0, 2, {Palette::Steelblue, a});
	FontAsset(U"MamelonHiRegular30")(U"右クリックで\n壁にワイヤをつける").draw(Vec2(110, -186), {Palette::Steelblue, a});
	TextureAsset(U"icon_z_key").draw(Vec2(110, -120), {Palette::Steelblue, a});
	FontAsset(U"MamelonHiRegular30")(U"でガイドの表示").draw(Vec2(140, -120), {Palette::Steelblue, a});

	// タイトル
	// FontAsset(U"RobotoSlab40")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, Palette::Black), start_pos.movedBy(0, -50), Palette::Lightgoldenrodyellow);
	double t = anim_keyframe[U"opening_title_t"];
	FontAsset(U"RobotoSlab40MSDF")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, { Palette::Black, t }), start_pos.movedBy(-70 * (1 - t), -50), { Palette::Lightgoldenrodyellow, t });
	*/






	//Array<Glyph> glyphs = FontAsset(U"RobotoSlab40").getGlyphs(U"EMOJENT");
	//double t = anim_keyframe[U"opening_title_t"];
	//{
	//	ScopedCustomShader2D shader_font(Font::GetPixelShader(FontAsset(U"RobotoSlab40").method()));
	//	for (int32 i = 0; i < 7; ++i) {
	//		// Vec2 to = start_pos.movedBy((40 * (i - 3)), );
	//		// double x = (40 * (i - 3)) * t;
	//		// double y = Abs(20 * (i - 3)) * t;
	//		double angle = 0.1 * (i - 3) * t;
	//		Vec2 draw_pos = start_pos.movedBy(0, 0) + 50 * Vec2(-Cos(Math::HalfPi + angle*3) * 2.3, -Sin(Math::HalfPi + angle*3) * 1.6);
	//		glyphs[i].texture.rotated(angle*3).drawAt(draw_pos, Palette::White);
	//		Print << U"draw_pos:{}"_fmt(draw_pos);
	//	}
	//}

	// anykey
	t = anim_keyframe[U"opening_anykey_t"];
	a = t * (Periodic::Sine0_1(1.5s) * 0.5 + 0.5);
	FontAsset(U"RobotoSlab14MSDF")(U"- PRESS ANY KEY -").drawAt(start_pos.movedBy(0, 50), { Palette::Black, a });
}

void PhysicsElements::Stage_Tutorial1::change_next_scene()
{
	changeScene(SceneState::Stage_Tutorial2, 500);
}

PhysicsElements::Stage_Tutorial1::Stage_Tutorial1(InitData const& init_data)
	: Stage(init_data),
	rt_opening(Scene::Size())
{
	init();
}

void PhysicsElements::Stage_Tutorial1::Update()
{
	update_pipeline();

	if (is_opening && anim_keyframe[U"opening_anykey_t"] > 0.5 && Keyboard::GetAllInputs().size() >= 1) {
		// オープニング終了
		is_opening = false;
		// 物理演算を有効に
		set_freeze(false);

		// カメラ関連
		Camera2DParameters param = Camera2DParameters::NoControl();
		param.scaleSmoothTime = 0.7;
		param.positionSmoothTime = 1.0;
		camera.setParameters(param);
		camera.setTargetCenter(Vec2(640, 360));
		camera.setTargetScale(1.0);

		// ステージタイマーのオン
		stopwatch.start();
	}
}

void PhysicsElements::Stage_Tutorial1::Draw() const
{
	draw_pipeline();

	// 最初の黒
	double t = anim_keyframe[U"opening_transition_t"];
	if (t < 1.0) {
		{
			BlendState bs;
			bs.srcAlpha = Blend::One;
			bs.dstAlpha = Blend::Zero;
			ScopedRenderTarget2D target(rt_opening.clear(ColorF(0,0,0,1)));
			ScopedRenderStates2D blend(bs);

			double r = hypot(Scene::Width(), Scene::Height()) * t;
			Circle(Scene::Center(), r).draw(ColorF(0, 0, 0, 0), ColorF(0, 0, 0, 1 - t * t));
		}

		rt_opening.draw();
	}
}

//void PhysicsElements::Stage_Tutorial1::update()
//{
//}
//
//void PhysicsElements::Stage_Tutorial1::draw() const
//{
//}
