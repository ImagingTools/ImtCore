#include <imtbase/CUrlParamComp.h>


namespace imtbase
{


// public methods

// reimplemented (IUrlParam)

void CUrlParamComp::SetUrl(const QUrl& url)
{
	if (!*m_isReadOnlyAttrPtr){
		BaseClass2::SetUrl(url);
	}
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


