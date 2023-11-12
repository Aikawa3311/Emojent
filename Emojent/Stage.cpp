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

	// ステージ内の物体の諸々の初期化
	world = std::make_shared<P2World>();
	player = std::make_shared<Player>(world, camera);
	wall = std::make_shared<Wall>(world);
	wire = std::make_shared<Wire>(world, camera, player, wall);

	gun_targets = std::make_shared<GunTarget>(wall);
	gun = std::make_shared<Gun>(world, camera, player, wall, wire, gun_targets);
	items = std::make_unique<Item>(player, wall);
	goal = std::make_unique<Goal>(player, wire, camera, rt_stage, stopwatch, getData().stage_num);
	player_miss_ctrl = std::make_unique<PlayerMissController>(world, player);

	// 現在のステージ番号の保存（Global::stage_scene_id_offset の下駄をはかせていることに注意）
	stage_num = getData().stage_num;

	// トレイルの寿命
	trail.setLifeTime(trail_life_time);

	// まだシェーダのタイマーが開始していなかったらタイマーを開始
	if (!shader_stopwatch.isStarted()) {
		shader_stopwatch.start();
	}

	// ステージを読み込む
	// create_stage();
	load_stage_toml(U"stage/stage_" + Format(stage_num - Global::stage_scene_id_offset + 1) + U".toml");

	// その他の変数は init() で代入
	// init();

	// もしステージ番号が 0 ならオープニング演出の準備をする
	opening_setup();

	// オープニングでなければステージのタイマースタート
	if (!is_opening) {
		stopwatch.start();
	}
}

