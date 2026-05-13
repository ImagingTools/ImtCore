// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlControllerComp.h>

// Qt includes
#include <QtCore/QJsonObject>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CGqlControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	QJsonObject rootObj;
	QJsonObject dataObj;
	dataObj.insert(QStringLiteral("Status"), QStringLiteral("OK"));
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


} // namespace imtservergql


