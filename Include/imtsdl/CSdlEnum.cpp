// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlEnum.h>

//Acf includes
#include <istd/CChangeNotifier.h>
#include <iprm/IParamsSet.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtsdl
{


// public methods


QList<QPair<QString, QString>> CSdlEnum::GetValues() const
{
	return m_values;
}


void CSdlEnum::SetValues(const QList<QPair<QString, QString>>& values)
{
	if (m_values != values){
		istd::CChangeNotifier notifier(this);
		m_values = values;
	}
}


void CSdlEnum::AddValue(const QPair<QString, QString>& value)
{
	istd::CChangeNotifier notifier(this);

	auto foundIter = std::find_if(m_values.begin(), m_values.end(), [&value](const QPair<QString, QString>& lookup){
		return (value.first == lookup.first);
	});

	if (foundIter ==  m_values.end()){
		m_values << value;
	}
	else {
		foundIter->second = value.second;
	}
}


// reimplemented(iser::ISerializable)

bool CSdlEnum::Serialize(iser::IArchive& archive)
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

	return retVal;
}


bool CSdlEnum::operator==(const CSdlEnum& other) const
{
	bool retVal = m_name == other.m_name;
	retVal = retVal && m_targetHeaderFilePath == other.m_targetHeaderFilePath;
	retVal = retVal && m_schemaFilePath == other.m_schemaFilePath;
	retVal = retVal && m_isExternal == other.m_isExternal;
	retVal = retVal && m_qmlImportDeclaration == other.m_qmlImportDeclaration;
	retVal = retVal && GetSchemaParams().IsEqual(other.GetSchemaParams());


	return retVal;
}


} // namespace imtsdl


