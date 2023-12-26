#include <imtbase/CUrlParamComp.h>


namespace imtbase
{


// public methods

// reimplemented (IUrlParam)

bool CUrlParamComp::SetUrl(const QUrl& url)
{
	if (!*m_isReadOnlyAttrPtr){
		return BaseClass2::SetUrl(url);
	}

	return false;
}


bool CUrlParamComp::IsReadOnly() const
{
	return *m_isReadOnlyAttrPtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CUrlParamComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QUrl url;

	QString scheme = *m_defaultSchemeAttrPtr;
	url.setScheme(scheme);

	QString host = *m_defaultHostAttrPtr;
	url.setHost(host);

	int port = *m_defaultPortAttrPtr;
	url.setPort(port);

	QString username = *m_defaultUsernameAttrPtr;
	if (!username.isEmpty()){
		url.setUserName(username);
	}

	QString password = *m_defaultPasswordAttrPtr;
	if (!password.isEmpty()){
		url.setPassword(password);
	}

	QString path = *m_defaultPathAttrPtr;
	if (!path.isEmpty()){
		url.setPath(path);
	}

	QString query = *m_defaultQueryAttrPtr;
	if (!query.isEmpty()){
		url.setQuery(query);
	}

	QString fragment = *m_defaultFragmentAttrPtr;
	if (!fragment.isEmpty()){
		url.setFragment(fragment);
	}

	BaseClass2::SetUrl(url);
}


} // namespace imtbase


