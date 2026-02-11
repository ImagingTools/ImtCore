// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlDocumentType.h>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/IArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtsdl
{


// public methods

const CSdlEntryBase& CSdlDocumentType::GetReferenceType() const
{
	if (!m_referenceType.GetName().isEmpty()){
		return m_referenceType;
	}

	return m_referenceUnionType;
}


void CSdlDocumentType::SetReferenceType(const CSdlType& referenceType)
{
	if (m_referenceType != referenceType){
		istd::CChangeNotifier notifier(this);
		m_referenceType = referenceType;
	}
}


void CSdlDocumentType::SetReferenceType(const CSdlUnion& referenceType)
{
	if (m_referenceUnionType != referenceType){
		istd::CChangeNotifier notifier(this);
		m_referenceUnionType = referenceType;
	}
}


QMultiMap<CSdlDocumentType::OperationType, CSdlRequest> CSdlDocumentType::GetOperationsList() const
{
	return m_operationsList;
}


bool CSdlDocumentType::HasRequest(OperationType operationType) const
{
	return m_operationsList.contains(operationType);
}


CSdlRequest CSdlDocumentType::GetRequest(OperationType operationType) const
{
	if (m_operationsList.contains(operationType)){
		return m_operationsList.value(operationType);
	}

	Q_ASSERT_X(false, __func__, "Request for operationType does not exist. Did you check it with HasRequest()?");

	return CSdlRequest();
}


void CSdlDocumentType::SetOperationsList(const QMultiMap<OperationType, CSdlRequest>& operationsList)
{
	if (m_operationsList != operationsList){
		istd::CChangeNotifier notifier(this);
		m_operationsList = operationsList;
	}
}

void CSdlDocumentType::AddOperation(OperationType type, const CSdlRequest& operation)
{
	Q_ASSERT(type == OT_GET_VIEW || !m_operationsList.contains(type));

	if (type == OT_GET_VIEW || !m_operationsList.contains(type)){
		istd::CChangeNotifier notifier(this);
		m_operationsList.insert(type, operation);
	}
}


SdlDocumentTypeList CSdlDocumentType::GetSubtypes() const
{
	return m_subtypes;
}


void CSdlDocumentType::SetSubtypes(const SdlDocumentTypeList& subtypes)
{
	if (m_subtypes != subtypes){
		istd::CChangeNotifier notifier(this);
		m_subtypes = subtypes;
	}
}


void CSdlDocumentType::AddSubtype(const CSdlDocumentType& subtype)
{
	Q_ASSERT(!m_subtypes.contains(subtype));

	if (!m_subtypes.contains(subtype)){
		istd::CChangeNotifier notifier(this);
		m_subtypes << subtype;
	}
}


bool CSdlDocumentType::operator==(const CSdlDocumentType& other) const
{
	return
		m_name == other.m_name &&
		m_referenceType == other.m_referenceType &&
		m_referenceUnionType == other.m_referenceUnionType &&
		m_operationsList == other.m_operationsList &&
		m_subtypes == other.m_subtypes &&
		GetSchemaParams().IsEqual(other.GetSchemaParams());
}



// reimplemented(iser::ISerializable)

bool CSdlDocumentType::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag nameTag("Name", "", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag referenceTag("Reference", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(referenceTag);
	retVal = retVal && m_referenceType.Serialize(archive);
	retVal = retVal && archive.EndTag(referenceTag);

	iser::CArchiveTag unionReferenceTag("UnionReference", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(unionReferenceTag);
	retVal = retVal && m_referenceUnionType.Serialize(archive);
	retVal = retVal && archive.EndTag(unionReferenceTag);

	retVal = retVal && SerializeOperationsList(archive, m_operationsList, "OperationList", "Operation", "Type", "Request");

	retVal = retVal && SerializeDocumentTypeList(archive, m_subtypes, "SubtypeList", "Subtype");

	return retVal;
}


// public static methods

bool CSdlDocumentType::SerializeDocumentTypeList(
			iser::IArchive& archive,
			SdlDocumentTypeList& container,
			const QByteArray& containerTagName,
			const QByteArray& elementTagName)
{
	iser::CArchiveTag elementsTag(containerTagName, "List of elements", iser::CArchiveTag::TT_MULTIPLE);
	iser::CArchiveTag elementTag(elementTagName, "Single element", iser::CArchiveTag::TT_LEAF, &elementsTag);

	bool retVal = true;

	bool isStoring = archive.IsStoring();
	int elementsCount = imtbase::narrow_cast<int>(container.size());

	retVal = retVal && archive.BeginMultiTag(elementsTag, elementTag, elementsCount);
	if (!retVal){
		return false;
	}

	if (isStoring){
		for (int i = 0; i < elementsCount; ++i){
			CSdlDocumentType sdlField = container[i];

			retVal = retVal && archive.BeginTag(elementTag);
			retVal = retVal && sdlField.Serialize(archive);
			retVal = retVal && archive.EndTag(elementTag);
		}
	}
	else{
		container.clear();

		for (int i = 0; i < elementsCount; ++i){
			CSdlDocumentType sdlField;
			retVal = retVal && archive.BeginTag(elementTag);
			retVal = retVal && sdlField.Serialize(archive);
			retVal = retVal && archive.EndTag(elementTag);

			if (retVal){
				container.push_back(sdlField);
			}
		}
	}

	retVal = retVal && archive.EndTag(elementsTag);

	return retVal;
}


bool CSdlDocumentType::SerializeOperationsList(
			iser::IArchive& archive,
			QMultiMap<OperationType, CSdlRequest>& container,
			const QByteArray& containerTagName,
			const QByteArray& elementTagName,
			const QByteArray& keyTagId,
			const QByteArray& valueTagId)
{
	bool retVal = true;

	static iser::CArchiveTag parametersTag(containerTagName, "Container", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag parameterTag(elementTagName, "Single element", iser::CArchiveTag::TT_GROUP, &parametersTag, true);
	static iser::CArchiveTag parameterKeyTag(keyTagId, "Key of parameter", iser::CArchiveTag::TT_LEAF, &parameterTag);
	static iser::CArchiveTag parameterValueTag(valueTagId, "Value of parameter", iser::CArchiveTag::TT_GROUP, &parameterTag, true);

	if (archive.IsStoring()){
		int paramsCount = imtbase::narrow_cast<int>(container.size());

		retVal = retVal && archive.BeginMultiTag(parametersTag, parameterTag, paramsCount);

		for (auto iterator = container.begin(); iterator != container.end(); ++iterator){
			retVal = retVal && archive.BeginTag(parameterTag);

			OperationType key = iterator.key();
			retVal = retVal && archive.BeginTag(parameterKeyTag);
			retVal = retVal && I_SERIALIZE_ENUM(OperationType, archive, key);
			retVal = retVal && archive.EndTag(parameterKeyTag);

			CSdlRequest value = iterator.value();
			retVal = retVal && archive.BeginTag(parameterValueTag);
			retVal = retVal && value.Serialize(archive);
			retVal = retVal && archive.EndTag(parameterValueTag);

			retVal = retVal && archive.EndTag(parameterTag);
		}

		retVal = retVal && archive.EndTag(parametersTag);
	}
	else{
		int paramsCount = 0;
		container.clear();

		retVal = retVal && archive.BeginMultiTag(parametersTag, parameterTag, paramsCount);

		if (!retVal){
			return false;
		}

		for (int i = 0; i < paramsCount; ++i){
			retVal = retVal && archive.BeginTag(parameterTag);

			OperationType key;
			retVal = retVal && archive.BeginTag(parameterKeyTag);
			retVal = retVal && I_SERIALIZE_ENUM(OperationType, archive, key);
			retVal = retVal && archive.EndTag(parameterKeyTag);

			if (!retVal){
				return false;
			}

			CSdlRequest value;
			retVal = retVal && archive.BeginTag(parameterValueTag);
			retVal = retVal && value.Serialize(archive);
			retVal = retVal && archive.EndTag(parameterValueTag);

			retVal = retVal && archive.EndTag(parameterTag);

			container.insert(key, value);
		}

		retVal = retVal && archive.EndTag(parametersTag);
	}

	return retVal;
}

} // namespace imtsdl
