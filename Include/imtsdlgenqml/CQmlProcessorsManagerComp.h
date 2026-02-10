// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdlgenqml
{

/// \todo rename it to SdlQmlManager
class CQmlProcessorsManagerComp: public ilog::CLoggerComponentBase,
								public iproc::CSyncProcessorBase
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQmlProcessorsManagerComp)
		I_REGISTER_INTERFACE(iproc::IProcessor)
		I_ASSIGN(m_sdlArgumentParserCompPtr, "ArgumentParser", "Argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlSchemaParamsCompPtr, "SdlSchemaParameters", "The parameters of the current schema", true, "SdlSchemaParameters")
		I_ASSIGN_MULTI_0(m_sdlProcessorsCompListPtr, "SdlProcessorList", "The list of SDL processors", true)
	I_END_COMPONENT;

protected:
	// reimplemented (iproc::IProcessor)
	virtual TaskState DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* outputPtr,
		ibase::IProgressManager* progressManagerPtr = nullptr) override;

private:
	bool CreateCode();

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_sdlArgumentParserCompPtr);
	I_REF(iprm::IParamsSet, m_sdlSchemaParamsCompPtr);
	I_MULTIREF(iproc::IProcessor, m_sdlProcessorsCompListPtr);
};


} // namespace imtsdlgenqml







