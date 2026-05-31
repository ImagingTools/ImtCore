// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRepresentationJoinerComp.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CGqlRepresentationJoinerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;
	for (int i = 0; i < m_representationControllersCompPtr.GetCount(); i++){
		imtgql::IGqlRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			QJsonObject representationObj = representationControllerPtr->CreateRepresentationFromRequest(gqlRequest, errorMessage);
			if (!representationObj.isEmpty()){
				QJsonObject sourceData = representationObj.contains(QStringLiteral("data"))
					? representationObj.value(QStringLiteral("data")).toObject()
					: representationObj;

				for (auto it = sourceData.begin(); it != sourceData.end(); ++it){
					dataObj.insert(it.key(), it.value());
				}
			}
		}
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


bool CGqlRepresentationJoinerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, const QJsonObject& representation) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return false;
	}

	if (representation.isEmpty()){
		return false;
	}

	for (int i = 0; i < m_representationControllersCompPtr.GetCount(); i++){
		imtgql::IGqlRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			bool result = representationControllerPtr->UpdateModelFromRepresentation(request, representation);
			if (!result){
				return false;
			}
		}
	}

	return true;
}


} // namespace imtservergql


