#include "stdafx.h"
#include <Siv3D.hpp>
#include "GlobalSetting.h"
#include "SaveData.h"
#include "ShaderConstantBuffer.h"


class StageSelect : public App::Scene {
private:
	struct StageInfo {
		String texture_name;
		double best_time;
		int32 best_medal_num;
	};

	// 各ステージの情報
	Array<StageInfo> stage_info;

	// ステージの矩形
	Array<RectF> rects;

	// 終わるボタンの判定
	RectF end_button;

	// 矩形のサイズなど
	Vec2 const rect_size = Vec2(300, 200);
	Vec2 const margin = Vec2(50, 50);

	// マウスオーバー時に音を鳴らすフラグ
	bool flag_audio_play;

	// ステージインフォの構築など
	void init();

public:
	StageSelect(InitData const& init_data);

	void update() override;
	void draw() const override;

	void Update();
	void Draw() const;
};
