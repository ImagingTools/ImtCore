#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IEventProvider.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtlog/IStorage.h>
#include <imtlog/IMessageIdFilter.h>
//#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationControllerCompBase:
			public icomp::CComponentBase,
			virtual public istd::IChangeable,
			virtual public ilog::IMessageConsumer,
			virtual public imtlog::IMessageIdFilter
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(imtlog::IMessageIdFilter);
		I_ASSIGN(m_eventProviderCompPtr, "EventProvider", "Event provider", true, "EventProvider");
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "TimeRangeProvider", false, "TimeRangeProvider");
		I_ASSIGN(m_storageCompPtr, "Storage", "Storage", false, "");
		I_ASSIGN(m_modelCompPtr, "RepresentationModel", "Representation model", false, "");
	I_END_COMPONENT

	// reimplemented (imtlog::IMessageIdFilter)
	virtual QSet<int> GetMessageFilterIds() const override;
	virtual FilterMode GetMessageFilterMode() const override;
	virtual void SetMessageFilterIds(const QSet<int>& ids) override;
	virtual void SetMessageFilterMode(FilterMode mode) override;

private:
	void OnTimeRangeChanged();

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractPositionProvider(CRepresentationControllerCompBase& component)
	{
		return component.m_timeAxisPtr;
	}


private:
	class TimeRangeObserver: public imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>
	{
	public:
		TimeRangeObserver();

		void SetParent(CRepresentationControllerCompBase* parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CRepresentationControllerCompBase* m_parent;
	};

protected:
	I_REF(imtlog::IEventProvider, m_eventProviderCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imtlog::IStorage, m_storageCompPtr);
	I_REF(istd::IChangeable, m_modelCompPtr);
};


} // namespace imtloggui


