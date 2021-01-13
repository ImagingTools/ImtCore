#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtlog
{


class CEventContainer:
			public ilog::CMessageContainer
{
public:
	typedef ilog::CMessageContainer BaseClass;

	const CTimeRange GetMessagesTimeRange() const;
	const CTimeRange& GetTimeRange() const;
	bool SetTimeRange(const CTimeRange& timeRange);

	// reimplemented (ilog::IMessageContainer)
	virtual int GetWorstCategory() const override;
	virtual Messages GetMessages() const override;
	virtual void ClearMessages() override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& messagePtr) override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive & archive) override;

private:
	mutable QMutex m_mutex;

	CTimeRange m_timeRange;
};


} // namespace imtlog


