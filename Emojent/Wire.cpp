#include "stdafx.h"
#include "Wire.h"

void PhysicsElements::Wire::init()
{
	// 壁一覧から wall_vertices を作る
	//wall_vertices.clear();

	//auto const& wall_bodies = wall->get_bodies();
	//for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
	//	switch (wall_bodies[i].second) {
	//	case WallShapeType::Circle:
	//		{
	//			// 円の頂点を追加
	//			Circle const circle = wall_bodies[i].first.as<P2Circle>(0)->getCircle();
	//			constexpr int32 reso = Global::circle_resolution;
	//			for (int32 j = 0; j < reso; ++j) {
	//				wall_vertices.emplace_back(circle.center + circle.r * Vec2(Cos(j / reso * Math::TwoPi), Sin(j / reso * Math::TwoPi)), i);
	//			}
	//		}

	//		break;

	//	case WallShapeType::Rect:
	//	{
	//		// Rect の頂点を追加
	//		Quad const quad = wall_bodies[i].first.as<P2Rect>(0)->getQuad();
	//		for (int32 j = 0; j < 4; ++j) {
	//			wall_vertices.emplace_back(quad.p(j), i);
	//		}
	//	}
	//		break;

	//	case WallShapeType::Polygon:
	//		// Polygon の頂点を追加
	//		Polygon const polygon = wall_bodies[i].first.as<P2Polygon>(0)->getPolygon();
	//		for (auto const& v : polygon.vertices()) {
	//			wall_vertices.emplace_back(v, i);
	//		}

	//		break;
	//	}
	//}
}

void PhysicsElements::Wire::check_collision()
{
	collision_info = { Vec2::Zero(), -1};
	double min_dist = std::numeric_limits<double>::max();
	// Vec2 const& player_pos = player->get_pos();

	// Wall から壁要素を受け取る
	auto const & wall_bodies = wall->get_bodies();

	// 全ての Wall について線分との交差判定を行う
	for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
		// ignore フィルタと Platform フィルタの壁は無視する
		if (wall_bodies[i].filter_type == WallFilterType::Ignore
			|| wall_bodies[i].filter_type == WallFilterType::Platform) {
			continue;
		}

		WallLineCollisionResult collision = wall->intersect_line(i, wire_sensor);
		// 交差が無ければ終了
		if (!collision.is_collided) continue;
		// 交差がまだない状態 or これまでの交差より近ければ更新
		if (collision_info.second == -1 || collision.dist < min_dist) {
			collision_info.first = collision.p;
			collision_info.second = i;
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
				Optional<Array<Vec2>> const pos_intersect = circle.intersectsAt(wire_sensor);
				if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
				for (Vec2 const& p : pos_intersect.value()) {
					// 交差がまだない状態 or これまでの交差より近ければ更新
					double dist = p.distanceFromSq(player_pos);
					if (collision_info.second == -1 || dist < min_dist) {
						collision_info.first = p;
						collision_info.second = i;
						min_dist = dist;
					}
				}
			}

			break;

		case WallShapeType::Rect:
			{
				// 四角形と線分の交差位置を求める
				Quad const quad = wall_bodies[i].body.as<P2Rect>(0)->getQuad();
				Optional<Array<Vec2>> const pos_intersect = quad.intersectsAt(wire_sensor);
				if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
				for (Vec2 const& p : pos_intersect.value()) {
					// 交差がまだない状態 or これまでの交差より近ければ更新
					double dist = p.distanceFromSq(player_pos);
					if (collision_info.second == -1 || dist < min_dist) {
						collision_info.first = p;
						collision_info.second = i;
						min_dist = dist;
					}
				}
			}

			break;

		case WallShapeType::Polygon:
			// ポリゴンと線分の交差位置を求める
			Polygon const polygon = wall_bodies[i].body.as<P2Polygon>(0)->getPolygon();
			Optional<Array<Vec2>> const pos_intersect = polygon.intersectsAt(wire_sensor);
			if (!pos_intersect.has_value() || pos_intersect.value().size() == 0) continue;
			for (Vec2 const& p : pos_intersect.value()) {
				// 交差がまだない状態 or これまでの交差より近ければ更新
				double dist = p.distanceFromSq(player_pos);
				if (collision_info.second == -1 || dist < min_dist) {
					collision_info.first = p;
					collision_info.second = i;
					min_dist = dist;
				}
			}

			break;
		}
	}
	*/

}

