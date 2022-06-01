#pragma once
#include "framework.h"

class Connection
{
private:
	int _id;
	std::thread _implementation;

	static int MAX_ID;
public:
	Connection();
	~Connection();

	// Функция запуска обработки соединения
	template <typename Function, typename ...Args> 
	void Start(Function&& init_function, Args&&... args)
	{
		_implementation = std::thread(std::forward<Function>(init_function), std::forward<Args>(args)...);
	}

	int GetID() const;

	friend bool operator<(const Connection& a, const Connection& b);
};

