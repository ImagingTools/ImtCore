#pragma once


// Qt includes
#include <QtCore/QList>

// std includes
#include <iostream>

// ImtCore includes
#include <imtmdbx/IMaskContainer.h>


namespace imtmdbx
{


class CMaskContainer: virtual public IMaskContainer, virtual public IMask
{
public:

	enum OperationType
	{
		OT_OR,
		OT_AND
	};

	CMaskContainer(OperationType operationType);

	// reimplemented (IMask)
	virtual bool GetUnit(quint64 position) override;
	virtual bool SetUnit(quint64 position, bool unit) override;

	virtual bool GetItem(quint64 offset, quint64& item) override;
	virtual bool SetItem(quint64 offset, quint64 item) override;

	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) override;
	virtual bool GetNextItemOffset(quint64& offset, qint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffffffffff) override;

	// reimplemented (IMaskContainer)
	void AddMask(IMask* mask, bool isInversion = false) override;
	void Clear() override;
	bool RemoveLastMask(bool isInversion = false) override;
	bool RemoveMask(int index, int n = 1, bool isInversion = false) override;
	int GetMaskCount(bool isInversion = false) override;
	void SetDocumentTable(IDocumentTable* documentTable) override;
	quint64 GetUnitCount() override;
	QList<quint64> GetUnitPositions(quint64 offset = 0, int limit = -1) override;
	QList<QByteArray> GetDocuments(quint64 offset = 0, int limit = -1) override;

protected:
	bool GetActiveItem(quint64& activeOffset, quint64& activeItem, bool isStart);

protected:
	QList<IMask*> m_maskList;
	QList<IMask*> m_maskListInv;
	OperationType m_operationType;
	IDocumentTable* m_documentTable;
};


}//namespace imtmdbx
