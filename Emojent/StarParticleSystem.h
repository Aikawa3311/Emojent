#pragma once
#include <Siv3D.hpp>

class StarParticleSystem
{
private:
	struct StarParticle {
		// 位置・サイズ・回転
		Vec2 pos;
		double size;
		double angle;

		// 速度
		Vec2 pos_speed;
		double size_speed;
		double angle_speed;

		// 加速度
		Vec2 pos_acc;
		double size_acc;
		double angle_acc;

		// 寿命
		Timer duration;
	};

	Array<StarParticle> stars;
	String texture_name = U"emoji_star";

public:
	StarParticleSystem();

	void emit(Vec2 const & pos);

	void Update();
	void Draw() const;
};

