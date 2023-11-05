#pragma once
#include <Siv3D.hpp>
#include "Player.h"
#include "Wall.h"

namespace PhysicsElements {

	struct WireBendInfo {
		Vec2 v;			// 屈曲の発生している頂点の座標
		int32 wall_id;	// 屈曲頂点を持つ wall の id
		Optional<bool> cw;		// 屈曲が時計回りかどうか、屈曲解消時の判定用
	};

	class Wire
	{
	public:
		// ワイヤのセンサの接触情報、接触座標と接触している壁の添え字
		using WireCollisionInfo = std::pair<Vec2, int32>;
		// using WireBendInfo = std::pair<Vec2, int32>;

	private:
		// カメラの参照(マウスの座標計算用)
		Camera2D const& camera;

		// 
		std::shared_ptr<P2World> world;
		std::shared_ptr<Player> player;
		std::shared_ptr<Wall> wall;

		// ワイヤ距離ジョイント関連
		P2DistanceJoint wire_joint;		// ワイヤ（距離ジョイント）本体
		Array<WireBendInfo> wire_joint_bend;	// ワイヤの屈曲部分
		Line wire_line_pre;				// ワイヤジョイントの前フレームの状態

		// ワイヤの屈曲による加速値
		double bend_adding_force = 5;	// ワイヤの屈曲発生時の加速値
		double bend_unwind_force = 5;	// ワイヤの屈曲解消時の加速値

		// 判定
		// P2Body wire_sensor;
		// P2Body wire_test;
		bool is_exist_wire = false;				// ワイヤ使用中かどうか
		Line wire_sensor = Line();				// ワイヤの判定用センサ
		WireCollisionInfo collision_info;		// ワイヤセンサの接触情報
		//Array<WallVertexInfo> wall_vertices;	// 壁の頂点情報
		//Array<bool> v_clockwise;
		//Array<bool> v_clockwise_pre;

		// 手を描くか
		bool flag_draw_hand = true;

		// 初期化処理
		void init();

		// ワイヤの壁接触判定
		void check_collision();

		// ワイヤの屈曲追加
		bool wire_bending_add();
		// ワイヤの屈曲解消
		bool wire_bending_unwind();

	public:
		// デバグラインの表示用
		bool debug_draw_wire_guide = false;

		Wire(std::shared_ptr<P2World>& world, Camera2D const & camera, std::shared_ptr<Player>& player, std::shared_ptr<Wall>& wall);

		// ワイヤを消す操作
		void wire_reset();

		void Update();
		void UpdatePhysics();
		void Draw() const;

		// -----
		// アクセサ
		// -----
		bool get_exist_wire() const;
	};


} // namespace PhysicsElements


