// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgencpp/ICxxFilePostProcessor.h>


namespace imtsdlgencpp
{


/**
	Joins the source files of the schema: appends the objects section (the implementation of the QObject based classes)
	and, if enabled, the includes of the moc output to the main source file.
	The files are processed, only if the schema has both the source and the objects source file.
*/
class CCxxSourceTailPostProcessorComp:
			public ilog::CLoggerComponentBase,
			virtual public ICxxFilePostProcessor,
			private imtsdl::CSdlTools
{

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCxxSourceTailPostProcessorComp)
		I_REGISTER_INTERFACE(ICxxFilePostProcessor)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
	I_END_COMPONENT

	// reimplemented (ICxxFilePostProcessor)
	virtual bool ProcessFiles(const CxxEntryFileInfo& files, const iprm::IParamsSet* paramsPtr = nullptr) const override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
};


} // namespace imtsdlgencpp
