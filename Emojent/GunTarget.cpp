#include "stdafx.h"
#include "GunTarget.h"

Color const GunTarget::target_col_on = Palette::Red;
Color const GunTarget::target_col_off = Palette::Blue;
double const GunTarget::target_size = 25;
double const GunTarget::target_hitbox_size = 45;

GunTarget::GunTarget(std::shared_ptr<PhysicsElements::Wall>& wall)
	: wall(wall)
{
}

void GunTarget::add_gun_target(P2BodyID target, Vec2 const& pos, PhysicsElements::WallFilterType filter_on, PhysicsElements::WallFilterType filter_off)
{
	GunTargetComponent gun_target;
	gun_target.target = target;
	gun_target.pos = pos;
	gun_target.filter_on = filter_on;
	gun_target.filter_off = filter_off;
	gun_target.is_on = false;
	gun_targets.emplace_back(gun_target);
}

Array<Circle> GunTarget::get_target_collision()
{
	Array<Circle> result;

	for (int i = 0; i < (int)gun_targets.size(); ++i) {
		result.emplace_back(Circle(gun_targets[i].pos, target_hitbox_size));
	}

    return result;
}

void GunTarget::switch_onoff(int32 const index)
{
	// on off を切り替えて壁の状態を変更
	PhysicsElements::WallFilterType filter = gun_targets[index].is_on ? gun_targets[index].filter_off : gun_targets[index].filter_on;
	gun_targets[index].is_on = !gun_targets[index].is_on;
	wall->change_filter(gun_targets[index].target, filter);
	// 音を鳴らす
	AudioAsset(U"gun_target").playOneShot(0.5);
}

void GunTarget::draw_pos(Vec2 const pos, bool const flag_on)
{
	Color col1 = (flag_on ? target_col_on : target_col_off).lerp(Palette::White, 0.5);
	Color col2 = (flag_on ? target_col_on : target_col_off).lerp(Palette::White, 0.3);
	Color col3 = (flag_on ? target_col_on : target_col_off).lerp(Palette::White, 0.1);
	// Quad(pos.movedBy(0, -target_size), pos.movedBy(-target_size, 0), pos.movedBy(0, target_size), pos.movedBy(target_size, 0)).draw(flag_on ? target_col_on : target_col_off).drawFrame(4, Palette::Darkgray);
	Quad(pos.movedBy(0, -target_size), pos.movedBy(-target_size, 0), pos.movedBy(0, target_size), pos.movedBy(target_size, 0)).draw(col1, col2, col3, col2).drawFrame(4, Palette::Darkgray);
}

void GunTarget::Update()
{
}

void GunTarget::Draw() const
{
	for (auto const& i : gun_targets) {
		draw_pos(i.pos, i.is_on);
	}
}
