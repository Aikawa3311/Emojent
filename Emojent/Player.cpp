#include "stdafx.h"
#include "Player.h"

PhysicsElements::Player::Player(std::shared_ptr<P2World>& world, Camera2D const & camera)
	: world(world),
	camera(camera)
{
	
}

void PhysicsElements::Player::set_pos(Vec2 const & world_pos)
{
	body.setPos(world_pos);
}

void PhysicsElements::Player::reset(Vec2 const& world_pos)
{
	// プレイヤーの P2Body を生成しなおし
	body = world->createCircle(P2Dynamic, world_pos, Circle(Vec2{ 0 , 0 }, body_size), PhysicsElements::Material::Player, PhysicsElements::Filter::Player);
	body.setFixedRotation(true);
	// プレイヤーの足元センサーを生成しなおし
	// foot_body = world->createCircleSensor(P2Dynamic, world_pos.movedBy(0, 6), 16, PhysicsElements::Filter::PlayerFoot);
	double sensor_size = body_size * 0.8;
	foot_body = world->createCircleSensor(P2Dynamic, world_pos.movedBy(0, (body_size - sensor_size)/2 + 4), sensor_size, PhysicsElements::Filter::PlayerFoot);
	foot_body.setGravityScale(0.0);
	foot_body.setFixedRotation(true);
	// プレイヤーの body と foot_body をくっつけるジョイントを生成しなおし
	foot_joint = world->createDistanceJoint(body, world_pos.movedBy(0, 0), foot_body, world_pos.movedBy(0, 0), 0);
}

void PhysicsElements::Player::Update(){
	// Print << U"速度：{:.2f}"_fmt(body.getVelocity());

	// 表情変化
	if (!body.getVelocity().isZero()) {
		if (body.getVelocity().length() > 300) {
			// expression = U"emoji_face_spiral_eyes";
		}
		else {
			// expression = U"emoji_face_sanglass";
		}
		expression_timer.reset();
	}
	else {
		/*if (!expression_timer.isRunning()) {
			expression_timer.set(SecondsF(idle_time));
		}*/
	}
	/*if (KeyT.down()) {
		expression = U"emoji_face_thinking";
	}
	if (KeyY.down()) {
		expression = U"emoji_face_sanglass";
	}*/

	// キー入力によるジャンプ
	if (!is_freeze && on_ground && (KeyUp.down() || KeyW.down())) {
		// 音を鳴らす
		AudioAsset(U"jump").playOneShot(1.0);
		body.applyLinearImpulse(Vec2(0, -jump_speed));
	}
	
	player_angle += body.getVelocity().x * Scene::DeltaTime() * 0.025;
}

void PhysicsElements::Player::UpdatePhysics()
{
	// foot_body.setPos(Cursor::PosF().movedBy(0, -600));
	// foot_body.setVelocity(Vec2(0, 0));

	// 一定以上の速度が出せないように抵抗値を設ける
	Vec2 move_resistance = { -move_horizontal_resistance_val * body.getVelocity().x, 0.0 };

	// キー入力によって移動
	// 左
	if (!is_freeze && (KeyLeft.pressed() || KeyA.pressed())) {
		// double minus_speed = Vec2();
		Vec2 move_force = (Vec2(-speed, 0) + move_resistance) * Global::physics_step_time;
		// ワイヤ on & 空中にいるときは反時計回りに力を与える
		/*if (is_exist_wire && !!on_ground) {
			move_force = wire_direction * ;
		}*/
		// 空中にいるときは制御を利きづらくする
		if (!on_ground) move_force *= move_air_horizontal_resistance_mag;
		body.applyForce(move_force);
	}
	// 右
	else if (!is_freeze && (KeyRight.pressed() || KeyD.pressed())) {
		Vec2 move_force = (Vec2(speed, 0) + move_resistance) * Global::physics_step_time;
		// 空中にいるときは制御を利きづらくする
		if (!on_ground) move_force *= move_air_horizontal_resistance_mag;
		body.applyForce(move_force);
	}
	else {
		if (on_ground) {
			// キー操作がなく地面に接している場合、抵抗値によってブレーキをかける
			body.applyForce(move_resistance * Global::physics_step_time);
		}
	}

	if (is_freeze) {
		body.setVelocity(Vec2::Zero());
	}

	// 上に上昇中（ジャンプ中）の際は下から通り抜けられる足場壁を通り抜ける
	const bool jumping = ((!on_ground) && (body.getVelocity().y < -0.1));
	foot_body.shape(0).setFilter(jumping ? Filter::PlayerJumpingFoot : Filter::PlayerFoot);
	body.shape(0).setFilter(jumping ? Filter::PlayerJumping : Filter::Player);
	// setFootFilter(jumping ? Filter::PlayerFootWhileJumping : Filter::PlayerFoot);
}

