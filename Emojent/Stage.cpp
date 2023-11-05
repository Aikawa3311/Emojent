#include "stdafx.h"
#include "Stage.h"

PhysicsElements::Stage::Stage(InitData const& init_data)
	: IScene(init_data),
	rt_wall(Scene::Size()),
	rt_stage(Scene::Size()),
	stage_size(Scene::Size())
{
	if (!shader_wall)
		throw Error{ U"Failed to load a shader file" };
	if (!shader_background)
		throw Error{ U"Failed to load a shader file" };

	// 諸々の初期化
	world = std::make_shared<P2World>();
	player = std::make_shared<Player>(world, camera);
	wall = std::make_shared<Wall>(world);
	wire = std::make_shared<Wire>(world, camera, player, wall);

	gun_targets = std::make_shared<GunTarget>(wall);
	gun = std::make_shared<Gun>(world, camera, player, wall, wire, gun_targets);
	items = std::make_unique<Item>(player, wall);
	goal = std::make_unique<Goal>(player, wire, camera, rt_stage, stopwatch, stage_num);
	player_miss_ctrl = std::make_unique<PlayerMissController>(world, player);

	trail.setLifeTime(trail_life_time);

	// その他の変数は init() で代入
	init();

	// ステージを作る
	// create_stage();
}

void PhysicsElements::Stage::init()
{
	// ステージ名
	stage_name = U"テストステージ";
	// シーンステート
	stage_scene_state = SceneState::Stage_Tutorial1;

	// シェーダの色など
	shader_col1->color = ColorF(Palette::White).toFloat4();
	shader_col2->color = ColorF(Palette::Deeppink).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// プレイヤースタート地点
	start_pos = Vec2(100, -100);
}

void PhysicsElements::Stage::update_pipeline()
{
	// ステージ内の物体の Update 呼び出し
	player->Update();
	wall->Update();
	wire->Update();
	player_miss_ctrl->Update();
	gun_targets->Update();
	gun->Update();

	// Print << U"経過時間：{}"_fmt(stopwatch.sF());

	// チェックポイントの更新
	update_checkpoint();

	// アイテムの処理
	items->Update();

	// ゴールの処理
	goal->Update();

	// ミス時からの復帰
	miss_state_check_and_change();

	// 場外ミス時の復帰
	update_out_room();

	// 星パーティクルの更新
	star_particle_system.Update();

	// トレイルの処理
	update_trail();

	// シェーダー値の更新
	shader_time->time = static_cast<float>(Scene::Time());

	// カメラの更新
	update_camera();

	// ワイヤの有効範囲の表示オン・オフ
	if (!is_freeze && KeyZ.down()) {
		// wire->debug_draw_wire_guide = !wire->debug_draw_wire_guide;
	}

	// リトライ
	if (!is_freeze && KeyR.down()) {
		changeScene(stage_scene_state, 500);
	}

	// ステージセレクト
	if (!is_freeze && KeyT.down()) {
		changeScene(SceneState::StageSelect, 500);
	}
}

void PhysicsElements::Stage::draw_pipeline() const
{
	{
		// マルチサンプルレンダーテクスチャに描画する
		ScopedRenderTarget2D target(rt_stage.clear(AlphaF(0)));
		BlendState bs;
		bs.srcAlpha = Blend::One;
		bs.dstAlpha = Blend::One;
		ScopedRenderStates2D blend(bs);

		// 背景
		draw_background();

		// 壁の描画
		draw_wall();
		// rt_wall.draw();

		// カメラから Transformer2D 作成
		{
			auto const transformer = camera.createTransformer();

			// 背景や壁よりも上に描画したいものを描画
			draw_mid_layer();

			// 墓石やミスしたプレイヤーの描画
			player_miss_ctrl->Draw();

			// 銃のターゲット
			gun_targets->Draw();

			// チェックポイント
			draw_checkpoint();

			// トレイルの描画
			trail.draw();

			// ゴールの描画
			goal->Draw();

			// ワイヤの描画
			wire->Draw();
			// プレイヤーの描画
			player->Draw();
			// 銃の描画
			gun->Draw();

			// アイテムの描画
			items->Draw();

			// 星パーティクルの描画
			star_particle_system.Draw();

			// 現在のタイムの描画
			draw_time();
		}
	}

	Graphics2D::Flush();
	rt_stage.resolve();

	if (!goal->get_result()) {
		// 普通にステージ全体を描画
		rt_stage.draw();
	}
	else {
		// リザルト中は goal に描画の権利を譲渡し全ての描画を向こうに委ねる
		goal->draw_result();
	}

	// カメラ操作の描画（デバグ用）
	// camera.draw();
}

