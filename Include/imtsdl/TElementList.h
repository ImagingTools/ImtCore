// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF include
#include <istd/TNullableValue.h>


namespace imtsdl
{


/**
	This is a comfort wrap for \c QList<istd::TNullableValue>
*/
template <class T>
class TElementList: public QList<istd::TNullableValue<T>>
{
public:
	TElementList();
	TElementList(std::initializer_list<T> args);
	QList<T> ToList() const;
	void FromList(const QList<T>& source);
};


template<class T>
inline TElementList<T>::TElementList()
{

}


template<class T>
inline TElementList<T>::TElementList(std::initializer_list<T> args)
{
	QList<T> list(args);
	this->FromList(list);
}


template <class T>
inline QList<T> TElementList<T>::ToList() const
{
	QList<T> retVal;
	for (const istd::TNullableValue<T>& element: *this){
		if (element){
			retVal << *element;
		}
	}

	return retVal;
}


template <class T>
inline void TElementList<T>::FromList(const QList<T>& source)
{
	this->clear();

	for (const T& element: source){
		this->append(istd::TNullableValue<T>(element));
	}
}


}  // namespace imtsdl



