#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtreport/CReportDocument.h>


namespace imtreport
{


/**
	Base implementation of a report document.
*/
class CReportDocumentComp: public icomp::CComponentBase, public CReportDocument
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CReportDocument BaseClass2;

	I_BEGIN_COMPONENT(CReportDocumentComp);
		I_REGISTER_INTERFACE(imtreport::IReportDocument);
		I_REGISTER_INTERFACE(idoc::IMultiPageDocument);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


} // namespace imtreport


