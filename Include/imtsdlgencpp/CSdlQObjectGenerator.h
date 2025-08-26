#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/ISdlEnumListProvider.h>
#include <imtsdl/ISdlUnionListProvider.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdlgencpp/CSdlGenTools.h>


namespace imtsdlgencpp
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

	bool ProcessHeaderClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const;
	bool ProcessSourceClassFile(QTextStream& stream, const imtsdl::CSdlEntryBase& sdlEntry) const;

protected:
	QString GetQObjectTypeName(const imtsdl::CSdlField& field, bool withPointer = true) const;

private:
	void CreatePropertiesForType(QTextStream& stream, const imtsdl::SdlFieldList& fieldList) const;
	void CreateAccessMethodsDefinition(QTextStream& stream, const imtsdl::SdlFieldList& fieldList) const;

private:
	const imtsdl::ISdlEnumListProvider& m_enumListProvider;
	const imtsdl::ISdlUnionListProvider& m_unionListProvider;
	const imtsdl::ISdlTypeListProvider& m_typeListProvider;
};


} // namespace imtsdlgencpp

