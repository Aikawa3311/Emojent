#pragma once
#include "Stage.h"

namespace PhysicsElements {
	class Stage_Tutorial3 : public Stage
	{
	private:


	protected:
		// 各変数の初期化など
		void init() override;

		// プレイヤー・ゴール・壁・アイテムの配置や壁の属性の設定
		void create_stage() override;

		void change_next_scene() override;

	public:
		Stage_Tutorial3(InitData const& init);

		void Update() override;
		void Draw() const override;

		// SceneManager の更新関数
		// void update() override;
		// SceneManager の描画関数
		// void draw() const override;
	};

} // namespace PhysicsElements