bool PhysicsElements::Wire::wire_bending_add()
{
	if (!is_exist_wire) return false;

	// 追加候補点
	Array<WireBendInfo> bend_booking;
	Vec2 const& now_pos = player->get_pos();
	Vec2 const& pre_pos = wire_line_pre.begin;

	// 屈曲の追加
	auto const& wall_bodies = wall->get_bodies();
	for (int32 i = 0; i < (int32)wall_bodies.size(); ++i) {
		// ignore フィルタ, Platform フィルタの壁は無視する
		if (wall_bodies[i].filter_type == WallFilterType::Ignore
			|| wall_bodies[i].filter_type == WallFilterType::Platform) {
			continue;
		}

		// 壁の頂点位置の計算
		Array<Vec2> vertices = wall->nth_wall_vertices(i);
		// clockwise の確認
		for (auto const& v : vertices) {
			// 頂点がワイヤ始点とプレイヤーの間にあるかチェック
			double dist = now_pos.distanceFromSq(wire_joint_bend.back().v);
			if (v.distanceFromSq(now_pos) >= dist || v.distanceFromSq(wire_joint_bend.back().v) >= dist) {
				continue;
			}

			bool cw_now = Geometry2D::IsClockwise(wire_joint.getAnchorPosB(), v, now_pos);
			bool cw_pre = Geometry2D::IsClockwise(wire_joint.getAnchorPosB(), v, pre_pos);
			if (cw_now != cw_pre) {
				// 屈曲の候補点を検出、とりあえず予約
				bend_booking.emplace_back(v, i, cw_now);
			}
		}
	}

	// 得られた頂点について、ワイヤの始点・プレイヤー座標と凸法を作るように頂点を追加
	if (bend_booking.isEmpty()) return false;
	bool cw = Geometry2D::IsClockwise(wire_joint.getAnchorPosB(), pre_pos, now_pos);

	// 距離ソート
	Vec2 vec_e = (now_pos - wire_joint_bend.back().v).normalized();
	vec_e.rotate90();
	std::sort(bend_booking.begin(), bend_booking.end(), [&](WireBendInfo const& lhs, WireBendInfo const& rhs) {
		double len1 = Abs(lhs.v.cross(vec_e)), len2 = Abs(rhs.v.cross(vec_e));
		return len1 < len2;
	});

	// 候補頂点から実際に頂点を追加していく
	for (int32 i = 0; i < (int32)bend_booking.size(); ++i) {
		// 最も後ろが none, あるいは異なる cw である場合は処理しない
		while (wire_joint_bend.back().cw.has_value() && wire_joint_bend.back().cw == cw && wire_joint_bend.size() >= 2) {
			// 反対周りになっている点は取り除く
			Vec2 const& v0 = wire_joint_bend[wire_joint_bend.size() - 2].v;
			Vec2 const& v1 = wire_joint_bend.back().v;
			Vec2 const& v2 = bend_booking[i].v;

			// 3 点が一直線上にある場合は消す
			if ((v1 - v0).cross(v2 - v0) == 0.0) {
				wire_joint_bend.pop_back();
			}
			else if (Geometry2D::IsClockwise(v0, v1, v2) == cw) {
				// cw が変わらなければ取り除かない
				break;
			}
			else {
				// cw が判定していたら頂点を取り除く
				wire_joint_bend.pop_back();
			}
		}
		// 追加
		wire_joint_bend.emplace_back(bend_booking[i]);
	}
	// 最後に追加された頂点について cw を判定
	while (wire_joint_bend.size() >= 2) {
		Vec2 const& v0 = wire_joint_bend[wire_joint_bend.size() - 2].v;
		Vec2 const& v1 = wire_joint_bend.back().v;
		Vec2 const& v2 = now_pos;
		if (Geometry2D::IsClockwise(v0, v1, v2) == cw) {
			// cw が変わらなければ取り除かない
			break;
		}
		// cw が判定していたら頂点を取り除く
		wire_joint_bend.pop_back();
	}

	// 9. ジョイントの生成しなおし
	auto const& wire_begin = wire_joint_bend.back();
	double wire_length = player->get_pos().distanceFrom(wire_begin.v);
	wire_joint = world->createDistanceJoint(player->get_body(), player->get_pos(), wall->get_nth_wall_body(wire_begin.wall_id), wire_begin.v, wire_length, EnableCollision::Yes);
	wire_joint.setMinLength(wire_length / 1);

	// 屈曲が発生したら少しだけ速度を上げる
	Vec2 dir = player->get_velocity().normalized();
	player->get_body().applyLinearImpulse(dir * bend_adding_force);

	// 音を鳴らす
	AudioAsset(U"bend" + Format(Clamp((int32)wire_joint_bend.size() - 1, 1, 8))).playOneShot(0.4);

	return true;
}

