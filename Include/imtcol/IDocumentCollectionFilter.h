// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtcol
{


class IDocumentCollectionFilter: virtual public iser::ISerializable
{
public:
	enum DocumentState
	{
		DS_ACTIVE,
		DS_INACTIVE,
		DS_DISABLED
	};

	typedef QVector<DocumentState> DocumentStates;

	virtual QByteArray GetDocumentId() const = 0;
	virtual void SetDocumentId(const QByteArray& documentId) = 0;
	virtual DocumentStates GetDocumentStates() const = 0;
	virtual bool AddDocumentState(DocumentState state) = 0;
	virtual bool RemoveDocumentState(DocumentState state) = 0;
};


} // namespace imtcol


