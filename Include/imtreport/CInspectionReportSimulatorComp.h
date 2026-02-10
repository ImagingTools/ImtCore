// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtreport/IReportDocument.h>
#include <imtreport/IInspectionReportBuilder.h>


namespace imtreport
{


class CInspectionReportSimulatorComp : public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInspectionReportSimulatorComp);
		I_ASSIGN(m_reportDocumentCompPtr, "ReportDocument", "Report document", true, "ReportDocument");
		I_ASSIGN(m_inspectionReportBuilderCompPtr, "InspectionReportBuilder", "Inspection report builder", true, "InspectionReportBuilder");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void PrepareReportInputData(IInspectionReportBuilder::ReportInputData& reportData) const;

private:
	I_REF(imtreport::IReportDocument, m_reportDocumentCompPtr);
	I_REF(imtreport::IInspectionReportBuilder, m_inspectionReportBuilderCompPtr);
};


} // namespace imtreport