void PhysicsElements::Stage::create_stage()
{
	world->setGravity(Vec2{0, 980});
	// P2World に物体を生成していく
	player->reset(Vec2(100, -100));
	
	// 床
	wall->addRect(Vec2(-640, 200), Vec2(1280, 300));

	// wall.addRectAt(Vec2(350, -100), Vec2(100, 60));
	// 左右の壁
	wall->addRect(Vec2(-400, -100), Vec2(20, 300));
	wall->addRect(Vec2(380, -100), Vec2(20, 300));

	// 天井
	wall->addRect(Vec2(-300, -200), Vec2(600, 30));

	// ゴール
	goal->set_pos(Vec2(-50, 100));

	// タイマー開始
	stopwatch.start();
}

void PhysicsElements::Stage::draw_mid_layer() const
{
}

void PhysicsElements::Stage::draw_background() const
{
	{
		ScopedCustomShader2D const scoped_shader(shader_background);
		// ScopedRenderStates2D blend(BlendState::Additive);
		Graphics2D::SetPSConstantBuffer(1, shader_col1);
		Graphics2D::SetPSConstantBuffer(2, shader_col2);
		Graphics2D::SetPSConstantBuffer(3, shader_size);
		Graphics2D::SetPSConstantBuffer(4, shader_time);

		RectF(Vec2::Zero(), Scene::Size()).draw(Palette::Pink);
	}

	RectF(Vec2::Zero(), Scene::Size()).draw({ Palette::Black, 0.8 });
}

void PhysicsElements::Stage::change_next_scene()
{
}

void PhysicsElements::Stage::physics_collision_check()
{
	// コリジョンを取得
	P2BodyID player_id = player->get_body_id();
	P2BodyID player_foot_id = player->get_foot_body_id();
	auto const& wall_ids = wall->get_wall_ids();
	auto const& wall_bodies = wall->get_bodies();
	for (auto&& [pair, collision] : world->getCollisions()) {
		// ----------
		// プレイヤーのミス判定
		// ----------

		// ミスしていない場合のみ判定
		if (!player_miss_ctrl->get_missed()) {
			// Dangerous 属性の壁に触れたらアウト、やり直し
			if (pair.a == player_id) {
				// もう片方 pair.b が wall でかつその wall が Dangerous ならアウト
				if (wall_ids.contains(pair.b)) {
					if (wall_bodies.at(wall_ids.at(pair.b)).filter_type == WallFilterType::Dangerous) {
						star_particle_system.emit(collision.contact(0).point);
						miss_by_contact();
					}
				}
			}
			else if (pair.b == player_id) {
				// もう片方 pair.a が wall でかつその wall が Dangerous ならアウト
				if (wall_ids.contains(pair.a)) {
					if (wall_bodies.at(wall_ids.at(pair.a)).filter_type == WallFilterType::Dangerous) {
						star_particle_system.emit(collision.contact(0).point);
						miss_by_contact();
					}
				}
			}
		}

		// ----------
		// プレイヤーの接地判定
		// ----------

		// プレイヤーの足元センサーを示す id と地面を示す id が接触していればプレイヤーは接地中
		if (pair.a == player_foot_id)
		{
			// もう片方 pair.b が wall だったら接地中
			bool on_ground = wall_ids.contains(pair.b);
			player->set_on_ground(on_ground);
		}
		else if (pair.b == player_foot_id)
		{
			// もう片方 pair.a が wall だったら接地中
			bool on_ground = wall_ids.contains(pair.a);
			player->set_on_ground(on_ground);
		}
	}
}

void PhysicsElements::Stage::miss_by_contact()
{
	// ワイヤがあれば消す
	wire->wire_reset();

	// トレイルを消す
	is_trail_on = false;

	// 銃を消す
	gun->set_hyde_gun(true);

	// 音を鳴らす
	AudioAsset(U"damage").playOneShot(0.5);

	// 起動する
	player_miss_ctrl->start_miss_anim();
}

void PhysicsElements::Stage::miss_by_out_room()
{
	// ワイヤがあれば消す
	wire->wire_reset();

	// トレイルを消す
	is_trail_on = false;

	restart_stage();
}

void PhysicsElements::Stage::miss_state_check_and_change()
{
	if (!player_miss_ctrl->get_anim_end()) return;
	// もしミス演出が終了したらステージをやり直す
	player_miss_ctrl->set_anim_end(false);
	gun->set_hyde_gun(false);
	restart_stage();
}

