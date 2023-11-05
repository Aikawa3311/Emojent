#pragma once
#include <Siv3D.hpp>
#include "GlobalSetting.h"
#include "PhysicsElements.h"
#include "Player.h"
#include "Wall.h"
#include "Wire.h"

namespace PhysicsElements {

	class StageTest : public App::Scene {
	private:
		// カメラ
		Camera2D camera{ Vec2(0, -100), 1.0 , CameraControl::None_ };

		// ステージの名前
		String stage_name = U"<NONE>";

		// 物理演算用のメンバ
		std::shared_ptr<P2World> world;
		std::shared_ptr<Player> player;
		std::shared_ptr<Wall> wall;
		std::shared_ptr<Wire> wire;
		double accumulated_time = 0;

		// ゴール
		Vec2 goal_pos;
		bool is_cleared = false;
		Vec2 key_pos;
		bool is_get_key = false;

		// カウント
		int32 restart_cnt = 0;

		// -----
		// メンバ関数
		// -----
		// ステージの生成
		void create_stage();

		// 衝突の処理
		void physics_collision_check();

	public:
		StageTest(InitData const& init);

		// SceneManager の更新関数
		void update() override;
		// SceneManager の描画関数
		void draw() const override;

		// general な更新関数
		void Update();

		// 物理演算用の更新関数、1 フレーム内で複数回呼び出される
		void UpdatePhysics();

		// general な描画関数
		void Draw() const;
	};


} // namespace PhysicsElements

