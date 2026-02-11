// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>


namespace imtauth
{


class CUserChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserChangeGeneratorComp);
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", true, "RoleCollection");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User group collection", true, "UserGroupCollection");
	I_END_COMPONENT;

protected:
	QString GetRoleName(const QByteArray& roleId) const;
	QString GetGroupName(const QByteArray& groupId) const;

	// reimplemented (imtbase::CDocumentChangeGeneratorCompBase)
	virtual bool CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage) override;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
};


} // namespace imtauth


