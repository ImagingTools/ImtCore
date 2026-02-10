// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/CDocumentTable.h>


namespace imtmdbx
{


extern int getItemCount;

class CMask: virtual public IMask, public CDocumentTable
{
public:
	CMask(
		const QString& name,
		mdbx::txn_managed& txn,
		mdbx::key_mode keyMode = mdbx::key_mode::ordinal,
		mdbx::value_mode valueMode = mdbx::value_mode::single,
		bool hasIndex = false);

	// reimplemented (imtmdbx::IMask)
	virtual bool GetUnit(quint64 position) override;
	virtual bool SetUnit(quint64 position, bool unit) override;

protected:
	// reimplemented (imtmdbx::IMask)
	virtual bool GetItem(quint64 offset, quint64& item) override;
	virtual bool SetItem(quint64 offset, quint64 item) override;
	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) override;
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffff) override;

protected:
	QMap<quint64,quint64> m_cache;
};


} // namespace imtmdbx

