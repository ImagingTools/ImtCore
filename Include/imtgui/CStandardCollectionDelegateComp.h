#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileMetaInfoProvider.h>
#include <idoc/IDocumentManager.h>
#include <iqtgui/IIconProvider.h>

// ImtCore includes
#include <imtgui/CObjectCollectionViewDelegate.h>


namespace imtgui
{


/**
	\ingroup Collection
*/
class CStandardCollectionDelegateComp:
			public CObjectCollectionViewDelegate,
			public icomp::CComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CObjectCollectionViewDelegate BaseClass2;

	I_BEGIN_COMPONENT(CStandardCollectionDelegateComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(imtgui::ICollectionViewDelegate);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID supported by this delegate", true, "ObjectTypeId");
		I_ASSIGN(m_statusIconsProviderCompPtr, "StatusIcons", "Icons for delegate visual status", false, "StatusIcons");
		I_ASSIGN(m_objectGuiCompPtr, "ObjectEditor", "Editor of the collection objects", false, "ObjectEditor");
		I_ASSIGN_TO(m_objectObserverCompPtr, m_objectGuiCompPtr, false);
	I_END_COMPONENT;

	CStandardCollectionDelegateComp();

	// reimplemented (ICollectionViewDelegate)
	virtual QByteArray GetSupportedTypeId() const override;
	virtual bool InitializeDelegate(imtbase::IObjectCollection* collectionPtr, iqtgui::IGuiObject* parentGuiPtr) override;
	virtual bool OpenDocumentEditor(const QByteArray& objectId, const QByteArray& viewTypeId = QByteArray()) const override;

protected:
	// reimplemented (CObjectCollectionViewDelegate)
	virtual void SetupCommands() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected Q_SLOTS:
	virtual void OnEdit();

private:
	void InitializeVisualStatus();

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
	I_REF(iqtgui::IIconProvider, m_statusIconsProviderCompPtr);

	I_REF(iqtgui::IGuiObject, m_objectGuiCompPtr);
	I_REF(imod::IObserver, m_objectObserverCompPtr);

private:
	ifile::IFileMetaInfoProvider::MetaInfoPtr m_selectedMetaInfoPtr;

	iqtgui::CHierarchicalCommand m_editContentsCommand;
};


} // namespace imtgui


