#include "stdafx.h"
#include "Gun.h"

Vec2 PhysicsElements::Gun::calc_muzzle_pos() const
{
	return player->get_pos() + ray_e * (draw_offset + 10);
}

void PhysicsElements::Gun::shoot()
{
	// 音
	AudioAsset(U"pistol2").playOneShot(0.4);

	// クールダウンタイマーのスタート
	cooldown_timer.set(SecondsF(gun_cool_time));
	cooldown_timer.start();

	GunCollisionInfo collision_info = check_collision();
	if (collision_info.index != -1) {
		// 的に衝突していたら的に作用
		gun_target->switch_onoff(collision_info.index);
	}

	// 描画用の軌道を追加
	Vec2 muzzle_pos = calc_muzzle_pos();
	Line ray(muzzle_pos, collision_info.v);
	ray_line.emplace_back(ray, Timer(SecondsF(ray_duration), StartImmediately::Yes));

	// プレイヤーに力を与える
	//double impulse_mag = 1.0;
	//if (!body.getVelocity().isZero()) {
	//	// 速度ベクトルと逆方向のダッシュは倍率をかけてより強く効くようにする
	//	impulse_mag = Math::Map(dash_direction.dot(body.getVelocity().normalized()), -1.0, 1.0, 2.0, 1.0 / dash_impulse_mag) * dash_impulse_mag;
	//}
	player->get_body().applyLinearImpulse(-ray_e * recoil_speed);
}

PhysicsElements::GunCollisionInfo PhysicsElements::Gun::check_collision()
{
	double min_dist = std::numeric_limits<double>::max();
	Vec2 const& player_pos = player->get_pos();
	Vec2 muzzle_pos = calc_muzzle_pos();
	Line gun_ray(muzzle_pos, muzzle_pos + ray_e * gun_length);
	GunCollisionInfo collision_info = { gun_ray.end, -1 };

	// Wall から壁要素を受け取る
	auto const& wall_bodies = wall->get_bodies();

	// 全ての Wall について線分との交差判定を行う
	for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
		// ignore フィルタと Platform フィルタの壁は無視する
		if (wall_bodies[i].filter_type == WallFilterType::Ignore
			|| wall_bodies[i].filter_type == WallFilterType::Platform) {
			continue;
		}

		WallLineCollisionResult collision = wall->intersect_line(i, gun_ray);
		// 交差が無ければ終了
		if (!collision.is_collided) continue;
		// 交差がまだない状態 or これまでの交差より近ければ更新
		if (collision.dist < min_dist) {
			collision_info.v = collision.p;
			min_dist = collision.dist;
		}
	}
	/*
	for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
		// ignore フィルタと Platform フィルタの壁は無視する
		if (wall_bodies[i].filter_type == WallFilterType::Ignore
			|| wall_bodies[i].filter_type == WallFilterType::Platform) {
			continue;
		}

		// 壁との衝突を判定, 形によって分岐
		switch (wall_bodies[i].shape_type) {
		case WallShapeType::Circle:
		{
			// 丸と線分の交差位置を求める
			Circle const circle = wall_bodies[i].body.as<P2Circle>(0)->getCircle();
			Optional<Array<Vec2>> const pos_intersect = circle.intersectsAt(gun_ray);
			if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
			for (Vec2 const& p : pos_intersect.value()) {
				// 交差がまだない状態 or これまでの交差より近ければ更新
				double dist = p.distanceFromSq(player_pos);
				if (dist < min_dist) {
					collision_info.v = p;
					min_dist = dist;
				}
			}
		}

		break;

		case WallShapeType::Rect:
		{
			// 四角形と線分の交差位置を求める
			Quad const quad = wall_bodies[i].body.as<P2Rect>(0)->getQuad();
			Optional<Array<Vec2>> const pos_intersect = quad.intersectsAt(gun_ray);
			if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
			for (Vec2 const& p : pos_intersect.value()) {
				// 交差がまだない状態 or これまでの交差より近ければ更新
				double dist = p.distanceFromSq(player_pos);
				if (dist < min_dist) {
					collision_info.v = p;
					min_dist = dist;
				}
			}
		}

		break;

		case WallShapeType::Polygon:
			// ポリゴンと線分の交差位置を求める
			Polygon const polygon = wall_bodies[i].body.as<P2Polygon>(0)->getPolygon();
			Optional<Array<Vec2>> const pos_intersect = polygon.intersectsAt(gun_ray);
			if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
			for (Vec2 const& p : pos_intersect.value()) {
				// 交差がまだない状態 or これまでの交差より近ければ更新
				double dist = p.distanceFromSq(player_pos);
				if (dist < min_dist) {
					collision_info.v = p;
					min_dist = dist;
				}
			}

			break;
		}
	}
	*/

	// 的との衝突を判定する
	Array<Circle> targets = gun_target->get_target_collision();
	for (int32 i = 0; i < (int32)targets.size(); ++i) {
		Optional<Array<Vec2>> const pos_intersect = targets[i].intersectsAt(gun_ray);
		if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
		for (Vec2 const& p : pos_intersect.value()) {
			// 交差がまだない状態 or これまでの交差より近ければ更新
			double dist = p.distanceFromSq(player_pos);
			if (dist < min_dist) {
				// 的と衝突した場合は的の index も更新
				collision_info.v = p;
				collision_info.index = i;
				min_dist = dist;
			}
		}
	}

	return collision_info;
}

