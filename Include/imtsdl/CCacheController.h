#pragma once


// Qt includes
#include <QtCore/QReadWriteLock>

// ACF includes
#include <ilog/CLoggerBase.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ICacheController.h>


namespace imtsdl
{


class CCacheController:
			private imod::CSingleModelObserverBase,
			public ilog::CLoggerBase,
			virtual public ICacheController
{

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef ilog::CLoggerBase BaseClass2;

	CCacheController();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ICacheController)

	/// \note never returns nullptr
	virtual CCache* GetCache() override;

	/// \note never returns nullptr
	virtual const CCache* GetCache() const override;
	virtual bool SetOperationalDevice(const std::shared_ptr<QIODevice>& devicePtr) override;
	virtual bool Save() const override;
	virtual bool LoadFromData(QIODevice* dataDevicePtr = nullptr) override;
	virtual void EnableAutoSave(bool enableAutosave = true) override;

protected:
	std::unique_ptr<CCache> m_cachePtr;
	bool m_isAutosaveEnabled;
	std::shared_ptr<QIODevice> m_operationalDevicePtr;
	mutable QReadWriteLock m_cacheGuard;
	mutable QReadWriteLock m_deviceGuard;
};


} // namespace imtsdl


