// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CDocumentServiceBase.h>


namespace imtdoc
{


class CCollectionDocumentServiceBase: virtual public CDocumentServiceBase
{
public:
	// reimplemented (imtdoc::CDocumentServiceBase) — task dispatch
	virtual void DoCreateNewDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoOpenDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoSaveDocument(const QByteArray& taskId, const TaskParams& params) override;
	virtual void DoCloseDocument(const QByteArray& taskId, const TaskParams& params) override;

	// reimplemented (imtdoc::IDocumentService) — synchronous helpers
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;

protected:
	virtual imtbase::IObjectCollection* GetCollection() const = 0;

private:
	QMap<QByteArray, QByteArray> m_proposedSourceDocumentIds;
};


} // namespace imtdoc
