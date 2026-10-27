// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentNameProvider.h>


namespace CollectionDocumentServiceIntegrationTestPck
{


/**
	Test name provider for \c iprm::ITextParam documents.
	The default document name is the configured prefix followed by the document text.
	An empty name is returned for an empty text.
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
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(const QByteArray& objectId, const istd::IChangeable& document) const override;

private:
	I_ATTR(QString, m_namePrefixAttrPtr);
};


} // namespace CollectionDocumentServiceIntegrationTestPck
