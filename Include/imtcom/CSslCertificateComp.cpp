#include <imtcom/CSslCertificateComp.h>


// Acf includes
#include <iprm/IOptionsList.h>


namespace imtcom
{


ISslCertificate::EncodingFormat CSslCertificateComp::GetEncodingFormat() const
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
	const bool isOptionValid = ISslCertificate::FromString(optionId, sslEncodingFormat);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL EncodingFormat. Set correct EncodingFormatSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return sslEncodingFormat;
}


QByteArray CSslCertificateComp::GetEncodedData() const
{
	if (m_encodedDataCompPtr.IsValid()){
		return m_encodedDataCompPtr->GetId();
	}

	return QByteArray();
}


QString CSslCertificateComp::GetFilePath() const
{
	if (m_filePathCompPtr.IsValid()){
		return m_filePathCompPtr->GetPath();
	}

	return QString();
}


bool CSslCertificateComp::IsFile() const
{
	if (!m_fileEnableCompPtr.IsValid()){
		SendCriticalMessage(0, "IsFile param is not set");
		I_CRITICAL();

		return false;
	}

	return m_fileEnableCompPtr->IsEnabled();
}



// reimplemented (iser::ISerializable)

bool CSslCertificateComp::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag encodingFormatTag("EncodingFormatSelection", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(encodingFormatTag);
	if (m_encodingFormatSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_encodingFormatSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(encodingFormatTag);	

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

void CSslCertificateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_encodingFormatSelectionParamModelCompPtr.IsValid()){
		m_encodingFormatSelectionBridgePtr.SetPtr(new imod::CModelUpdateBridge(this));
		m_encodingFormatSelectionParamModelCompPtr->AttachObserver(m_encodingFormatSelectionBridgePtr.GetPtr());
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


void CSslCertificateComp::OnComponentDestroyed()
{
	if (m_encodingFormatSelectionParamModelCompPtr.IsValid()){
		Q_ASSERT(m_encodingFormatSelectionBridgePtr.IsValid());
		if (m_encodingFormatSelectionParamModelCompPtr->IsAttached(m_encodingFormatSelectionBridgePtr.GetPtr())){
			m_encodingFormatSelectionParamModelCompPtr->DetachObserver(m_encodingFormatSelectionBridgePtr.GetPtr());
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