bool PhysicsElements::Wire::wire_bending_unwind()
{
	if (!is_exist_wire) return false;

	// 屈曲の解消
	bool flag_change = false;
	while (wire_joint_bend.size() >= 2) {
		// clockwise の確認
		Vec2 const& p1 = wire_joint_bend[wire_joint_bend.size() - 2].v;
		Vec2 const& p2 = wire_joint_bend.back().v;
		Vec2 const& p3 = player->get_pos();

		if (Geometry2D::IsClockwise(p1, p2, p3) == wire_joint_bend.back().cw) {
			// 同じなら解消は発生しない
			break;
		}
		// clockwise が異なるので消す
		wire_joint_bend.pop_back();
		flag_change = true;
	}
	if (flag_change) {
		// ジョイントの生成しなおし
		double wire_length = player->get_pos().distanceFrom(wire_joint_bend.back().v);
		wire_joint = world->createDistanceJoint(player->get_body(), player->get_pos(), wall->get_nth_wall_body(wire_joint_bend.back().wall_id), wire_joint_bend.back().v, wire_length, EnableCollision::Yes);
		wire_joint.setMinLength(wire_length / 1);

		// 屈曲が解消されたら少しだけ速度を上げる
		Vec2 dir = player->get_velocity().normalized();
		player->get_body().applyLinearImpulse(dir * bend_unwind_force);

		// 音を鳴らす
		AudioAsset(U"bend" + Format(Clamp((int32)wire_joint_bend.size() - 1, 1, 8))).playOneShot(0.4);

		return true;
	}

	return false;
}

PhysicsElements::Wire::Wire(std::shared_ptr<P2World>& world, Camera2D const& camera, std::shared_ptr<Player>& player, std::shared_ptr<Wall>& wall)
	: world(world),
	camera(camera),
	player(player),
	wall(wall)
{
	// wire_test = world->createLine(P2BodyType::Static, Vec2(10, 10), Line(Vec2(0, 0), Vec2(50, 0)), OneSided::Yes, P2Material(), Filter::WireLineSensor);
	// wire_test = world->createRect(P2BodyType::Static, Vec2(-400, -400), RectF(Vec2(0, 0), Vec2(800, 800)), P2Material(), P2Filter{.categoryBits = 0b0001'0000'0000'0000, .maskBits = 0b1111'1111'1111'1111});
	init();
}

void PhysicsElements::Wire::wire_reset()
{
	// ワイヤを消す
	wire_joint.release();

	// 屈曲を全消去
	wire_joint_bend.clear();

	// ワイヤ使用中フラグをオフ
	is_exist_wire = false;
}

void PhysicsElements::Wire::Update()
{
	// 旧ワイヤ判定
	// Line wire_line(player->get_pos(), Cursor::PosF());
	// if (!wire_sensor.isEmpty()) wire_sensor.release();
	// {
	// 	Transformer2D const transformer = camera.createTransformer();
	// 	wire_sensor = world->createLine(P2BodyType::Static, player->get_pos(), Line(Vec2(0, 0), Cursor::PosF() - player->get_pos()), OneSided::Yes, P2Material(), Filter::WireLineSensor);
	// }
	// wire_sensor.release();

	// ワイヤのセンサ更新
	{
		Transformer2D const transformer = camera.createTransformer();
		// プレイヤー座標 -> カーソル座標の正規ベクトル
		Vec2 e = (Cursor::PosF() - player->get_pos()).normalized();
		
		wire_sensor.begin = player->get_pos();
		wire_sensor.end = player->get_pos() + e * 600;
	}

	// センサと壁の接触を判定
	check_collision();

	// プレイヤーのセンサ更新
	player->set_exist_wire(is_exist_wire);
	Vec2 const wire_dir = (wire_joint.getAnchorPosA() - wire_joint.getAnchorPosB()).normalized();
	player->set_wire_direction(Vec2(wire_dir.y, -wire_dir.x));

	// クリックでワイヤ操作
	if (!player->get_freeze() && MouseR.down()) {
		// ワイヤが既に伸びていればワイヤを消す
		if (is_exist_wire) {
			wire_reset();
			// 音を鳴らす
			AudioAsset(U"wire").playOneShot(0.5, 0.0, 0.8);
		}
		// まだワイヤが伸びていなければワイヤを伸ばす
		else if(collision_info.second != -1){
			// 音を鳴らす
			AudioAsset(U"wire").playOneShot(0.5);

			// ワイヤを生成
			double wire_length = player->get_pos().distanceFrom(collision_info.first);
			wire_joint = world->createDistanceJoint(player->get_body(), player->get_pos(), wall->get_nth_wall_body(collision_info.second), collision_info.first, wire_length, EnableCollision::Yes);
			wire_joint.setMinLength(wire_length / 2);

			// ワイヤの屈曲の根元を追加
			wire_joint_bend.emplace_back(collision_info.first, collision_info.second);
			// 前フレームのワイヤ状態を初期化
			wire_line_pre.begin = wire_joint.getAnchorPosA();
			wire_line_pre.end = wire_joint.getAnchorPosB();
			// ワイヤ使用中フラグ
			is_exist_wire = true;
		}
	}

	// ワイヤ屈曲
	if (!wire_bending_add()) {
		// 屈曲が無い場合は解消
		wire_bending_unwind();
	}

	// 前フレームのワイヤの状態を更新
	if (is_exist_wire) {
		wire_line_pre.begin = wire_joint.getAnchorPosA();
		wire_line_pre.end = wire_joint.getAnchorPosB();
	}
}

