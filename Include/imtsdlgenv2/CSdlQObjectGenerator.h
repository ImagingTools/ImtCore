#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/ISdlTypeListProvider.h>
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

	CSdlQObjectGenerator(const imtsdl::ISdlEnumListProvider& enumListProvider,
				const imtsdl::ISdlUnionListProvider& unionListProvider,
				const imtsdl::ISdlTypeListProvider& typeListProvider);

	bool ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType);
	bool ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlType& sdlType);

protected:
	QString GetQObjectTypeName(const imtsdl::CSdlField& field, bool withPointer = true);

private:
	const imtsdl::ISdlEnumListProvider& m_enumListProvider;
	const imtsdl::ISdlUnionListProvider& m_unionListProvider;
	const imtsdl::ISdlTypeListProvider& m_typeListProvider;
};


} // namespace imtsdlgenv2

