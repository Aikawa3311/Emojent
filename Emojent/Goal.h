#pragma once
#include <Siv3D.hpp>
#include "Wall.h"
#include "Wire.h"
#include "Player.h"
#include "StateProperty.h"
#include "ShaderConstantBuffer.h"
#include "SaveData.h"

// ゴール座標とゴールの判定・描画、およびゴール演出
class Goal
{
private:
	// 演出ステート
	enum class GoalState {
		Not,
		CameraMoving,
		ResultAnim,
		Result
	};

	// プレイヤー
	std::shared_ptr<PhysicsElements::Player> player;
	// ワイヤー
	std::shared_ptr<PhysicsElements::Wire> wire;
	// カメラ
	Camera2D& camera;
	// ステージ全体のマルチサンプルレンダーテクスチャの参照
	MSRenderTexture& rt_stage;
	// ステージの経過時間の参照
	Stopwatch& stopwatch;
	// ステージ番号
	int32 stage_num;

	// ゴール座標
	Vec2 goal_pos = Vec2::Zero();
	// クリアしたかどうか
	bool is_cleared = false;
	// リザルト表示中かどうか
	bool is_result = false;
	// リザルト中に next ボタンが押された
	bool is_clicked_next_button = false;
	// リザルト中に restart ボタンが押された
	bool is_clicked_restart_button = false;
	// リザルト中に back ボタンが押された
	bool is_clicked_back_button = false;
	// ニューレコードか
	bool is_new_record = false;

	// ステージのメダル獲得基準タイム
	std::array<double, 2> score_time = { 0.0, 0.0 };
	// メダル
	int32 medal_num = 0;

	// セーブデータ側のベストタイム
	double best_time = 0.0;
	// セーブデータ側のメダルの数
	int32 best_medal_num = 0;

	// ゴールした瞬間のプレイヤーの場所
	Vec2 player_pos = Vec2::Zero();
	// double player_size;
	double player_angle = 0.0;

	// 現在の演出ステート
	StateProperty<GoalState> state;
	double camera_move_time = 0.5;
	// リザルト演出のキーフレーム
	SimpleAnimation anim_keyframes;

	// 演出用のストップウォッチ
	Stopwatch result_stopwatch;

	// 音を鳴らす用の変数
	double roll_accumulated_time = 0.0;
	bool is_medal1_audio_play = false;
	bool is_medal2_audio_play = false;
	bool is_medal3_audio_play = false;
	bool is_new_record_audio_play = false;

	// ブラーをかけるシェーダ
	PixelShader const shader = HLSL{ Resource(U"shader/average_blur.hlsl"), U"PS_Texture" };
	ConstantBuffer<PSPixelScreenSize> shader_size;
	ConstantBuffer<PSPixelTime> shader_time;

	// 描画位置など
	Vec2 const draw_pos_start_mission_complete = Vec2(-300, 230);
	Vec2 const draw_pos_end_mission_complete = Vec2(1000, 230);
	RectF const draw_pos_start_result_square = RectF(Vec2(750, 20), Vec2(400, 100));
	RectF const draw_pos_end_result_square = RectF(Vec2(750, 100), Vec2(500, 590));
	// Vec2 const draw_pos_start_result_time = Vec2(860, 280);
	Vec2 const draw_pos_result_time = Vec2(860, 340);
	Vec2 const draw_pos_result_rank = Vec2(1070, 340);
	Vec2 const draw_pos_timer = Vec2(800, 355);
	Vec2 const draw_pos_best_time = Vec2(835, 425);
	Vec2 const draw_pos_best_timer = Vec2(800, 445);
	Vec2 const draw_pos_new_record = Vec2(885, 525);
	Vec2 const draw_pos_next_rank = Vec2(1010, 480);
	RectF const draw_pos_next_button = RectF(Vec2(800, 560), Vec2(100, 100));
	RectF const draw_pos_restart_button = RectF(Vec2(950, 560), Vec2(100, 100));
	RectF const draw_pos_back_button = RectF(Vec2(1100, 560), Vec2(100, 100));
	Vec2 const draw_pos_rel_medal1 = Vec2(280, 330);
	Vec2 const draw_pos_rel_medal2 = Vec2(350, 330);
	Vec2 const draw_pos_rel_medal3 = Vec2(420, 330);

	// State ごとの Update 処理
	void update_according_to_state();

	// 文字列の輪郭取得(https://siv3d.github.io/ja-jp/tutorial2/font/#3126-%E6%96%87%E5%AD%97%E3%82%92-linestring-%E3%81%A7%E5%8F%96%E5%BE%97%E3%81%99%E3%82%8B)
	Array<LineString> to_line_strings(const Vec2& basePos, const Array<OutlineGlyph>& glyphs) const;

	// セーブデータからタイムなどを読み取り
	void read_savedata();
	// セーブデータにベストタイムなどの記録
	void write_savedata();

	// メダルの数を数える
	void calc_medal_num();

	// ステージに関する変数の初期化
	void init_stage_data();

public:
	Goal(std::shared_ptr<PhysicsElements::Player> & player, std::shared_ptr<PhysicsElements::Wire>& wire, Camera2D& camera, MSRenderTexture& rt_stage, Stopwatch& stopwatch, int32 const stage_num);

	void Update();
	void Draw() const;
	void draw_result() const;

	// -----
	// アクセサ
	// -----
	void set_pos(Vec2 const & pos);
	void set_stage_num(int32 const num);

	bool get_cleared() const;
	bool get_result() const;
	bool get_clicked_next_button() const;
	bool get_clicked_restart_button() const;
	bool get_clicked_back_button() const;
};

