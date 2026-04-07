// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CDocumentManagerBase.h>


namespace imtdoc
{


class CCollectionDocumentManagerBase: virtual public CDocumentManagerBase
{
public:
	// reimplemented (imtdoc::IDocumentManager)
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) override;
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual OperationStatus SaveDocument(
		const QByteArray& userId,
		const QByteArray& documentId,
		const QString& documentName = QString(),
		QString* errorMessage = nullptr) override;

protected:
	virtual imtbase::IObjectCollection* GetCollection() const = 0;
};


} // namespace imtdoc
