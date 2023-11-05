#pragma once
#include <Siv3D.hpp>
#include "PhysicsElements.h"
#include "GlobalSetting.h"

namespace PhysicsElements {

	enum class WallShapeType {
		Circle,
		Rect,
		Triangle,
		Polygon
	};

	// 壁の状態（主にDraw用）
	enum class WallFilterType {
		Normal,
		Ignore,
		Dangerous,
		Platform	// 通り抜け可能な壁
	};

	// 各 wall の値
	struct WallBodyInfo {
		P2Body body;					// P2Body 本体
		WallShapeType shape_type;		// wall の形のタイプ
		WallFilterType filter_type;		// wall のフィルタタイプ
	};

	// 壁と線分の衝突判定の結果
	struct WallLineCollisionResult {
		bool is_collided = false;
		double dist = -1.0;
		Vec2 p = Vec2::Zero();
	};

	class Wall {
	public:
		// 壁の P2Body とその形状を示す列挙型のペア
		// using BodyInfo = std::pair<P2Body, WallShapeType>;
		// 壁の頂点情報、頂点座標とその頂点を持つ壁の添え字
		// using WallVertexInfo = std::pair<Vec2, int32>;

	private:
		std::shared_ptr<P2World> world;
		Array<WallBodyInfo> bodies;
		// HashSet<P2BodyID> wall_ids;
		HashTable<P2BodyID, int32> wall_ids_body;
		Color col = Palette::Darkslategray;

	public:
		Wall(std::shared_ptr<P2World>& world);

		// 四角形状の壁要素追加
		P2BodyID addRect(Vec2 const& world_pos, SizeF const& size);
		P2BodyID addRectAt(Vec2 const& world_pos, SizeF const& size);
		// 円形状の壁要素追加
		P2BodyID addCircle(Vec2 const& world_pos, double const r);
		// 三角形状の壁要素追加
		P2BodyID addTriangle(Vec2 const& world_pos, Vec2 const& v0, Vec2 const& v1, Vec2 const& v2);

		// 指定した ID の壁のフィルタを変更する
		void change_filter(P2BodyID const id, WallFilterType const filter);

		// n 番目の壁の頂点集合を返す
		Array<Vec2> nth_wall_vertices(int32 const n) const;
		// n 番目の壁の重心を返す
		Vec2 nth_wall_barycenter(int32 const n) const;

		// n 番目の壁と与えられた線分が交差するか調べる
		WallLineCollisionResult intersect_line(int const n, Line const & line);

		void Update();
		void UpdatePhysics();
		// 全ての wall を描画
		void Draw() const;

		void draw_normal_walls() const;
		void draw_ignore_walls() const;
		void draw_danger_walls(Color const & col1, Color const& col2) const;

		// -----
		// アクセサ
		// -----
		P2Body const& get_nth_wall_body(int32 const n) const;
		HashTable<P2BodyID, int32> const& get_wall_ids() const;
		// HashSet<P2BodyID> const& get_wall_ids() const;
		Array<WallBodyInfo> const& get_bodies() const;

		void set_col(Color const& c);
	};

} // namespace PhysicsElements