void PhysicsElements::Wire::UpdatePhysics()
{
}

void PhysicsElements::Wire::Draw() const
{
	// プレイヤーからカーソルまでの線
	// Line(player->get_pos(), Cursor::PosF()).draw(LineStyle::SquareDot(Scene::Time() * 20), 5, { Palette::Green, 0.6 });

	// （デバグ用）物理演算ライン
	// P2Body sensor = world->createLine(P2BodyType::Static, player->get_pos(), Line(Vec2(0, 0), Cursor::PosF() - player->get_pos()), OneSided::Yes, P2Material(), Filter::WireLineSensor);
	// sensor.draw(Palette::Red);
	// Print << sensor.getPos();
	// sensor.release();

	// wire_sensor.draw(Palette::Red);
	// P2BodyID wire_id = wire_sensor.id();
	// auto const& wall_ids = wall->get_wall_ids();

	//Print << U"接触数：{}"_fmt(world->getCollisions().size());
	//for (auto&& [pair, collision] : world->getCollisions()) {
	//	if (pair.a == wire_id) {
	//		if (!wall_ids.contains(pair.b)) continue;
	//		collision.contact(0).point;
	//		flag = true;
	//		break;
	//	}
	//	else if(pair.b == wire_id) {
	//		if (!wall_ids.contains(pair.a)) continue;
	//		flag = true;
	//		break;
	//	}

	//	// テスト用：
	//	if (pair.a == wire_test.id()) {
	//		Print << U"接触中!!!";
	//		if (!wall_ids.contains(pair.b)) continue;
	//		break;
	//	}
	//	else if (pair.b == wire_test.id()) {
	//		Print << U"接触中!!!";
	//		if (!wall_ids.contains(pair.a)) continue;
	//		break;
	//	}
	//}

	//Print << U"ワイヤID：{}"_fmt(wire_id);
	//Print << U"接触：{}"_fmt(flag);

	//if (flag == false) {
	//	wire_sensor.draw(Palette::Red);
	//}
	// wire_test.draw(Palette::Red);

	/*wire_sensor.draw(Palette::Red);
	Print << collision_info;
	if (collision_info.second != -1) {
		Circle(collision_info.first, 20).drawFrame(5, Palette::Red);
	}*/

	// Print << U"ワイヤ中：{}"_fmt(is_exist_wire);
	// Print << U"屈曲数：{}"_fmt(wire_joint_bend.size());

	// Print << U"頂点数：{}"_fmt(wall_vertices.size());

	// TextureAsset(U"emoji_grab_hand").resized(40).rotated(Scene::Time()*0.8).draw(Vec2(200, 60));

	if (is_exist_wire) {
		for (int32 i = 1; i < (int32)wire_joint_bend.size(); ++i) {
			Line(wire_joint_bend[i - 1].v, wire_joint_bend[i].v).draw(4.0, Palette::Darkorange);
		}
		Line{ wire_joint.getAnchorPosA(), wire_joint.getAnchorPosB() }.draw(LineStyle::SquareDot, 4.0, Palette::Darkorange);

		// 手を描く
		if (flag_draw_hand) {
			Vec2 vec = wire_joint.getAnchorPosB() - wire_joint.getAnchorPosA();
			if (vec.lengthSq() > 1600) {
				double angle = Atan2(vec.x, vec.y);
				// Print << angle;
				TextureAsset(U"emoji_grab_hand").resized(40).rotated(Math::HalfPi - angle).drawAt(player->get_pos() + vec.normalized() * 40);
			}

		}
	}
	else if(debug_draw_wire_guide){
		wire_sensor.draw(4, { Palette::Red, 0.6 });
		// Print << collision_info;
		if (collision_info.second != -1) {
			Circle(collision_info.first, 20).drawFrame(5, Palette::Red);
		}
	}
}

bool PhysicsElements::Wire::get_exist_wire() const
{
	return is_exist_wire;
}

