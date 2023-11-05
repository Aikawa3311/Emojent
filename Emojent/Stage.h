#pragma once
#include <Siv3D.hpp>
#include "GlobalSetting.h"
#include "PhysicsElements.h"
#include "ShaderConstantBuffer.h"
#include "Player.h"
#include "Wall.h"
#include "Wire.h"
#include "Item.h"
#include "Goal.h"
#include "GunTarget.h"
#include "Gun.h"
#include "PlayerMissController.h"
#include "StarParticleSystem.h"

namespace PhysicsElements{

	class Stage : public App::Scene {
	private:
		// 物理演算用のメンバ
		std::shared_ptr<Wire> wire;
		double accumulated_time = 0;

		// ステージの状態
		bool is_cleared = false;	// クリアしたかどうか
		bool is_last = false;		// 最後のステージかどうか
		bool is_freeze = false;		// ステージの挙動を止めるかどうか
		int32 miss_count = 0;		// ミス回数

		// 描画用のレンダーテクスチャ
		RenderTexture rt_wall;
		MSRenderTexture rt_stage;

		// ミス時のプレイヤーおよび墓石の管理
		std::unique_ptr<PlayerMissController> player_miss_ctrl;

		// 星のパーティクル
		StarParticleSystem star_particle_system;

		// ワイヤ移動時のプレイヤーのトレイル
		Trail trail;
		double trail_frequency = 30;
		double trail_accumulated_time = 0;
		double trail_life_time = 0.7;
		double trail_size = 25;
		bool is_trail_on = false; // 一度中空でワイヤを使用してから次に着地するまで true
		
		// -----
		// メンバ関数
		// -----

		// 衝突の処理
		void physics_collision_check();

		// プレイヤーのミス判定
		void miss_by_contact();		// 接触によるミス
		void miss_by_out_room();	// 場外によるミス
		void miss_state_check_and_change();	// ミス状態の遷移

		// リザルトからシーン遷移用
		void update_result();

		// トレイルの点追加および更新処理
		void update_trail();

		// 場外判定
		void update_out_room();

		// 壁の描画
		void draw_wall() const;

		// 右下の方にタイムを描画
		void draw_time() const;

		// デバグ用
		int debug_cnt = 0;

	protected:
		// ステージの名前
		String stage_name = U"<NONE>";
		SceneState stage_scene_state;
		// ステージ番号
		int32 stage_num = 0;

		// ステージのサイズ
		SizeF stage_size;

		// ステージのカラー
		Color stage_col1 = Palette::White;
		Color stage_col2 = Palette::Deeppink;

		// 物理演算用のメンバ
		std::shared_ptr<P2World> world;
		std::shared_ptr<Player> player;
		std::shared_ptr<Wall> wall;

		// 銃関連
		std::shared_ptr<GunTarget> gun_targets;
		std::shared_ptr<Gun> gun;

		// カメラ
		Camera2D camera{ Vec2(640, 360), 1.0 , CameraControl::None_ };
		// カメラの移動制御を有効にするかどうか
		bool flag_camera_moving_limit = false;

		// シェーダオブジェクト
		// PixelShader const shader_wall = HLSL{ U"shader/wall_diagonal_stripe.hlsl", U"PS_Texture" };
		// PixelShader const shader_background = HLSL{ U"shader/checker.hlsl", U"PS_Shape" };
		PixelShader const shader_wall = HLSL{ Resource(U"shader/wall_diagonal_stripe.hlsl"), U"PS_Texture" };
		PixelShader const shader_background = HLSL{ Resource(U"shader/checker.hlsl"), U"PS_Shape" };
		ConstantBuffer<PSPixelColor4> shader_col1;
		ConstantBuffer<PSPixelColor4> shader_col2;
		ConstantBuffer<PSPixelScreenSize> shader_size;
		ConstantBuffer<PSPixelTime> shader_time;

		// スタート地点
		Vec2 start_pos;
		// リスポーン地点
		Vec2 respawn_pos;

		// ゴール
		std::unique_ptr<Goal> goal;

		// チェックポイント（床始点）
		Array<Vec2> checkpoints;

		// ステージの経過時間
		Stopwatch stopwatch;

		// 状態が変化する壁
		Array<P2BodyID> wall_toggle_ignore;		// ignore にトグルする壁
		Array<P2BodyID> wall_toggle_dangerous;	// dangerous にトグルする壁

		// アイテム
		std::unique_ptr<Item> items;

		// -----
		// メンバ関数
		// -----
		// 変数の初期化など
		virtual void init();

		// 基本的な update の処理パイプライン
		void update_pipeline();

		// 基本的な draw の処理パイプライン
		void draw_pipeline() const;

		// カメラの位置などの更新
		void update_camera();

		// チェックポイントの更新
		void update_checkpoint();
		// チェックポイントの描画
		void draw_checkpoint() const;

		// 背景の描画
		virtual void draw_background() const;

		// 背景・壁よりも上のレイヤーに描画
		virtual void draw_mid_layer() const;

		// ステージの生成
		virtual void create_stage();

		// チェックポイントを追加（床始点）
		// virtual void add_checkpoint(Vec2 const & pos);

		// 次のシーンに移動
		virtual void change_next_scene();

	public:
		Stage(InitData const & init);

		// SceneManager の更新関数
		void update() override;
		// SceneManager の描画関数
		void draw() const override;

		// 部屋のリスタート（アイテムの取得状況などは変化しない）
		void restart_stage();

		// general な更新関数
		virtual void Update();

		// 物理演算用の更新関数、1 フレーム内で複数回呼び出される
		void UpdatePhysics();

		// general な描画関数
		virtual void Draw() const;


		// void drawFadeIn(double t) const override;
		// void drawFadeOut(double t) const override;

		// -----
		// アクセサ
		// -----
		// 物理演算を止める/動かす、プレイヤー操作も止める/動かす
		void set_freeze(bool const flag);
	};

} // namespace PhysicsElements