void PhysicsElements::Player::Draw() const
{
	//
	Vec2 dash_direction = (body.getPos() - Cursor::PosF()).normalized();

	// Print << U"座標：{:.2f}"_fmt(body.getPos());
	// Print << U"速度：{:.2f}"_fmt(body.getVelocity());

	// Print << U"{:.2f}"_fmt(body.getVelocity().normalized());
	// Print << U"{:.2f}"_fmt(dash_direction.dot(body.getVelocity().normalized()));
	// double impulse_mag = Math::Map(dash_direction.dot(body.getVelocity().normalized()), -1.0, 1.0, 2.0, 1.0 / dash_impulse_mag) * dash_impulse_mag;
	// Print << U"{:.2f}"_fmt(impulse_mag);

	if (is_hyde) {
		return;
	}

	// テクスチャを描画
	TextureAsset(expression).resized(50).rotated(player_angle).drawAt(body.getPos());

	// （デバグ）P2Body の輪郭を描画
	// body.drawFrame(2.0, { Palette::Red, 0.5 });
	// foot_body.drawFrame(2.0, { Palette::Green, 0.5 });

	// （デバグ）カーソルとプレイヤーを結ぶ線分
	// Circle(Cursor::PosF(), 20).draw(Palette::Red);
	// Line(body.getPos(), Cursor::PosF()).drawArrow(5, SizeF(10.0, 10.0), Palette::Green);
}

P2Body const& PhysicsElements::Player::get_body() const
{
	return body;
}

P2Body & PhysicsElements::Player::get_body()
{
	return body;
}

P2BodyID PhysicsElements::Player::get_body_id() const
{
	return body.id();
}

P2BodyID PhysicsElements::Player::get_foot_body_id() const
{
	return foot_body.id();
}

Vec2 PhysicsElements::Player::get_pos() const
{
	return body.getPos();
}

Vec2 PhysicsElements::Player::get_velocity() const
{
	return body.getVelocity();
}

double PhysicsElements::Player::get_body_size() const
{
	return body_size;
}

double PhysicsElements::Player::get_angle() const
{
	return player_angle;
}

bool PhysicsElements::Player::get_on_ground() const
{
	return on_ground;
}

bool PhysicsElements::Player::get_freeze() const
{
	return is_freeze;
}

bool PhysicsElements::Player::get_hyde() const
{
	return is_hyde;
}

void PhysicsElements::Player::set_on_ground(bool const flag)
{
	on_ground = flag;
}

void PhysicsElements::Player::set_expression(String const& str)
{
	expression = str;
}

void PhysicsElements::Player::set_freeze(bool const flag)
{
	is_freeze = flag;
}

void PhysicsElements::Player::set_hyde(bool const flag)
{
	is_hyde = flag;
}

void PhysicsElements::Player::set_hyde_gun(bool const flag)
{
	is_hyde_gun = flag;
}

void PhysicsElements::Player::set_wire_direction(Vec2 const& vec)
{
	wire_direction = vec;
}

void PhysicsElements::Player::set_exist_wire(bool const flag)
{
	is_exist_wire = flag;
}