void PhysicsElements::Stage::init()
{
	// ステージ名
	stage_name = U"テストステージ";
	// シーンステート
	// stage_scene_state = SceneState::Stage_Tutorial1;

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
	// shader_time->time = static_cast<float>(Scene::Time());
	shader_time->time = static_cast<float>(shader_stopwatch.sF());

	// カメラの更新
	update_camera();

	// ワイヤの有効範囲の表示オン・オフ
	if (!is_freeze && KeyZ.down()) {
		// wire->debug_draw_wire_guide = !wire->debug_draw_wire_guide;
	}

	// リトライ
	if (!is_freeze && KeyR.down()) {
		changeScene(stage_num, 500);
	}

	// ステージセレクト
	if (!is_freeze && KeyT.down()) {
		changeScene(Global::stage_select_scene_id, 500);
	}

	// オープニングの終了判定
	opening_update();
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
	// チュートリアルの描画
	if (stage_num == Global::stage_scene_id_offset + 0) {
		draw_tutorial1();
	}
	else if (stage_num == Global::stage_scene_id_offset + 1) {
		draw_tutorial2();
	}
	else if (stage_num == Global::stage_scene_id_offset + 2) {
		draw_tutorial3();
	}
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
	changeScene(stage_num + 1, 500);
	++getData().stage_num;
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
		changeScene(stage_num, 500);
	}
	if (goal->get_clicked_back_button()) {
		// 音
		AudioAsset(U"button").playOneShot(0.5);
		// ステージセレクト画面へ
		changeScene(Global::stage_select_scene_id, 500);
		getData().stage_num = Global::stage_select_scene_id;
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
		//auto const& id_table = wall->get_wall_ids();
		//for (auto const id : wall_toggle_ignore) {
		//	int32 index = id_table.at(id);
		//	Vec2 draw_pos = wall->nth_wall_barycenter(index);
		//	WallBodyInfo const& wall_info = wall_bodies[index];
		//	double a = (wall_info.filter_type == WallFilterType::Normal) ? 1.0 : 0.3;
		//	String texture_name = (wall_info.filter_type == WallFilterType::Normal) ? U"emoji_lock" : U"emoji_unlock";

		//	TextureAsset(texture_name).resized(40).drawAt(draw_pos , AlphaF(a));
		//}

		//// dangerous にトグルする壁
		//for (auto const id : wall_toggle_dangerous) {
		//	int32 index = id_table.at(id);
		//	Vec2 draw_pos = wall->nth_wall_barycenter(index);

		//	TextureAsset(U"emoji_warning").resized(30).drawAt(draw_pos);
		//}

		auto const& id_table = wall->get_wall_ids();
		for (auto const& [wall_id, icon_name] : wall_icons) {
			int32 index = id_table.at(wall_id);
			Vec2 draw_pos = wall->nth_wall_barycenter(index);

			// 壁が ignore ならアイコンも薄く表示する
			WallBodyInfo const& wall_info = wall_bodies[index];
			double a = (wall_info.filter_type == WallFilterType::Ignore) ? 0.3 : 1.0;
			// 錠前なら少し大きく表示
			double size = (icon_name == U"emoji_lock") ? 40 : 30;
			// 錠前なら ignore 時に開いたアイコンに変える
			String texture_name = icon_name;
			if (texture_name == U"emoji_lock" && wall_info.filter_type == WallFilterType::Ignore) {
				texture_name = U"emoji_unlock";
			}

			// 描画
			TextureAsset(texture_name).resized(size).drawAt(draw_pos, AlphaF(a));
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

void PhysicsElements::Stage::load_stage_toml(FilePath const & path)
{
	TOMLReader tomlr(Resource(path));
	if (!path) {
		throw Error{ U"Failed to load `{}`"_fmt(path) };
	}

	// Console << U"----- toml {} 読み込み"_fmt(path);
	// Console << U"ステージデータ読み込み";

	// 各ステージ情報の取得
	// stage_num = tomlr[U"stageinfo.id"].get<int32>();
	goal->set_stage_num(stage_num);
	stage_name = tomlr[U"stageinfo.name"].getString();
	start_pos = Vec2{ tomlr[U"stageinfo.start_pos.x"].get<double>(), tomlr[U"stageinfo.start_pos.y"].get<double>() };
	player->reset(start_pos);
	goal->set_pos(Vec2{ tomlr[U"stageinfo.goal_pos.x"].get<double>(), tomlr[U"stageinfo.goal_pos.y"].get<double>() });
	stage_col1 = Color( tomlr[U"stageinfo.col1.r"].get<int8>(), tomlr[U"stageinfo.col1.g"].get<int8>(), tomlr[U"stageinfo.col1.b"].get<int8>() );
	stage_col2 = Color(tomlr[U"stageinfo.col2.r"].get<int8>(), tomlr[U"stageinfo.col2.g"].get<int8>(), tomlr[U"stageinfo.col2.b"].get<int8>());
	stage_size = Vec2(tomlr[U"stageinfo.size.w"].get<int32>(), tomlr[U"stageinfo.size.h"].get<int32>());
	world->setGravity(tomlr[U"stageinfo.gravity"].get<double>());
	score_time.emplace_back(tomlr[U"stageinfo.rank2_time"].get<double>());
	score_time.emplace_back(tomlr[U"stageinfo.rank3_time"].get<double>());
	score_time.emplace_back(tomlr[U"stageinfo.rank4_time"].get<double>());
	goal->set_score_time(score_time);

	// シェーダの色など
	shader_col1->color = ColorF(stage_col1).toFloat4();
	shader_col2->color = ColorF(stage_col2).toFloat4();
	shader_size->size = Float2(Scene::Size());

	// Console << U"壁データ読み込み";
	// 壁の情報の取得
	for (auto const& wall_info : tomlr[U"stageinfo.wall"].tableArrayView()) {
		// 形を確認
		String shape_type_str = wall_info[U"shape"].getString();
		WallShapeType shape_type = WallShapeType::Rect;
		if (shape_type_str.lowercased() == U"circle") {
			shape_type = WallShapeType::Circle;
		}
		else if (shape_type_str.lowercased() == U"triangle") {
			shape_type = WallShapeType::Triangle;
		}
		else if (shape_type_str.lowercased() == U"polygon") {
			shape_type = WallShapeType::Polygon;
		}

		// 頂点座標などを取得し実際に追加
		P2BodyID id = 0;
		switch (shape_type) {
		case WallShapeType::Circle:
			{
				// 円の中心と半径を取得する
				String center_name = (wall_info[U"center"].isEmpty()) ? U"pos" : U"center";
				Vec2 center = Vec2{ wall_info[center_name + U".x"].get<double>(), wall_info[center_name + U".y"].get<double>() };
				double r = wall_info[U"r"].get<double>();
				id = wall->addCircle(center, r);
			}
			break;

		case WallShapeType::Rect:
			{
				// 長方形の左上の座標とサイズを取得する
				Vec2 pos = Vec2{ wall_info[U"pos.x"].get<double>(), wall_info[U"pos.y"].get<double>() };
				Vec2 size = Vec2{ wall_info[U"size.x"].get<double>(), wall_info[U"size.y"].get<double>() };
				id = wall->addRect(pos, size);
			}
			break;

		case WallShapeType::Triangle:
			{
				// 三角形の基準点と三点の座標を取得する
				Vec2 pos = Vec2{ wall_info[U"pos.x"].get<double>(), wall_info[U"pos.y"].get<double>() };
				Vec2 v0 = Vec2{ wall_info[U"v0.x"].get<double>(), wall_info[U"v0.y"].get<double>() };
				Vec2 v1 = Vec2{ wall_info[U"v1.x"].get<double>(), wall_info[U"v1.y"].get<double>() };
				Vec2 v2 = Vec2{ wall_info[U"v2.x"].get<double>(), wall_info[U"v2.y"].get<double>() };

				id = wall->addTriangle(pos, v0, v1, v2);
			}
			break;

		case WallShapeType::Polygon:
			{
				// ポリゴンの各点を取得する
				Array<Vec2> vertices;
				for (int32 i = 0; !wall_info[U"v" + Format(i)].isEmpty(); ++i) {
					// Vec2 v = Vec2{ wall_info[U"v" + Format(i) + U".x"].get<double>(), wall_info[U"v" + Format(i) + U".y"].get<double>() };
					vertices.emplace_back(wall_info[U"v" + Format(i) + U".x"].get<double>(), wall_info[U"v" + Format(i) + U".y"].get<double>());
				}
			
			}
			break;
		}

		// 壁の属性の確定
		String attr = (wall_info[U"attr"].isEmpty()) ? U"normal" : wall_info[U"attr"].getString();
		WallFilterType filter = WallFilterType::Normal;
		if (attr.lowercased() == U"ignore") {
			filter = WallFilterType::Ignore;
		}
		else if (attr.lowercased() == U"dangerous") {
			filter = WallFilterType::Dangerous;
		}
		else if (attr.lowercased() == U"platform") {
			filter = WallFilterType::Platform;
		}
		wall->change_filter(id, filter);

		// 壁のアイコンの確定
		if (!wall_info[U"icon"].isEmpty()) {
			String icon = wall_info[U"icon"].getString();
			if (icon.lowercased() == U"lock") {
				icon = U"emoji_lock";
			}
			else if(icon.lowercased() == U"warning") {
				icon = U"emoji_warning";
			}

			wall_icons.emplace_back(id, icon);
		}
	}

	auto const& wall_bodies = wall->get_bodies();

	// Console << U"アイテム読み込み";
	if (!tomlr[U"stageinfo.item"].isEmpty()) {
		// アイテムの情報
		for (auto const& item_info : tomlr[U"stageinfo.item"].tableArrayView()) {
			// 場所の取得
			Vec2 pos = Vec2{ item_info[U"pos.x"].get<double>(), item_info[U"pos.y"].get<double>() };

			// 切り替え対象の壁
			Array<int32> target_ids;
			for (const auto& target_id : item_info[U"target_id"].arrayView())
			{
				target_ids << wall_bodies[target_id.get<int32>()].body.id();
			}

			// 取得後の属性
			String attr = item_info[U"get_attr"].getString();
			WallFilterType filter = WallFilterType::Normal;
			if (attr.lowercased() == U"ignore") {
				filter = WallFilterType::Ignore;
			}
			else if (attr.lowercased() == U"dangerous") {
				filter = WallFilterType::Dangerous;
			}
			else if (attr.lowercased() == U"platform") {
				filter = WallFilterType::Platform;
			}

			// テクスチャ名の取得
			String texture = U"emoji_key";

			items->item_add(target_ids.front(), filter, pos, texture);
		}
	}

	// Console << U"的読み込み";
	if (!tomlr[U"stageinfo.gun_target"].isEmpty()) {
		// 的の情報
		for (auto const& gun_target_info : tomlr[U"stageinfo.gun_target"].tableArrayView()) {
			// 場所の取得
			Vec2 pos = Vec2{ gun_target_info[U"pos.x"].get<double>(), gun_target_info[U"pos.y"].get<double>() };

			// 切り替え対象の壁
			Array<int32> target_ids;
			for (const auto& target_id : gun_target_info[U"target_id"].arrayView())
			{
				target_ids << wall_bodies[target_id.get<int32>()].body.id();
			}

			// on 時の属性
			String attr = gun_target_info[U"on_attr"].getString();
			WallFilterType filter_on = WallFilterType::Normal;
			if (attr.lowercased() == U"ignore") {
				filter_on = WallFilterType::Ignore;
			}
			else if (attr.lowercased() == U"dangerous") {
				filter_on = WallFilterType::Dangerous;
			}
			else if (attr.lowercased() == U"platform") {
				filter_on = WallFilterType::Platform;
			}

			// off 時の属性
			attr = gun_target_info[U"off_attr"].getString();
			WallFilterType filter_off = WallFilterType::Normal;
			if (attr.lowercased() == U"ignore") {
				filter_off = WallFilterType::Ignore;
			}
			else if (attr.lowercased() == U"dangerous") {
				filter_off = WallFilterType::Dangerous;
			}
			else if (attr.lowercased() == U"platform") {
				filter_off = WallFilterType::Platform;
			}

			gun_targets->add_gun_target(target_ids.front(), pos, filter_on, filter_off);
		}
	}

	// Console << U"----- toml読み取り完了";
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

void PhysicsElements::Stage::updateFadeIn(double t)
{
	if (!shader_stopwatch.isStarted()) {
		shader_stopwatch.start();
	}
	shader_time->time = static_cast<float>(shader_stopwatch.sF());
}

void PhysicsElements::Stage::draw_tutorial1() const {
	// 光
	double length = 120;
	int32 n = 16;
	for (int32 i = 0; i < n; ++i) {
		Vec2 const p1 = start_pos + length * Vec2(Cos(((double)i / n) * Math::TwoPi), Sin(((double)i / n) * Math::TwoPi));
		Vec2 const p2 = start_pos + length * Vec2(Cos(((double)(i + 1) / n) * Math::TwoPi), Sin(((double)(i + 1) / n) * Math::TwoPi));
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
	TextureAsset(U"icon_a_key").draw(Vec2(448, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_left").draw(Vec2(480, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_d_key").draw(Vec2(576, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_right").draw(Vec2(544, 552), { Palette::Steelblue, a });
	TextureAsset(U"icon_w_key").draw(Vec2(511, 486), { Palette::Steelblue, a });
	TextureAsset(U"icon_arrow_up").draw(Vec2(512, 520), { Palette::Steelblue, a });

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
	FontAsset(U"RoundedMgenplus28")(U"右クリックで壁に\nワイヤをつける・外す").draw(Vec2(820, 105), { Palette::Steelblue, a });
	// TextureAsset(U"icon_z_key").draw(Vec2(720, 240), {Palette::Steelblue, a});
	// FontAsset(U"MamelonHiRegular30")(U"でガイドの表示").draw(Vec2(750, 240), {Palette::Steelblue, a});
	// 例を表示
	Circle(Vec2(860, 40), 20).drawFrame(8, { Palette::Red, 0.6 });
	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(640, 180), AlphaF(0.6));
	Line(Vec2(640, 180), Vec2(860, 40)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.6 });
	Line(Vec2(640, 180).movedBy(25, 25), Vec2(640, 180).movedBy(50, 50)).drawArrow(10, SizeF(20, 20), { Palette::Darkorange, 0.6 });

	// オープニングの描画
	opening_draw();
}

void PhysicsElements::Stage::draw_tutorial2() const
{
	Vec2 const& ppos = player->get_pos();
	double a = 0;
	if (ppos.x < 500 && ppos.y < 300) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(94, 94), Vec2(266, 80), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"点滅している壁に").draw(Vec2(106, 98), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"当たるとミス").draw(Vec2(106, 130), { Palette::Steelblue, a });

	if (ppos.x > 500 && ppos.y < 300) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(800, 128), Vec2(332, 152), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"左クリックで発砲").draw(Vec2(810, 130), { Palette::Steelblue, a });
	GunTarget::draw_pos(Vec2(840, 200), false);
	FontAsset(U"RoundedMgenplus28")(U"に弾を当てると").draw(Vec2(880, 180), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"壁の状態が切り替わる").draw(Vec2(810, 230), { Palette::Steelblue, a });

	if (ppos.y > 332) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(664, 576), Vec2(352, 90), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"発砲の反動で\n逆方向に速度を得られる").draw(Vec2(674, 578), { Palette::Steelblue, a });
	Circle(Vec2(830, 420), 20).drawFrame(8, { Palette::Red, 0.6 });
	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(695, 510), AlphaF(0.5));
	Line(Vec2(695, 510), Vec2(830, 420)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.6 });
	TextureAsset(U"emoji_gun").resized(45).rotated(Math::QuarterPi).drawAt(Vec2(665, 470), AlphaF(0.6));
}

