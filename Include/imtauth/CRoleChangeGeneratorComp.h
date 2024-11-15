#pragma once


// ImtCore includes
#include <imtbase/CDocumentChangeGeneratorCompBase.h>


namespace imtauth
{


class CRoleChangeGeneratorComp: public imtbase::CDocumentChangeGeneratorCompBase
{
public:
	typedef imtbase::CDocumentChangeGeneratorCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleChangeGeneratorComp);
	I_END_COMPONENT;

protected:
	QString GetRoleName(const QByteArray& roleId) const;

	// reimplemented (imtbase::CDocumentChangeGeneratorCompBase)
	virtual bool CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage) override;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const override;
};


} // namespace imtauth


