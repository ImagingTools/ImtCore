#include <imtauthgui/CPersonalAccessTokenManagerGuiComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtauth/IPersonalAccessToken.h>


namespace imtauthgui
{


// public methods

CPersonalAccessTokenManagerGuiComp::CPersonalAccessTokenManagerGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CPersonalAccessTokenManagerGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	RefreshTokenList();
}


void CPersonalAccessTokenManagerGuiComp::OnGuiModelAttached()
{
	RefreshTokenList();
}


void CPersonalAccessTokenManagerGuiComp::OnGuiModelDetached()
{
	// Clear UI when model is detached
	TokenListWidget->clear();
	TokenDetailsText->clear();
}


void CPersonalAccessTokenManagerGuiComp::UpdateModel() const
{
	// This is a view/manager component, most operations are command-based
	// No direct model updates from GUI state
}


// reimplemented (iqtgui::CGuiComponentBase)

void CPersonalAccessTokenManagerGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
	
	// Connect signals
	connect(TokenListWidget, &QListWidget::itemSelectionChanged, 
			this, &CPersonalAccessTokenManagerGuiComp::OnTokenSelectionChanged);
	connect(CreateTokenButton, &QPushButton::clicked,
			this, &CPersonalAccessTokenManagerGuiComp::OnCreateTokenClicked);
	connect(RevokeTokenButton, &QPushButton::clicked,
			this, &CPersonalAccessTokenManagerGuiComp::OnRevokeTokenClicked);
	connect(DeleteTokenButton, &QPushButton::clicked,
			this, &CPersonalAccessTokenManagerGuiComp::OnDeleteTokenClicked);
	connect(RefreshButton, &QPushButton::clicked,
			this, &CPersonalAccessTokenManagerGuiComp::OnRefreshClicked);
	
	// Initialize button states
	RevokeTokenButton->setEnabled(false);
	DeleteTokenButton->setEnabled(false);
	
	// Load user ID from attribute or login component
	if (m_userIdAttrPtr.IsValid() && !m_userIdAttrPtr->isEmpty()) {
		m_currentUserId = *m_userIdAttrPtr;
	} else if (m_loginCompPtr.IsValid()) {
		m_currentUserId = m_loginCompPtr->GetLoggedUserId();
	}
}


void CPersonalAccessTokenManagerGuiComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// reimplemented (icomp::CComponentBase)

void CPersonalAccessTokenManagerGuiComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CPersonalAccessTokenManagerGuiComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

void CPersonalAccessTokenManagerGuiComp::RefreshTokenList()
{
	const imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr == nullptr) {
		return;
	}
	
	// Update user ID from attribute or login component
	if (m_userIdAttrPtr.IsValid() && !m_userIdAttrPtr->isEmpty()) {
		m_currentUserId = *m_userIdAttrPtr;
	} else if (m_loginCompPtr.IsValid()) {
		m_currentUserId = m_loginCompPtr->GetLoggedUserId();
	}
	
	if (m_currentUserId.isEmpty()) {
		TokenListWidget->clear();
		TokenDetailsText->clear();
		UserIdLabel->setText("User: <not logged in>");
		return;
	}
	
	// Get list of token IDs for the user
	QByteArrayList tokenIds = managerPtr->GetTokenIds(m_currentUserId);
	
	// Clear and repopulate the list
	TokenListWidget->clear();
	
	for (const QByteArray& tokenId : tokenIds) {
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = managerPtr->GetToken(tokenId);
		if (tokenPtr.IsValid()) {
			QString displayName = tokenPtr->GetName();
			if (displayName.isEmpty()) {
				displayName = QString::fromUtf8(tokenId);
			}
			
			// Add indicator if token is revoked
			if (tokenPtr->IsRevoked()) {
				displayName += " [REVOKED]";
			}
			
			QListWidgetItem* item = new QListWidgetItem(displayName);
			item->setData(Qt::UserRole, tokenId);
			TokenListWidget->addItem(item);
		}
	}
	
	// Update user ID label
	UserIdLabel->setText(QString("User: %1").arg(QString::fromUtf8(m_currentUserId)));
}


void CPersonalAccessTokenManagerGuiComp::UpdateTokenDetails(const QByteArray& tokenId)
{
	const imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr == nullptr) {
		TokenDetailsText->clear();
		return;
	}
	
	imtauth::IPersonalAccessTokenSharedPtr tokenPtr = managerPtr->GetToken(tokenId);
	if (!tokenPtr.IsValid()) {
		TokenDetailsText->clear();
		return;
	}
	
	// Format token details
	QString details;
	details += QString("<b>Token ID:</b> %1<br>").arg(QString::fromUtf8(tokenPtr->GetId()));
	details += QString("<b>Name:</b> %1<br>").arg(tokenPtr->GetName());
	details += QString("<b>Description:</b> %1<br>").arg(tokenPtr->GetDescription());
	details += QString("<b>Scopes:</b> %1<br>").arg(FormatScopes(tokenPtr->GetScopes()));
	details += QString("<b>Created At:</b> %1<br>").arg(FormatDateTime(tokenPtr->GetCreatedAt()));
	details += QString("<b>Expires At:</b> %1<br>").arg(FormatDateTime(tokenPtr->GetExpiresAt()));
	details += QString("<b>Last Used At:</b> %1<br>").arg(FormatDateTime(tokenPtr->GetLastUsedAt()));
	details += QString("<b>Revoked:</b> %1<br>").arg(tokenPtr->IsRevoked() ? "Yes" : "No");
	
	if (tokenPtr->IsRevoked()) {
		details += QString("<b>Revoked At:</b> %1<br>").arg(FormatDateTime(tokenPtr->GetRevokedAt()));
	}
	
	TokenDetailsText->setHtml(details);
}


