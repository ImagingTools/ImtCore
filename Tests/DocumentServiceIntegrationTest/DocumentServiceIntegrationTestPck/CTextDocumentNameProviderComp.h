// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentNameProvider.h>


namespace DocumentServiceIntegrationTestPck
{


/**
	Test name provider for \c iprm::ITextParam documents.
	The default document name is the configured prefix followed by the document text.
	An empty name is returned for an empty text.
	If \c AppendSource is enabled, the collection object ID and the object revision from
	the document source parameters are appended: \c "<name> [<objectId>/<revision>]".
*/
class CTextDocumentNameProviderComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextDocumentNameProviderComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
		I_ASSIGN(m_namePrefixAttrPtr, "NamePrefix", "Prefix of the generated document name", false, "");
		I_ASSIGN(m_appendSourceAttrPtr, "AppendSource", "Append the object ID and the revision from the document source parameters", true, false);
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(const istd::IChangeable& document, const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	I_ATTR(QString, m_namePrefixAttrPtr);
	I_ATTR(bool, m_appendSourceAttrPtr);
};


} // namespace DocumentServiceIntegrationTestPck
