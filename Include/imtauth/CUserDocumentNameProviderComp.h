// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentNameProvider.h>


namespace imtauth
{


class CUserDocumentNameProviderComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentNameProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserDocumentNameProviderComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentNameProvider);
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentNameProvider)
	virtual QString GetDefaultDocumentName(const QByteArray& objectId, const istd::IChangeable& document) const override;
};


} // namespace imtauth
