#pragma once


// Acf includes
#include <istd/IPolymorphic.h>


// forward declaration
class QIODevice;


namespace imtoas
{


class CJsonSchemaParser: public istd::IPolymorphic
{
public:
	bool ParseSchema(QIODevice& schemaIoDevice);
};


} // namespace imtoas
