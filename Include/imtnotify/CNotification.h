// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtnotify/INotification.h>


namespace imtnotify
{


/**
	Notification as an ilog::CMessage.
*/
class CNotification:
			public ilog::CMessage,
			virtual public INotification
{
public:
	typedef ilog::CMessage BaseClass;

	CNotification();

	static QByteArray GetMessageTypeId();

	// reimplemented (imtnotify::INotification)
	virtual void SetCategory(InformationCategory category) override;
	virtual void SetDescription(const QString& description) override;
	virtual void SetTimeStamp(const QDateTime& timeStamp) override;
	virtual const NotificationScope& GetScope() const override;
	virtual void SetScope(const NotificationScope& scope) override;
	virtual const QByteArray& GetTypeId() const override;
	virtual void SetTypeId(const QByteArray& typeId) override;
	virtual const ObjectLinkInfo& GetSubject() const override;
	virtual void SetSubject(const ObjectLinkInfo& subject) override;
	virtual const ObjectLinkInfos& GetActors() const override;
	virtual void SetActors(const ObjectLinkInfos& actors) override;
	virtual const NotificationArguments& GetArguments() const override;
	virtual void SetArguments(const NotificationArguments& arguments) override;
	virtual const QByteArray& GetGroupKey() const override;
	virtual void SetGroupKey(const QByteArray& groupKey) override;
	virtual int GetGroupCount() const override;
	virtual void SetGroupCount(int groupCount) override;
	virtual bool IsDelivered() const override;
	virtual void SetDelivered(bool isDelivered) override;
	virtual bool IsRead() const override;
	virtual void SetRead(bool isRead) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;

private:
	NotificationScope m_scope;
	QByteArray m_typeId;
	ObjectLinkInfo m_subject;
	ObjectLinkInfos m_actors;
	NotificationArguments m_arguments;
	QByteArray m_groupKey;
	int m_groupCount;
	bool m_isDelivered;
	bool m_isRead;
};


} // namespace imtnotify