PhysicsElements::Gun::Gun(std::shared_ptr<P2World>& world, Camera2D const& camera, std::shared_ptr<Player>& player, std::shared_ptr<Wall>& wall, std::shared_ptr<Wire>& wire, std::shared_ptr<GunTarget>& gun_target)
	: world(world),
	camera(camera),
	player(player),
	wall(wall),
	wire(wire),
	gun_target(gun_target)
{

}

void PhysicsElements::Gun::Update()
{
	// 銃の射線更新
	{
		Transformer2D const transformer = camera.createTransformer();
		// プレイヤー座標 -> カーソル座標の正規ベクトル
		ray_e = (Cursor::PosF() - player->get_pos()).normalized();
	}

	// freeze したらタイマーを一時停止する
	// if(player->get_freeze())

	// ワイヤが使用中＆中空ならクールダウンタイマーを一時停止する
	/*
	if (cooldown_timer.isRunning() && wire->get_exist_wire() && !player->get_on_ground()) {
		cooldown_timer.pause();
	}
	if (cooldown_timer.isPaused() && (!wire->get_exist_wire() || player->get_on_ground())) {
		cooldown_timer.resume();
	}
	*/

	// タイマーを止める
	if (cooldown_timer.isStarted() && cooldown_timer.reachedZero()) {
		AudioAsset(U"pistol_set").playOneShot(0.4);
		cooldown_timer.reset();
	}

	// 発砲ボタンが押されたら発砲＆プレイヤーに力を与える
	if (!is_freeze && !player->get_freeze() && !cooldown_timer.isStarted() && Scene::Rect().intersects(Cursor::Pos()) && MouseL.down()) {
		shoot();
	}

	// 軌跡を消す
	Erase_if(ray_line, [](auto const& r) { return r.second.reachedZero(); });
}

void PhysicsElements::Gun::Draw() const
{
	// 弾の軌跡の描画
	for (auto const& i : ray_line) {
		double t = i.second.sF() / ray_duration;
		i.first.draw(2, { Palette::Ghostwhite, t});
	}

	// 銃の描画
	if (!is_hyde_gun) {
		double angle = Atan2(-ray_e.x, -ray_e.y);
		// TextureAsset(U"icon_gun").rotated(Math::HalfPi - angle).drawAt(body.getPos(), Palette::Yellow);
		TextureAsset(U"emoji_gun").resized(50).flipped(angle < 0.0).rotated(Math::HalfPi - angle).drawAt(player->get_pos() + ray_e * draw_offset);
	}

	// ダッシュのクールタイムを描画
	if (cooldown_timer.isStarted()) {
		Circle(player->get_pos(), player->get_body_size() * 1.4).drawArc(0, -(cooldown_timer.sF() / gun_cool_time) * Math::TwoPi, 0, 5, Palette::Red);
	}
}

bool PhysicsElements::Gun::get_is_cooldown() const
{
	return cooldown_timer.isRunning();
}

void PhysicsElements::Gun::set_hyde_gun(bool const flag)
{
	is_hyde_gun = flag;
}

void PhysicsElements::Gun::set_freeze(bool const flag)
{
	is_freeze = flag;
}
