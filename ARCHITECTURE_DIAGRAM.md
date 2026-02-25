# CPersonalAccessTokenManagerGuiComp - Architecture Diagram

## Component Structure

```
┌─────────────────────────────────────────────────────────────────────────┐
│                 CPersonalAccessTokenManagerGuiComp                      │
│                                                                         │
│  Inherits: TDesignerGuiObserverCompBase<Ui, IPersonalAccessTokenManager>│
└─────────────────────────────────────────────────────────────────────────┘
                              │
                              │ observes
                              ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                    IPersonalAccessTokenManager                          │
│                                                                         │
│  • CreateToken(userId, name, desc, scopes, expiresAt)                  │
│  • ValidateToken(rawToken) → bool                                      │
│  • GetTokenIds(userId) → QByteArrayList                                │
│  • GetToken(tokenId) → IPersonalAccessTokenSharedPtr                   │
│  • RevokeToken(tokenId) → bool                                         │
│  • DeleteToken(tokenId) → bool                                         │
└─────────────────────────────────────────────────────────────────────────┘
                              │
                              │ uses
                              ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                      IObjectCollection                                  │
│                  (PersonalAccessTokenCollection)                        │
│                                                                         │
│  • IChangeable interface for change notifications                      │
│  • Stores IPersonalAccessToken objects                                 │
└─────────────────────────────────────────────────────────────────────────┘
                              │
                              │ observed by
                              ▼
┌─────────────────────────────────────────────────────────────────────────┐
│         TModelUpdateBinder<IObjectCollection, GuiComp>                 │
│                                                                         │
│  • m_tokenCollectionObserver                                           │
│  • RegisterObject() in OnGuiModelAttached()                            │
│  • UnregisterObject() in OnGuiModelDetached()                          │
│  • Callback: OnTokenCollectionUpdated()                                │
└─────────────────────────────────────────────────────────────────────────┘
```

## UI Layout Structure

```
┌────────────────────────────────────────────────────────────────────────┐
│  CPersonalAccessTokenManagerGuiComp (QWidget)                          │
│  ┌──────────────────────────────────────────────────────────────────┐  │
│  │  UserIdLabel: "User: <userId>"                                    │  │
│  └──────────────────────────────────────────────────────────────────┘  │
│                                                                        │
│  ┌────────────────────────────────────────────────────────────────┐   │
│  │  MainSplitter (QSplitter - Horizontal)                          │   │
│  │                                                                  │   │
│  │  ┌────────────────────┐  ┌─────────────────────────────────┐   │   │
│  │  │ LeftPanel          │  │ RightPanel                      │   │   │
│  │  │                    │  │                                 │   │   │
│  │  │ TokenListLabel     │  │ TokenDetailsLabel               │   │   │
│  │  │ ┌────────────────┐ │  │ ┌─────────────────────────────┐ │   │   │
│  │  │ │TokenListWidget │ │  │ │ TokenDetailsText            │ │   │   │
│  │  │ │ (QListWidget)  │ │  │ │ (QTextBrowser)              │ │   │   │
│  │  │ │                │ │  │ │                             │ │   │   │
│  │  │ │ • Token 1      │ │  │ │ <b>Token ID:</b> abc123     │ │   │   │
│  │  │ │ • Token 2      │ │  │ │ <b>Name:</b> API Token      │ │   │   │
│  │  │ │ • Token 3 [R]  │ │  │ │ <b>Description:</b> ...     │ │   │   │
│  │  │ │                │ │  │ │ <b>Scopes:</b> read, write  │ │   │   │
│  │  │ │                │ │  │ │ <b>Created At:</b> ...      │ │   │   │
│  │  │ └────────────────┘ │  │ │ <b>Expires At:</b> ...      │ │   │   │
│  │  │                    │  │ │ <b>Last Used At:</b> ...    │ │   │   │
│  │  │ ┌─────────────────┐│  │ │ <b>Revoked:</b> No          │ │   │   │
│  │  │ │[Create][Revoke] ││  │ └─────────────────────────────┘ │   │   │
│  │  │ │[Delete][Refresh]││  │                                 │   │   │
│  │  │ └─────────────────┘│  │                                 │   │   │
│  │  └────────────────────┘  └─────────────────────────────────┘   │   │
│  └────────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────────┘
```

## Lifecycle Flow

```
Component Creation
       ↓
OnComponentCreated()
       ↓
GUI Creation
       ↓
OnGuiCreated()
    • Connect Qt signals to slots
    • Initialize button states
    • Load user ID attribute
       ↓
Model Attachment
       ↓
OnGuiModelAttached()
    • Register observer to token collection
    • Call RefreshTokenList()
       ↓
┌─────────────────────────┐
│   Normal Operation      │
│                         │
│  Model Changes          │
│        ↓                │
│  OnTokenCollectionUpdated()
│        ↓                │
│  RefreshTokenList()     │
│        ↓                │
│  UpdateGui()            │
│                         │
│  User Actions           │
│        ↓                │
│  Qt Slots               │
│    • OnCreateTokenClicked()
│    • OnRevokeTokenClicked()
│    • OnDeleteTokenClicked()
│    • OnRefreshClicked() │
│    • OnTokenSelectionChanged()
│        ↓                │
│  Manager Operations     │
│        ↓                │
│  RefreshTokenList()     │
└─────────────────────────┘
       ↓
Model Detachment
       ↓
OnGuiModelDetached()
    • Unregister observer
       ↓
GUI Destruction
       ↓
OnGuiDestroyed()
       ↓
Component Destruction
       ↓
OnComponentDestroyed()
    • Final cleanup
    • Unregister observer (safety)
```

## Data Flow - Token List Refresh

