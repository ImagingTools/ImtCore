#pragma once

#include "qtypes.h"


namespace imtmdbx
{


class IMask
{
public:
	virtual bool GetUnit(qint64 position) = 0;
	virtual bool SetUnit(qint64 position, bool unit) = 0;

	virtual bool GetItem(qint64 offset, quint64& item) = 0;
	virtual bool SetItem(qint64 offset, quint64 item) = 0;

	virtual qint64 GetNextItemOffset(qint64 startOffset = 0) = 0;
	virtual qint64 GetPreviosItemOffset(qint64 startOffset = 0xffffffffffffff) = 0;

};



}//namespace imtmdbx
