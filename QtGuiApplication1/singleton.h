#ifndef SINGLETON_H
#define SINGLETON_H

#include <QtGlobal>
#include <QScopedPointer>
#include "call_once.h"

//Qt thread-safe singleton
template <class T>
class Singleton
{
private:
	typedef T* (*CreateInstanceFunction)();
public:
	static T* instance(CreateInstanceFunction create);
	static void DestroyInstance();

private:
	static void init();

	Singleton();
	~Singleton();
	Q_DISABLE_COPY(Singleton)
		static QBasicAtomicPointer<void> create;
	static QBasicAtomicInt flag;
	static QBasicAtomicPointer<void> tptr;
	bool inited;
};

template <class T>
T* Singleton<T>::instance(CreateInstanceFunction create)
{
	Singleton::create.store(create);
	qCallOnce(init, flag);
	return (T*)tptr.load();
}

template <class T>
void Singleton<T>::init()
{
	static Singleton singleton;
	if (singleton.inited) {
		CreateInstanceFunction createFunction = (CreateInstanceFunction)Singleton::create.load();
		tptr.store(createFunction());
	}
};

template <class T>
void Singleton<T>::DestroyInstance()
{
	T* createdTptr = (T*)tptr.fetchAndStoreOrdered(nullptr);
	if (createdTptr) {
		delete createdTptr;
	}
	create.store(nullptr);
};


template <class T>
Singleton<T>::Singleton() {
	inited = true;
};

template <class T>
Singleton<T>::~Singleton() {
	T* createdTptr = (T*)tptr.fetchAndStoreOrdered(nullptr);
	if (createdTptr) {
		delete createdTptr;
	}
	create.store(nullptr);
}




template<class T> QBasicAtomicPointer<void> Singleton<T>::create = Q_BASIC_ATOMIC_INITIALIZER(nullptr);
template<class T> QBasicAtomicInt Singleton<T>::flag = Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);
template<class T> QBasicAtomicPointer<void> Singleton<T>::tptr = Q_BASIC_ATOMIC_INITIALIZER(nullptr);

#endif // SINGLETON_H

//--use sample
//MyClass* MyClass::instance()
//{
//	return Singleton<MyClass>::instance(MyClass::createInstance);
//}

