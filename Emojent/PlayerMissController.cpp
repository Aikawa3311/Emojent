#include "stdafx.h"
#include "PlayerMissController.h"

PhysicsElements::PlayerMissController::PlayerMissController(std::shared_ptr<P2World>& world, std::shared_ptr<Player>& player)
	: world(world),
	player(player)
{
}

void PhysicsElements::PlayerMissController::start_miss_anim()
{
	// ミスフラグを on にする
	is_missed = true;

	// 一旦操作不可にして表示を隠す
	player->set_freeze(true);
	player->set_hyde(true);

	// ミスの演出時間を設定
	anim_timer.set(SecondsF(anim_time));
	anim_timer.start();

	// ミスしたプレイヤーを出現させる
	player_miss = world->createCircle(P2Dynamic, player->get_pos(), Circle(Vec2::Zero(), player->get_body_size()), Material::PlayerMiss, Filter::PlayerMiss)
		.setVelocity(-player->get_velocity() / 1.5); // 初速はプレイヤーの速度の反対方向

	// player->get_body().set(false);
}

void PhysicsElements::PlayerMissController::Update()
{
	// タイマーが 0 になったら戻す
	if (is_missed && anim_timer.reachedZero()) {
		anim_timer.reset();

		// 墓石を立てる
		headstones << world->createRect(P2BodyType::Dynamic, player_miss.getPos(), RectF(Arg::center = Vec2::Zero(), 40, 50), Material::Headstone, Filter::Headstone)
			.setVelocity(player_miss.getVelocity());

		if (headstones.size() > 7) {
			// 古いものを撤去する
			headstones.front().release();
			headstones.pop_front();
		}

		// player_miss を消す
		player_miss.release();

		// ミスフラグを false にする
		is_missed = false;
		is_anim_end = true;
	}
}

void PhysicsElements::PlayerMissController::UpdatePhysics()
{
}

void PhysicsElements::PlayerMissController::Draw() const
{
	// ミス中のプレイヤーの描画
	if (is_missed) {
		TextureAsset(U"emoji_face_spiral_eyes").resized(50).rotated(player_miss.getAngle()).drawAt(player_miss.getPos());
		// TextureAsset(U"emoji_rotated_star").resized(40).rotated(player_miss.getAngle() - Math::QuarterPi).drawAt(player_miss.getPos() + Vec2(Sin(player_miss.getAngle()), -Cos(player_miss.getAngle())) * 30);

	}

	// 墓石の描画
	for (auto const& i : headstones) {
		TextureAsset(U"emoji_headstone").resized(50).rotated(i.getAngle()).drawAt(i.getPos());
	}
}

bool PhysicsElements::PlayerMissController::get_anim_end() const
{
	return is_anim_end;
}

bool PhysicsElements::PlayerMissController::get_missed() const
{
	return is_missed;
}

void PhysicsElements::PlayerMissController::set_anim_end(bool const flag)
{
	is_anim_end = flag;
}
