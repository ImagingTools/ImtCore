#include <imtbase/CDelegatedUrlParamComp.h>


namespace imtbase
{


// public methods

// reimplemented (IUrlParam)

const QUrl& CDelegatedUrlParamComp::GetUrl() const
{
	if (m_urlParamCompPtr.IsValid()){
		return m_urlParamCompPtr->GetUrl();
	}

	return QUrl();
}


bool CDelegatedUrlParamComp::SetUrl(const QUrl& url)
{
	if (m_urlParamCompPtr.IsValid()){
		return m_urlParamCompPtr->SetUrl(url);
	}

	return false;
}


bool CDelegatedUrlParamComp::IsReadOnly() const
{
	if (m_urlParamCompPtr.IsValid()){
		return m_urlParamCompPtr->IsReadOnly();
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CDelegatedUrlParamComp::Serialize(iser::IArchive& archive)
{
	if (m_urlParamCompPtr.IsValid()){
		return m_urlParamCompPtr->Serialize(archive);
	}

	return false;
}


} // namespace imtbase