void PhysicsElements::Stage::update_result()
{
	// もしゴールしたらステージを止める
	if (goal->get_cleared()) {
		wire->debug_draw_wire_guide = false;
		flag_camera_moving_limit = false;
		set_freeze(true);
		gun->set_freeze(true);
		gun->set_hyde_gun(true);
	}

	// リザルト中でなければ終
	if (!goal->get_result()) return;

	// リザルトのどのボタンが押されたか
	if (goal->get_clicked_next_button()) {
		// 音
		AudioAsset(U"button").playOneShot(0.5);
		// 次のステージへ
		change_next_scene();
	}
	if (goal->get_clicked_restart_button()) {
		// 音
		AudioAsset(U"button").playOneShot(0.5);
		// リスタートする
		changeScene(stage_scene_state, 500);
	}
	if (goal->get_clicked_back_button()) {
		// 音
		AudioAsset(U"button").playOneShot(0.5);
		// いったん終了する
		changeScene(SceneState::StageSelect, 500);
	}
}

void PhysicsElements::Stage::draw_wall() const
{
	// 壁の輪郭はわかりやすく描画する、これはレンダーテクスチャ関係なし
	auto const & wall_bodies = wall->get_bodies();
	{
		auto const transformer = camera.createTransformer();
		for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
			// ignore フィルタがついていれば薄く表示
			double a = wall_bodies[i].filter_type == WallFilterType::Ignore ? 0.3 : 1.0;
			LineString line(wall->nth_wall_vertices(i));
			for (int32 j = 1; j < (int32)line.size(); ++j) {
				Line(line[j], line[j - 1]).draw(LineStyle::RoundCap, 8, { stage_col2, a });
			}
			Line(line.back(), line.front()).draw(LineStyle::RoundCap, 8, { stage_col2, a });

			/*if (wall_bodies[i].filter_type == WallFilterType::Ignore) {
				line.drawClosed(32, { stage_col2, 0.3 });
			}
			else {
				line.drawClosed(LineStyle::RoundCap, 32, stage_col2);
			}*/
		}
	}

	// 通常の壁と Ignore の壁をレンダーテクスチャに描画する
	{
		auto const transformer = camera.createTransformer();

		BlendState bs = BlendState::Default2D;
		bs.srcAlpha = Blend::One;
		bs.dstAlpha = Blend::Zero;
		ScopedRenderTarget2D target(rt_wall.clear(AlphaF(0.0)));
		ScopedRenderStates2D blend(bs);

		wall->draw_normal_walls();
		wall->draw_ignore_walls();
		wall->draw_danger_walls(stage_col1, stage_col2);
	}

	// 不透明度を 1 にするために一旦描画
	rt_wall.draw();

	// レンダーテクスチャ自体はカメラに依らず画面上に固定するように描画する
	// シェーダに渡す
	{
		ScopedCustomShader2D const scoped_shader(shader_wall);
		ScopedRenderStates2D blend(BlendState::Additive);
		Graphics2D::SetPSConstantBuffer(1, shader_col1);
		Graphics2D::SetPSConstantBuffer(2, shader_col2);
		Graphics2D::SetPSConstantBuffer(3, shader_size);
		Graphics2D::SetPSConstantBuffer(4, shader_time);

		// シェーダを介して描画、これは不透明度 1 にならない（？）
		rt_wall.draw();
	}

	// 危険な壁をそのまま描画する
	{
		auto const transformer = camera.createTransformer();

		wall->draw_danger_walls(stage_col1, stage_col2);
	}

	// トグル壁に絵文字を持たせる
	{
		auto const transformer = camera.createTransformer();
		// ignore にトグルする壁
		auto const& id_table = wall->get_wall_ids();
		for (auto const id : wall_toggle_ignore) {
			int32 index = id_table.at(id);
			Vec2 draw_pos = wall->nth_wall_barycenter(index);
			WallBodyInfo const& wall_info = wall_bodies[index];
			double a = (wall_info.filter_type == WallFilterType::Normal) ? 1.0 : 0.3;
			String texture_name = (wall_info.filter_type == WallFilterType::Normal) ? U"emoji_lock" : U"emoji_unlock";

			TextureAsset(texture_name).resized(40).drawAt(draw_pos , AlphaF(a));
			//if (wall_info.filter_type == WallFilterType::Normal) {
			//	TextureAsset(U"emoji_lock").resized(40).drawAt(wall_info.body.getPos());
			//	// TextureAsset(U"emoji_lock").resized(40).drawAt(draw_pos);
			//}
			//else {
			//	TextureAsset(U"emoji_unlock").resized(40).drawAt(wall_info.body.getPos(), AlphaF(0.3));
			//}
		}

		// dangerous にトグルする壁
		for (auto const id : wall_toggle_dangerous) {
			int32 index = id_table.at(id);
			Vec2 draw_pos = wall->nth_wall_barycenter(index);

			TextureAsset(U"emoji_warning").resized(30).drawAt(draw_pos);
		}
	}
}

