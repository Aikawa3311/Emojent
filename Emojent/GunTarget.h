#pragma once
#include <Siv3D.hpp>
#include "Wall.h"
#include "Player.h"

// 壁の状態を変える的
class GunTarget
{
private:
	// 的一つ一つの情報
	struct GunTargetComponent {
		Vec2 pos;
		bool is_on;	// オンになっているかどうか
		P2BodyID target; // 変更対象のid
		PhysicsElements::WallFilterType filter_on;	// on になったときのフィルタ
		PhysicsElements::WallFilterType filter_off; // off になったときのフィルタ
	};

	// 壁
	std::shared_ptr<PhysicsElements::Wall> wall;
	// 的集合
	Array<GunTargetComponent> gun_targets;

	// 大きさ（半径）
	static double const target_size;
	static double const target_hitbox_size;

	// on のときの色
	static Color const target_col_on;
	// off のときの色
	static Color const target_col_off;

public:
	GunTarget(std::shared_ptr<PhysicsElements::Wall>& wall);

	void add_gun_target(P2BodyID target, Vec2 const& pos, PhysicsElements::WallFilterType filter_on, PhysicsElements::WallFilterType filter_off = PhysicsElements::WallFilterType::Normal);

	// 的全ての判定を取得
	Array<Circle> get_target_collision();

	// 的のオンオフ切り替え
	void switch_onoff(int32 const index);

	// 任意の位置に描画
	static void draw_pos(Vec2 const pos, bool const flag_on);

	void Update();
	void Draw() const;

	// -----
	// アクセサ
	// -----

};

