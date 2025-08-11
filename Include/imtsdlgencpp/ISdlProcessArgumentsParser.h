#pragma once


//Acf includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdlgencpp
{

/// \todo remove it!
class ISdlProcessArgumentsParser: virtual public imtsdl::ISdlProcessArgumentsParser
{
public:
	virtual quint16 GetGenerationVersion() const = 0;

};


} // namespace imtsdlgencpp


