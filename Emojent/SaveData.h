#pragma once
#include <Siv3D.hpp>
#include "GlobalSetting.h"

//namespace SaveData
//{
//	void create_save_file();
//	double read_best_time(int32 const offset);
//	int32 read_medal_num(int32 const offset);
//	void write_best_time(int32 const offset, double const time);
//	void write_medal_num(int32 const offset, int32 const medal);
//} // namespace SaveData

class SaveData {
private:
	FilePath const path = U"savedata.bin";
	BinaryReader reader;
	BinaryWriter writer;

	// 初期化セーブデータの新規作成
	void create_save_file();

	// writer の作成
	void prepare_writer();

	// reader の作成
	void prepare_reader();

public:
	SaveData() = default;

	double read_best_time(int32 const offset);
	int32 read_medal_num(int32 const offset);

	void write_best_time(int32 const offset, double const time);
	void write_medal_num(int32 const offset, int32 const medal);
};
