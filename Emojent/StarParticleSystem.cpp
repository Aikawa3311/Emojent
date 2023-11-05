#include "stdafx.h"
#include "StarParticleSystem.h"

StarParticleSystem::StarParticleSystem()
{
}

void StarParticleSystem::emit(Vec2 const& pos)
{
	int32 num = Random(4, 7);

	// 星を追加する
	for (int32 i = 0; i < num; ++i) {
		Vec2 vec_e = RandomVec2() + Vec2::Up();
		double start_dist = Random(5.0, 30.0);
		double start_vel = Random(90.0, 180.0);
		double start_size = Random(20.0, 40.0);
		double start_angle = Random(0.0, Math::TwoPi);

		StarParticle star;

		star.pos = pos + vec_e * start_dist;
		star.pos_speed = vec_e * start_vel;
		star.pos_acc = -vec_e * 10.0 + Vec2::Down() * 400.0;
		star.size = start_size;
		star.size_speed = Random(-2.0, -1.0);
		star.size_acc = -80.0;
		star.angle = start_angle;
		star.angle_speed = Random(-10, 10);
		star.angle_acc = 0;

		star.duration.set(2.0s);
		stars.emplace_back(star);
		stars.back().duration.start();
	}
}

void StarParticleSystem::Update()
{
	Array<int32> erase_booking;

	// 星を動かす
	for (int32 i = 0; i < (int32)stars.size(); ++i) {
		auto& star = stars[i];
		// もし寿命が来たら消す予約を入れる
		if (star.duration.reachedZero()) {
			erase_booking.emplace_back(i);
		}

		// 位置・サイズ・角度変更
		star.pos += Scene::DeltaTime() * star.pos_speed;
		star.size = Max(star.size + Scene::DeltaTime() * star.size_speed, 0.0);
		star.angle += Scene::DeltaTime() * star.angle_speed;

		// 速度変更
		star.pos_speed += Scene::DeltaTime() * star.pos_acc;
		star.size_speed += Scene::DeltaTime() * star.size_acc;
		star.angle_speed += Scene::DeltaTime() * star.angle_acc;
		if (star.angle_acc < 0) {
			star.angle_speed = Max(star.angle_speed, 0.0);
		}
		else {
			star.angle_speed = Min(star.angle_speed, 0.0);
		}
	}

	// 星を消す
	for (int32 i = 0; i < (int)erase_booking.size(); ++i) {
		stars.erase(stars.begin() + erase_booking[i] - i);
	}
}

void StarParticleSystem::Draw() const
{
	for (auto const& star : stars) {
		TextureAsset(texture_name).resized(star.size).rotated(star.angle).drawAt(star.pos);
	}
}
