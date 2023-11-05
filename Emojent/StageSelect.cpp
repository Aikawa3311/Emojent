#include "StageSelect.h"

void StageSelect::init()
{
	// セーブファイルから読み込み
	SaveData savedata;

	for (int32 i = 0; i < Global::stage_num; ++i) {
		StageInfo info;
		info.texture_name = U"stage_thumbnail" + Format(i);
		info.best_time = savedata.read_best_time(i);
		info.best_medal_num = savedata.read_medal_num(i);
		stage_info.emplace_back(info);
	}

	// 矩形を構築
	Vec2 draw_base_pos = Scene::CenterF().movedBy(-rect_size.x * 1.5 - margin.x, -220);
	Vec2 draw_pos = draw_base_pos;

	for (int32 i = 0; i < Global::stage_num; ++i) {
		rects.emplace_back(RectF(draw_pos, rect_size));
		draw_pos.x += rect_size.x + margin.x;
		if (i % 3 == 2) {
			draw_pos.x = draw_base_pos.x;
			draw_pos.y += rect_size.y + margin.y;
		}
	}

	// end button
	end_button = RectF(Scene::Center().movedBy(-90, 280), Vec2(180, 60));
}

StageSelect::StageSelect(InitData const& init_data)
	:IScene(init_data)
{
	init();
}

void StageSelect::update()
{
	Update();
}

void StageSelect::draw() const
{
	Draw();
}

void StageSelect::Update()
{
	bool flag_mouse_over = false;

	for (int32 i = 0; i < Global::stage_num; ++i) {
		bool flag_rect_mouse_intersect = rects[i].intersects(Cursor::PosF());
		flag_mouse_over |= flag_rect_mouse_intersect;
		if (flag_rect_mouse_intersect && MouseL.down()) {
			// 音を鳴らす
			AudioAsset(U"button").playOneShot(0.5);
			// ステージに移動
			switch (i) {
			case 0:
				changeScene(SceneState::Stage_Tutorial1, 500);
				break;
			case 1:
				changeScene(SceneState::Stage_Tutorial2, 500);
				break;
			case 2:
				changeScene(SceneState::Stage_Tutorial3, 500);
				break;
			case 3:
				changeScene(SceneState::Stage_Stage1, 500);
				break;
			case 4:
				changeScene(SceneState::Stage_Stage2, 500);
				break;
			case 5:
				changeScene(SceneState::Stage_Stage3, 500);
				break;
			default:
				changeScene(SceneState::Stage_Tutorial1, 500);
			}
		}
	}

	bool end_button_mouse_over = end_button.intersects(Cursor::Pos());
	if (end_button_mouse_over && MouseL.down()) {
		// ゲーム終了
		System::Exit();
	}

	if (flag_mouse_over && !flag_audio_play) {
		// 初めてマウスが重なったフレームで音を鳴らす
		flag_audio_play = true;
		AudioAsset(U"mouseover").playOneShot(0.3);
	}
	else if (end_button_mouse_over && !flag_audio_play) {
		// 終了ボタンも同様
		flag_audio_play = true;
		AudioAsset(U"mouseover").playOneShot(0.3);
	}
	else if (flag_audio_play && !flag_mouse_over && !end_button_mouse_over) {
		// マウスが離れたら false にする
		flag_audio_play = false;
	}
}

void StageSelect::Draw() const
{
	// 背景の描画
	// Scene::Rect().draw(Palette::Black);
	for (int32 i = 0; i < Scene::Height() / 32 + 2; ++i) {
		for (int32 j = 0; j < Scene::Width() / 32 + 2; ++j) {
			Color col = (i + j) % 2 == 1 ? Palette::Deeppink : Palette::White;
			RectF(Vec2(j * 32 - Fmod(Scene::Time() * 0.3, 1.0) * 32, i * 32 - Fmod(Scene::Time() * 0.3, 1.0) * 32), Vec2(32, 32)).draw(col);
		}
	}
	Scene::Rect().draw({ Palette::Black, 0.8 });
	Circle(Scene::Center(), Scene::Size().length() * 0.5).draw(Alpha(0), { Palette::Darkslategray, 0.4 });
	
	// ステージセレクト
	FontAsset(U"RobotoSlab60")(U"- STAGE SELECT -").draw(Arg::center = Scene::Center().movedBy(0, -300), Palette::White);

	// 各ステージの描画
	for (int32 i = 0; i < Global::stage_num; ++i) {
		// ステージの描画
		bool flag = rects[i].intersects(Cursor::PosF());
		double s = flag ? 1.2 : 1.0;
		double a = flag ? 1.0 : 0.7;
		double angle = flag ? (Periodic::Sine0_1(2.0s) - 0.5) * 0.1 : 0.0;
		TextureAsset(stage_info[i].texture_name).resized(rect_size).scaled(s).rotated(angle).drawAt(rects[i].pos + rect_size / 2, AlphaF(a)).drawFrame(8.0, {Palette::White});

		// メダルの描画
		for (int32 j = 0; j < stage_info[i].best_medal_num; ++j) {
			TextureAsset(U"emoji_medal").resized(50).drawAt(rects[i].scaled(s).br().movedBy(-j * 15, 0));
		}
		// ベストタイムの描画
		Stopwatch temp_stopwatch;
		temp_stopwatch.set(SecondsF(stage_info[i].best_time));
		FontAsset(U"Alkatra36SDF")(temp_stopwatch.format(U"mm.ss.xx")).drawAt(TextStyle::Outline(0.3, Palette::Black), 36 * s, rects[i].pos.movedBy(rect_size.x / 2, rect_size.y - 20), Palette::White);
	}

	// 終了ボタンの描画
	double s = end_button.intersects(Cursor::PosF()) ? 1.2 : 1.0;
	end_button.scaled(s).draw({Palette::White, Periodic::Sine0_1(2s) * 0.2 + 0.4}).drawFrame(4, Palette::White);
	FontAsset(U"RoundedMgenplus50")(U"終わる").draw(Arg::center = end_button.center(), Palette::Black);

	// RectF(Vec2::Zero(), Scene::Size()).draw({ Palette::Black, 0.8 });
}
