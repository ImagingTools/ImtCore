// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/CDocumentTable.h>

// Qt includes
#include <QtCore/QMap>


namespace imtmdbx
{


class CMemoryMask: virtual public IMask
{
public:
    CMemoryMask();

	virtual bool GetUnit(quint64 position) override;
	virtual bool SetUnit(quint64 position, bool unit) override;

    virtual bool GetItem(quint64 offset, quint64& item) override;
    virtual bool SetItem(quint64 offset, quint64 item) override;

protected:
	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) override;
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffff) override;
	QMap<quint64,quint64> m_cache;
};

}//namespace imtmdbx
