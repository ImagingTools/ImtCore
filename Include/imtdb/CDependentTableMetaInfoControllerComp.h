// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDependentMetaInfoController.h>


namespace imtdb
{


class CDependentTableMetaInfoControllerComp: public ilog::CLoggerComponentBase, protected imod::CSingleModelObserverBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	
	I_BEGIN_COMPONENT(CDependentTableMetaInfoControllerComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", false, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_dependentMetaInfoControllerCompPtr, "DependentMetaInfoController", "Dependent meta info controller", true, "DependentMetaInfoController");
		I_ASSIGN(m_metaInfoIdAttrPtr, "MetaInfoId", "Meta info ID", false, "");
		I_ASSIGN(m_isDocumentSourceAttrPtr, "IsDocumentSource", "If true, the information will be taken from the document, else from the meta information", false, true);
		I_ASSIGN(m_dependentTableNameAttrPtr, "DependentTableName", "Dependent table name", true, "");
		I_ASSIGN_MULTI_0(m_metaInfoNameAttrPtr, "MetaInfoNames", "Meta info names", false);
		I_ASSIGN_MULTI_0(m_dependentMetaInfoNameAttrPtr, "DependentMetaInfoNames", "Dependent meta info names", true);
	I_END_COMPONENT;
	
protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;
	
private:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);
	I_REF(imtdb::IDependentMetaInfoController, m_dependentMetaInfoControllerCompPtr);
	I_ATTR(QString, m_metaInfoIdAttrPtr);
	I_ATTR(QString, m_dependentTableNameAttrPtr);
	I_ATTR(bool, m_isDocumentSourceAttrPtr);
	I_MULTIATTR(QString, m_metaInfoNameAttrPtr);
	I_MULTIATTR(QString, m_dependentMetaInfoNameAttrPtr);
};


} // namespace imtdb

