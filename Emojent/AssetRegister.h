#pragma once
#include <Siv3D.hpp>

class AssetRegister {
public:
	static void Regist() {
		// ---------------------------------------------
		// フォントアセット
		// ---------------------------------------------
		FontAsset::Register(U"Bold40", 40, Typeface::Bold);

		FontAsset::Register(U"Alkatra44", 44, Resource(U"font/Alkatra.ttf"));
		FontAsset::Register(U"Alkatra30", 30, Resource(U"font/Alkatra.ttf"));
		FontAsset::Register(U"Alkatra36SDF", FontMethod::SDF, 36, Resource(U"font/Alkatra.ttf"));

		FontAsset::Register(U"RobotoSlab60", 60, Resource(U"font/RobotoSlab.ttf"));
		FontAsset::Register(U"RobotoSlab40MSDF", FontMethod::MSDF, 40, Resource(U"font/RobotoSlab.ttf"));
		FontAsset::Register(U"RobotoSlab14MSDF", FontMethod::MSDF, 14, Resource(U"font/RobotoSlab.ttf"));

		// FontAsset::Register(U"MamelonHiRegular30", 30, Resource(U"font/Mamelon-4-Hi-Regular.otf"));
		// FontAsset::Register(U"MamelonHiRegular50", 50, Resource(U"font/Mamelon-4-Hi-Regular.otf"));

		FontAsset::Register(U"RoundedMgenplus28", 28, Resource(U"font/rounded-mgenplus-2c-regular.ttf"));
		FontAsset::Register(U"RoundedMgenplus50", 50, Resource(U"font/rounded-mgenplus-2c-regular.ttf"));

		// ---------------------------------------------
		// 絵文字などのアセット
		// ---------------------------------------------
		TextureAsset::Register(U"emoji_face_sanglass", U"😎"_emoji);
		TextureAsset::Register(U"emoji_face_thinking", U"🤔"_emoji);
		TextureAsset::Register(U"emoji_face_spiral_eyes", U"😵‍💫"_emoji);
		TextureAsset::Register(U"emoji_face_crying", U"😭"_emoji);
		TextureAsset::Register(U"emoji_face_smiling", U"😊"_emoji);
		TextureAsset::Register(U"emoji_grab_hand", U"✊"_emoji);
		TextureAsset::Register(U"emoji_gun", U"🔫"_emoji);
		// TextureAsset::Register(U"emoji_key", U"🗝️"_emoji);
		TextureAsset::Register(U"emoji_key", U"🔑"_emoji);
		TextureAsset::Register(U"emoji_lock", U"🔒"_emoji);
		TextureAsset::Register(U"emoji_unlock", U"🔓"_emoji);
		TextureAsset::Register(U"emoji_warning", U"⚠"_emoji);
		TextureAsset::Register(U"emoji_headstone", U"🪦"_emoji);
		TextureAsset::Register(U"emoji_rotated_star", U"💫"_emoji);
		TextureAsset::Register(U"emoji_star", U"⭐"_emoji);

		TextureAsset::Register(U"emoji_next", U"⏩"_emoji);
		TextureAsset::Register(U"emoji_restart", U"🔁"_emoji);
		TextureAsset::Register(U"emoji_back", U"◀"_emoji);
		TextureAsset::Register(U"emoji_medal", U"🏅"_emoji);

		TextureAsset::Register(U"emoji_round_pin", U"📍"_emoji);

		// ---------------------------------------------
		// アイコンなどのアセット
		// ---------------------------------------------
		TextureAsset::Register(U"icon_a_key", 0xF0BEB_icon, 40);
		TextureAsset::Register(U"icon_d_key", 0xF0BF4_icon, 40);
		TextureAsset::Register(U"icon_r_key", 0xF0C1E_icon, 40);
		TextureAsset::Register(U"icon_t_key", 0xF0C24_icon, 40);
		TextureAsset::Register(U"icon_w_key", 0xF0C2D_icon, 40);
		TextureAsset::Register(U"icon_z_key", 0xF0C36_icon, 40);
		TextureAsset::Register(U"icon_arrow_right", 0xF0734_icon, 40);
		TextureAsset::Register(U"icon_arrow_left", 0xF0731_icon, 40);
		TextureAsset::Register(U"icon_arrow_up", 0xF0737_icon, 40);
		TextureAsset::Register(U"icon_gun", 0xF0703_icon, 40);

		// ---------------------------------------------
		// 画像アセット
		// ---------------------------------------------
		TextureAsset::Register(U"result_mission_complete", Resource(U"image/MissionComplete.png"));
		TextureAsset::Register(U"result_back", Resource(U"image/Back.png"));
		TextureAsset::Register(U"result_next", Resource(U"image/Next.png"));
		TextureAsset::Register(U"result_retry", Resource(U"image/Retry.png"));
		TextureAsset::Register(U"result_time", Resource(U"image/Time.png"));
		TextureAsset::Register(U"result_best_time", Resource(U"image/BestTime.png"));
		TextureAsset::Register(U"result_rank", Resource(U"image/Rank.png"));

		TextureAsset::Register(U"stage_thumbnail0", Resource(U"stage_thumbnail/tutorial1.png"));
		TextureAsset::Register(U"stage_thumbnail1", Resource(U"stage_thumbnail/tutorial2.png"));
		TextureAsset::Register(U"stage_thumbnail2", Resource(U"stage_thumbnail/tutorial3.png"));
		TextureAsset::Register(U"stage_thumbnail3", Resource(U"stage_thumbnail/stage1.png"));
		TextureAsset::Register(U"stage_thumbnail4", Resource(U"stage_thumbnail/stage2.png"));
		TextureAsset::Register(U"stage_thumbnail5", Resource(U"stage_thumbnail/stage3.png"));

		// ---------------------------------------------
		// BGM
		// ---------------------------------------------
		AudioAsset::Register(U"bgm", Audio::Stream, Resource(U"BGM/inaka_no_daisougen_otologic.mp3"));

		// ---------------------------------------------
		// 効果音
		// ---------------------------------------------
		AudioAsset::Register(U"pistol2", Resource(U"SE/pistol_selab.mp3"));
		AudioAsset::Register(U"pistol_set", Resource(U"SE/pistol_set_selab.mp3"));
		AudioAsset::Register(U"damage", Resource(U"SE/damage_selab.mp3"));
		AudioAsset::Register(U"wire", Resource(U"SE/wire_selab.mp3"));
		AudioAsset::Register(U"gun_target", Resource(U"SE/gun_target_selab.mp3"));
		AudioAsset::Register(U"key", Resource(U"SE/key_selab.mp3"));
		AudioAsset::Register(U"button", Resource(U"SE/button_selab.mp3"));
		AudioAsset::Register(U"clear", Resource(U"SE/clear_selab.mp3"));
		AudioAsset::Register(U"roll", Resource(U"SE/roll_selab.mp3"));
		AudioAsset::Register(U"medal", Resource(U"SE/medal_selab.mp3"));
		AudioAsset::Register(U"new_record", Resource(U"SE/new_record_selab.mp3"));
		AudioAsset::Register(U"mouseover", Resource(U"SE/mouseover_selab.mp3"));
		AudioAsset::Register(U"jump", Resource(U"SE/jump_selab.mp3"));
		AudioAsset::Register(U"bend1", Resource(U"SE/bend1.wav"));
		AudioAsset::Register(U"bend2", Resource(U"SE/bend2.wav"));
		AudioAsset::Register(U"bend3", Resource(U"SE/bend3.wav"));
		AudioAsset::Register(U"bend4", Resource(U"SE/bend4.wav"));
		AudioAsset::Register(U"bend5", Resource(U"SE/bend5.wav"));
		AudioAsset::Register(U"bend6", Resource(U"SE/bend6.wav"));
		AudioAsset::Register(U"bend7", Resource(U"SE/bend7.wav"));
		AudioAsset::Register(U"bend8", Resource(U"SE/bend8.wav"));
	}
};
