// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF include
#include <istd/TSharedNullable.h>


namespace imtsdl
{


/**
	This is a comfort wrap for \c QList<istd::TSharedNullable>
*/
template <class T>
class TElementList: public QList<istd::TSharedNullable<T>>
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
	for (const istd::TSharedNullable<T>& element: *this){
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

	for (const istd::TSharedNullable<T>& element: source){
		if (element){
			this->append(*element);
		}
	}
}


}  // namespace imtsdl



