#pragma once
#include <Siv3D.hpp>
#include "PhysicsElements.h"
#include "GlobalSetting.h"
#include "Player.h"

namespace PhysicsElements {

	// ミス中のプレイヤーの演出や墓石など
	class PlayerMissController {
	public:

	private:
		std::shared_ptr<P2World> world;
		std::shared_ptr<Player> player;

		// ミス演出のタイマー
		Timer anim_timer;
		double const anim_time = 1.0;

		// ミス中かどうか
		bool is_missed = false;
		// 演出が終了したかどうか
		bool is_anim_end = false;

		// ミス中の P2Body
		P2Body player_miss;
		Array<P2Body> headstones;

	public:
		PlayerMissController(std::shared_ptr<P2World>& world, std::shared_ptr<Player> & player);

		// プレイヤーが接触によるミスを起こした際の演出の開始
		void start_miss_anim();

		void Update();
		void UpdatePhysics();
		void Draw() const;

		// -----
		// アクセサ
		// -----
		bool get_anim_end() const;
		bool get_missed() const;

		void set_anim_end(bool const flag);
	};

} // namespace PhysicsElements


