#include "stdafx.h"
#include "Item.h"

Item::Item(std::shared_ptr<PhysicsElements::Player>& player, std::shared_ptr<PhysicsElements::Wall>& wall)
	: player(player),
	wall(wall)
{
}

void Item::item_add(P2BodyID target, PhysicsElements::WallFilterType filter, Vec2 const& pos, String const& texture_name)
{
	ItemComponent item;
	item.target = target;
	item.filter = filter;
	item.is_gotten = false;
	item.pos = pos;
	item.texture_name = texture_name;
	items.emplace_back(item);
}

void Item::Update()
{
	// 与えられた座標と接触判定
	for (int32 i = 0; i < (int32)items.size(); ++i) {
		if (!items[i].is_gotten && player->get_pos().distanceFromSq(items[i].pos) < 2500) {
			// 取得できたので壁の状態を変更
			items[i].is_gotten = true;
			wall->change_filter(items[i].target, items[i].filter);
			// 音を鳴らす
			AudioAsset(U"key").playOneShot(0.9);
		}
	}
}

void Item::Draw() const
{
	for (int32 i = 0; i < (int32)items.size(); ++i) {
		if (items[i].is_gotten) {
			TextureAsset(items[i].texture_name).resized(50).drawAt(items[i].pos, AlphaF(0.3));
		}
		else {
			TextureAsset(items[i].texture_name).resized(50).drawAt(items[i].pos.movedBy(Periodic::Sine0_1(3) * Vec2::Up() * 10));
		}
	}
}

