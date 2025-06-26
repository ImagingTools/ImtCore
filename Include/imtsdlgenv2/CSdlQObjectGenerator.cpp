#include <imtsdlgenv2/CSdlQObjectGenerator.h>


// Qt includes
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


bool CSdlQObjectGenerator::ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType)
{
	stream << QStringLiteral("// here must be a QOBject code))");

	return true;
}


bool CSdlQObjectGenerator::ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType)
{
	stream << QStringLiteral("// here must be a QOBject code))");

	return true;
}


} // namespace imtsdlgenv2

