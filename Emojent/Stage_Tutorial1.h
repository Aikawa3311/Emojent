#pragma once
#include "Stage.h"

namespace PhysicsElements {
	// タイトルを兼ねるため特殊
	class Stage_Tutorial1 : public Stage
	{
	private:
		// 初回実行時のアニメ
		SimpleAnimation anim_keyframe;

		// シェーダ
		// PixelShader const shader_opening = HLSL{ U"shader/black_transition.hlsl", U"PS_Shape" };
		// オープニングの黒
		RenderTexture rt_opening;

		// オープニングアニメーション中か
		bool is_opening = true;

		// マウスの描画
		void draw_mouse(Vec2 const & pos, double const r, double const margin, int32 click, Color const & col) const;

	protected:
		// 各変数の初期化など
		void init() override;

		// プレイヤー・ゴール・壁・アイテムの配置や壁の属性の設定
		void create_stage() override;

		void draw_mid_layer() const override;

		void change_next_scene() override;

	public:
		Stage_Tutorial1(InitData const& init);

		void Update() override;
		void Draw() const override;

		// SceneManager の更新関数
		// void update() override;
		// SceneManager の描画関数
		// void draw() const override;
	};

} // namespace PhysicsElements


