#pragma once


// Qt includes
#include <QtWidgets/QTableWidget>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <GeneratedFiles/imtauthgui/ui_CPersonalAccessTokenManagerGuiComp.h>


namespace imtauthgui
{


/**
	GUI component for viewing and managing personal access tokens.
	
	Provides a widget-based interface for:
	- Viewing list of registered tokens for a user
	- Displaying token properties (name, description, scopes, dates)
	- Creating new tokens
	- Revoking and deleting tokens
	
	\ingroup Authorization
*/
class CPersonalAccessTokenManagerGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CPersonalAccessTokenManagerGuiComp, imtauth::IPersonalAccessTokenManager>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CPersonalAccessTokenManagerGuiComp, imtauth::IPersonalAccessTokenManager> BaseClass;

	I_BEGIN_COMPONENT(CPersonalAccessTokenManagerGuiComp);
		I_ASSIGN(m_tokenCollectionCompPtr, "TokenCollection", "Token collection to observe", false, "PersonalAccessTokenCollection");
		I_ATTR(QByteArray, m_userIdAttrPtr);
	I_END_COMPONENT;

	CPersonalAccessTokenManagerGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void RefreshTokenList();
	void UpdateTokenDetails(const QByteArray& tokenId);
	void OnTokenCollectionUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* collectionPtr);
	QString FormatDateTime(const QDateTime& dateTime) const;
	QString FormatScopes(const QByteArrayList& scopes) const;

private Q_SLOTS:
	void OnTokenSelectionChanged();
	void OnCreateTokenClicked();
	void OnRevokeTokenClicked();
	void OnDeleteTokenClicked();
	void OnRefreshClicked();

private:
	I_REF(imtbase::IObjectCollection, m_tokenCollectionCompPtr);
	I_ATTR(QByteArray, m_userIdAttrPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CPersonalAccessTokenManagerGuiComp> m_tokenCollectionObserver;
	QByteArray m_currentUserId;
};


} // namespace imtauthgui


