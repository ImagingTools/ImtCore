// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CDelegatedUrlParamComp.h>


// Qt includes
#include <QtCore/QUrl>


namespace imtbase
{


// public methods

// reimplemented (IUrlParam)

const QUrl& CDelegatedUrlParamComp::GetUrl() const
{
	if (m_urlParamCompPtr.IsValid()){
		return m_urlParamCompPtr->GetUrl();
	}

	static QUrl emptyUrl;

	return emptyUrl;
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


// reimplemented (ifile::IFileNameParam)

int CDelegatedUrlParamComp::GetPathType() const
{
	return ifile::IFileNameParam::PT_URL;
}


const QString& CDelegatedUrlParamComp::GetPath() const
{
	const ifile::IFileNameParam* pathPtr = dynamic_cast<const ifile::IFileNameParam*>(m_urlParamCompPtr.GetPtr());
	if (pathPtr != nullptr){
		return pathPtr->GetPath();
	}

	static QString empty;

	return empty;
}


void CDelegatedUrlParamComp::SetPath(const QString& path)
{
	ifile::IFileNameParam* pathPtr = dynamic_cast<ifile::IFileNameParam*>(m_urlParamCompPtr.GetPtr());
	if (pathPtr != nullptr){
		pathPtr->SetPath(path);
	}
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