```
User Action / Collection Change
            ↓
     RefreshTokenList()
            ↓
┌───────────────────────────────────┐
│ 1. Get manager from observed model│
│ 2. Get userId from attribute      │
│ 3. Call GetTokenIds(userId)       │
└───────────────────────────────────┘
            ↓
         For each tokenId
            ↓
┌───────────────────────────────────┐
│ 1. Call GetToken(tokenId)         │
│ 2. Extract token name             │
│ 3. Add "[REVOKED]" if revoked     │
│ 4. Create QListWidgetItem         │
│ 5. Store tokenId in UserRole      │
│ 6. Add to TokenListWidget         │
└───────────────────────────────────┘
            ↓
     Update UI state
```

## Data Flow - Token Creation

```
User clicks "Create" button
            ↓
   OnCreateTokenClicked()
            ↓
┌───────────────────────────────────┐
│ QInputDialog::getText()           │
│   • Get token name                │
│   • Get token description         │
└───────────────────────────────────┘
            ↓
┌───────────────────────────────────┐
│ manager->CreateToken()            │
│   • userId, name, description     │
│   • scopes (empty for now)        │
│   • expiresAt (invalid = no exp)  │
└───────────────────────────────────┘
            ↓
     TokenCreationResult
            ↓
┌───────────────────────────────────┐
│ Display raw token to user         │
│ ⚠️ Only shown once!                │
│                                   │
│ QMessageBox::information()        │
│   • Token ID: abc123              │
│   • RAW TOKEN: imt_pat_abc123...  │
└───────────────────────────────────┘
            ↓
     RefreshTokenList()
    (auto-triggered by observer)
```

## Signal/Slot Connections

```
TokenListWidget::itemSelectionChanged
    ↓
OnTokenSelectionChanged()
    • Get selected tokenId
    • Call UpdateTokenDetails(tokenId)
    • Enable/disable Revoke/Delete buttons

CreateTokenButton::clicked
    ↓
OnCreateTokenClicked()
    • Show input dialogs
    • Create token via manager
    • Show raw token to user

RevokeTokenButton::clicked
    ↓
OnRevokeTokenClicked()
    • Show confirmation dialog
    • Call manager->RevokeToken(tokenId)
    • Refresh list

DeleteTokenButton::clicked
    ↓
OnDeleteTokenClicked()
    • Show confirmation dialog
    • Call manager->DeleteToken(tokenId)
    • Refresh list

RefreshButton::clicked
    ↓
OnRefreshClicked()
    • Call RefreshTokenList()
```

## Component Dependencies

```
CPersonalAccessTokenManagerGuiComp
    ├── ACF Framework
    │   ├── iqtgui::TDesignerGuiObserverCompBase
    │   ├── icomp::CComponentBase
    │   ├── imod::IObserver
    │   └── istd::IChangeable
    │
    ├── ImtCore
    │   ├── imtauth::IPersonalAccessTokenManager
    │   ├── imtauth::IPersonalAccessToken
    │   ├── imtbase::IObjectCollection
    │   └── imtbase::TModelUpdateBinder
    │
    └── Qt Framework
        ├── QtWidgets
        │   ├── QWidget
        │   ├── QListWidget
        │   ├── QTextBrowser
        │   ├── QPushButton
        │   ├── QLabel
        │   ├── QSplitter
        │   ├── QMessageBox
        │   └── QInputDialog
        │
        └── QtCore
            ├── QObject (Q_OBJECT macro)
            ├── QString
            ├── QByteArray
            └── QDateTime
```

## Observer Pattern Details

```
┌─────────────────────────────────────────────────────────────┐
│              TModelUpdateBinder Pattern                     │
└─────────────────────────────────────────────────────────────┘

Constructor:
    TModelUpdateBinder<IObjectCollection, GuiComp>(this)
        • Store reference to parent GuiComp
        • Initialize to unregistered state

RegisterObject():
    m_tokenCollectionObserver.RegisterObject(
        collectionPtr,
        &GuiComp::OnTokenCollectionUpdated)
        • Attach to IChangeable interface
        • Store callback method pointer
        • Enable change notifications

IObjectCollection changes:
    • Token added/removed/modified
    • Triggers IChangeable::NotifyChanged()
        ↓
    TModelUpdateBinder receives notification
        ↓
    Calls: GuiComp::OnTokenCollectionUpdated(changeSet, collectionPtr)
        ↓
    GuiComp::RefreshTokenList()
        ↓
    UI updates automatically

UnregisterObject():
    m_tokenCollectionObserver.UnregisterObject()
        • Detach from IChangeable interface
        • Clear callback
        • Disable change notifications
```

## Security Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Security Boundaries                       │
└─────────────────────────────────────────────────────────────┘

Token Storage (Secure):
    • Only SHA-256 hash stored
    • Raw token never persisted
    • Hash comparison uses constant-time algorithm

Token Creation:
    User Input → CreateToken() → Raw Token Generated
                                        ↓
                        ┌───────────────┴───────────────┐
                        ↓                               ↓
                Raw Token (once)              Token Hash (stored)
                        ↓
                QMessageBox (show)
                        ↓
                User saves manually
                        ↓
                ⚠️ Never shown again

Token Display (UI):
    • Token ID: ✅ Shown
    • Token Name: ✅ Shown
    • Token Hash: ❌ Never shown
    • Raw Token: ❌ Never shown (except at creation)
    • Scopes: ✅ Shown
    • Timestamps: ✅ Shown
    • Revocation status: ✅ Shown

Destructive Operations:
    • Revoke: Requires QMessageBox::question confirmation
    • Delete: Requires QMessageBox::question confirmation
    • No undo available
    • Permanent changes to collection

Access Control:
    • User ID attribute controls token visibility
    • Only tokens for specified user shown
    • No cross-user access in UI
```
