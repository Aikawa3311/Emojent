#include "stdafx.h"
#include "StageTest.h"

PhysicsElements::StageTest::StageTest(InitData const& init)
	: IScene(init)
{
	world = std::make_shared<P2World>();
	player = std::make_shared<Player>(world, camera);
	wall = std::make_shared<Wall>(world);
	wire = std::make_shared<Wire>(world, camera, player, wall);
	// ステージを作る
	create_stage();
}

void PhysicsElements::StageTest::create_stage()
{
	world->setGravity(Vec2{ 0, 980 });
	// P2World に物体を生成していく
	player->reset(Vec2(-530, -130));
	goal_pos = Vec2(530, 180);
	key_pos = Vec2(450, -200);

	// 真ん中の正方形
	wall->addRect(Vec2(0, -200), Vec2(30, 30));

	// 右の低い足場
	wall->addRect(Vec2(320, 200), Vec2(400, 50));

	// 右の高い足場
	wall->addRect(Vec2(400, -150), Vec2(400, 20));

	// 左の高い足場
	wall->addRect(Vec2(-800, -0), Vec2(400, 50));

	// 天井
	wall->addRect(Vec2(-400, -400), Vec2(800, 30));

	// ロックされた壁
	wall->addRect(Vec2(450, -150), Vec2(20, 350));

}

void PhysicsElements::StageTest::physics_collision_check()
{
	// コリジョンを取得
	P2BodyID player_foot_id = player->get_foot_body_id();
	auto const& wall_ids = wall->get_wall_ids();
	for (auto&& [pair, collision] : world->getCollisions()) {
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

void PhysicsElements::StageTest::update()
{
	// SceneManager の更新関数
	// 通常の更新
	Update();
	// 物理演算の更新
	UpdatePhysics();
}

void PhysicsElements::StageTest::draw() const
{
	// SceneManager の描画関数
	Draw();
}

void PhysicsElements::StageTest::Update()
{
	// ステージ内の物体の Update 呼び出し
	player->Update();
	wall->Update();
	wire->Update();

	// 場外判定
	if (player->get_body().getPos().y > 900) {
		player->reset(Vec2(-530, -130));
		++restart_cnt;
	}
	if (player->get_body().getPos().x > 1200) {
		player->reset(Vec2(-530, -130));
		++restart_cnt;
	}

	// クリア判定
	if (player->get_pos().distanceFrom(goal_pos) < 50) {
		is_cleared = true;
	}
	// 鍵取得判定
	if (!is_get_key && player->get_pos().distanceFrom(key_pos.movedBy(25, 25)) < 25) {
		is_get_key = true;
		P2BodyID id = wall->get_bodies().back().body.id();
		wall->change_filter(id, WallFilterType::Ignore);
		wire->debug_draw_wire_guide = false;
	}

	// リスタート
	if (KeyR.down()) {
		player->set_expression(U"emoji_face_sanglass");
		player->reset(Vec2(-530, -130));
		++restart_cnt;
	}

	if (KeySpace.down()) {
		player->set_expression(U"emoji_face_sanglass");
		wire->debug_draw_wire_guide = !wire->debug_draw_wire_guide;
	}

	// カメラの更新
	camera.update();
}

void PhysicsElements::StageTest::UpdatePhysics()
{
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

void PhysicsElements::StageTest::Draw() const
{
	// ステージ内の物体の Draw 呼び出し

	// カメラから Transformer2D 作成
	{
		auto const transformer = camera.createTransformer();

		// 背景
		Scene::SetBackground(Palette::Skyblue);

		// 壁の描画
		wall->Draw();

		// 鍵と南京錠の描画
		if (is_get_key) {
			TextureAsset(U"emoji_key").resized(50).draw(key_pos, AlphaF(0.3));
			TextureAsset(U"emoji_unlock").resized(50).drawAt(460, 25, AlphaF(0.5));
		}
		else {
			TextureAsset(U"emoji_key").resized(50).draw(key_pos.movedBy(Periodic::Sine0_1(3) * Vec2::Up() * 10));
			// Vec2(450, -150), Vec2(20, 350)
			TextureAsset(U"emoji_lock").resized(50).drawAt(460, 25);
		}

		// ゴール
		if (is_cleared) {
			TextureAsset(U"emoji_face_smiling").resized(50).drawAt(goal_pos);
		}
		else {
			TextureAsset(U"emoji_face_crying").resized(50).drawAt(goal_pos);
		}
		Circle(goal_pos, 20).drawFrame(2, { Palette::Red, 0.5 });

		// プレイヤーの描画
		player->Draw();
		
		// ワイヤの描画
		wire->Draw();
	}

	// カメラ操作の描画（デバグ用）
	// camera.draw();
}
