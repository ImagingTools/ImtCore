#pragma once


// Qt includes
#include <QtWidgets/QTableWidget>

// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtauth/ILogin.h>
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
		I_ASSIGN(m_loginCompPtr, "Login", "Login component to get current user ID", false, "Login");
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
	QString FormatDateTime(const QDateTime& dateTime) const;
	QString FormatScopes(const QByteArrayList& scopes) const;

private Q_SLOTS:
	void OnTokenSelectionChanged();
	void OnCreateTokenClicked();
	void OnRevokeTokenClicked();
	void OnDeleteTokenClicked();
	void OnRefreshClicked();

private:
	I_REF(imtauth::ILogin, m_loginCompPtr);
	I_ATTR(QByteArray, m_userIdAttrPtr);

	QByteArray m_currentUserId;
};


} // namespace imtauthgui


