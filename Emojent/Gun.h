#pragma once
#include <Siv3D.hpp>
#include "Player.h"
#include "Wall.h"
#include "Wire.h"
#include "GunTarget.h"

namespace PhysicsElements {

	struct GunCollisionInfo {
		Vec2 v;			// 衝突場所
		int32 index;	// 衝突が発生した GunTarget の index
	};

	class Gun
	{
	private:
		// カメラの参照(マウスの座標計算用)
		Camera2D const& camera;

		// ステージ関連
		std::shared_ptr<P2World> world;
		std::shared_ptr<Player> player;
		std::shared_ptr<Wall> wall;
		std::shared_ptr<Wire> wire;
		std::shared_ptr<GunTarget> gun_target;

		// 射線
		Vec2 ray_e = Vec2::Zero();
		// 射程
		double gun_length = 1280;
		// 反動の力
		double recoil_speed = 20;
		// クールタイム
		double gun_cool_time = 1;
		Timer cooldown_timer;

		// 銃の描画オフセット
		double draw_offset = 45;
		// 弾の軌道描画用
		Array<std::pair<Line, Timer>> ray_line;
		// 軌道の寿命
		double ray_duration = 1.4;

		// 銃を描画しないかどうか
		bool is_hyde_gun = false;
		// 銃を使用不可にするかどうか
		bool is_freeze = false;

		// -----
		// メンバ関数
		// -----
		// 銃口の位置を計算
		Vec2 calc_muzzle_pos() const;

		// 銃の発砲全般
		void shoot();

		// 衝突の計算
		GunCollisionInfo check_collision();

		// 初期化処理
		void init();

	public:
		Gun(std::shared_ptr<P2World>& world, Camera2D const& camera, std::shared_ptr<Player>& player, std::shared_ptr<Wall>& wall, std::shared_ptr<Wire>& wire, std::shared_ptr<GunTarget>& gun_target);

		void Update();
		void UpdatePhysics();
		void Draw() const;

		// -----
		// アクセサ
		// -----
		bool get_is_cooldown() const;

		void set_hyde_gun(bool const flag);
		void set_freeze(bool const flag);
	};


} // namespace PhysicsElements


