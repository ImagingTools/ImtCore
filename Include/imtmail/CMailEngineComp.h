#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <istd/IChangeable.h>
#include <imod/TModelWrap.h>

#include <imtbase/IObjectCollection.h>
#include <imtbase/CObjectCollectionBase.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtmail/ISmtpMailSendRequest.h>
#include <imtmail/CMailSettings.h>


namespace imtmail
{


class CMailEngineComp: public QObject,
		public ilog::CLoggerComponentBase,
		public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{

	Q_OBJECT;
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CMailEngineComp)
		I_REGISTER_INTERFACE(CMailEngineComp);
		I_REGISTER_SUBELEMENT(MailSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(MailSettings, imtmail::IMailSettings, ExtractMailSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(MailSettings, istd::IChangeable, ExtractMailSettings);
		I_REGISTER_SUBELEMENT_INTERFACE(MailSettings, imod::IModel, ExtractMailSettings);
		I_ASSIGN(m_mailSettingsCompPtr, "MailSettings", "Mail access settings", false, "MailSettings");
		I_ASSIGN(m_objectCollectionCompPtr, "SmtpRequestsCollection", "Comment", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
	I_END_COMPONENT

	CMailEngineComp();

	void SendEmail(const QString &destination,
				   const QString &subject, const QString &body,
				   QStringList files = QStringList(),
				   QHash<QString, QByteArray> binaryData = QHash<QString, QByteArray>());

protected:
	void OnMailSettingsChanged(const istd::IChangeable::ChangeSet& changeSet, const imtmail::IMailSettings* mailSettingsPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;


	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

signals:
	void SendEmailSignal();

public slots:
	void SendEmailSlot();

private:
	template <typename Interface>
	static Interface* ExtractMailSettings(CMailEngineComp& component)
	{
		return &component.m_workingMailSettings;
	}


private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);
	I_REF(imtmail::IMailSettings, m_mailSettingsCompPtr);

	imtbase::TModelUpdateBinder<imtmail::IMailSettings, CMailEngineComp> m_mailSettingsObserver;
	imod::TModelWrap<imtmail::CMailSettings> m_workingMailSettings;
};


}


