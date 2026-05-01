#pragma once

// TODO: Constrain to types that don't already have ownership semantics?
template <typename T>
struct Owned
{
	T val;

	Owned(const Owned&) = delete;
	Owned& operator=(const Owned&) = delete;

	Owned(Owned&&);
	Owned& operator=(Owned&&);

	Owned(T&&);
	Owned& operator=(T&&);
};

template <typename T>
Owned<T>::Owned(Owned&& x)
	: val(move(x))
{
	x.val = {};
}

template <typename T>
Owned<T>&
Owned<T>::operator=(Owned&& x)
{
	val = move(x.val);
	x.val = {};
	return *this;
}

template <typename T>
Owned<T>::Owned(T&& x)
	: val(move(x))
{
	x = {};
}

template <typename T>
Owned<T>&
Owned<T>::operator=(T&& x)
{
	val = move(x);
	x = {};
	return *this;
}