QString CPersonalAccessTokenManagerGuiComp::FormatDateTime(const QDateTime& dateTime) const
{
	if (!dateTime.isValid()) {
		return "N/A";
	}
	return dateTime.toString(Qt::ISODate);
}


QString CPersonalAccessTokenManagerGuiComp::FormatScopes(const QByteArrayList& scopes) const
{
	if (scopes.isEmpty()) {
		return "None";
	}
	
	QStringList scopeStrings;
	for (const QByteArray& scope : scopes) {
		scopeStrings.append(QString::fromUtf8(scope));
	}
	
	return scopeStrings.join(", ");
}


// private slots

void CPersonalAccessTokenManagerGuiComp::OnTokenSelectionChanged()
{
	QList<QListWidgetItem*> selectedItems = TokenListWidget->selectedItems();
	
	if (selectedItems.isEmpty()) {
		TokenDetailsText->clear();
		RevokeTokenButton->setEnabled(false);
		DeleteTokenButton->setEnabled(false);
		return;
	}
	
	QByteArray tokenId = selectedItems.first()->data(Qt::UserRole).toByteArray();
	UpdateTokenDetails(tokenId);
	
	// Enable/disable buttons based on token state
	const imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr != nullptr) {
		imtauth::IPersonalAccessTokenSharedPtr tokenPtr = managerPtr->GetToken(tokenId);
		if (tokenPtr.IsValid()) {
			bool isRevoked = tokenPtr->IsRevoked();
			RevokeTokenButton->setEnabled(!isRevoked);
			DeleteTokenButton->setEnabled(true);
		}
	}
}


void CPersonalAccessTokenManagerGuiComp::OnCreateTokenClicked()
{
	imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr == nullptr || m_currentUserId.isEmpty()) {
		QMessageBox::warning(GetWidget(), "Error", "No token manager or user ID available.");
		return;
	}
	
	// Get token name from user
	bool ok;
	QString name = QInputDialog::getText(GetWidget(), "Create Token",
		"Token name:", QLineEdit::Normal, "", &ok);
	
	if (!ok || name.isEmpty()) {
		return;
	}
	
	// Get token description
	QString description = QInputDialog::getText(GetWidget(), "Create Token",
		"Token description:", QLineEdit::Normal, "", &ok);
	
	if (!ok) {
		return;
	}
	
	// For now, create token with no scopes and no expiration
	// In a real implementation, you would have a dialog for selecting scopes and expiration
	QByteArrayList scopes;
	QDateTime expiresAt; // Invalid QDateTime means no expiration
	
	imtauth::IPersonalAccessTokenManager::TokenCreationResult result =
		managerPtr->CreateToken(m_currentUserId, name, description, scopes, expiresAt);
	
	if (result.success) {
		// Show the raw token to the user (this is the only time they can see it)
		QString message = QString(
			"Token created successfully!\n\n"
			"Token ID: %1\n\n"
			"RAW TOKEN (save this, you won't be able to see it again):\n%2")
			.arg(QString::fromUtf8(result.tokenId))
			.arg(QString::fromUtf8(result.rawToken));
		
		QMessageBox::information(GetWidget(), "Token Created", message);
		
		// Refresh the token list
		RefreshTokenList();
	} else {
		QMessageBox::warning(GetWidget(), "Error", "Failed to create token.");
	}
}


void CPersonalAccessTokenManagerGuiComp::OnRevokeTokenClicked()
{
	QList<QListWidgetItem*> selectedItems = TokenListWidget->selectedItems();
	if (selectedItems.isEmpty()) {
		return;
	}
	
	QByteArray tokenId = selectedItems.first()->data(Qt::UserRole).toByteArray();
	
	int ret = QMessageBox::question(GetWidget(), "Revoke Token",
		"Are you sure you want to revoke this token? This cannot be undone.",
		QMessageBox::Yes | QMessageBox::No);
	
	if (ret != QMessageBox::Yes) {
		return;
	}
	
	imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr != nullptr) {
		if (managerPtr->RevokeToken(tokenId)) {
			QMessageBox::information(GetWidget(), "Success", "Token revoked successfully.");
			RefreshTokenList();
		} else {
			QMessageBox::warning(GetWidget(), "Error", "Failed to revoke token.");
		}
	}
}


void CPersonalAccessTokenManagerGuiComp::OnDeleteTokenClicked()
{
	QList<QListWidgetItem*> selectedItems = TokenListWidget->selectedItems();
	if (selectedItems.isEmpty()) {
		return;
	}
	
	QByteArray tokenId = selectedItems.first()->data(Qt::UserRole).toByteArray();
	
	int ret = QMessageBox::question(GetWidget(), "Delete Token",
		"Are you sure you want to permanently delete this token? This cannot be undone.",
		QMessageBox::Yes | QMessageBox::No);
	
	if (ret != QMessageBox::Yes) {
		return;
	}
	
	imtauth::IPersonalAccessTokenManager* managerPtr = GetObservedObject();
	if (managerPtr != nullptr) {
		if (managerPtr->DeleteToken(tokenId)) {
			QMessageBox::information(GetWidget(), "Success", "Token deleted successfully.");
			RefreshTokenList();
		} else {
			QMessageBox::warning(GetWidget(), "Error", "Failed to delete token.");
		}
	}
}


void CPersonalAccessTokenManagerGuiComp::OnRefreshClicked()
{
	RefreshTokenList();
}


} // namespace imtauthgui


