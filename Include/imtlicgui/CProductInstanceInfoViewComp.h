#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CProductInstanceInfoViewComp.h>


namespace imtlicgui
{


/**
	License instance viewer. This component shows information about available licenses and features in the product.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewComp);
	I_END_COMPONENT;

	CProductInstanceInfoViewComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private:
	void UpdateProductName();
	void UpdateFeatureTree();

	I_REF(imtcrypt::IEncryptionKeysProvider, m_encryptionKeysProviderCompPtr);
};


} // namespace imtlicgui


