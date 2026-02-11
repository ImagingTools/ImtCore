// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>


namespace imtbase
{


class CQtResourceLoaderComp: public ilog::CLoggerComponentBase
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


