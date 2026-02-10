// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/IDocumentTable.h>


namespace imtmdbx
{


class IMaskContainer: istd::IPolymorphic
{
public:
	virtual void AddMask(IMask* mask, bool isInversion = false) = 0;
	virtual void Clear() = 0;
	virtual bool RemoveLastMask(bool isInversion = false) = 0;
	virtual bool RemoveMask(int index, int n = 1, bool isInversion = false) = 0;
	virtual int GetMaskCount(bool isInversion = false) = 0;
	virtual void SetDocumentTable(IDocumentTable* documentTable) = 0;
	virtual quint64 GetUnitCount() = 0;
	virtual QList<quint64> GetUnitPositions(quint64 offset = 0, quint64 limit = -1) = 0;
	virtual QList<QByteArray> GetDocuments(quint64 offset = 0, quint64 limit = -1) = 0;
};


} // namespace imtmdbx
