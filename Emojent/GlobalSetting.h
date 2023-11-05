#pragma once
#include <Siv3D.hpp>

enum class SceneState {
	Stage_Tutorial1,
	Stage_Tutorial2,
	Stage_Tutorial3,
	Stage_Stage1,
	Stage_Stage2,
	Stage_Stage3,
	StageSelect
};

using App = SceneManager<SceneState>;




namespace Global {
	// 物理演算のシミュレーションのステップタイム（1秒に200回）
	constexpr double physics_step_time = (1.0 / 200.0);

	// 物理演算のシミュレーションの総経過時間（(1/physics_step_time) 回シミュレーションで 1 秒経過）
	// double physics_accumulateda_time;

	// 円の近似度
	constexpr int32 circle_resolution = 14;

	// 全ステージ数
	constexpr int32 stage_num = 6;

	// 初期クリアタイム
	constexpr double init_clear_time = 3599.99;

	// メダル獲得の基準タイム
	constexpr std::array<const std::array<double, 2>, stage_num> score_time = {
		std::array<double, 2>{30.0, 16.0},
		std::array<double, 2>{60.0, 30.0},
		std::array<double, 2>{50.0, 25.0},
		std::array<double, 2>{50.0, 16.0},
		std::array<double, 2>{60.0, 18.0},
		std::array<double, 2>{50.0, 20.0},
	};

} // namespace Global

