// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectProviderComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>


namespace imtservergql
{


// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)

QJsonObject CObjectProviderComp::CreateInternalResponse(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_objectCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'Object' was not set", "CObjectProviderComp");
		return QJsonObject();
	}

	QByteArray json;
	{
		iser::CJsonMemWriteArchive archive;
		if (!m_objectCompPtr->Serialize(archive)){
			errorMessage = QStringLiteral("Failed to perform serialization to the archive from the object").toUtf8();

			return QJsonObject();
		}

		json = archive.GetData();
	}

	QJsonObject rootObj;
	QJsonDocument doc = QJsonDocument::fromJson(json);
	if (doc.isNull() || !doc.isObject()){
		errorMessage = QStringLiteral("Failed to convert to a json object from json").toUtf8();

		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), doc.object());

	return rootObj;
}


} // namespace imtservergql