void PhysicsElements::Stage::update_trail()
{
	if (player->get_on_ground()) {
		// 接地していれば false
		is_trail_on = false;
	}
	else if (wire->get_exist_wire()) {
		// 空中でかつワイヤーが使用されたら true
		is_trail_on = true;
	}

	// 時間を測る（必要？）
	if (is_trail_on) {
		trail_accumulated_time += Scene::DeltaTime();
		if (trail_accumulated_time >= 1.0 / trail_frequency) {
			trail_accumulated_time = Fmod(trail_accumulated_time, 1.0 / trail_frequency);
			// 点を追加
			trail.add(player->get_pos(), stage_col1.lerp(stage_col2, 0.5), trail_size);
		}
	}

	// トレイルの更新
	trail.update();
}

void PhysicsElements::Stage::update_out_room()
{
	Vec2 const& ppos = player->get_pos();
	if (ppos.x < -50 || ppos.x > stage_size.x + 50 || ppos.y < -50 || ppos.y > stage_size.y + 50) {
		miss_by_out_room();
	}
}

void PhysicsElements::Stage::draw_time() const
{
	if (!is_cleared) {
		RectF(Vec2(1140, 670), Vec2(140, 80)).draw({ Palette::White, 0.6 });
		FontAsset(U"Alkatra30")(stopwatch.format(U"mm.ss.xx")).draw(Vec2(1150, 680), Palette::Black);
	}
}

void PhysicsElements::Stage::update()
{
	// SceneManager の更新関数
	update_result();

	// 通常の更新
	Update();
	// 物理演算の更新
	UpdatePhysics();
}

void PhysicsElements::Stage::draw() const
{
	// SceneManager の描画関数
	Draw();
}

void PhysicsElements::Stage::update_camera()
{
	// プレイヤー座標を中心にする
	// camera.jumpTo(player->get_pos(), camera.getScale());
	// カメラの更新
	camera.update();
}

void PhysicsElements::Stage::restart_stage()
{
	// ワイヤがあれば処分する
	wire->wire_reset();

	// プレイヤーを元の位置に戻す
	player->reset(start_pos);

	// プレイヤーのフラグをデフォルトに戻す
	player->set_freeze(false);
	player->set_hyde(false);
}

void PhysicsElements::Stage::Update()
{
	update_pipeline();
}

void PhysicsElements::Stage::UpdatePhysics()
{
	// もし freeze しているなら物理世界の更新はしない
	if (is_freeze) return;

	// 物理演算のシミュレーションループ
	double constexpr step_time = Global::physics_step_time;

	for (accumulated_time += Scene::DeltaTime(); step_time <= accumulated_time; accumulated_time -= step_time)
	{
		// プレイヤーの接地判定を false にする
		player->set_on_ground(false);
		
		// 衝突のチェック、プレイヤーの接地判定の更新
		physics_collision_check();

		// プレイヤーの更新
		player->UpdatePhysics();

		// 壁の更新
		wall->UpdatePhysics();

		// ワイヤの更新
		wire->UpdatePhysics();

		// 2D 物理演算のワールドを StepTime 秒進める
		world->update(step_time);
	}
}

void PhysicsElements::Stage::Draw() const
{
	draw_pipeline();
}

//void PhysicsElements::Stage::drawFadeIn(double t) const {
//	Print << t;
//	Scene::Rect().draw({ Palette::Red, 1 - t });
//}
//
//void PhysicsElements::Stage::drawFadeOut(double t) const {
//	Scene::Rect().draw({ Palette::Red, t });
//}

void PhysicsElements::Stage::update_checkpoint()
{

}

void PhysicsElements::Stage::draw_checkpoint() const
{
}

//void PhysicsElements::Stage::add_checkpoint(Vec2 const& pos)
//{
//}

void PhysicsElements::Stage::set_freeze(bool const flag)
{
	is_freeze = flag;
	player->set_freeze(flag);
}



