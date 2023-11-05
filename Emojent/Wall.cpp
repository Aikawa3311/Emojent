#include "stdafx.h"
#include "Wall.h"

PhysicsElements::Wall::Wall(std::shared_ptr<P2World>& world)
	: world(world)
{
}

P2BodyID PhysicsElements::Wall::addRect(Vec2 const& world_pos, SizeF const& size)
{
	bodies.emplace_back(world->createRect(P2Static, world_pos, RectF(Vec2(0, 0), size), PhysicsElements::Material::Wall, PhysicsElements::Filter::Wall).setAwake(true), WallShapeType::Rect, WallFilterType::Normal);
	// wall_ids.emplace(bodies.back().first.id());
	wall_ids_body.emplace(bodies.back().body.id(), (int32)bodies.size() - 1);

	return bodies.back().body.id();
}

P2BodyID PhysicsElements::Wall::addRectAt(Vec2 const& world_pos, SizeF const& size)
{
	bodies.emplace_back(world->createRect(P2Static, world_pos, size, PhysicsElements::Material::Wall, PhysicsElements::Filter::Wall).setAwake(true), WallShapeType::Rect, WallFilterType::Normal);
	// wall_ids.emplace(bodies.back().first.id());
	wall_ids_body.emplace(bodies.back().body.id(), (int32)bodies.size() - 1);

	return bodies.back().body.id();
}

P2BodyID PhysicsElements::Wall::addCircle(Vec2 const& world_pos, double const r)
{
	bodies.emplace_back(world->createCircle(P2Static, world_pos, r, PhysicsElements::Material::Wall, PhysicsElements::Filter::Wall).setAwake(true), WallShapeType::Circle, WallFilterType::Normal);
	wall_ids_body.emplace(bodies.back().body.id(), (int32)bodies.size() - 1);

	return bodies.back().body.id();
}

P2BodyID PhysicsElements::Wall::addTriangle(Vec2 const& world_pos, Vec2 const& v0, Vec2 const& v1, Vec2 const& v2)
{
	bodies.emplace_back(world->createTriangle(P2Static, world_pos, Triangle(v0, v1, v2), PhysicsElements::Material::Wall, PhysicsElements::Filter::Wall).setAwake(true), WallShapeType::Triangle, WallFilterType::Normal);
	wall_ids_body.emplace(bodies.back().body.id(), (int32)bodies.size() - 1);

	return bodies.back().body.id();
}

void PhysicsElements::Wall::change_filter(P2BodyID const id, WallFilterType const filter)
{
	// 配列で何番目か取得
	int32 index = wall_ids_body[id];
	int32 n = (int32)bodies[index].body.num_shapes();

	// フィルタの変更
	switch (filter) {
	case WallFilterType::Normal:
		for (int32 i = 0; i < n; ++i) {
			bodies[index].body.shape(i).setFilter(Filter::Wall);
		}
		break;

	case WallFilterType::Ignore:
		for (int32 i = 0; i < n; ++i) {
			bodies[index].body.shape(i).setFilter(Filter::WallIgnore);
		}
		break;

	case WallFilterType::Platform:
		for (int32 i = 0; i < n; ++i) {
			bodies[index].body.shape(i).setFilter(Filter::WallPlatform);
		}
		break;
	}

	// フィルタータイプを変更
	bodies[index].filter_type = filter;
}

Array<Vec2> PhysicsElements::Wall::nth_wall_vertices(int32 const n) const
{
	Array<Vec2> vertices;
	switch (bodies[n].shape_type) {
	case WallShapeType::Circle:
		{
			// 円の頂点を追加
			Circle const circle = bodies[n].body.as<P2Circle>(0)->getCircle();
			constexpr int32 reso = Global::circle_resolution;
			for (int32 i = 0; i < reso; ++i) {
				vertices.emplace_back(circle.center + circle.r * Vec2(Cos((double)i / reso * Math::TwoPi), Sin((double)i / reso * Math::TwoPi)));
			}
		}

		break;

	case WallShapeType::Rect:
		{
			// Rect の頂点を追加
			Quad const quad = bodies[n].body.as<P2Rect>(0)->getQuad();
			for (int32 i = 0; i < 4; ++i) {
				vertices.emplace_back(quad.p(i));
			}
		}

		break;

	case WallShapeType::Triangle:
		{
			// 三角形の頂点を追加
			Triangle const tri = bodies[n].body.as<P2Triangle>(0)->getTriangle();
			for (int32 i = 0; i < 3; ++i) {
				vertices.emplace_back(tri.p(i));
			}
		}

		break;

	case WallShapeType::Polygon:
		// Polygon の頂点を追加
		Polygon const polygon = bodies[n].body.as<P2Polygon>(0)->getPolygon();
		for (auto const& v : polygon.vertices()) {
			vertices.emplace_back(v);
		}

		break;
	}

	return vertices;
}

