// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtbase
{


/**
	Adapter of the \c iprm::IOptionsList interface to \c imtbase::ICollectionInfo
	\ingroup Collection
*/
class CCollectionInfoAdapterComp:
			public icomp::CComponentBase,
			virtual public ICollectionInfo
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionInfoAdapterComp);
		I_REGISTER_INTERFACE(ICollectionInfo);
		I_ASSIGN(m_optionsListCompPtr, "OptionsList", "List of options used as collection elements", true, "OptionsList");
		I_ASSIGN_TO(m_optionsListModelCompPtr, m_optionsListCompPtr, false);
	I_END_COMPONENT;

	CCollectionInfoAdapterComp();

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	int GetOptionIndex(const Id& elementId) const;

private:
	I_REF(iprm::IOptionsList, m_optionsListCompPtr);
	I_REF(imod::IModel, m_optionsListModelCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtbase

