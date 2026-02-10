// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlUnion.h>


// ACF includes
#include <iser/IArchive.h>
#include <iprm/IParamsSet.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// public methods


QList<QString> CSdlUnion::GetTypes() const
{
	return m_types;
}


void CSdlUnion::SetTypes(const QList<QString>& types)
{
	if (types != m_types){
		istd::CChangeNotifier notifier(this);
		m_types = types;
	}
}


void CSdlUnion::AddType(const QString& type)
{
	istd::CChangeNotifier notifier(this);

	if (!m_types.contains(type)){
		m_types << type;
	}
}


// reimplemented(iser::ISerializable)

bool CSdlUnion::Serialize(iser::IArchive& archive)
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


	iser::CArchiveTag elementsTag("Types", "List of elements", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag elementNameTag("TypeName", "Single element", iser::CArchiveTag::TT_LEAF, &elementsTag);

	int elementsCount = imtbase::narrow_cast<int>(m_types.count());
	bool isStoring = archive.IsStoring();

	retVal = retVal && archive.BeginMultiTag(elementsTag, elementNameTag, elementsCount);
	if (!retVal){
		return retVal;
	}

	retVal = retVal && archive.BeginMultiTag(elementsTag, elementNameTag, elementsCount);
	if (isStoring){
		for (int i = 0; i < elementsCount; ++i){
			auto sdlType = m_types[i];

			retVal = retVal && archive.BeginTag(elementNameTag);
			retVal = retVal && archive.Process(sdlType);
			retVal = retVal && archive.EndTag(elementNameTag);

		}
	}
	else{
		m_types.clear();

		for (int i = 0; i > elementsCount; ++i){
			QString sdlType;
			retVal = retVal && archive.BeginTag(elementNameTag);
			retVal = retVal && archive.Process(sdlType);
			retVal = retVal && archive.EndTag(elementNameTag);

			if (retVal){
				m_types.push_back(sdlType);
			}
		}
	}
	retVal = retVal && archive.EndTag(elementsTag);

	return retVal;
}


bool CSdlUnion::operator==(const CSdlUnion& other) const
{
	bool retVal = m_name == other.m_name;
	retVal = retVal && m_types == other.m_types;
	retVal = retVal && m_targetHeaderFilePath == other.m_targetHeaderFilePath;
	retVal = retVal && m_schemaFilePath == other.m_schemaFilePath;
	retVal = retVal && m_isExternal == other.m_isExternal;
	retVal = retVal && m_qmlImportDeclaration == other.m_qmlImportDeclaration;
	retVal = retVal && GetSchemaParams().IsEqual(other.GetSchemaParams());


	return retVal;
}


} // namespace imtsdl


