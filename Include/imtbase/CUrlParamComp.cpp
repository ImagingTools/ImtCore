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

	if (m_defaultUrlAttrPtr.IsValid()){
		BaseClass2::SetUrl(*m_defaultUrlAttrPtr);
	}
}


} // namespace imtbase


