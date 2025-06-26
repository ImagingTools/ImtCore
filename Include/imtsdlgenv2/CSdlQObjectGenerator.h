#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdlgenv2
{

/**
	A Qt QObject C++ class generator of SDL types
*/
class CSdlQObjectGenerator:
			public istd::IPolymorphic,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef istd::IPolymorphic BaseClass;

	bool ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType);
	bool ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType);
};


} // namespace imtsdlgenv2

