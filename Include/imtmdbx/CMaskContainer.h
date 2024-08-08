#pragma once


// ImtCore includes
#include <imtmdbx/IMask.h>
#include <imtmdbx/IDocumentTable.h>

// Qt includes
#include <QtCore/QList>

// std includes
#include <iostream>


namespace imtmdbx
{


class CMaskContainer
{
public:

	enum OperationType
	{
		OT_OR,
		OT_AND
	};

	CMaskContainer(OperationType operationType);

	void AddMask(IMask* mask, bool isInversion = false);
	void Clear();
	bool RemoveLastMask(bool isInversion = false);
	bool RemoveMask(int index, bool isInversion = false);
	int GetMaskCount(bool isInversion = false);
	void SetDocumentTable(IDocumentTable* documentTable);
	quint64 GetUnitCount();
	QList<quint64> GetUnitPositions(quint64 offset = 0, int limit = -1);
	QList<QByteArray> GetDocuments(quint64 offset = 0, int limit = -1);

protected:
	bool GetActiveItem(quint64& activeOffset, quint64& activeItem, bool isStart);

protected:
	QList<IMask*> m_maskList;
	QList<IMask*> m_maskListInv;
	OperationType m_operationType;
	IDocumentTable* m_documentTable;
};


}//namespace imtmdbx
