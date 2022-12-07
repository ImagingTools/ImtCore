#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>


namespace imtbase
{


class CQtResourceLoaderComp: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQtResourceLoaderComp);
		I_ASSIGN(m_qrcPathAttrPtr, "PathToFile", "Path to file for load", true, "PathToFile");
		I_ASSIGN(m_targetObjectCompPtr, "TargetCollection", "Collection for serialize readed data", true, "TargetCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_qrcPathAttrPtr);
	I_REF(iser::ISerializable, m_targetObjectCompPtr);
};


} // namespace imtbase



