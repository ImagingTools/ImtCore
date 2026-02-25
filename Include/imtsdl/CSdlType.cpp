// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlType.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iprm/CParamsSet.h>


// ImtCore includes
#include <imtsdl/CSdlField.h>


namespace imtsdl
{


// public methods


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


// reimplemented(iser::ISerializable)

bool CSdlType::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag targetHeaderFileTag("TargetHeaderFile", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetHeaderFileTag);
	retVal = retVal && archive.Process(m_targetHeaderFilePath);
	retVal = retVal && archive.EndTag(targetHeaderFileTag);

	iser::CArchiveTag schemaFileTag("SchemaFile", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(schemaFileTag);
	retVal = retVal && archive.Process(m_schemaFilePath);
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
	retVal = retVal && m_targetHeaderFilePath == other.m_targetHeaderFilePath;
	retVal = retVal && m_schemaFilePath == other.m_schemaFilePath;
	retVal = retVal && m_isExternal == other.m_isExternal;
	retVal = retVal && m_qmlImportDeclaration == other.m_qmlImportDeclaration;
	retVal = retVal && GetSchemaParams().IsEqual(other.GetSchemaParams());


	return retVal;
}


} // namespace imtsdl


