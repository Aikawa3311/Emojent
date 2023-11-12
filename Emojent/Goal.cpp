#include "stdafx.h"
#include "Goal.h"

void Goal::update_according_to_state()
{
	switch (state.getstate()) {
	case GoalState::Not:
		// ゴールの判定, ゴール座標に近ければクリア
		if (!is_cleared && player->get_pos().distanceFromSq(goal_pos) < 2500) {
			// クリアしたので演出準備
			is_cleared = true;

			// 音を鳴らす
			AudioAsset(U"clear").playOneShot(0.4);

			// メダルの数取得
			calc_medal_num();
			// ニューレコード判定
			is_new_record = stopwatch.sF() < best_time;
			// セーブデータ更新
			write_savedata();

			// プレイヤーの動きを止める
			player->set_freeze(true);
			player->set_hyde(true);
			// ワイヤーも消す
			wire->wire_reset();
			// ダミー用のプレイヤーを用意するため座標を記録
			player_pos = player->get_pos();
			player_angle = player->get_angle();
			// player->get_body().setGravityScale(0);
			player->get_body().setAwake(false);
			// ステージの時間を止める
			stopwatch.pause();

			// カメラを移動させる
			Camera2DParameters param = Camera2DParameters::NoControl();
			param.scaleSmoothTime = camera_move_time;
			param.positionSmoothTime = camera_move_time;
			// camera.setParameters({ .scaleSmoothTime = camera_move_time, .positionSmoothTime = camera_move_time });
			camera.setParameters(param);
			camera.setTargetCenter((player->get_pos() + goal_pos) / 2.0);
			camera.setTargetScale(3.0);

			// カメラ移動のステートへ進む
			state.changestate(GoalState::CameraMoving);
		}

		break;

	case GoalState::CameraMoving:
		// カメラを寄せ待ち
		if (state.gettime() > camera_move_time * 2.5) {
			// リザルトアニメへ移行
			is_result = true;
			anim_keyframes.start();
			state.changestate(GoalState::ResultAnim);
		}

		break;

	case GoalState::ResultAnim:
		// キーフレームが進行中
		shader_time->time = float(anim_keyframes[U"blur_t"]);
		// ストップウォッチを進める
		result_stopwatch.set(SecondsF(Math::Lerp(0.0, stopwatch.sF(), anim_keyframes[U"result_timer_t"])));
		// result_stopwatch.set(SecondsF(anim_keyframes[U"result_timer_r"]));

		// ボタンの押下状態を確認
		if (anim_keyframes[U"button_t"] > 0.0) {
			// next ボタン
			if (stage_num - Global::stage_scene_id_offset < Global::stage_num - 1) {
				is_clicked_next_button = draw_pos_next_button.leftClicked();
			}
			// restart ボタン
			is_clicked_restart_button = draw_pos_restart_button.leftClicked();
			// back ボタン
			is_clicked_back_button = draw_pos_back_button.leftClicked();
		}

		// Time のロール音を持続的にならす
		if (anim_keyframes[U"result_timer_t"] > 0.0 && anim_keyframes[U"result_timer_t"] < 1.0) {
			roll_accumulated_time += Scene::DeltaTime();
			if (roll_accumulated_time > 0.08) {
				roll_accumulated_time = Fmod(roll_accumulated_time, 0.08);
				AudioAsset(U"roll").playOneShot(0.4);
			}
		}

		// メダルの音を鳴らす
		if (anim_keyframes[U"medal1_t"] > 0.0 && medal_num >= 1 && !is_medal1_audio_play) {
			is_medal1_audio_play = true;
			AudioAsset(U"medal").playOneShot(0.7);
		}
		if (anim_keyframes[U"medal2_t"] > 0.0 && medal_num >= 2 && !is_medal2_audio_play) {
			is_medal2_audio_play = true;
			AudioAsset(U"medal").playOneShot(0.7);
		}
		if (anim_keyframes[U"medal3_t"] > 0.0 && medal_num >= 3 && !is_medal3_audio_play) {
			is_medal3_audio_play = true;
			AudioAsset(U"medal").playOneShot(0.7);
		}

		// ニューレコードの音を鳴らす
		if (anim_keyframes[U"result_new_record_t"] > 0.0 && stopwatch.sF() < best_time && !is_new_record_audio_play) {
			is_new_record_audio_play = true;
			AudioAsset(U"new_record").playOneShot(0.5);
		}

		break;
	}
}

