// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleDocumentNameProviderComp.h>


// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauth
{


// reimplemented (imtdoc::IDocumentNameProvider)

QString CRoleDocumentNameProviderComp::GetDefaultDocumentName(const QByteArray& /*objectId*/, const istd::IChangeable& document) const
{
	const IRole* rolePtr = dynamic_cast<const IRole*>(&document);
	if (rolePtr != nullptr){
		return rolePtr->GetRoleName();
	}

	return QString();
}


} // namespace imtauth
