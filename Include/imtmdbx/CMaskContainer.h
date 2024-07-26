#pragma once


// ImtCore includes
#include <imtmdbx/IMask.h>

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

	void AddMask(IMask* mask, bool isInversion);
	quint64 GetUnitCount();
	QList<quint64> GetUnitPositions(quint64 offset = 0, int limit = -1);

protected:
	QList<IMask*> m_maskList;
	QList<IMask*> m_maskListInv;
	OperationType m_operationType;
};


}//namespace imtmdbx
