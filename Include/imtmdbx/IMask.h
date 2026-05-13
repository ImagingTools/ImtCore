// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtmdbx
{


class IMask: istd::IPolymorphic
{
public:
	virtual bool GetUnit(quint64 position) = 0;
	virtual bool SetUnit(quint64 position, bool unit) = 0;

	virtual bool GetItem(quint64 offset, quint64& item) = 0;
	virtual bool SetItem(quint64 offset, quint64 item) = 0;

	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) = 0;
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) = 0;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffffffffff) = 0;
};



}//namespace imtmdbx
