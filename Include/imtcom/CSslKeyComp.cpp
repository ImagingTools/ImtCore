#include <imtcom/CSslKeyComp.h>


// Acf includes
#include <iprm/IOptionsList.h>


namespace imtcom
{


ISslKey::KeyAlgorithm CSslKeyComp::GetKeyAlgorithm() const
{
	KeyAlgorithm sslKeyAlgorithm = KA_OPAQUE;

	if(!m_keyAlgorithmSelectionParamCompPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL KeyAlgorithm is set. OPAQUE will be returned. Set KeyAlgorithmSelection to fix it.");

		return sslKeyAlgorithm;
	}

	const int selectedIndex = m_keyAlgorithmSelectionParamCompPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* keyAlgorithmListPtr = m_keyAlgorithmSelectionParamCompPtr->GetSelectionConstraints();
	if (keyAlgorithmListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL KeyAlgorithm is set. OPAQUE will be returned. Set KeyAlgorithmSelection's constraints to fix it.");
		I_CRITICAL();

		return sslKeyAlgorithm;
	}

	const QByteArray optionId = keyAlgorithmListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = ISslKey::FromString(optionId, sslKeyAlgorithm);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL KeyAlgorithm. Set correct KeyAlgorithmSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return sslKeyAlgorithm;
}


ISslKey::EncodingFormat CSslKeyComp::GetEncodingFormat() const
{
	EncodingFormat sslEncodingFormat = EF_PEM;

	if(!m_encodingFormatSelectionParamCompPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL EncodingFormat is set. PEM will be returned. Set EncodingFormatSelection to fix it.");

		return sslEncodingFormat;
	}

	const int selectedIndex = m_encodingFormatSelectionParamCompPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* encodingFormatListPtr = m_encodingFormatSelectionParamCompPtr->GetSelectionConstraints();
	if (encodingFormatListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL EncodingFormat is set. PEM will be returned. Set EncodingFormatSelection's constraints to fix it.");
		I_CRITICAL();

		return sslEncodingFormat;
	}

	const QByteArray optionId = encodingFormatListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = ISslKey::FromString(optionId, sslEncodingFormat);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL EncodingFormat. Set correct EncodingFormatSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return sslEncodingFormat;
}


ISslKey::KeyType CSslKeyComp::GetKeyType() const
{
	KeyType sslKeyType = KT_PRIVATE_KEY;

	if(!m_keyTypeSelectionParamCompPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL KeyType is set. PRIVATE_KEY will be returned. Set KeyTypeSelection to fix it.");

		return sslKeyType;
	}

	const int selectedIndex = m_keyTypeSelectionParamCompPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* keyTypeListPtr = m_keyTypeSelectionParamCompPtr->GetSelectionConstraints();
	if (keyTypeListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL KeyType is set. PRIVATE_KEY will be returned. Set KeyTypeSelection's constraints to fix it.");
		I_CRITICAL();

		return sslKeyType;
	}

	const QByteArray optionId = keyTypeListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = ISslKey::FromString(optionId, sslKeyType);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL KeyType. Set correct KeyTypeSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return sslKeyType;
}


QByteArray CSslKeyComp::GetPassPhrase() const
{
	if (m_passPhraseCompPtr.IsValid()){
		return m_passPhraseCompPtr->GetId();
	}

	return QByteArray();
}


QByteArray CSslKeyComp::GetEncodedData() const
{
	if (m_encodedDataCompPtr.IsValid()){
		return m_encodedDataCompPtr->GetId();
	}

	return QByteArray();
}


QString CSslKeyComp::GetFilePath() const
{
	if (m_filePathCompPtr.IsValid()){
		return m_filePathCompPtr->GetPath();
	}

	return QString();
}


bool CSslKeyComp::IsFile() const
{
	if (!m_fileEnableCompPtr.IsValid()){
		SendCriticalMessage(0, "IsFile param is not set");
		I_CRITICAL();

		return false;
	}

	return m_fileEnableCompPtr->IsEnabled();
}


// reimplemented (iser::ISerializable)

bool CSslKeyComp::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag keyAlgorithmTag("KeyAlgorithm", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(keyAlgorithmTag);
	if (m_keyAlgorithmSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_keyAlgorithmSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(keyAlgorithmTag);

	iser::CArchiveTag encodingFormatTag("EncodingFormatSelection", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(encodingFormatTag);
	if (m_encodingFormatSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_encodingFormatSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(encodingFormatTag);

	iser::CArchiveTag keyTypeTag("KeyType", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(keyTypeTag);
	if (m_keyTypeSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_keyTypeSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(keyTypeTag);

	iser::CArchiveTag passPhraseTag("PassPhrase", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(passPhraseTag);
	if (m_passPhraseCompPtr.IsValid()){
		retVal = retVal && m_passPhraseCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(passPhraseTag);

	iser::CArchiveTag encodedTag("EncodedData", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(encodedTag);
	if (m_encodedDataCompPtr.IsValid()){
		retVal = retVal && m_encodedDataCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(encodedTag);

	iser::CArchiveTag filePathTag("FilePath", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(filePathTag);
	if (m_filePathCompPtr.IsValid()){
		retVal = retVal && m_filePathCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(filePathTag);

	iser::CArchiveTag fileTag("IsFile", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(fileTag);
	if (m_fileEnableCompPtr.IsValid()){
		retVal = retVal && m_fileEnableCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(fileTag);

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CSslKeyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_keyAlgorithmSelectionParamModelCompPtr.IsValid()){
		m_keyAlgorithmSelectionBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_keyAlgorithmSelectionParamModelCompPtr->AttachObserver(m_keyAlgorithmSelectionBridgePtr.GetPtr());
	}
	if (m_encodingFormatSelectionParamModelCompPtr.IsValid()){
		m_encodingFormatSelectionBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_encodingFormatSelectionParamModelCompPtr->AttachObserver(m_encodingFormatSelectionBridgePtr.GetPtr());
	}
	if (m_keyTypeSelectionParamModelCompPtr.IsValid()){
		m_keyTypeSelectionBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_keyTypeSelectionParamModelCompPtr->AttachObserver(m_keyTypeSelectionBridgePtr.GetPtr());
	}
	if (m_passPhraseModelCompPtr.IsValid()){
		m_passPhraseBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_passPhraseModelCompPtr->AttachObserver(m_passPhraseBridgePtr.GetPtr());
	}
	if (m_encodedDataModelCompPtr.IsValid()){
		m_encodedDataBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_encodedDataModelCompPtr->AttachObserver(m_encodedDataBridgePtr.GetPtr());
	}
	if (m_filePathModelCompPtr.IsValid()){
		m_filePathBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_filePathModelCompPtr->AttachObserver(m_filePathBridgePtr.GetPtr());
	}
	if (m_fileEnableModelCompPtr.IsValid()){
		m_fileEnablerBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_fileEnableModelCompPtr->AttachObserver(m_fileEnablerBridgePtr.GetPtr());
	}
}


void CSslKeyComp::OnComponentDestroyed()
{
	if (m_keyAlgorithmSelectionParamModelCompPtr.IsValid()){
		Q_ASSERT(m_keyAlgorithmSelectionBridgePtr.IsValid());
		if (m_keyAlgorithmSelectionParamModelCompPtr->IsAttached(m_keyAlgorithmSelectionBridgePtr.GetPtr())){
			m_keyAlgorithmSelectionParamModelCompPtr->DetachObserver(m_keyAlgorithmSelectionBridgePtr.GetPtr());
		}
	}
	if (m_encodingFormatSelectionParamModelCompPtr.IsValid()){
		Q_ASSERT(m_encodingFormatSelectionBridgePtr.IsValid());
		if (m_encodingFormatSelectionParamModelCompPtr->IsAttached(m_encodingFormatSelectionBridgePtr.GetPtr())){
			m_encodingFormatSelectionParamModelCompPtr->DetachObserver(m_encodingFormatSelectionBridgePtr.GetPtr());
		}
	}
	if (m_keyTypeSelectionParamModelCompPtr.IsValid()){
		Q_ASSERT(m_keyTypeSelectionBridgePtr.IsValid());
		if (m_keyTypeSelectionParamModelCompPtr->IsAttached(m_keyTypeSelectionBridgePtr.GetPtr())){
			m_keyTypeSelectionParamModelCompPtr->DetachObserver(m_keyTypeSelectionBridgePtr.GetPtr());
		}
	}
	if (m_passPhraseModelCompPtr.IsValid()){
		Q_ASSERT(m_passPhraseBridgePtr.IsValid());
		if (m_passPhraseModelCompPtr->IsAttached(m_passPhraseBridgePtr.GetPtr())){
			m_passPhraseModelCompPtr->DetachObserver(m_passPhraseBridgePtr.GetPtr());
		}
	}
	if (m_encodedDataModelCompPtr.IsValid()){
		Q_ASSERT(m_encodedDataBridgePtr.IsValid());
		if (m_encodedDataModelCompPtr->IsAttached(m_encodedDataBridgePtr.GetPtr())){
			m_encodedDataModelCompPtr->DetachObserver(m_encodedDataBridgePtr.GetPtr());
		}
	}
	if (m_filePathModelCompPtr.IsValid()){
		Q_ASSERT(m_filePathBridgePtr.IsValid());
		if (m_filePathModelCompPtr->IsAttached(m_filePathBridgePtr.GetPtr())){
			m_filePathModelCompPtr->DetachObserver(m_filePathBridgePtr.GetPtr());
		}
	}
	if (m_fileEnableModelCompPtr.IsValid()){
		Q_ASSERT(m_fileEnablerBridgePtr.IsValid());
		if (m_fileEnableModelCompPtr->IsAttached(m_fileEnablerBridgePtr.GetPtr())){
			m_fileEnableModelCompPtr->DetachObserver(m_fileEnablerBridgePtr.GetPtr());
		}
	}

	BaseClass::OnComponentDestroyed();
}



} // namespace imtcom