Array<LineString> Goal::to_line_strings(const Vec2& basePos, const Array<OutlineGlyph>& glyphs) const
{
	Array<LineString> lines;

	Vec2 penPos{ basePos };

	for (const auto& glyph : glyphs)
	{
		for (const auto& ring : glyph.rings)
		{
			lines << ring.movedBy(penPos + glyph.getOffset());
		}

		penPos.x += glyph.xAdvance;
	}

	return lines;

}

void Goal::read_savedata()
{
	SaveData data;
	best_time = data.read_best_time(stage_num - Global::stage_scene_id_offset);
	best_medal_num = data.read_medal_num(stage_num - Global::stage_scene_id_offset);
}

void Goal::write_savedata()
{
	SaveData data;
	// タイムがこれまでのベストより良ければ更新
	if (stopwatch.sF() < best_time) {
		data.write_best_time(stage_num - Global::stage_scene_id_offset, stopwatch.sF());
	}
	// メダル数がこれまでより良ければ更新
	if (medal_num > best_medal_num) {
		data.write_medal_num(stage_num - Global::stage_scene_id_offset, medal_num);
	}
}

void Goal::calc_medal_num()
{
	if (stopwatch.sF() < score_time[1]) {
		// メダル 3 つ
		medal_num = 3;
	}
	else if (stopwatch.sF() < score_time[0]) {
		// メダル 2 つ
		medal_num = 2;
	}
	else {
		// メダル 1 つ
		medal_num = 1;
	}
}

void Goal::init_stage_data()
{
	// そのステージのメダル獲得タイムを取得
	// score_time = Global::score_time[stage_num - Global::stage_scene_id_offset];

	// セーブデータ側からそのステージのベストタイムとメダル読み込み
	read_savedata();
}

Goal::Goal(std::shared_ptr<PhysicsElements::Player>& player, std::shared_ptr<PhysicsElements::Wire>& wire, Camera2D& camera, MSRenderTexture& rt_stage, Stopwatch& stopwatch, int32 const stage_num)
	: player(player),
	wire(wire),
	camera(camera),
	state(GoalState::Not),
	rt_stage(rt_stage),
	stopwatch(stopwatch),
	stage_num(stage_num)
{
	// シェーダーのサイズ
	shader_size->size = Float2(Scene::Size());

	// キーフレームの初期化
	anim_keyframes.set(U"blur_t", { 0s, 0 }, { 2.0s, 1.0 }, EaseOutCubic)
		.set(U"photo_a", { 0s, 0 }, { 0.5s, 1.0 }, EaseOutQuad)
		.set(U"photo_angle", { 0s, -0.1 }, { 1s, -0.3 }, EaseInCubic)
		.set(U"photo_size", { 0s, 0.9 }, { 1s, 0.7 }, EaseInQuad)
		.set(U"photo_rel_x", { 0s, 0 }, { 1.0s, -100 }, EaseInQuad)
		.set(U"photo_rel_y", { 0s, 0 }, { 1.0s, -50 }, EaseInQuad)
		.set(U"mission_complete_t", { 1.5s, 0 }, { 2.0s, 1 }, EaseOutQuint)
		.set(U"result_time_t", { 2.5s, 0.0 }, { 2.8s, 1.0 }, EaseOutQuint)
		.set(U"result_timer_t", { 3.0s, 0.0 }, { 3.5s, 1.0 }, EaseOutQuad)
		.set(U"result_pb_time_t", { 3.5s, 0.0 }, { 3.7s, 1.0 }, EaseOutQuad)
		.set(U"result_new_record_t", { 3.7s, 0.0 }, { 4.0s, 1.0 }, EaseOutQuad)
		.set(U"button_t", { 3.7s, 0.0 }, { 4.0s, 1.0 }, EaseOutQuad)
		.set(U"medal1_t", { 3.7s, 0.0 }, { 4.0s, 1.0 }, EaseOutQuad)
		.set(U"medal2_t", { 4.1s, 0.0 }, { 4.4s, 1.0 }, EaseOutQuad)
		.set(U"medal3_t", { 4.5s, 0.0 }, { 4.8s, 1.0 }, EaseOutQuad)
		.set(U"next_rank_t", { 5.0s, 0.0 }, { 5.3s, 1.0 }, EaseOutQuad);

	// ステージに関するデータの初期化
	init_stage_data();
}

