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
#include <imtcom/ISslKey.h>


namespace imtcom
{


class CSslKeyComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslKey
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSslKeyComp)
		I_REGISTER_INTERFACE(ISslKey)

		I_ASSIGN(m_keyAlgorithmSelectionParamCompPtr, "KeyAlgorithmSelection", "Key algorithm", true, "KeyAlgorithmSelection");
		I_ASSIGN_TO(m_keyAlgorithmSelectionParamModelCompPtr, m_keyAlgorithmSelectionParamCompPtr, true);
		I_ASSIGN(m_encodingFormatSelectionParamCompPtr, "EncodingFormatSelection", "Encoding format", true, "EncodingFormatSelection");
		I_ASSIGN_TO(m_encodingFormatSelectionParamModelCompPtr, m_encodingFormatSelectionParamCompPtr, true);
		I_ASSIGN(m_keyTypeSelectionParamCompPtr, "KeyTypeSelection", "Key type", true, "KeyTypeSelection");
		I_ASSIGN_TO(m_keyTypeSelectionParamModelCompPtr, m_keyTypeSelectionParamCompPtr, true);
		I_ASSIGN(m_passPhraseCompPtr, "PassPhrase", "If the key is encrypted the PassPhrase is used to decrypt it.", false, "PassPhrase");
		I_ASSIGN_TO(m_passPhraseModelCompPtr, m_passPhraseCompPtr, true);
		I_ASSIGN(m_encodedDataCompPtr, "EncodedData", "SSL Key will be created by decoding the string in the byte array", false, "EncodedData")
		I_ASSIGN_TO(m_encodedDataModelCompPtr, m_encodedDataCompPtr, true)
		I_ASSIGN(m_filePathCompPtr, "FilePath", "SSL Key will be created by reading and decoding data from the file.", false, "FilePath")
		I_ASSIGN_TO(m_filePathModelCompPtr, m_filePathCompPtr, true)
		I_ASSIGN(m_fileEnableCompPtr, "IsFile", "Defines how the SSL Key will be constructed. (from file or data)", true, "IsFile")
		I_ASSIGN_TO(m_fileEnableModelCompPtr, m_fileEnableCompPtr, true)
	I_END_COMPONENT

	// reimplemented (ISslKey)
	virtual KeyAlgorithm GetKeyAlgorithm() const override;
	virtual EncodingFormat GetEncodingFormat() const override;
	virtual KeyType GetKeyType() const override;
	virtual QByteArray GetPassPhrase() const override;
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
	I_REF(iprm::ISelectionParam, m_keyAlgorithmSelectionParamCompPtr);
	I_REF(imod::IModel, m_keyAlgorithmSelectionParamModelCompPtr);
	I_REF(iprm::ISelectionParam, m_encodingFormatSelectionParamCompPtr);
	I_REF(imod::IModel, m_encodingFormatSelectionParamModelCompPtr);
	I_REF(iprm::ISelectionParam, m_keyTypeSelectionParamCompPtr);
	I_REF(imod::IModel, m_keyTypeSelectionParamModelCompPtr);
	I_REF(iprm::IIdParam, m_passPhraseCompPtr);
	I_REF(imod::IModel, m_passPhraseModelCompPtr);
	I_REF(iprm::IIdParam, m_encodedDataCompPtr);
	I_REF(imod::IModel, m_encodedDataModelCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
	I_REF(imod::IModel, m_filePathModelCompPtr);
	I_REF(iprm::IEnableableParam, m_fileEnableCompPtr);
	I_REF(imod::IModel, m_fileEnableModelCompPtr);


	istd::TDelPtr<imod::CModelUpdateBridge> m_keyAlgorithmSelectionBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_encodingFormatSelectionBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_keyTypeSelectionBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_passPhraseBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_encodedDataBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_filePathBridgePtr;
	istd::TDelPtr<imod::CModelUpdateBridge> m_fileEnablerBridgePtr;

};


} // namespace imtcom


