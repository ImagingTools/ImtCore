// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdoc/CDocumentServiceCompBase.h>


namespace imtdoc
{


class CDocumentServiceComp: public CDocumentServiceCompBase
{
public:
	typedef CDocumentServiceCompBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_ASSIGN(m_undoManagerFolderCompPtr, "UndoManagerFolder", "Root folder for undo manager snapshots", false, "UndoManagerFolder");
	I_END_COMPONENT

protected:
	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual OperationStatus CloseDocumentInternal(const QByteArray& userId, const QByteArray& documentId) override;

private:
	void RemoveUndoManagerDocumentDirectory(const QByteArray& documentId) const;

private:
	I_REF(ifile::IFileNameParam, m_undoManagerFolderCompPtr);
};


} // namespace imtdoc
