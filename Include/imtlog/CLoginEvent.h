#pragma once


// ACF includes
#include <ilog/CMessage.h>


namespace imtlog
{


class CLoginEvent: virtual public ilog::CMessage
{
public:
	typedef ilog::CMessage BaseClass;

	enum MessageId
	{
		MI_LOGIN_EVENT = 100000000
	};

	enum Action
	{
		ACT_LOGIN = 0,
		ACT_LOGOUT
	};

	struct LoginEventInfo
	{
		int action;
		QString userName;
	};

	CLoginEvent();
	CLoginEvent(
				const LoginEventInfo& loginEventInfo,
				istd::IInformationProvider::InformationCategory category,
				const QString& text = QString(),
				const QString& source = QString(),
				int flags = 0,
				const QDateTime* timeStampPtr = nullptr);

	const LoginEventInfo& GetLoginEventInfo() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

public:
	static QByteArray GetTypeId();

private:
	LoginEventInfo m_loginEventInfo;
};


} // namespace imtlog


