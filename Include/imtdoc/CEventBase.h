#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdoc
{


class CEventBase: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray Type() const;
	bool IsAccepted() const;
	void Accept();
	void Reject();

	QByteArray GetUserId() const;
	QByteArray GetDocumentId() const;
	QByteArray GetDocumentTypeId() const;
	QString GetDocumentName() const;
	QUrl GetDocumentUrl() const;
	bool IsDocumentDirty() const;

protected:
	bool m_isAccepted = true;

	QByteArray m_userId;
	QByteArray m_documentId;
	QByteArray m_documentTypeId;
	QString m_documentName;
	QUrl m_documentUrl;
	bool m_isDirty = false;
};


} // namespace imtdoc


