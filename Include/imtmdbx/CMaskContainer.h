// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QPair>

// ImtCore includes
#include <imtmdbx/imtmdbx.h>
#include <imtmdbx/IMaskContainer.h>


namespace imtmdbx
{


class CMaskContainer:
			virtual public IMask,
			virtual public IMaskContainer
{
public:
	enum OperationType
	{
		OT_OR,
		OT_AND
	};

	CMaskContainer(OperationType operationType);

	// reimplemented (imtmdbx::IMask)
	virtual bool GetUnit(quint64 position) override;
	virtual bool SetUnit(quint64 position, bool unit) override;

	virtual bool GetItem(quint64 offset, quint64& item) override;
	virtual bool SetItem(quint64 offset, quint64 item) override;

	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) override;
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffffffffff) override;

	// reimplemented (imtmdbx::IMaskContainer)
	virtual void AddMask(IMask* mask, bool isInversion = false) override;
	virtual void Clear() override;
	virtual bool RemoveLastMask(bool isInversion = false) override;
	virtual bool RemoveMask(int index, int n = 1, bool isInversion = false) override;
	virtual int GetMaskCount(bool isInversion = false) override;
	virtual void SetDocumentTable(IDocumentTable* documentTable) override;
	virtual quint64 GetUnitCount() override;
	virtual QList<quint64> GetUnitPositions(quint64 offset = 0, quint64 limit = -1) override;
	virtual QList<QByteArray> GetDocuments(quint64 offset = 0, quint64 limit = -1) override;

	void Erase();
	bool isEmpty();
	bool GetActiveItem(quint64& activeOffset, quint64& activeItem, bool isStart);

protected:
	QList<QPair<IMask*/*maskPtr*/,quint64/*nextOffset*/>> m_maskList;
	QList<QPair<IMask*/*maskPtr*/,quint64/*nextOffset*/>> m_maskListInv;
	OperationType m_operationType;
	IDocumentTable* m_documentTable;
};


}//namespace imtmdbx

