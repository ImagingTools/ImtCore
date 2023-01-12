#include <imtbase/CObjectRepresentationControllerCompBase.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

QByteArray CObjectRepresentationControllerCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


} // namespace imtbase


