#include <imtsdl/CSdlType.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iprm/CParamsSet.h>


// ImtCore includes
#include <imtsdl/CSdlField.h>


namespace imtsdl
{


// public methods

CSdlType::CSdlType()
	:m_isExternal(false)
{
}


QString CSdlType::GetName() const
{
	return m_name;
}


void CSdlType::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}


SdlFieldList CSdlType::GetFields() const
{
	return m_fields;
}


void CSdlType::SetFields(const SdlFieldList& fields)
{
	if (fields != m_fields){
		istd::CChangeNotifier notifier(this);
		m_fields = fields;
	}
}


QString CSdlType::GetNamespace() const
{
	return m_namespace;
}


void CSdlType::SetNamespace(const QString& aNamespace)
{
	if (m_namespace != aNamespace){
		istd::CChangeNotifier notifier(this);
		m_namespace = aNamespace;
	}
}


QString CSdlType::GetTargetHeaderFile() const
{
	return m_targetHeaderFile;
}


void CSdlType::SetTargetHeaderFile(const QString& headerFile)
{
	if (m_targetHeaderFile != headerFile){
		istd::CChangeNotifier notifier(this);
		m_targetHeaderFile = headerFile;
	}
}


QString CSdlType::GetSchemaFile() const
{
	return m_schemaFile;
}


void CSdlType::SetSchemaFile(const QString& schemaFile)
{
	if (m_schemaFile != schemaFile){
		istd::CChangeNotifier notifier(this);
		m_schemaFile = schemaFile;
	}
}


bool CSdlType::IsExternal() const
{
	return m_isExternal;
}


void CSdlType::SetExternal(bool isExternal)
{
	if (m_isExternal != isExternal){
		istd::CChangeNotifier notifier(this);
		m_isExternal = isExternal;
	}
}


QString CSdlType::GetQmlImportDeclaration() const
{
	return m_qmlImportDeclaration;
}


void CSdlType::SetQmlImportDeclaration(const QString& qmlImportDeclaration)
{
	if (m_qmlImportDeclaration != qmlImportDeclaration) {
		istd::CChangeNotifier notifier(this);
		m_qmlImportDeclaration = qmlImportDeclaration;
	}
}


const iprm::IParamsSet& CSdlType::GetSchemaParams() const
{
	if (m_schemaParamsPtr == nullptr){
		static const iprm::CParamsSet dummyParams;

		return dummyParams;
	}

	return *m_schemaParamsPtr;
}


void CSdlType::SetSchemaParamsPtr(const std::shared_ptr<iprm::IParamsSet>& schemaParamsPtr)
{
	m_schemaParamsPtr = schemaParamsPtr;

	// ensure, params is comparable
	if (schemaParamsPtr != nullptr){
		const bool isEqual = schemaParamsPtr->IsEqual(*schemaParamsPtr);
		if (!isEqual){
			Q_ASSERT_X(false, "Initializing incomparable paras!", __func__);
			qFatal("Initializing incomparable paras!");
		}
	}
}


// reimplemented(iser::ISerializable)

bool CSdlType::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag namespaceTag("Namespace", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(namespaceTag);
	retVal = retVal && archive.Process(m_namespace);
	retVal = retVal && archive.EndTag(namespaceTag);

	iser::CArchiveTag targetHeaderFileTag("TargetHeaderFile", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetHeaderFileTag);
	retVal = retVal && archive.Process(m_targetHeaderFile);
	retVal = retVal && archive.EndTag(targetHeaderFileTag);

	iser::CArchiveTag schemaFileTag("SchemaFile", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(schemaFileTag);
	retVal = retVal && archive.Process(m_schemaFile);
	retVal = retVal && archive.EndTag(schemaFileTag);

	iser::CArchiveTag externalTag("IsExternal", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(externalTag);
	retVal = retVal && archive.Process(m_isExternal);
	retVal = retVal && archive.EndTag(externalTag);

	iser::CArchiveTag qmlDeclarationTag("QmlImportDeclaration", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(qmlDeclarationTag);
	retVal = retVal && archive.Process(m_qmlImportDeclaration);
	retVal = retVal && archive.EndTag(qmlDeclarationTag);

	retVal = retVal && CSdlField::SerializeSdlFieldList(archive, m_fields, "Fields", "Field");

	return retVal;
}


bool CSdlType::operator==(const CSdlType& other) const
{
	bool retVal = m_name == other.m_name;
	retVal = retVal && m_fields == other.m_fields;
	retVal = retVal && m_namespace == other.m_namespace;
	retVal = retVal && m_targetHeaderFile == other.m_targetHeaderFile;
	retVal = retVal && m_schemaFile == other.m_schemaFile;
	retVal = retVal && m_isExternal == other.m_isExternal;
	retVal = retVal && m_qmlImportDeclaration == other.m_qmlImportDeclaration;
	retVal = retVal && GetSchemaParams().IsEqual(other.GetSchemaParams());


	return retVal;
}


} // namespace imtsdl


