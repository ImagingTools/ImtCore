#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtbase/CTimeRange.h>


namespace imtlog
{


class CEventContainer: protected ilog::CMessageContainer
{
public:
	typedef ilog::CMessageContainer BaseClass;

	int GetMessagesCount() const;
	imtbase::CTimeRange GetTimeRange() const;

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
};


} // namespace imtlog


