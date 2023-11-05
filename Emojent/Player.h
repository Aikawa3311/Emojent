#pragma once
#include <Siv3D.hpp>
#include "PhysicsElements.h"
#include "GlobalSetting.h"

namespace PhysicsElements {
	class Player{
	private:
		// カメラの参照(マウス操作時のダッシュ用)
		Camera2D const& camera;

		// 物理演算用
		std::shared_ptr<P2World> world;

		P2Body body;
		P2Body foot_body; // 接地判定用の足元センサー
		P2DistanceJoint foot_joint; // body と foot_body をくっつけるためのジョイント

		// プレイヤーの種々の値
		double const body_size = 20; // 体の大きさ
		double speed = 22000;
		double jump_speed = 80;
		double dash_speed = 20;
		double move_horizontal_resistance_val = 100; // 左右操作の抵抗値
		double move_air_horizontal_resistance_mag = 0.3; // 中空時の左右操作の抵抗値
		double player_angle = 0;
		Vec2 wire_direction = Vec2::Zero();

		double dash_impulse_mag = 1.5; // 速度ベクトルと逆方向のダッシュは倍率をかける
		double dash_cool_time = 1; // ダッシュのクールタイム
		Timer dash_timer; // クールタイムを測るためのタイマー

		// Vec2 direction = Vec2::Zero();
		bool on_ground = false;
		bool is_freeze = false;		// 操作可能状態かどうか
		bool is_hyde = false;		// 描画するかどうか
		bool is_hyde_gun = false;	// 銃を描画するかどうか
		bool is_exist_wire = false;	// ワイヤが存在しているかどうか

		String expression = U"emoji_face_sanglass";
		Timer expression_timer;
		double idle_time = 7;
		
	public:
		Player(std::shared_ptr<P2World>& world, Camera2D const& camera);

		void set_pos(Vec2 const & world_pos);

		// プレイヤーを指定した位置に再生成する
		void reset(Vec2 const & world_pos);

		void Update();
		void UpdatePhysics();
		void Draw() const;

		// -----
		// アクセサ
		// -----
		P2Body const& get_body() const;
		P2Body & get_body() ;
		P2BodyID get_body_id() const;
		P2BodyID get_foot_body_id() const;
		Vec2 get_pos() const;
		Vec2 get_velocity() const;
		double get_body_size() const;
		double get_angle() const;
		bool get_on_ground() const;
		bool get_freeze() const;
		bool get_hyde() const;

		void set_on_ground(bool const flag);
		void set_expression(String const& str);
		void set_freeze(bool const flag);
		void set_hyde(bool const flag);
		void set_hyde_gun(bool const flag);
		void set_wire_direction(Vec2 const & vec);
		void set_exist_wire(bool const flag);
	};

} // namespace PhysicsElements