void PhysicsElements::Stage::draw_tutorial3() const
{
	Vec2 const& ppos = player->get_pos();
	double a = 0;
	if (ppos.x < 600) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(256, 384), Vec2(298, 88), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"ワイヤが屈曲すると").draw(Vec2(266, 386), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"速度が向上する").draw(Vec2(266, 422), { Palette::Steelblue, a });
	Circle(Vec2(380, 100), 20).drawFrame(8, { Palette::Red, 0.4 });
	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(124, 338), AlphaF(0.4));
	Line(Vec2(380, 100), Vec2(124, 338)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.4 });

	if (ppos.x > 600) {
		a = 1.0;
	}
	else {
		a = 0.9;
	}
	RoundRect(Vec2(800, 472), Vec2(320, 126), 10).draw({ Palette::White, a }).drawFrame(4.0, { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"ワイヤ操作中は").draw(Vec2(810, 478), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"左右移動をしない方が").draw(Vec2(810, 512), { Palette::Steelblue, a });
	FontAsset(U"RoundedMgenplus28")(U"速度を出しやすい").draw(Vec2(810, 546), { Palette::Steelblue, a });
	Circle(Vec2(960, 294), 20).drawFrame(8, { Palette::Red, 0.4 });
	TextureAsset(U"emoji_face_sanglass").resized(50).rotated(Math::QuarterPi).drawAt(Vec2(674, 426), AlphaF(0.4));
	Line(Vec2(960, 294), Vec2(674, 426)).draw(LineStyle::SquareDot, 4.0, { Palette::Darkorange, 0.4 });
}

