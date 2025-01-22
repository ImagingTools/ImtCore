#include <imtauth/CJsonWebToken.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/TDelPtr.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

// reimplemented (imtauth::IJsonWebToken)

QJsonObject CJsonWebToken::GetHeader() const
{
	return m_header;
}


void CJsonWebToken::SetHeader(const QJsonObject& header)
{
	if (m_header != header){
		istd::CChangeNotifier changeNotifier(this);

		m_header = header;
	}
}


QJsonObject CJsonWebToken::GetPayload() const
{
	return m_payload;
}


void CJsonWebToken::SetPayload(const QJsonObject& payload)
{
	if (m_payload != payload){
		istd::CChangeNotifier changeNotifier(this);

		m_payload = payload;
	}
}


QByteArray CJsonWebToken::GetSignature() const
{
	return m_signature;
}


void CJsonWebToken::SetSignature(const QByteArray& signature)
{
	if (m_signature != signature){
		istd::CChangeNotifier changeNotifier(this);

		m_signature = signature;
	}
}


// reimplemented (iser::ISerializable)

bool CJsonWebToken::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	iser::CArchiveTag signatureTag("Signature", "Signature", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(signatureTag);
	retVal = retVal && archive.Process(m_signature);
	retVal = retVal && archive.EndTag(signatureTag);

	return true;
}


// reimplemented (iser::IChangeable)

bool CJsonWebToken::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CJsonWebToken* sourcePtr = dynamic_cast<const CJsonWebToken*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_signature = sourcePtr->m_signature;
		m_header = sourcePtr->m_header;
		m_payload = sourcePtr->m_payload;

		return true;
	}

	return false;
}


bool CJsonWebToken::IsEqual(const IChangeable& object) const
{
	bool retVal = true;
	const CJsonWebToken* sourcePtr = dynamic_cast<const CJsonWebToken*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_signature == sourcePtr->m_signature;
		retVal = retVal && m_header == sourcePtr->m_header;
		retVal = retVal && m_payload == sourcePtr->m_payload;

		return retVal;
	}

	return false;
}


istd::IChangeable* CJsonWebToken::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CJsonWebToken> clonePtr(new CJsonWebToken);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CJsonWebToken::ResetData(CompatibilityMode /*mode*/)
{
	m_signature.clear();

	return true;
}


} // namespace imtauth


