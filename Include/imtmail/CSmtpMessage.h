// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtmail/ISmtpMessage.h>


namespace imtmail
{


class CSmtpMessage: virtual public imtmail::ISmtpMessage
{
public:
	virtual QString GetFrom() const override;
	virtual void SetFrom(const QString& from) override;
	virtual QString GetTo() const override;
	virtual void SetTo(const QString& to) override;
	virtual QString GetSubject() const override;
	virtual void SetSubject(const QString& subject) override;
	virtual QString GetBody() const override;
	virtual void SetBody(const QString& body) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_from;
	QString m_to;
	QString m_subject;
	QString m_body;
};


} // namespace imtmail