Vec2 PhysicsElements::Wall::nth_wall_barycenter(int32 const n) const
{
	Vec2 result = Vec2::Zero();
	Array<Vec2> v = nth_wall_vertices(n);
	for (auto const& i : v) {
		result += i;
	}
	return result / (double)v.size();
}

PhysicsElements::WallLineCollisionResult PhysicsElements::Wall::intersect_line(int const n, Line const& line)
{
	WallLineCollisionResult result;
	result.dist = std::numeric_limits<double>::max();
	if (n >= bodies.size()) return { false, -1.0, Vec2::Zero() };

	// 壁との衝突を判定, 形によって分岐
	Optional<Array<Vec2>> pos_intersect;
	switch (bodies[n].shape_type) {
	case WallShapeType::Circle:
		{
			// 丸と線分の交差位置を求める
			Circle const circle = bodies[n].body.as<P2Circle>(0)->getCircle();
			pos_intersect = circle.intersectsAt(line);
		}

		break;

	case WallShapeType::Rect:
		{
			// 四角形と線分の交差位置を求める
			Quad const quad = bodies[n].body.as<P2Rect>(0)->getQuad();
			pos_intersect = quad.intersectsAt(line);
		}

		break;

	case WallShapeType::Triangle:
		{
			// 三角形と線分の交差位置を求める
			Triangle const tri = bodies[n].body.as<P2Triangle>(0)->getTriangle();
			pos_intersect = tri.intersectsAt(line);
		}

		break;

	case WallShapeType::Polygon:
		// ポリゴンと線分の交差位置を求める
		Polygon const polygon = bodies[n].body.as<P2Polygon>(0)->getPolygon();
		pos_intersect = polygon.intersectsAt(line);

		break;
	}

	// 交差が無ければ終了
	if (!pos_intersect.has_value()) return { false, -1.0, Vec2::Zero() };
	// 交差している点の内 line の始点から最も近いものを採択
	for (Vec2 const& p : pos_intersect.value()) {
		// 近ければ更新
		double dist = p.distanceFromSq(line.begin);
		if (dist < result.dist) {
			result.dist = dist;
			result.is_collided = true;
			result.p = p;
		}
	}

	return result;
}

void PhysicsElements::Wall::Update()
{
	
}

void PhysicsElements::Wall::UpdatePhysics()
{
}

void PhysicsElements::Wall::Draw() const
{
	/*for (int32 i = 0; i < (int)bodies.size(); ++i) {
		if (bodies_filter[i] == WallFilterType::Normal) {
			bodies[i].body.draw(col);
		}
		else if (bodies_filter[i] == WallFilterType::Ignore) {
			bodies[i].body.draw({ col, 0.3 });
		}
	}*/

	draw_normal_walls();
	draw_ignore_walls();
	draw_danger_walls(col, Palette::Red);
}

void PhysicsElements::Wall::draw_normal_walls() const
{
	// 普通の壁のみ描画
	for (int32 i = 0; i < (int)bodies.size(); ++i) {
		if (bodies[i].filter_type == WallFilterType::Normal) {
			bodies[i].body.draw(col);
		}
	}
}

void PhysicsElements::Wall::draw_ignore_walls() const
{
	// 通り抜けられる壁のみ描画
	for (int32 i = 0; i < (int)bodies.size(); ++i) {
		if (bodies[i].filter_type == WallFilterType::Ignore) {
			bodies[i].body.draw({ col, 0.2 });
		}
	}
}

void PhysicsElements::Wall::draw_danger_walls(Color const& col1, Color const& col2) const
{
	// 危険な壁のみ描画
	for (int32 i = 0; i < (int)bodies.size(); ++i) {
		if (bodies[i].filter_type == WallFilterType::Dangerous) {
			bodies[i].body.draw({ col1 });
			bodies[i].body.draw({ col2, Periodic::Sine0_1(2s)});
		}
	}
}

P2Body const& PhysicsElements::Wall::get_nth_wall_body(int32 const n) const
{
	return bodies[n].body;
}

HashTable<P2BodyID, int32> const& PhysicsElements::Wall::get_wall_ids() const
{
	return wall_ids_body;
}
//HashSet<P2BodyID> const& PhysicsElements::Wall::get_wall_ids() const
//{
//	return wall_ids;
//}

Array<PhysicsElements::WallBodyInfo> const& PhysicsElements::Wall::get_bodies() const
{
	return bodies;
}

void PhysicsElements::Wall::set_col(Color const& c)
{
	col = c;
}

