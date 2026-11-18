// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/IDocumentNameProvider.h>


namespace imtlicgql
{


/**
	Names a product document by the name of the product.
*/
class CProductDocumentNameProviderComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductDocumentNameProviderComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(
				const istd::IChangeable& document,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
};


} // namespace imtlicgql


