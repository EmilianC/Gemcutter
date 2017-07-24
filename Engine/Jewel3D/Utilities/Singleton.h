// Copyright (c) 2017 Emilian Cioca
#pragma once

/*
 Inherit from this class to create a singleton.
 You must declare your class in this way when doing so:
 
 static class MyClass : public Singleton<MyClass>
 {
 	//...
 } &MyClass = Singleton<class MyClass>::instanceRef;
*/

namespace Jwl
{
	template<class T>
	class Singleton
	{
	public:
		static T& instanceRef;

	protected:
		explicit Singleton() {}
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

	private:
		static T instance;
	};

	template<class T>
	T Singleton<T>::instance;

	template<class T>
	T& Singleton<T>::instanceRef = Singleton<T>::instance;
}
