#pragma once


// Acf includes
#include <imod/TModelWrap.h>
#include <ilog/CMessageContainer.h>


namespace imtlog
{


class CEventHistoryResultContainer:
			public imod::TModelWrap<ilog::CMessageContainer>
{
public:
	CEventHistoryResultContainer();

	void Close();
	bool IsClosed();

private:
	bool m_isClosed;
	QMutex m_mutex;
};


} // namespace imtlog