void PhysicsElements::Stage::opening_setup()
{
	// オープニングステージ（最初のステージ）でなければ何もしない
	if (stage_num != Global::stage_scene_id_offset) {
		return;
	}

	// キーフレームの設定
	opening_keyframe.set(U"opening_transition_t", { 0.5s, 0.0 }, { 1.5s, 1.0 }, EaseOutQuad)
		.set(U"opening_title_t", { 1.5s, 0.0 }, { 2.0s, 1.0 }, EaseOutQuint)
		.set(U"opening_anykey_t", { 2.5s, 0.0 }, { 3.0s, 1.0 }, EaseInOutQuad);

	// タイトルを兼ねてるため特別処理、プレイヤーの動きをいったん止める
	set_freeze(true);

	// オープニングステージでは銃は使えないようにしておく
	gun->set_freeze(true);
	gun->set_hyde_gun(true);

	// オープニングフラグのオン
	is_opening = true;

	// オープニングの明転用レンダーテクスチャのサイズ変更
	rt_opening = RenderTexture(Scene::Size());

	// カメラを寄る
	camera.jumpTo(start_pos, 4.5);

	// スタート
	opening_keyframe.start();
}

void PhysicsElements::Stage::opening_update()
{
	// オープニングの終了判定
	if (is_opening && opening_keyframe[U"opening_anykey_t"] > 0.5 && Keyboard::GetAllInputs().size() >= 1) {
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

void PhysicsElements::Stage::opening_draw() const
{
	// タイトル
	// FontAsset(U"RobotoSlab40")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, Palette::Black), start_pos.movedBy(0, -50), Palette::Lightgoldenrodyellow);
	double t = opening_keyframe[U"opening_title_t"];
	FontAsset(U"RobotoSlab40MSDF")(U"EMOJENT").drawAt(TextStyle::Outline(0.2, { Palette::Black, t }), start_pos.movedBy(-70 * (1 - t), -50), { Palette::Lightgoldenrodyellow, t });

	// anykey
	t = opening_keyframe[U"opening_anykey_t"];
	double a = t * (Periodic::Sine0_1(1.5s) * 0.5 + 0.5);
	FontAsset(U"RobotoSlab14MSDF")(U"- PRESS ANY KEY -").drawAt(start_pos.movedBy(0, 50), { Palette::Black, a });
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

	// オープニング中の最初の明転
	if (is_opening) {
		double t = opening_keyframe[U"opening_transition_t"];
		if (t < 1.0) {
			{
				BlendState bs;
				bs.srcAlpha = Blend::One;
				bs.dstAlpha = Blend::Zero;
				ScopedRenderTarget2D target(rt_opening.clear(ColorF(0, 0, 0, 1)));
				ScopedRenderStates2D blend(bs);

				double r = hypot(Scene::Width(), Scene::Height()) * t;
				Circle(Scene::Center(), r).draw(ColorF(0, 0, 0, 0), ColorF(0, 0, 0, 1 - t * t));
			}

			rt_opening.draw();
		}
	}
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