void Goal::Update()
{
	// ステートごとの update を処理
	update_according_to_state();
}

void Goal::Draw() const
{
	// ゴールのキャラクターの描画
	if (is_cleared) {
		TextureAsset(U"emoji_face_smiling").resized(50).drawAt(goal_pos);
	}
	else {
		TextureAsset(U"emoji_face_crying").resized(50).drawAt(goal_pos);
	}

	if (is_cleared) {
		// プレイヤーのダミーを描画
		TextureAsset(U"emoji_face_sanglass").resized(50).rotated(player_angle).drawAt(player_pos);
	}
}

void Goal::draw_result() const
{
	// リザルトのバックを描画
	{
		ScopedCustomShader2D const scoped_shader(shader);
		ScopedRenderStates2D blend(BlendState::Additive);
		Graphics2D::SetPSConstantBuffer(1, shader_size);
		Graphics2D::SetPSConstantBuffer(2, shader_time);

		rt_stage.draw();
	}

	// 写真みたいな感じで描画
	rt_stage.scaled(anim_keyframes[U"photo_size"]).rotated(anim_keyframes[U"photo_angle"]).drawAt(Scene::Size()/2 + Vec2(anim_keyframes[U"photo_rel_x"], anim_keyframes[U"photo_rel_y"]), AlphaF(anim_keyframes[U"photo_a"])).drawFrame(10, Palette::White);

	// リザルト全般の描画
	double t = anim_keyframes[U"mission_complete_t"];
	// 枠
	Vec2 square_pos = Math::Lerp(draw_pos_start_result_square.pos, draw_pos_end_result_square.pos, t);
	RectF(square_pos, Math::Lerp(draw_pos_start_result_square.size, draw_pos_end_result_square.size, t)).draw({ Palette::White, t * t * 0.8 });
	// ミッションコンプリート（MSDFは重い）
	// FontAsset(U"MangabeyRegular100MSDF")(U"  MISSION\nCOMPLETE!").draw(TextStyle::Outline(0.4, Palette::Black), Arg::center = Math::Lerp(draw_start_mission_complete, draw_end_mission_complete, t), {Palette::Deeppink, t * t});

	// FontAsset(U"MangabeyRegular100")(U"  MISSION\nCOMPLETE!").draw(Arg::center = Math::Lerp(draw_pos_start_mission_complete, draw_pos_end_mission_complete, t), { Palette::Deeppink, t * t });
	TextureAsset(U"result_mission_complete").draw(Arg::center = Math::Lerp(draw_pos_start_mission_complete, draw_pos_end_mission_complete, t), { Palette::White, t * t });

	/*FontAsset(U"MangabeyRegular100").renderOutlines(U"  MISSION\nCOMPLETE!");
	int32 sum = 0;
	auto line_arr = to_line_strings(Vec2::Zero(), FontAsset(U"MangabeyRegular30").renderOutlines(U"  MISSION\nCOMPLETE!"));
	for (auto const i : line_arr) {
		sum += i.size();
	}
	Print << U"線分の数：{}"_fmt(sum);*/

	// メダル
	if (t > 0.0) {
		TextureAsset(U"emoji_medal").resized(80).drawAt(square_pos + draw_pos_rel_medal1, { Palette::Black, t * t * 0.8 * 0.4 });
		TextureAsset(U"emoji_medal").resized(80).drawAt(square_pos + draw_pos_rel_medal2, { Palette::Black, t * t * 0.8 * 0.4 });
		TextureAsset(U"emoji_medal").resized(80).drawAt(square_pos + draw_pos_rel_medal3, { Palette::Black, t * t * 0.8 * 0.4 });

		//
		double t_medal = anim_keyframes[U"medal1_t"];
		TextureAsset(U"emoji_medal").resized(Math::Lerp(120, 80, t_medal)).drawAt(square_pos + draw_pos_rel_medal1, { Palette::White, t_medal });
		t_medal = anim_keyframes[U"medal2_t"];
		if (stopwatch.sF() < score_time[0] && t_medal > 0.0) {
			TextureAsset(U"emoji_medal").resized(Math::Lerp(120, 80, t_medal)).drawAt(square_pos + draw_pos_rel_medal2, { Palette::White, t_medal });
		}
		t_medal = anim_keyframes[U"medal3_t"];
		if (stopwatch.sF() < score_time[1] && t_medal > 0.0) {
			TextureAsset(U"emoji_medal").resized(Math::Lerp(120, 80, t_medal)).drawAt(square_pos + draw_pos_rel_medal3, { Palette::White, t_medal });
		}

		// if(result_stopwatch.sF() < )
		// FontAsset(U"MangabeyRegular30")(score_time[0]).draw(square_pos + draw_pos_rel_medal1, {Palette::Black, t * t});
	}

	// タイム
	t = anim_keyframes[U"result_time_t"];
	// FontAsset(U"MangabeyRegular30")(U"TIME").draw(Math::Lerp(draw_pos_start_result_time, draw_pos_end_result_time, t), { Palette::Black, t * t });
	// TextureAsset(U"result_time").draw(Math::Lerp(draw_pos_start_result_time, draw_pos_end_result_time, t), { Palette::White, t * t });
	TextureAsset(U"result_time").draw(draw_pos_result_time.movedBy(0, -100).lerp(draw_pos_result_time, t), {Palette::White, t * t});
	// ランク
	TextureAsset(U"result_rank").draw(draw_pos_result_rank.movedBy(0, -100).lerp(draw_pos_result_rank, t), { Palette::White, t * t });

	// タイマー
	t = anim_keyframes[U"result_timer_t"];
	if (t > 0.0) {
		// FontAsset(U"MangabeyRegular50")(result_stopwatch.format(U"mm.ss.xx")).draw(draw_pos_timer, { Palette::Black, t });
		FontAsset(U"Alkatra44")(result_stopwatch.format(U"mm.ss.xx")).draw(draw_pos_timer, { Palette::Black, t });
	}

	// ベストタイム
	t = anim_keyframes[U"result_pb_time_t"];
	// FontAsset(U"Alkatra44")(best_time_stopwatch.format(U"mm.ss.xx")).draw(draw_pos_best_timer, { Palette::Black, t });
	TextureAsset(U"result_best_time").draw(draw_pos_best_time.movedBy(-100, 0).lerp(draw_pos_best_time, t), {Palette::White, t});
	Stopwatch stopwatch_draw_temp;
	stopwatch_draw_temp.set(SecondsF(best_time));
	FontAsset(U"Alkatra44")(stopwatch_draw_temp.format(U"mm.ss.xx")).draw(draw_pos_best_timer.movedBy(-100, 0).lerp(draw_pos_best_timer, t), {Palette::Black, t});

	// ニューレコード
	if (is_new_record){
		t = anim_keyframes[U"result_new_record_t"];
		FontAsset(U"Alkatra30")(U"NEW RECORD!").drawAt(draw_pos_new_record.movedBy(-100, 0).lerp(draw_pos_new_record, t), { Palette::Deeppink.lerp(Palette::Blueviolet, Periodic::Sine0_1(1s)), t});
	}

	// 次のランクに必要なタイム
	if (medal_num < 3) {
		if (medal_num == 1) {
			stopwatch_draw_temp.set(SecondsF(score_time[0]));
		}
		else if (medal_num == 2) {
			stopwatch_draw_temp.set(SecondsF(score_time[1]));
		}
		t = anim_keyframes[U"next_rank_t"];
		// FontAsset(U"Alkatra30")(U"NEXT:").draw(draw_pos_next_rank.movedBy(-100, 0).lerp(draw_pos_next_rank, t), { Palette::Black, t });
		TextureAsset(U"result_next").draw(draw_pos_next_rank.movedBy(-100, 0).lerp(draw_pos_next_rank, t), {Palette::White, t});
		FontAsset(U"Alkatra30")(stopwatch_draw_temp.format(U"mm.ss.xx")).draw(draw_pos_next_rank.movedBy(-30, -10).lerp(draw_pos_next_rank.movedBy(70, -10), t), {Palette::Black, t});
	}

	// ボタンの描画
	t = anim_keyframes[U"button_t"];
	// 次ボタン
	// draw_pos_next_button.draw({ Palette::Green, t});
	double a = 0.0;
	double s = 1.0;
	bool is_mouse_on = (draw_pos_next_button.intersects(Cursor::PosF()));
	a = is_mouse_on ? t * 1.0 : t * 0.5;
	s = is_mouse_on ? 1.2 : 1.0;
	if (stage_num - Global::stage_scene_id_offset < Global::stage_num - 1) {
		TextureAsset(U"emoji_next").resized(100).draw(draw_pos_next_button.pos, {Palette::White, a});
		TextureAsset(U"result_next").draw(Arg::center = draw_pos_next_button.pos + draw_pos_next_button.size.movedBy(-10, -10), { Palette::Black, a });
	}
	// リスタートボタン
	is_mouse_on = (draw_pos_restart_button.intersects(Cursor::PosF()));
	a = is_mouse_on ? t * 1.0 : t * 0.5;
	s = is_mouse_on ? 1.2 : 1.0;
	TextureAsset(U"emoji_restart").resized(100).draw(draw_pos_restart_button.pos, AlphaF(a));
	// FontAsset(U"MangabeyRegular30")(U"RETRY").draw(Arg::center = draw_pos_restart_button.pos + draw_pos_restart_button.size.movedBy(-10, -10), { Palette::Black, a });
	TextureAsset(U"result_retry").draw(Arg::center = draw_pos_restart_button.pos + draw_pos_restart_button.size.movedBy(-10, -10), { Palette::Black, a });
	// 戻るボタン
	is_mouse_on = (draw_pos_back_button.intersects(Cursor::PosF()));
	a = is_mouse_on ? t * 1.0 : t * 0.5;
	s = is_mouse_on ? 1.2 : 1.0;
	TextureAsset(U"emoji_back").resized(100).draw(draw_pos_back_button.pos, AlphaF(a));
	// FontAsset(U"MangabeyRegular30")(U"BACK").draw(Arg::center = draw_pos_back_button.pos + draw_pos_back_button.size.movedBy(-10, -10), { Palette::Black, a });
	TextureAsset(U"result_back").draw(Arg::center = draw_pos_back_button.pos + draw_pos_back_button.size.movedBy(-10, -10), { Palette::Black, a });
}

void Goal::set_pos(Vec2 const& pos)
{
	goal_pos = pos;
}

void Goal::set_stage_num(int32 const num)
{
	stage_num = num;
	init_stage_data();
}

void Goal::set_score_time(Array<double> const& time)
{
	score_time = time;
}

bool Goal::get_cleared() const
{
    return is_cleared;
}

bool Goal::get_result() const
{
	return is_result;
}

bool Goal::get_clicked_next_button() const
{
	return is_clicked_next_button;
}

bool Goal::get_clicked_restart_button() const
{
	return is_clicked_restart_button;
}

bool Goal::get_clicked_back_button() const
{
	return is_clicked_back_button;
}
