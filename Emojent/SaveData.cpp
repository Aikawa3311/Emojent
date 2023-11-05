#include "stdafx.h"
#include "SaveData.h"

//void SaveData::create_save_file()
//{
//	// 各ステージのタイムとメダルの数を記録（ステージ数 * (8 + 4) バイト）
//	BinaryWriter writer(U"savedata.bin");
//	if (!writer) {
//		throw Error{ U"Failed to open `savedata.bin`" };
//	}
//	// 各ステージのタイムを最大値で初期化、メダルは 0 で初期化
//	for (int32 i = 0; i < Global::stage_num; ++i) {
//		writer.write<double>(std::numeric_limits<double>::max());
//		writer.write<int32>(0);
//	}
//}
//
//double SaveData::read_best_time(int32 const offset)
//{
//	if (!FileSystem::Exists(U"savedata.bin")) {
//		// savedata.bin が無い場合は作る
//		create_save_file();
//	}
//
//	BinaryReader reader(U"savedata.bin");
//	if (!reader) {
//		throw Error{ U"Failed to open `savedata.bin`" };
//	}
//
//	double result = 0.0;
//	reader.setPos((sizeof(double) + sizeof(int32)) * offset);
//	reader.read<double>(result);
//
//	return result;
//}
//
//int32 SaveData::read_medal_num(int32 const offset)
//{
//	if (!FileSystem::Exists(U"savedata.bin")) {
//		// savedata.bin が無い場合は作る
//		create_save_file();
//	}
//
//	BinaryReader reader(U"savedata.bin");
//	if (!reader) {
//		throw Error{ U"Failed to open `savedata.bin`" };
//	}
//
//	int32 result = 0;
//	reader.setPos((sizeof(double) + sizeof(int32)) * offset + sizeof(double));
//	reader.read<int32>(result);
//
//	return result;
//}
//
//void SaveData::write_best_time(int32 const offset, double const time)
//{
//	if (!FileSystem::Exists(U"savedata.bin")) {
//		// savedata.bin が無い場合は作る
//		create_save_file();
//	}
//
//	BinaryWriter writer(U"savedata.bin");
//	if (!writer) {
//		throw Error{ U"Failed to open `savedata.bin`" };
//	}
//
//	writer.setPos((sizeof(double) + sizeof(int32)) * offset);
//	writer.write<double>(time);
//}
//
//void SaveData::write_medal_num(int32 const offset, int32 const medal)
//{
//	if (!FileSystem::Exists(U"savedata.bin")) {
//		// savedata.bin が無い場合は作る
//		create_save_file();
//	}
//
//	BinaryWriter writer(U"savedata.bin");
//	if (!writer) {
//		throw Error{ U"Failed to open `savedata.bin`" };
//	}
//
//	writer.setPos((sizeof(double) + sizeof(int32)) * offset + sizeof(double));
//	writer.write<int32>(medal);
//}

void SaveData::create_save_file()
{
	// 各ステージのタイムとメダルの数を記録（ステージ数 * (8 + 4) バイト）
	BinaryWriter writer_new_data(path);

	if (!writer_new_data) {
		throw Error{ U"create_save_file()：Failed to open `{}`"_fmt(path)};
	}
	// 各ステージのタイムを最大値で初期化、メダルは 0 で初期化
	for (int32 i = 0; i < Global::stage_num; ++i) {
		writer_new_data.write<double>(Global::init_clear_time);
		writer_new_data.write<int32>(0);
	}
}

void SaveData::prepare_writer()
{
	// savedata.bin が無ければ作成
	if (!FileSystem::Exists(path)) {
		create_save_file();
	}

	// writer が無ければ準備
	if (!writer.isOpen()) {
		writer.open(path, OpenMode::Append);
	}

	// 開けなかったらエラー
	if (!writer) {
		throw Error{ U"prepare_writer()：Failed to open `{}`"_fmt(path) };
	}
}

void SaveData::prepare_reader()
{
	// savedata.bin が無ければ作成
	if (!FileSystem::Exists(path)) {
		create_save_file();
	}

	// reader が無ければ準備
	if (!reader.isOpen()) {
		reader.open(path);
	}

	if (!reader) {
		throw Error{ U"prepare_reader()：Failed to open `{}`"_fmt(path) };
	}
}

double SaveData::read_best_time(int32 const offset)
{
	prepare_reader();

	double result = 0.0;
	reader.setPos((sizeof(double) + sizeof(int32)) * offset);
	reader.read<double>(result);

	return result;
}

int32 SaveData::read_medal_num(int32 const offset)
{
	prepare_reader();

	int32 result = 0;
	reader.setPos((sizeof(double) + sizeof(int32)) * offset + sizeof(double));
	reader.read<int32>(result);

	return result;
}

void SaveData::write_best_time(int32 const offset, double const time)
{
	prepare_writer();

	writer.setPos((sizeof(double) + sizeof(int32)) * offset);
	writer.write<double>(time);
}

void SaveData::write_medal_num(int32 const offset, int32 const medal)
{
	prepare_writer();

	writer.setPos((sizeof(double) + sizeof(int32)) * offset + sizeof(double));
	writer.write<int32>(medal);
}
