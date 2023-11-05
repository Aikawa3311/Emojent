#pragma once
#include <Siv3D.hpp>

namespace PhysicsElements{
	namespace Material {
		// 壁マテリアル（密度低, 反発無し, 摩擦低）
		constexpr P2Material Wall{ .density = 0.1, .restitution = 0.0, .friction = 0.1 };

		// プレイヤーマテリアル（密度高, 反発無し, 摩擦低）
		constexpr P2Material Player{ .density = 1.0, .restitution = 0.0, .friction = 0.1 };

		// ミスしたプレイヤーマテリアル（密度高, 反発あり, 摩擦低）
		constexpr P2Material PlayerMiss{ .density = 1.0, .restitution = 0.5, .friction = 0.1 };

		// 墓石マテリアル（密度高, 反発あり, 摩擦低）
		constexpr P2Material Headstone{ .density = 1.0, .restitution = 0.5, .friction = 0.1 };

		// プレイヤー足マテリアル（密度高, 反発無し, 摩擦低）
		// constexpr P2Material PlayerFoot{ .density = 1.0, .restitution = 0.0, .friction = 0.1 };
	} // namespace Material

	namespace Filter {
		// 壁の干渉フィルタ
		constexpr P2Filter Wall{ .categoryBits = 0b0000'0000'0000'0001, .maskBits = 0b1111'1111'1111'1111 };

		// 無効壁の干渉フィルタ
		constexpr P2Filter WallIgnore{ .categoryBits = 0b0000'0000'0000'0001, .maskBits = 0b0000'0000'0000'0000 };

		// 下から通り抜けられる足場壁の干渉フィルタ
		constexpr P2Filter WallPlatform{ .categoryBits = 0b0000'0000'0000'0010, .maskBits = 0b1111'1111'1111'1111 };

		// プレイヤーの干渉フィルタ
		constexpr P2Filter Player{ .categoryBits = 0b0000'0000'0001'0000, .maskBits = 0b1111'1111'1001'1111 };

		// プレイヤーの足元の干渉フィルタ
		constexpr P2Filter PlayerFoot{ .categoryBits = 0b0000'0000'0010'0000, .maskBits = 0b1111'1111'1010'1111 };

		// ジャンプ中のプレイヤーの干渉フィルタ（下から通り抜けられる足場壁を無視する）
		constexpr P2Filter PlayerJumping{ .categoryBits = 0b0000'0000'0001'0000, .maskBits = 0b1111'1111'1001'1101 };

		// ジャンプ中のプレイヤーの足元の干渉フィルタ（下から通り抜けられる足場壁を無視する）
		constexpr P2Filter PlayerJumpingFoot{ .categoryBits = 0b0000'0000'0010'0000, .maskBits = 0b1111'1111'1010'1101 };

		// ミスしたプレイヤーの干渉フィルタ
		constexpr P2Filter PlayerMiss{ .categoryBits = 0b0000'0000'0100'0000, .maskBits = 0b1111'1111'0000'1111 };
		
		// 墓石の干渉フィルタ
		constexpr P2Filter Headstone{ .categoryBits = 0b0000'0001'0000'0000, .maskBits = 0b0000'0010'0000'1111 };

		// 薬莢の干渉フィルタ
		constexpr P2Filter Cartridge{ .categoryBits = 0b0000'0010'0000'0000, .maskBits = 0b0000'0001'0000'1111 };

		// ワイヤのセンサー用の干渉フィルタ（未使用）
		// constexpr P2Filter WireLineSensor{ .categoryBits = 0b0001'0000'0000'0000, .maskBits = 0b0000'0000'0000'0001 };
		// constexpr P2Filter WireLineSensor{ .categoryBits = 0b0001'0000'0000'0000, .maskBits = 0b1111'1111'1111'1111 };

	} // namespace Filter

} // namespace PhysicsElements
