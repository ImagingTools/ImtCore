#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF includes
//#include <ilog/CMessageContainer.h>
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <iser/ISerializable.h>
#include <imtlog/IMessageHistoryConsumer.h>
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class CMessageHistoryContainer:
			virtual public iser::ISerializable,
			virtual public IMessageHistoryConsumer
{
public:
	const QDateTime& GetBeginTime();
	bool SetBeginTime(const QDateTime& time);

	const QDateTime& GetEndTime();
	bool SetEndTime(const QDateTime& time);

	CTimeRange GetTimeRange() const;
	bool SetTimeRange(const CTimeRange& timeRange);
	
	int GetMessagesCount();
	int GetWorstCategory() const;
	Messages GetMessages() const;
	void ClearMessages();

	// reimplemented (imtlog::IMessageHistoryConsumer)
	virtual bool IsMessageSupported(const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive & archive) override;

private:
	mutable QMutex m_mutex;

	ilog::CMessageContainer m_messageContainer;
	QList<quint64> m_messageIds;
	QDateTime m_beginTime;
	QDateTime m_endTime;
};


} // namespace imtlog


