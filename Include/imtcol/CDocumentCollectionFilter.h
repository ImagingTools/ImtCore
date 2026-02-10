// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcol/IDocumentCollectionFilter.h>


namespace imtcol
{


class CDocumentCollectionFilter: virtual public IDocumentCollectionFilter
{
public:
	// reimplemented (imtcol::IDocumentCollectionFilter)
	virtual QByteArray GetDocumentId() const override;
	virtual void SetDocumentId(const QByteArray& documentId) override;
	virtual DocumentStates GetDocumentStates() const override;
	virtual bool AddDocumentState(DocumentState state) override;
	virtual bool RemoveDocumentState(DocumentState state) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_documentId;
	DocumentStates m_states;
};


} // namespace imtcol


