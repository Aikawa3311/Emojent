#pragma once
#include <Siv3D.hpp>
#include "Wall.h"
#include "Player.h"

// 壁の状態を変えるアイテム
class Item
{
private:
	// アイテム一つ一つの情報
	struct ItemComponent {
		Vec2 pos;
		bool is_gotten;
		String texture_name = U""; // テクスチャの名前
		P2BodyID target; // 変更対象のid
		PhysicsElements::WallFilterType filter; // 変更後のフィルタ
	};

	// プレイヤー
	std::shared_ptr<PhysicsElements::Player> player;
	// 壁
	std::shared_ptr<PhysicsElements::Wall> wall;
	// アイテム集合
	Array<ItemComponent> items;

public:
	Item(std::shared_ptr<PhysicsElements::Player>& player, std::shared_ptr<PhysicsElements::Wall>& wall);

	void item_add(P2BodyID target, PhysicsElements::WallFilterType filter, Vec2 const& pos, String const& texture_name);

	void Update();
	void Draw() const;

	// -----
	// アクセサ
	// -----

};

