#pragma once
#include <Siv3D.hpp>

// stateを列挙型で管理　一度に表現できる状態は一つのみ
template <typename Statetype>
class StateProperty {
private:
	Statetype state;
	Stopwatch timer;

public:
	StateProperty(Statetype const init_state);

	// stateの変更
	void changestate(Statetype const dst_state);
	// 現在のstateの取得
	Statetype getstate() const;
	// 現在のstateになってからの経過時間(秒)
	double gettime() const;
};


template<typename Statetype>
inline StateProperty<Statetype>::StateProperty(Statetype const init_state)
	: state(init_state)
{
	timer.start();
}

template<typename Statetype>
inline void StateProperty<Statetype>::changestate(Statetype const dst_state) {
	state = dst_state;
	timer.restart();
}

template<typename Statetype>
inline Statetype StateProperty<Statetype>::getstate() const {
	return state;
}

template<typename Statetype>
inline double StateProperty<Statetype>::gettime() const {
	return timer.sF();
}
