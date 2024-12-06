#pragma once


// Qt includes
#include <QtCore/QReadWriteLock>

// ACF includes
#include <istd/ILogger.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ICacheController.h>


namespace imtsdl
{


class CCacheController:
			protected imod::CSingleModelObserverBase,
			protected istd::ILogger,
			virtual public ICacheController
{

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef istd::ILogger BaseClass2;

	CCacheController();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (ICacheController)

	virtual CCache& GetCache() override;
	virtual const CCache& GetCache() const override;
	virtual bool SetOperationalDevice(const std::shared_ptr<QIODevice>& devicePtr) override;
	virtual bool Save() const override;
	virtual LoadError LoadFromData(QIODevice* dataDevicePtr = nullptr) override;
	virtual void EnableAutoSave(bool enableAutosave = true) override;

private:
	bool SendInfoMessage(int id, const QString& message) const;
	bool SendWarningMessage(int id, const QString& message) const;
	bool SendErrorMessage(int id, const QString& message) const;
	bool SendCriticalMessage(int id, const QString& message) const;

protected:
	std::unique_ptr<CCache> m_cachePtr;
	bool m_isAutosaveEnabled;
	std::shared_ptr<QIODevice> m_operationalDevicePtr;
	mutable QReadWriteLock m_cacheGuard;
	mutable QReadWriteLock m_deviceGuard;
};


} // namespace imtsdl


