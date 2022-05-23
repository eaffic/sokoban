#pragma once
#include "GameObject.h"

class EventItem : public GameObject {
public:
	//変更不可
	typedef shared_ptr<GameObject>	SP;
	typedef weak_ptr<GameObject>	WP;
public:
	//変更可
	int number;
	bool isWork;

	virtual void Open() = 0;
	virtual void Close() = 0;
};