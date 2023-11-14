#include <imtbase/CUrlParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// reimplemented (IUrlParam)

const QUrl& CUrlParam::GetUrl() const
{
	return m_url;
}


bool CUrlParam::SetUrl(const QUrl& url)
{
	if (m_url != url){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_url = url;
	}

	return true;
}


bool CUrlParam::IsReadOnly() const
{
	return false;
}


// reimplemented (iser::ISerializable)

bool CUrlParam::Serialize(iser::IArchive& archive)
{
	iser::CArchiveTag urlTag("Url", "Url", iser::CArchiveTag::TT_LEAF);

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);
	Q_UNUSED(notifier);

	QString urlStr = m_url.toString();

	bool retVal = archive.BeginTag(urlTag);
	retVal = retVal && archive.Process(urlStr);
	retVal = retVal && archive.EndTag(urlTag);

	if (retVal && !archive.IsStoring()){
		m_url = QUrl(urlStr);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CUrlParam::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_OBSERVE | SO_RESET;
}


bool CUrlParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const IUrlParam* sourcePtr = dynamic_cast<const IUrlParam*>(&object);
	if (sourcePtr != NULL){
		SetUrl(sourcePtr->GetUrl());

		return true;
	}

	return false;
}


bool CUrlParam::IsEqual(const IChangeable& object) const
{
	const IUrlParam* sourcePtr = dynamic_cast<const IUrlParam*>(&object);
	if (sourcePtr != NULL){
		return m_url == sourcePtr->GetUrl();
	}

	return false;
}


istd::IChangeable* CUrlParam::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CUrlParam> clonePtr(new CUrlParam);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CUrlParam::ResetData(CompatibilityMode /*mode*/)
{
	SetUrl(QUrl());

	return true;
}


} // namespace imtbase


