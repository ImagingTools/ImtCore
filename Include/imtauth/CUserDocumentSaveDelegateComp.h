// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdoc/IDocumentSaveDelegate.h>


namespace imtauth
{


/**
	Writes saved User documents over the stored users.
	Directory grants are owned by the credential sign-in, so the grants of the opened
	document are replaced by the stored ones under the update lock of the user.
*/
class CUserDocumentSaveDelegateComp:
			public icomp::CComponentBase,
			virtual public imtdoc::IDocumentSaveDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserDocumentSaveDelegateComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentSaveDelegate)
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentSaveDelegate)
	virtual bool UpdateCollectionObject(
				imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				istd::IChangeable& document,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
};


} // namespace imtauth


