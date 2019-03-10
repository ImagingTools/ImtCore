#include <imtbase/CObjectContainerComp.h>


namespace imtbase
{


// protected methods

istd::IChangeable* CObjectContainerComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = m_typeIdsAttrPtr.FindValue(typeId);
	if (factoryIndex >= 0){
		if (factoryIndex < m_objectFactoriesCompPtr.GetCount()){
			return m_objectFactoriesCompPtr.CreateInstance(factoryIndex);
		}
	}

	return BaseClass2::CreateInstance(typeId);
}


} // namespace imtbase


