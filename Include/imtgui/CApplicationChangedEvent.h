#pragma once


// ACF includes
#include <ilog/CExtMessage.h>


namespace imtgui
{


class CApplicationChangedEvent: virtual public ilog::CExtMessage
{
public:
	typedef ilog::CExtMessage BaseClass;

	enum MessageId
	{
		MI_DOCUMENT_CHANGED = 110000000,
	};

	/**
		Information about the application changes.
	*/
	struct ApplicationChangesInfo
	{
		/**
			Name of the application.
		*/
		QString applicationName;

		/**
			Logged in user.
		*/
		QString userName;
	};

	CApplicationChangedEvent();
	CApplicationChangedEvent(
				const ApplicationChangesInfo& applicationChangesInfo,
				istd::IInformationProvider::InformationCategory category,
				const QString& text,
				const QString& source,
				int flags = 0,
				const QDateTime* timeStampPtr = nullptr,
				const iser::IObjectFactory* factoryPtr = nullptr);

	const ApplicationChangesInfo& GetApplicationChangesInfo() const;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

public:
	static QByteArray GetTypeId();

private:
	ApplicationChangesInfo m_applicationChangesInfo;
};


} // namespace imtgui


