// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserDocumentNameProviderComp.h>


// ImtCore includes
#include <imtauth/IUserBaseInfo.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CUserDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const IUserBaseInfo* userBaseInfoPtr = dynamic_cast<const IUserBaseInfo*>(&document);
	if (userBaseInfoPtr != nullptr){
		return userBaseInfoPtr->GetName();
	}

	return QString();
}


} // namespace imtauth
