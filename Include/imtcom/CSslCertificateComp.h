#pragma once


// Acf includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <iprm/ITextParam.h>
#include <ifile/IFileNameParam.h>
#include <imod/IModel.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtcom/ISslCertificate.h>


namespace imtcom
{


class CSslCertificateComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslCertificate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSslCertificateComp)
		I_REGISTER_INTERFACE(ISslCertificate)
		I_ASSIGN(m_encodingFormatSelectionParamCompPtr, "EncodingFormatSelection", "Encoding format", true, "EncodingFormatSelection");
		I_ASSIGN_TO(m_encodingFormatSelectionParamModelCompPtr, m_encodingFormatSelectionParamCompPtr, true);
		I_ASSIGN(m_encodedDataCompPtr, "EncodedData", "SSL certificate will be created by decoding the string in the byte array", false, "EncodedData")
		I_ASSIGN_TO(m_encodedDataModelCompPtr, m_encodedDataCompPtr, true)
		I_ASSIGN(m_filePathCompPtr, "FilePath", "SSL certificate will be created by reading and decoding data from the file.", false, "FilePath")
		I_ASSIGN_TO(m_filePathModelCompPtr, m_filePathCompPtr, true)
		I_ASSIGN(m_fileEnableCompPtr, "IsFile", "Defines how the SSL certificate will be constructed. (from file or data)", false, "IsFile")
		I_ASSIGN_TO(m_fileEnableModelCompPtr, m_fileEnableCompPtr, true)
	I_END_COMPONENT

	// reimplemented (ISslCertificate)
	virtual EncodingFormat GetEncodingFormat() const override;
	virtual QByteArray GetEncodedData() const override;
	virtual QString GetFilePath() const override;
	virtual bool IsFile() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(iprm::ISelectionParam, m_encodingFormatSelectionParamCompPtr);
	I_REF(imod::IModel, m_encodingFormatSelectionParamModelCompPtr);
	I_REF(iprm::IIdParam, m_encodedDataCompPtr);
	I_REF(imod::IModel, m_encodedDataModelCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
	I_REF(imod::IModel, m_filePathModelCompPtr);
	I_REF(iprm::IEnableableParam, m_fileEnableCompPtr);
	I_REF(imod::IModel, m_fileEnableModelCompPtr);

	istd::TDelPtr<imod::CModelUpdateBridge> m_encodingFormatSelectionBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_encodedDataBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_filePathBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_fileEnablerBridgePtr;
};


} // namespace imtcom


