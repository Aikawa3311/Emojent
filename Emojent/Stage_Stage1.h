#pragma once
#include "Stage.h"

namespace PhysicsElements {
	class Stage_Stage1 : public Stage
	{
	private:

	protected:
		// 各変数の初期化など
		void init() override;

		// プレイヤー・ゴール・壁・アイテムの配置や壁の属性の設定
		void create_stage() override;

		void draw_mid_layer() const override;

		void change_next_scene() override;

	public:
		Stage_Stage1(InitData const& init);

		void Update() override;
		void Draw() const override;

		// SceneManager の更新関数
		// void update() override;
		// SceneManager の描画関数
		// void draw() const override;
	};

} // namespace PhysicsElements


