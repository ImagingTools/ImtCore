# ImtCore QML — прикладные модули

Коллекция переиспользуемых QML-модулей ImtCore — визуальные компоненты, экраны и
логика представления для десктопных и веб- (через JQML) приложений на базе ACF/ImtCore.

Каждый подкаталог — это отдельный **QML-модуль** (`module <имя>` в `qmldir`),
подключаемый строкой `import <имя> 1.0`. Компоненты внутри модуля адресуются по
**имени типа** (например, `UserView { }`), а сопоставление «имя типа → файл»
задаётся в `qmldir`. Физический состав ресурса (что попадает в бинарник через
`rcc`) описан в файле `*.qrc` каждого модуля.

## Зачем это нужно

Модули дают готовый, единообразный по стилю набор строительных блоков (кнопки,
таблицы, диалоги, коллекции, документо-ориентированные рабочие области, чат,
аутентификация, лицензирование и т. д.), что позволяет собирать прикладные экраны
из типовых элементов и переиспользовать бизнес-логику представления между
продуктами.

## Соглашения и структура

Правила выбора модуля и подпапки для нового компонента (таблицы модулей
и подпапок) — в [STRUCTURE.md](STRUCTURE.md). Базовые модули (`imtcontrols`,
`imtstyle`, `imtqmlutils`, `imtgraphics2d`, `imtgui`) описаны в [STRUCTURE-base.md](STRUCTURE-base.md).
Обзор и навигация по всей коллекции — в [README.md](../README.md).

### Как читать описания компонентов

- **singleton** — тип-одиночка (`pragma Singleton`), единственный экземпляр на приложение.
- **Базовый тип** — корневой QML-тип файла; оформлен ссылкой на компонент в этом
  документе, на исходный файл в репозитории (для типов из `imtgui`/`imtcontrols`)
  или на документацию Qt.
- **Наследники** — компоненты из этих девяти модулей, расширяющие данный тип
  (как раздел «Inherited By» в документации Qt).
- В списках **Свойства / Сигналы / Функции** приведён только **публичный** контракт
  **корневого** объекта. Приватные члены (по соглашению JS/QML имена с префиксом
  `_` или `$`) намеренно опущены.


## Модули

| Модуль | Назначение | Файлов |
|---|---|---|
| [`imtauthgui`](#imtauthgui) | Аутентификация, пользователи и организации | 66 |
| [`imtdeskgui`](#imtdeskgui) | Служба поддержки (тикеты) | 8 |
| [`imtdocgui`](#imtdocgui) | Документо-ориентированные рабочие области | 30 |
| [`imtcolgui`](#imtcolgui) | Коллекции и представления коллекций | 33 |
| [`imtguigql`](#imtguigql) | GraphQL/SDL-инфраструктура и удалённые данные | 41 |
| [`imtchatgui`](#imtchatgui) | Чат и обмен сообщениями | 12 |
| [`imtlicgui`](#imtlicgui) | Лицензирование (продукты, функции, лицензии) | 17 |
| [`imtgeogui`](#imtgeogui) | Адреса и географические структуры | 3 |
| [`imt3dgui`](#imt3dgui) | 3D-сцены (обёртки над Qt Quick 3D) | 54 |

---

<a id="imtauthgui"></a>
## `imtauthgui` — Аутентификация, пользователи и организации

Экраны и логика входа/регистрации, управление пользователями, ролями, группами и правами доступа, а также полный набор страниц для администрирования организаций (tenant): участники, приглашения, роли, права, контракты, кросс-организационные гранты, профиль пользователя и смена пароля. Слой доступа к данным реализован через API-клиенты (REST/GraphQL) и кэшируемые коллекции.

**Импорт:** `import imtauthgui 1.0` · **Файлов:** 66

### Pages/ — Страницы

<a id="type-imtauthgui-AuthorizationPage"></a>
#### `AuthorizationPage`

Экранная страница авторизации — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/AuthorizationPage.qml](imtauthgui/Pages/AuthorizationPage.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `state: string`
- `mainRadius: int`
- `mainColor: string`
- `canRecoveryPassword: bool`
- `canRegisterUser: bool`
- `appName: string`
- `isAuthenticating: bool`
- `rememberMe: bool`

**Сигналы**

- `login(string login, string password)`
- `registerUser(var userData)`

**Функции**

- `onLocalizationChanged(language)`
- `passwordRecovery()`
- `setDecorators()`

<a id="type-imtauthgui-ProfileAccessPage"></a>
#### `ProfileAccessPage`

Экранная страница профиля — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/ProfileAccessPage.qml](imtauthgui/Pages/ProfileAccessPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `profileData: var` — только чтение
- `isSuperuser: bool` — только чтение

**Функции**

- `applyProfile(profile)`
- `updateGui()`

<a id="type-imtauthgui-ProfileGeneralPage"></a>
#### `ProfileGeneralPage`

Экранная страница профиля — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/ProfileGeneralPage.qml](imtauthgui/Pages/ProfileGeneralPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `profileData: var` — только чтение

**Функции**

- `applyProfile(profile)`
- `updateGui()`
- `save()`

<a id="type-imtauthgui-ProfileOrganizationsPage"></a>
#### `ProfileOrganizationsPage`

Экранная страница профиля — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/ProfileOrganizationsPage.qml](imtauthgui/Pages/ProfileOrganizationsPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `organizationsList: var`

**Функции**

- `applyOrganizations(list)`

<a id="type-imtauthgui-ProfileTokensPage"></a>
#### `ProfileTokensPage`

Экранная страница профиля — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/ProfileTokensPage.qml](imtauthgui/Pages/ProfileTokensPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `tokenList: var`

<a id="type-imtauthgui-SuperuserPasswordPage"></a>
#### `SuperuserPasswordPage`

Экранная страница суперпользователя — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/SuperuserPasswordPage.qml](imtauthgui/Pages/SuperuserPasswordPage.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `isSubmitting: bool`
- `errorMessage: string`

**Сигналы**

- `passwordSetted()`

**Функции**

- `setSuperuserPassword()`

<a id="type-imtauthgui-TenantConnectOrganizationPage"></a>
#### `TenantConnectOrganizationPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantConnectOrganizationPage.qml](imtauthgui/Pages/TenantConnectOrganizationPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `tenantData: var` — только чтение
- `stateManager: var`
- `apiClient: var`
- `sending: bool`
- `lastError: string`
- `processingRequestId: string`
- `processingRequestAction: string`
- `lastIncomingRequestId: string`
- `lastIncomingRequestStatus: string`
- `lastIncomingRequestStatusError: bool`
- `lastSentRequestId: string`
- `lastSentRequestStatus: string`
- `lastSentRequestStatusError: bool`

**Функции**

- `updateGui()`
- `refreshIncomingList()`
- `refreshConnectionsList()`

<a id="type-imtauthgui-TenantConnectionCodePage"></a>
#### `TenantConnectionCodePage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantConnectionCodePage.qml](imtauthgui/Pages/TenantConnectionCodePage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `tenantData: var` — только чтение
- `stateManager: var`
- `apiClient: var`
- `connectionCode: string`
- `allowConnectionsByCode: bool`
- `loading: bool`

**Функции**

- `updateGui()`

<a id="type-imtauthgui-TenantContractsPage"></a>
#### `TenantContractsPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantContractsPage.qml](imtauthgui/Pages/TenantContractsPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `updateGui()`

<a id="type-imtauthgui-TenantCrossOrgGrantsPage"></a>
#### `TenantCrossOrgGrantsPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantCrossOrgGrantsPage.qml](imtauthgui/Pages/TenantCrossOrgGrantsPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `removeItems(ids)`

<a id="type-imtauthgui-TenantGeneralPage"></a>
#### `TenantGeneralPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantGeneralPage.qml](imtauthgui/Pages/TenantGeneralPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `tenantData: var` — только чтение
- `stateManager: var`

**Функции**

- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-TenantGroupsPage"></a>
#### `TenantGroupsPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantGroupsPage.qml](imtauthgui/Pages/TenantGroupsPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `removeItems(ids)`

<a id="type-imtauthgui-TenantMembersPage"></a>
#### `TenantMembersPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantMembersPage.qml](imtauthgui/Pages/TenantMembersPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `removeItem(id)`
- `confirmTransferOwnership()`
- `showMemberOrgPermissionsPopup(memberData)`

<a id="type-imtauthgui-TenantMessagesPage"></a>
#### `TenantMessagesPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantMessagesPage.qml](imtauthgui/Pages/TenantMessagesPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `updateGui()`

<a id="type-imtauthgui-TenantMyConnectionsPage"></a>
#### `TenantMyConnectionsPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantMyConnectionsPage.qml](imtauthgui/Pages/TenantMyConnectionsPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `removeItems(ids)`

<a id="type-imtauthgui-TenantPermissionsPage"></a>
#### `TenantPermissionsPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantPermissionsPage.qml](imtauthgui/Pages/TenantPermissionsPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `tenantData: var` — только чтение
- `stateManager: var`
- `apiClient: var`

**Функции**

- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-TenantRolesPage"></a>
#### `TenantRolesPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantRolesPage.qml](imtauthgui/Pages/TenantRolesPage.qml)
- **Базовый тип:** [`TenantSimpleCollectionPage`](#type-imtauthgui-TenantSimpleCollectionPage)

**Функции**

- `removeItems(ids)`

<a id="type-imtauthgui-TenantSimpleCollectionPage"></a>
#### `TenantSimpleCollectionPage`

Экранная страница организации (tenant) — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/TenantSimpleCollectionPage.qml](imtauthgui/Pages/TenantSimpleCollectionPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)
- **Наследники:** [`TenantContractsPage`](#type-imtauthgui-TenantContractsPage), [`TenantCrossOrgGrantsPage`](#type-imtauthgui-TenantCrossOrgGrantsPage), [`TenantGroupsPage`](#type-imtauthgui-TenantGroupsPage), [`TenantMembersPage`](#type-imtauthgui-TenantMembersPage), [`TenantMessagesPage`](#type-imtauthgui-TenantMessagesPage), [`TenantMyConnectionsPage`](#type-imtauthgui-TenantMyConnectionsPage), [`TenantRolesPage`](#type-imtauthgui-TenantRolesPage)

**Свойства**

- `tenantData: var` — только чтение
- `stateManager: var`
- `apiClient: var`
- `entityName: string`
- `entityNamePlural: string`
- `listHeaderTitle: string`
- `descriptionText: string`
- `createButtonText: string`
- `emptyText: string`
- `filterPlaceholder: string`
- `deleteSingleTitle: string`
- `deleteMultipleTitle: string`
- `documentManager: var`
- `objectTypeId: string`
- `listModel: var`
- `delegateComponent: Component`
- `headerButtonsComponent: Component`
- `customEditorComponent: Component`
- `showCreateButton: bool`
- `managePermissionIds: var`
- `createPermissionIds: var`
- `editPermissionIds: var`
- `deletePermissionIds: var`
- `documentNameFields: var`
- `selectionManager: var`
- `dataProvider: var`
- `filterText: string` — только чтение

**Функции**

- `removeItems(ids)`
- `updateGui()`
- `updateModel()`
- `refresh()`
- `resolveDocumentName(documentId)`
- `popEditor()`
- `openCreate()`
- `openEdit(itemId, itemName, itemDescription)`

### Views/ — Представления

<a id="type-imtauthgui-AdministrationUi"></a>
#### `AdministrationUi` · _не экспортируется как тип (загружается по пути/внутренний)_

Интерфейсная оболочка администрирования: собирает экран из готовых блоков.

- **Файл:** [Views/AdministrationUi.qml](imtauthgui/Views/AdministrationUi.qml)
- **Базовый тип:** [`SingleDocumentWorkspacePageView`](#type-imtdocgui-SingleDocumentWorkspacePageView)

**Свойства**

- `productId: string`
- `webSocketUrl: string`

**Сигналы**

- `commandsModelChanged(var commandsModel)`

**Функции**

- `setLoginData(accessToken, username, permissions)`
- `login(login, password)`
- `logout()`

<a id="type-imtauthgui-AdministrationView"></a>
#### `AdministrationView`

Визуальное представление администрирования: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/AdministrationView.qml](imtauthgui/Views/AdministrationView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `multiPageView: alias`
- `productId: string`
- `documentManager: var`

**Сигналы**

- `multiPageUpdated()`

**Функции**

- `onLocalizationChanged(language)`

<a id="type-imtauthgui-ContractView"></a>
#### `ContractView`

Визуальное представление контрактов: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/ContractView.qml](imtauthgui/Views/ContractView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `tenantData: var`

**Сигналы**

- `contractCreated()`

**Функции**

- `updateGui()`
- `updateModel()`
- `submitContract()`

<a id="type-imtauthgui-CrossOrgGrantView"></a>
#### `CrossOrgGrantView`

Визуальное представление кросс-орг. грантов: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/CrossOrgGrantView.qml](imtauthgui/Views/CrossOrgGrantView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `grantData: var`
- `apiClient: var`
- `isReadOnly: bool`

**Функции**

- `expiresAtToIndex(iso)`
- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-MessageView"></a>
#### `MessageView`

Визуальное представление сообщений: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/MessageView.qml](imtauthgui/Views/MessageView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`
- `tenantData: var`
- `messageData: var`

**Функции**

- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-PasswordInput"></a>
#### `PasswordInput`

Поле ввода пароля: элемент для ручного ввода значения.

- **Файл:** [Views/PasswordInput.qml](imtauthgui/Views/PasswordInput.qml)
- **Базовый тип:** [`Column`](https://doc.qt.io/qt-6/qml-qtquick-column.html)

**Свойства**

- `oldPassword: alias`
- `newPassword: alias`
- `accepted: bool`
- `currentPasswordInputVisible: bool`

**Функции**

- `checkPassword()`

<a id="type-imtauthgui-PermissionsTableView"></a>
#### `PermissionsTableView`

Табличное представление прав доступа: данные в виде строк и столбцов.

- **Файл:** [Views/PermissionsTableView.qml](imtauthgui/Views/PermissionsTableView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `permissionsTree: var`
- `selectedIds: var`
- `readOnly: bool`
- `showControlPanel: bool`
- `treeToScrollbarSpacing: int`
- `controlPanelTopMargin: int`
- `treeTopMargin: int`
- `treeBottomMargin: int`
- `preferredHeight: real` — только чтение

**Сигналы**

- `selectionChanged()`

**Функции**

- `getCheckedIds()`
- `checkAll()`
- `uncheckAll()`
- `expandAll()`
- `collapseAll()`
- `applySelection(ids)`
- `rebuild(treeData)`
- `rebuildFromTreeItemModel(treeItemModel)`
- `rebuildFromFlatArray(groups)`

<a id="type-imtauthgui-ProfileNoticeBanner"></a>
#### `ProfileNoticeBanner`

Баннер профиля: информационная полоса-уведомление вверху экрана.

- **Файл:** [Views/ProfileNoticeBanner.qml](imtauthgui/Views/ProfileNoticeBanner.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `message: string`
- `isError: bool`

**Функции**

- `show(text, error)`
- `hide()`

<a id="type-imtauthgui-ProfileView"></a>
#### `ProfileView`

Визуальное представление профиля: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/ProfileView.qml](imtauthgui/Views/ProfileView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `apiClient: var`

<a id="type-imtauthgui-RelationshipView"></a>
#### `RelationshipView`

Визуальное представление связей организаций: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/RelationshipView.qml](imtauthgui/Views/RelationshipView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `relationshipData: var`
- `apiClient: var`
- `tenantData: var`

**Функции**

- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-RoleCollectionView"></a>
#### `RoleCollectionView`

Представление коллекции роли: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/RoleCollectionView.qml](imtauthgui/Views/RoleCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `productId: string`
- `tenantId: string`
- `documentManager: var`

**Функции**

- `handleSubscription(dataModel)`

<a id="type-imtauthgui-RoleView"></a>
#### `RoleView`

Визуальное представление роли: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/RoleView.qml](imtauthgui/Views/RoleView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `productId: string`
- `tenantId: string`
- `permissionsProvider: var`
- `roleData: RoleData`

**Функции**

- `updateGui()`
- `updateModel()`
- `getHeaders()`
- `doUpdateGuiPermissions()`
- `doUpdateModelPermissions()`

<a id="type-imtauthgui-StatusBadge"></a>
#### `StatusBadge`

Бейдж статуса статуса: компактный визуальный индикатор состояния.

- **Файл:** [Views/StatusBadge.qml](imtauthgui/Views/StatusBadge.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `text: alias`
- `textColor: color`
- `badgeColor: color`
- `badgeBorderColor: color`
- `badgeBorderWidth: int`
- `horizontalPadding: real`
- `verticalPadding: real`
- `fontPixelSize: int`
- `boldText: bool`

<a id="type-imtauthgui-TenantCollectionListView"></a>
#### `TenantCollectionListView`

Представление коллекции организации (tenant): список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/TenantCollectionListView.qml](imtauthgui/Views/TenantCollectionListView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `collectionId: string`
- `tenantId: string`
- `filterPlaceholder: string`
- `emptyMessage: string`
- `canManage: bool`
- `selectionMode: string`
- `menuCommands: var`
- `selectionManager: alias` — только чтение

**Сигналы**

- `editRequested(string itemId, string itemName, string itemDescription)`
- `deleteRequested(string itemId, string itemName)`
- `menuCommandTriggered(string commandId, string itemId, string itemName)`
- `selectionChanged(var selectedIds)`

**Функции**

- `refresh()`

<a id="type-imtauthgui-TenantCollectionView"></a>
#### `TenantCollectionView`

Представление коллекции организации (tenant): список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/TenantCollectionView.qml](imtauthgui/Views/TenantCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `tenantManagementApiClient: GqlBasedTenantManagementApiClient`
- `acceptInvitationInput: AcceptTenantInvitationInput`
- `acceptInvitationSender: GqlSdlRequestSender`
- `rejectInvitationInput: RejectTenantInvitationInput`
- `rejectInvitationSender: GqlSdlRequestSender`

**Функции**

- `acceptInvitation(invitationId)`
- `rejectInvitation(invitationId)`
- `switchToTenant(tenantId)`
- `openTenantDocument(tenantId)`
- `requestOpenTenantDocument(tenantId, tenantName)`
- `closeTenantEditorsForSwitch(activeTenantId)`
- `leaveTenant(tenantId)`

<a id="type-imtauthgui-TenantTableContainer"></a>
#### `TenantTableContainer`

Контейнер организации (tenant): компоновочная обёртка для дочерних элементов.

- **Файл:** [Views/TenantTableContainer.qml](imtauthgui/Views/TenantTableContainer.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `maxTableWidth: int`
- `contentItem: alias`

<a id="type-imtauthgui-TenantTableHeader"></a>
#### `TenantTableHeader`

Заголовок организации (tenant): шапка таблицы/раздела.

- **Файл:** [Views/TenantTableHeader.qml](imtauthgui/Views/TenantTableHeader.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `selectedCount: int`
- `totalCount: int`
- `checkState: int`
- `checkBoxSize: int` — только чтение

**Сигналы**

- `selectAllToggled()`

<a id="type-imtauthgui-UserActionCollectionView"></a>
#### `UserActionCollectionView`

Представление коллекции пользователя: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/UserActionCollectionView.qml](imtauthgui/Views/UserActionCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtauthgui-UserCollectionView"></a>
#### `UserCollectionView`

Представление коллекции пользователя: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/UserCollectionView.qml](imtauthgui/Views/UserCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `productId: string`
- `documentManager: var`

**Сигналы**

- `saved()`

<a id="type-imtauthgui-UserGroupCollectionView"></a>
#### `UserGroupCollectionView`

Представление коллекции пользователя: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/UserGroupCollectionView.qml](imtauthgui/Views/UserGroupCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `productId: string`
- `documentManager: var`

**Функции**

- `handleSubscription(dataModel)`

<a id="type-imtauthgui-UserGroupView"></a>
#### `UserGroupView`

Визуальное представление пользователя: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/UserGroupView.qml](imtauthgui/Views/UserGroupView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `groupData: GroupData`
- `productId: string`

**Функции**

- `updateGui()`
- `updateModel()`
- `getHeaders()`

<a id="type-imtauthgui-UserPanel"></a>
#### `UserPanel`

Панель пользователя: область интерфейса (список, боковая или нижняя панель).

- **Файл:** [Views/UserPanel.qml](imtauthgui/Views/UserPanel.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `enabled: bool`
- `iconSource: alias`
- `isExitButton: bool`
- `profileApiClient: GqlBasedProfileApiClient`
- `organizationsInput: GetProfileInput`

**Функции**

- `setUserPanelEnabled(enabled)`
- `setVisible(visible)`

<a id="type-imtauthgui-UserView"></a>
#### `UserView`

Визуальное представление пользователя: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/UserView.qml](imtauthgui/Views/UserView.qml)
- **Базовый тип:** [`DocumentViewBase`](#type-imtdocgui-DocumentViewBase)

**Свойства**

- `userData: UserData`
- `productId: string`
- `passwordInput: alias`
- `passwordInputConfirm: alias`
- `isNew: bool`
- `hasValidUserId: bool` — только чтение
- `contextEntityDisplayName: string` — только чтение

**Функции**

- `updateGui()`
- `updateModel()`
- `getHeaders()`
- `checkChangePasswordLogic()`
- `checkSystemId()`

### Dialogs/ — Диалоги

<a id="type-imtauthgui-ChangePasswordDialog"></a>
#### `ChangePasswordDialog`

Модальный диалог пароля. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/ChangePasswordDialog.qml](imtauthgui/Dialogs/ChangePasswordDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `currentPasswordInputVisible: bool`

<a id="type-imtauthgui-PasswordRecoveryDialog"></a>
#### `PasswordRecoveryDialog`

Модальный диалог пароля. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/PasswordRecoveryDialog.qml](imtauthgui/Dialogs/PasswordRecoveryDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `currentIndex: int`
- `login: string`
- `username: string`
- `email: string`
- `code: string`
- `newPassword: string`
- `defaultSecs: int`
- `secs: int`
- `stackView: StackView`

**Сигналы**

- `refreshSendButton()`

### Editors/ — Редакторы

<a id="type-imtauthgui-TenantDocumentEditorShell"></a>
#### `TenantDocumentEditorShell`

Оболочка организации (tenant): каркас, объединяющий части экрана.

- **Файл:** [Editors/TenantDocumentEditorShell.qml](imtauthgui/Editors/TenantDocumentEditorShell.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `documentManager: var`
- `objectTypeId: string`
- `objectId: string`
- `createNew: bool`
- `generateNewId: bool`
- `documentNameResolver: var`
- `activeShellTarget: var`
- `shellView: alias` — только чтение

**Сигналы**

- `closed()`

<a id="type-imtauthgui-TenantEditor"></a>
#### `TenantEditor`

Редактор организации (tenant) — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/TenantEditor.qml](imtauthgui/Editors/TenantEditor.qml)
- **Базовый тип:** [`DocumentViewBase`](#type-imtdocgui-DocumentViewBase)

**Свойства**

- `tenantData: TenantData`
- `apiClient: var`
- `isNewTenant: bool` — только чтение

**Функции**

- `updateGui()`
- `updateModel()`

<a id="type-imtauthgui-UserGeneralEditor"></a>
#### `UserGeneralEditor`

Редактор пользователя — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/UserGeneralEditor.qml](imtauthgui/Editors/UserGeneralEditor.qml)
- **Базовый тип:** [`Column`](https://doc.qt.io/qt-6/qml-qtquick-column.html)

**Свойства**

- `userData: UserData`
- `readOnly: bool`
- `usernameInput: alias`
- `nameInput: alias`
- `mailInput: alias`
- `passwordInput: alias`
- `confirmPasswordInput: alias`
- `changePasswordButton: alias`
- `canHideGroup: bool`

**Сигналы**

- `emitUpdateModel()`
- `emitUpdateGui()`

**Функции**

- `updateGui()`
- `updateModel()`

### Delegates/ — Делегаты

<a id="type-imtauthgui-RoleCollectionViewCommandsDelegate"></a>
#### `RoleCollectionViewCommandsDelegate`

Делегат команд роли: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/RoleCollectionViewCommandsDelegate.qml](imtauthgui/Delegates/RoleCollectionViewCommandsDelegate.qml)
- **Базовый тип:** [`DocumentCollectionViewDelegate`](#type-imtdocgui-DocumentCollectionViewDelegate)

**Функции**

- `updateStateBaseCommands(selection, commandsController, elementsModel)`

<a id="type-imtauthgui-TenantCollectionItemDelegateBase"></a>
#### `TenantCollectionItemDelegateBase`

Базовый делегат организации (tenant) — основа для конкретных делегатов элементов.

- **Файл:** [Delegates/TenantCollectionItemDelegateBase.qml](imtauthgui/Delegates/TenantCollectionItemDelegateBase.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`TenantMemberDelegate`](#type-imtauthgui-TenantMemberDelegate)

**Свойства**

- `itemId: string`
- `itemTitle: string`
- `itemDescription: string`
- `itemParameters: var`
- `selectionManager: var`
- `collectionPage: var`
- `canManage: bool`
- `canEdit: bool`
- `canDelete: bool`
- `showCheckBox: bool`
- `enableDefaultDoubleClickEdit: bool`
- `showDefaultActionsMenu: bool`
- `customActionsComponent: Component`
- `isSelected: bool` — только чтение
- `isHovered: bool` — только чтение
- `checkBoxSize: int` — только чтение
- `index: int` — только чтение
- `totalCount: int` — только чтение
- `isLastItem: bool` — только чтение

**Сигналы**

- `itemDoubleClicked()`

<a id="type-imtauthgui-TenantMemberDelegate"></a>
#### `TenantMemberDelegate`

Делегат организации (tenant): шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/TenantMemberDelegate.qml](imtauthgui/Delegates/TenantMemberDelegate.qml)
- **Базовый тип:** [`TenantCollectionItemDelegateBase`](#type-imtauthgui-TenantCollectionItemDelegateBase)

**Свойства**

- `kind: string`
- `memberData: var`
- `tenantData: var`
- `stateManager: var`
- `isOwner: bool`
- `isMember: bool` — только чтение
- `isMemberOwner: bool` — только чтение
- `isMemberCreator: bool` — только чтение
- `isCurrentUser: bool` — только чтение
- `isExpired: bool` — только чтение
- `isRevoked: bool` — только чтение
- `effectiveStatus: string` — только чтение
- `canEditMember: bool` — только чтение
- `canChangeMemberRole: bool` — только чтение
- `canManageOrganizationMembers: bool` — только чтение
- `canRemoveMember: bool` — только чтение
- `canInviteMember: bool` — только чтение
- `selectionId: string` — только чтение

**Сигналы**

- `memberActionsRequested()`
- `inviteActionsRequested()`
- `memberEditRequested(string userId, string userName)`

### Controllers/ — Контроллеры

<a id="type-imtauthgui-AuthorizationController"></a>
#### `AuthorizationController` · **singleton**

Невизуальный контроллер авторизации: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/AuthorizationController.qml](imtauthgui/Controllers/AuthorizationController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `productId: string`
- `rememberMe: bool`
- `lastUser: string`
- `storedRefreshToken: string`
- `currentTenantId: string`
- `currentTenantName: string`
- `authenticationTokenHeaderId: string` — только чтение
- `pendingInvitationsCount: int`
- `pendingInvitations: var`
- `storage: Settings`
- `requestProxy: XmlHttpRequestProxy`
- `userTokenProvider: UserTokenProvider`
- `tenantCollectionListener: RemoteCollectionChangeListener`
- `registerUserInput: RegisterUserInput`
- `registerUserRequestSender: GqlSdlRequestSender`
- `changePasswordInput: ChangePasswordInput`
- `changePasswordGqlSender: GqlSdlRequestSender`
- `logoutGqlSender: GqlSdlRequestSender`
- `getPermissionsInput: TokenInput`
- `getPermissionsGqlSender: GqlSdlRequestSender`
- `refreshTokenGqlSender: GqlSdlRequestSender`
- `refreshTokenForLoginGqlSender: GqlSdlRequestSender`
- `selectTenantInput: SelectTenantInput`
- `selectTenantGqlSender: GqlSdlRequestSender`

**Сигналы**

- `userModeChanged(string userMode)`
- `superuserExistResult(string status, string error)`
- `loginFailed(string message)`
- `loggedIn()`
- `loggedOut()`
- `tenantSelected(string tenantId)`
- `tenantSelectionFailed(string error)`
- `changePasswordSuccessfully()`
- `changePasswordFailed()`
- `registerSuccessfully()`
- `registerFailed()`
- `tenantInvitationReceived(string tenantId, string tenantName, string role)`
- `tenantInvitationAccepted(string tenantId, string membershipId)`
- `tenantInvitationRejected(string tenantId, string membershipId)`
- `tenantOwnershipTransferred(string tenantId)`
- `tenantMembershipRoleChanged(string tenantId, string userId, string role)`
- `tenantMembershipRemoved(string tenantId, string userId)`
- `tenantInvitationRevoked(string tenantId, string invitationId)`

**Функции**

- `refreshPendingInvitations()`
- `loadLoginSettings()`
- `normalizePermissions(rawPermissions)`
- `refreshPermissions(callback)`
- `saveLoginSettings()`
- `clearLoginSettings()`
- `onAccessTokenUnauthorizedEvent(parameters)`
- `isLoggedIn()`
- `handleAuthFailure(gqlData, gqlRequestRef)`
- `completeTokenRefresh(ok)`
- `readDataFromStorage()`
- `saveDataToStorage()`
- `removeDataFromStorage()`
- `saveRefreshTokenIfRememberMe()`
- `clearRefreshToken()`
- `loginWithRefreshToken(userName, refreshToken)`
- `updateSuperuserModel()`
- `updateUserManagementModel()`
- `loggedUserIsSuperuser()`
- `getLoggedUserId()`
- `getPermissions()`
- `getSystemId()`
- `logout()`
- `logoutForce(reasonMessage)`
- `getAccessToken()`
- `setAccessToken(token)`
- `setRefreshToken(token)`
- `login(login, password)`
- `getUserMode()`
- `getUserId()`
- `getTenantId()`
- `selectTenant(tenantId)`
- `isStrongUserManagement()`
- `isSimpleUserManagement()`
- `changePassword(userId, oldPassword, newPassword)`
- `registerUser(userData)`
- `setLoginData(refreshToken, accessToken, userId, login, systemId, permissions)`

<a id="type-imtauthgui-PermissionsController"></a>
#### `PermissionsController` · **singleton**

Невизуальный контроллер прав доступа: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/PermissionsController.qml](imtauthgui/Controllers/PermissionsController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `checkPermission(permissionId)`
- `getPermissions()`

<a id="type-imtauthgui-TenantEditorStateManager"></a>
#### `TenantEditorStateManager`

Менеджер состояния организации (tenant): хранит и переключает состояние редактора/экрана.

- **Файл:** [Controllers/TenantEditorStateManager.qml](imtauthgui/Controllers/TenantEditorStateManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `tenantData: var`
- `apiClient: var`
- `pendingMembers: var`
- `pendingInvitations: var`
- `receivedRoleData: var`
- `receivedGroupData: var`
- `receivedUserData: var`
- `isNewTenant: bool` — только чтение
- `isCreator: bool` — только чтение
- `isOwner: bool` — только чтение
- `isAdmin: bool` — только чтение
- `canChangeOrganizationName: bool` — только чтение
- `canChangeOrganizationDescription: bool` — только чтение
- `canEditOrganization: bool` — только чтение
- `canViewOrganizationMembers: bool` — только чтение
- `canInviteOrganizationMember: bool` — только чтение
- `canExcludeOrganizationMember: bool` — только чтение
- `canChangeOrganizationMember: bool` — только чтение
- `canChangeOrganizationMemberRole: bool` — только чтение
- `canManageOrganizationMembers: bool` — только чтение
- `canViewOrganizationRoles: bool` — только чтение
- `canManageOrganizationRoles: bool` — только чтение
- `canViewOrganizationGroups: bool` — только чтение
- `canManageOrganizationGroups: bool` — только чтение
- `canViewOrganizationPermissions: bool` — только чтение
- `canEditOrganizationMemberPermissions: bool` — только чтение
- `canViewOrganizationConnections: bool` — только чтение
- `canViewOrganizationConnectionCode: bool` — только чтение
- `canConnectOrganization: bool` — только чтение
- `canRemoveOrganizationConnection: bool` — только чтение
- `canManageOrganizationConnections: bool` — только чтение
- `pagesConfigKey: string` — только чтение
- `canManageMembers: bool` — только чтение
- `isReadOnly: bool` — только чтение

**Функции**

- `hasPermission(permissionId)`
- `hasAnyPermission(permissionIds)`
- `formatDateTime(value)`
- `displayNameOrId(name, id)`
- `formatInvitationInfo(invitedByName, expiresAt)`
- `isInvitationExpired(expiresAt)`
- `loadMembersFromModel()`
- `loadInvitationsFromModel()`
- `removeMemberById(userId)`
- `removePendingInvitation(invitationId)`
- `inviteSelectedUsers(selectedItems)`
- `syncMembersToModel()`

<a id="type-imtauthgui-UserSettingsController"></a>
#### `UserSettingsController`

Невизуальный контроллер пользователя: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/UserSettingsController.qml](imtauthgui/Controllers/UserSettingsController.qml)
- **Базовый тип:** [`ParamsSetController`](imtgui/Params/ParamsSetController.qml)

**Свойства**

- `getSettingsQuery: GqlSdlRequestSender`
- `setSettingsQuery: GqlSdlRequestSender`

**Сигналы**

- `settingsReceived()`
- `settingsReceiveFailed()`
- `settingsSaved()`
- `settingsSaveFailed()`

**Функции**

- `getSettings()`
- `saveParam()`

### Providers/ — Провайдеры

<a id="type-imtauthgui-GroupCollectionDataProvider"></a>
#### `GroupCollectionDataProvider`

Провайдер данных группы: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/GroupCollectionDataProvider.qml](imtauthgui/Providers/GroupCollectionDataProvider.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)
- **Наследники:** [`CachedGroupCollection`](#type-imtauthgui-CachedGroupCollection)

<a id="type-imtauthgui-PermissionsProvider"></a>
#### `PermissionsProvider`

Провайдер данных прав доступа: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/PermissionsProvider.qml](imtauthgui/Providers/PermissionsProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`GqlBasedPermissionsProvider`](#type-imtguigql-GqlBasedPermissionsProvider)

**Свойства**

- `productId: string`
- `loading: bool`
- `lastError: string`
- `permissions: var`
- `allPermissions: var`
- `userPermissions: var`
- `tenantPermissions: var`
- `tenantPermissionsTenantId: string`
- `organizationPermissions: var`
- `memberOrganizationPermissions: var`

**Сигналы**

- `userPermissionsReceived()`
- `organizationPermissionsReceived()`
- `requestStarted(string tenantId)`
- `permissionsReceived(var permissions, string tenantId)`
- `allPermissionsReceived()`
- `tenantPermissionsReceived(string tenantId)`
- `requestFailed(string message, string tenantId)`

**Функции**

- `requestPermissions(tenantId)`
- `requestAllPermissions()`
- `requestUserPermissions()`
- `requestOrganizationPermissions(tenantId, userId)`
- `clearCache()`
- `getHeaders()`

<a id="type-imtauthgui-RoleCollectionDataProvider"></a>
#### `RoleCollectionDataProvider`

Провайдер данных роли: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/RoleCollectionDataProvider.qml](imtauthgui/Providers/RoleCollectionDataProvider.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)
- **Наследники:** [`CachedRoleCollection`](#type-imtauthgui-CachedRoleCollection)

**Свойства**

- `productId: string`

**Функции**

- `setCustomInputParams(inputParams)`
- `getHeaders()`

<a id="type-imtauthgui-UserCollectionDataProvider"></a>
#### `UserCollectionDataProvider`

Провайдер данных пользователя: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/UserCollectionDataProvider.qml](imtauthgui/Providers/UserCollectionDataProvider.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)
- **Наследники:** [`CachedUserCollection`](#type-imtauthgui-CachedUserCollection)

<a id="type-imtauthgui-UserTokenProvider"></a>
#### `UserTokenProvider`

Провайдер данных пользователя: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/UserTokenProvider.qml](imtauthgui/Providers/UserTokenProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `accessToken: string`
- `refreshToken: string`
- `login: string`
- `userId: string`
- `systemId: string`
- `productId: string`
- `permissions: var`
- `isTokenGlobal: bool`
- `modelState: string`
- `request: GqlSdlRequestSender`
- `authorizationGqlModel: GqlModel`

**Сигналы**

- `accepted()`
- `failed(string message)`

**Функции**

- `getHeaders()`
- `authorization(loginF, passwordF)`
- `setLoginData(refreshToken, accessToken, userId, login, systemId, permissions)`

### Api/ — Сеть и API

<a id="type-imtauthgui-GqlBasedProfileApiClient"></a>
#### `GqlBasedProfileApiClient`

Клиент API профиля: выполняет запросы к серверу и передаёт результат в интерфейс.

- **Файл:** [Api/GqlBasedProfileApiClient.qml](imtauthgui/Api/GqlBasedProfileApiClient.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `loading: bool`
- `permissionsProvider: var`
- `lastProfile: var`
- `lastOrganizations: var`
- `getProfileRequest: GqlSdlRequestSender`
- `setProfileRequest: GqlSdlRequestSender`

**Сигналы**

- `profileReceived(var profile)`
- `profileSaved()`
- `organizationsReceived(var list)`
- `invitationAccepted()`
- `invitationRejected()`
- `leftTenant(string tenantId)`
- `tokenListReceived(var tokens)`
- `tokenCreated(string token)`
- `tokenDeleted(string message)`
- `tokenRevoked(string message)`
- `profileOperationFailed(string message)`
- `organizationOperationFailed(string message)`
- `tokenOperationFailed(string message)`

**Функции**

- `getProfile()`
- `setProfile(id, name, email)`
- `getOrganizations()`
- `acceptInvitation(invitationId)`
- `rejectInvitation(invitationId)`
- `leaveTenant(tenantId)`
- `getTokenList()`
- `createToken(name, description, scopes, expiresAt)`
- `deleteToken(tokenId)`
- `revokeToken(tokenId)`

<a id="type-imtauthgui-GqlBasedTenantManagementApiClient"></a>
#### `GqlBasedTenantManagementApiClient`

Клиент API организации (tenant): выполняет запросы к серверу и передаёт результат в интерфейс.

- **Файл:** [Api/GqlBasedTenantManagementApiClient.qml](imtauthgui/Api/GqlBasedTenantManagementApiClient.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `productId: string`
- `tenantId: string`
- `rolePermissionsTenantId: string`
- `roleObjectTypeId: string`
- `groupObjectTypeId: string`
- `userObjectTypeId: string`
- `crossOrgGrantObjectTypeId: string`
- `contractObjectTypeId: string`
- `relationshipObjectTypeId: string`
- `crossTenantMessageObjectTypeId: string`
- `roleDocumentManager: var` — только чтение
- `groupDocumentManager: var` — только чтение
- `userDocumentManager: var` — только чтение
- `relationshipDocumentManager: var` — только чтение
- `crossOrgGrantDocumentManager: var` — только чтение
- `permissionsProvider: var` — только чтение
- `allPermissions: var`
- `tenantPermissions: var`
- `organizationPermissions: var`
- `memberOrganizationPermissions: var`
- `contractsModel: ListModel`
- `tenantRelationshipsModel: ListModel`
- `connectionRequestsModel: ListModel`
- `connectionsModel: ListModel`
- `relationshipProposalsModel: ListModel`
- `crossTenantMessagesModel: ListModel`
- `orderRequestsModel: ListModel`
- `roleListDataProvider: FilterableSelectGqlDataProvider`
- `groupListDataProvider: FilterableSelectGqlDataProvider`
- `invitableUsersListDataProvider: FilterableSelectGqlDataProvider`
- `tenantsListDataProvider: FilterableSelectGqlDataProvider`
- `tenantRelationshipsListDataProvider: FilterableSelectGqlDataProvider`
- `crossOrgGrantsListDataProvider: FilterableSelectGqlDataProvider`
- `connectionsDataProvider: FilterableSelectGqlDataProvider`

**Сигналы**

- `invitationCreated()`
- `invitationRevoked(string invitationId)`
- `invitationResent(string invitationId)`
- `ownershipTransferred()`
- `memberRemoved(string userId)`
- `roleCreated()`
- `rolesRemoved()`
- `crossOrgGrantsRemoved()`
- `roleUpdated(string roleId)`
- `roleDataReceived(var data)`
- `groupCreated()`
- `groupsRemoved()`
- `groupUpdated(string groupId)`
- `groupDataReceived(var data)`
- `userCreated()`
- `usersRemoved()`
- `userUpdated(string userId)`
- `userDataReceived(var data)`
- `contractCreated(string contractId)`
- `contractStatusUpdated(string contractId)`
- `contractTerminated(string contractId)`
- `contractsReceived(var contracts)`
- `requestFailed(string message)`
- `subscriptionInvitationReceived(string tenantId, string tenantName, string role)`
- `subscriptionInvitationAccepted(string tenantId, string membershipId)`
- `subscriptionInvitationRejected(string tenantId, string membershipId)`
- `subscriptionOwnershipTransferred(string tenantId)`
- `subscriptionMembershipRoleChanged(string tenantId, string userId, string role)`
- `subscriptionMembershipRemoved(string tenantId, string userId)`
- `crossTenantMessageSent(string messageId)`
- `crossTenantMessageStatusUpdated(string messageId)`
- `crossTenantMessagesReceived(var messages)`
- `orderRequestConfirmed(string requestId)`
- `orderRequestRejected(string requestId)`
- `orderRequestStatusUpdated(string requestId)`
- `orderRequestsReceived(var orderRequests)`
- `subscriptionCrossTenantMessageReceived(var notification)`
- `subscriptionCrossTenantMessageStatusChanged(var notification)`
- `subscriptionConnectionCodesChanged(var notification)`
- `memberPermissionsUpdated()`
- `allPermissionsReceived()`
- `tenantPermissionsReceived()`
- `organizationPermissionsReceived()`
- `connectionCodeReceived(string code, bool allowByCode)`
- `connectionCodeRegenerated(string newCode)`
- `allowConnectionsByCodeChanged(bool allow)`
- `connectionRequestCreated(string requestId)`
- `connectionRequestError(string errorMessage)`
- `connectionRequestApproved(string connectionId)`
- `connectionRequestRejected()`
- `connectionRequestCanceled()`
- `connectionRequestsReceived()`
- `connectionsReceived()`
- `connectionRemoved(string connectionId)`
- `tenantRelationshipRemoved()`
- `tenantRelationshipsReceived()`
- `relationshipProposalCreated()`
- `relationshipProposalApproved(string relationshipId)`
- `relationshipProposalRejected()`
- `relationshipProposalCanceled()`
- `relationshipProposalsReceived()`

**Функции**

- `createInvitation(tenantId, userId)`
- `revokeInvitation(invitationId)`
- `resendInvitation(invitationId)`
- `transferOwnership(tenantId, newOwnerId)`
- `setMemberOrganizationPermissionsByUser(tenantId, userId, permissions)`
- `setMemberOrganizationPermissions(membershipId, permissionsList)`
- `removeMember(tenantId, userId)`
- `removeRoles(roleIds)`
- `removeGroups(groupIds)`
- `setRolePermissionsTenantId(tenantId)`
- `fetchAllPermissions()`
- `fetchTenantPermissions(tenantId)`
- `fetchOrganizationPermissions(tenantId, userId)`
- `revokeCrossOrgGrant(grantId)`
- `removeCrossOrgGrants(grantIds)`
- `fetchContracts(tenantId)`
- `createContract(relationshipId, sourceTenantId, targetTenantId, scope, validFrom, validUntil, description, terms)`
- `updateContractStatus(contractId, status)`
- `terminateContract(contractId)`
- `getConnectionCode(tenantId)`
- `regenerateConnectionCode(tenantId)`
- `setAllowConnectionsByCode(tenantId, allow)`
- `fetchConnectionRequests(tenantId)`
- `createConnectionRequest(sourceTenantId, connectionCode, message)`
- `approveConnectionRequest(requestId, tenantId)`
- `rejectConnectionRequest(requestId, tenantId)`
- `cancelConnectionRequest(requestId, tenantId)`
- `fetchConnections(tenantId)`
- `removeConnection(connectionId, tenantId)`
- `fetchTenantRelationships(tenantId)`
- `removeTenantRelationship(tenantId, relationshipId)`
- `fetchRelationshipProposals(tenantId)`
- `createRelationshipProposal(connectionId, initiatorTenantId, proposalType, proposedSourceRole, proposedTargetRole, proposedScope, proposedDescription, message)`
- `approveRelationshipProposal(proposalId, tenantId)`
- `rejectRelationshipProposal(proposalId, tenantId)`
- `cancelRelationshipProposal(proposalId, tenantId)`
- `fetchCrossTenantMessages(tenantId, direction)`
- `sendCrossTenantMessage(sourceTenantId, targetTenantId, relationshipId, messageType, payload, sourceObjectId, customType, expiresAt)`
- `updateCrossTenantMessageStatus(messageId, status, errorMessage)`
- `fetchOrderRequests(tenantId)`
- `confirmOrderRequest(orderRequestId, note)`
- `rejectOrderRequest(orderRequestId, reason)`
- `updateOrderRequestStatus(orderRequestId, status, note)`

<a id="type-imtauthgui-ProfileApiClient"></a>
#### `ProfileApiClient`

Клиент API профиля: выполняет запросы к серверу и передаёт результат в интерфейс.

- **Файл:** [Api/ProfileApiClient.qml](imtauthgui/Api/ProfileApiClient.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `loading: bool`
- `permissionsProvider: var`
- `lastProfile: var`
- `lastOrganizations: var`

**Сигналы**

- `profileReceived(var profile)`
- `profileSaved()`
- `organizationsReceived(var list)`
- `invitationAccepted()`
- `invitationRejected()`
- `leftTenant(string tenantId)`
- `tokenListReceived(var tokens)`
- `tokenCreated(string token)`
- `tokenDeleted(string message)`
- `tokenRevoked(string message)`
- `profileOperationFailed(string message)`
- `organizationOperationFailed(string message)`
- `tokenOperationFailed(string message)`

**Функции**

- `getProfile()`
- `setProfile(id, name, email)`
- `getOrganizations()`
- `acceptInvitation(invitationId)`
- `rejectInvitation(invitationId)`
- `leaveTenant(tenantId)`
- `getTokenList()`
- `createToken(name, description, scopes, expiresAt)`
- `deleteToken(tokenId)`
- `revokeToken(tokenId)`

<a id="type-imtauthgui-TenantManagementApiClient"></a>
#### `TenantManagementApiClient`

Клиент API организации (tenant): выполняет запросы к серверу и передаёт результат в интерфейс.

- **Файл:** [Api/TenantManagementApiClient.qml](imtauthgui/Api/TenantManagementApiClient.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `tenantId: string`
- `roleDocumentManager: var`
- `groupDocumentManager: var`
- `userDocumentManager: var`
- `roleObjectTypeId: string`
- `groupObjectTypeId: string`
- `userObjectTypeId: string`
- `permissionsProvider: var`
- `allPermissions: var`
- `tenantPermissions: var`
- `crossOrgGrantDocumentManager: var`
- `crossOrgGrantsListDataProvider: var`
- `tenantsListDataProvider: var`
- `tenantRelationshipsListDataProvider: var`
- `contractsModel: var`
- `connectionRequestsModel: var`
- `connectionsModel: var`
- `tenantRelationshipsModel: var`
- `relationshipProposalsModel: var`
- `crossTenantMessagesModel: var`
- `orderRequestsModel: var`

**Сигналы**

- `invitationCreated()`
- `invitationRevoked(string invitationId)`
- `invitationResent(string invitationId)`
- `ownershipTransferred()`
- `memberRemoved(string userId)`
- `roleCreated()`
- `roleRemoved(string roleId)`
- `roleUpdated(string roleId)`
- `roleDataReceived(var data)`
- `groupCreated()`
- `groupRemoved(string groupId)`
- `groupUpdated(string groupId)`
- `groupDataReceived(var data)`
- `userCreated()`
- `userRemoved(string userId)`
- `userUpdated(string userId)`
- `userDataReceived(var data)`
- `crossOrgGrantsRemoved()`
- `contractCreated(string contractId)`
- `contractStatusUpdated(string contractId)`
- `contractTerminated(string contractId)`
- `contractsReceived(var contracts)`
- `connectionCodeReceived(string code, bool allowByCode)`
- `connectionCodeRegenerated(string newCode)`
- `allowConnectionsByCodeChanged(bool allow)`
- `connectionRequestCreated(string requestId)`
- `connectionRequestError(string errorMessage)`
- `connectionRequestApproved(string connectionId)`
- `connectionRequestRejected()`
- `connectionRequestCanceled()`
- `connectionRequestsReceived()`
- `connectionsReceived()`
- `connectionRemoved(string connectionId)`
- `tenantRelationshipRemoved()`
- `tenantRelationshipsReceived()`
- `relationshipProposalCreated()`
- `relationshipProposalApproved(string relationshipId)`
- `relationshipProposalRejected()`
- `relationshipProposalCanceled()`
- `relationshipProposalsReceived()`
- `crossTenantMessageSent(string messageId)`
- `crossTenantMessageStatusUpdated(string messageId)`
- `crossTenantMessagesReceived(var messages)`
- `orderRequestConfirmed(string orderRequestId)`
- `orderRequestRejected(string orderRequestId)`
- `orderRequestStatusUpdated(string orderRequestId)`
- `orderRequestsReceived(var orderRequests)`
- `requestFailed(string message)`
- `subscriptionInvitationReceived(string tenantId, string tenantName, string role)`
- `subscriptionInvitationAccepted(string tenantId, string membershipId)`
- `subscriptionInvitationRejected(string tenantId, string membershipId)`
- `subscriptionOwnershipTransferred(string tenantId)`
- `subscriptionMembershipRoleChanged(string tenantId, string userId, string role)`
- `subscriptionMembershipRemoved(string tenantId, string userId)`
- `subscriptionConnectionNotification(var notification)`
- `allPermissionsReceived()`
- `tenantPermissionsReceived()`

**Функции**

- `createInvitation(tenantId, userId)`
- `revokeInvitation(invitationId)`
- `resendInvitation(invitationId)`
- `transferOwnership(tenantId, newOwnerId)`
- `removeMember(tenantId, userId)`
- `createRoleData()`
- `insertRole(roleId, roleData)`
- `removeRole(roleId)`
- `setRoleData(roleId, roleData)`
- `getRoleData(roleId)`
- `createGroupData()`
- `insertGroup(groupId, groupData)`
- `removeGroup(groupId)`
- `setGroupData(groupId, groupData)`
- `getGroupData(groupId)`
- `createUserData()`
- `insertUser(userId, userData)`
- `removeUser(userId)`
- `setUserData(userId, userData)`
- `getUserData(userId)`
- `setRolePermissionsTenantId(tenantId)`
- `fetchAllPermissions()`
- `fetchTenantPermissions(tenantId)`
- `revokeCrossOrgGrant(grantId)`
- `removeCrossOrgGrants(grantIds)`
- `fetchContracts(tenantId)`
- `createContract(relationshipId, sourceTenantId, targetTenantId, scope, validFrom, validUntil, description, terms)`
- `updateContractStatus(contractId, status)`
- `terminateContract(contractId)`
- `getConnectionCode(tenantId)`
- `regenerateConnectionCode(tenantId)`
- `setAllowConnectionsByCode(tenantId, allow)`
- `fetchConnectionRequests(tenantId)`
- `createConnectionRequest(sourceTenantId, connectionCode, message)`
- `approveConnectionRequest(requestId, tenantId)`
- `rejectConnectionRequest(requestId, tenantId)`
- `cancelConnectionRequest(requestId, tenantId)`
- `fetchConnections(tenantId)`
- `removeConnection(connectionId, tenantId)`
- `fetchTenantRelationships(tenantId)`
- `removeTenantRelationship(tenantId, relationshipId)`
- `fetchRelationshipProposals(tenantId)`
- `createRelationshipProposal(connectionId, initiatorTenantId, proposalType, proposedSourceRole, proposedTargetRole, proposedScope, proposedDescription, message)`
- `approveRelationshipProposal(proposalId, tenantId)`
- `rejectRelationshipProposal(proposalId, tenantId)`
- `cancelRelationshipProposal(proposalId, tenantId)`
- `fetchCrossTenantMessages(tenantId, direction)`
- `sendCrossTenantMessage(sourceTenantId, targetTenantId, relationshipId, messageType, payload, sourceObjectId, customType, expiresAt)`
- `updateCrossTenantMessageStatus(messageId, status, errorMessage)`
- `fetchOrderRequests(tenantId)`
- `confirmOrderRequest(orderRequestId, note)`
- `rejectOrderRequest(orderRequestId, reason)`
- `updateOrderRequestStatus(orderRequestId, status, note)`

### Models/ — Модели и данные

<a id="type-imtauthgui-CachedAccountCollection"></a>
#### `CachedAccountCollection` · **singleton**

Кэшируемая коллекция учётной записи (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedAccountCollection.qml](imtauthgui/Models/CachedAccountCollection.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)

<a id="type-imtauthgui-CachedGroupCollection"></a>
#### `CachedGroupCollection` · **singleton**

Кэшируемая коллекция группы (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedGroupCollection.qml](imtauthgui/Models/CachedGroupCollection.qml)
- **Базовый тип:** [`GroupCollectionDataProvider`](#type-imtauthgui-GroupCollectionDataProvider)

<a id="type-imtauthgui-CachedRoleCollection"></a>
#### `CachedRoleCollection` · **singleton**

Кэшируемая коллекция роли (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedRoleCollection.qml](imtauthgui/Models/CachedRoleCollection.qml)
- **Базовый тип:** [`RoleCollectionDataProvider`](#type-imtauthgui-RoleCollectionDataProvider)

<a id="type-imtauthgui-CachedUserCollection"></a>
#### `CachedUserCollection` · **singleton**

Кэшируемая коллекция пользователя (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedUserCollection.qml](imtauthgui/Models/CachedUserCollection.qml)
- **Базовый тип:** [`UserCollectionDataProvider`](#type-imtauthgui-UserCollectionDataProvider)

---

<a id="imtdeskgui"></a>
## `imtdeskgui` — Служба поддержки (тикеты)

Лёгкая система тикетов службы поддержки: страница стола заявок, список и просмотр тикетов, создание/редактирование, а также диалог тикетов в контексте конкретной сущности.

**Импорт:** `import imtdeskgui 1.0` · **Файлов:** 8

### Pages/ — Страницы

<a id="type-imtdeskgui-DeskPage"></a>
#### `DeskPage`

Экранная страница службы поддержки — самостоятельный раздел интерфейса, встраиваемый в навигацию или рабочую область.

- **Файл:** [Pages/DeskPage.qml](imtdeskgui/Pages/DeskPage.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Функции**

- `checkCurrentPage(commandId)`

### Views/ — Представления

<a id="type-imtdeskgui-TicketBadge"></a>
#### `TicketBadge`

Бейдж статуса тикетов: компактный визуальный индикатор состояния.

- **Файл:** [Views/TicketBadge.qml](imtdeskgui/Views/TicketBadge.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `badgeType: string`
- `value: int`
- `labelColor: string`
- `labelName: string`
- `badgeColor: string` — только чтение
- `badgeLabel: string` — только чтение

<a id="type-imtdeskgui-TicketCollectionView"></a>
#### `TicketCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/TicketCollectionView.qml](imtdeskgui/Views/TicketCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtdeskgui-TicketView"></a>
#### `TicketView`

Визуальное представление тикетов: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/TicketView.qml](imtdeskgui/Views/TicketView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `ticketId: string`
- `number: int`
- `title: string`
- `description: string`
- `ticketType: int`
- `status: int`
- `stateReason: int`
- `priority: int`
- `assigneeIds: var`
- `assigneeName: string`
- `reporterId: string`
- `reporterName: string`
- `conversationId: string`
- `createdAt: string`
- `updatedAt: string`
- `closedAt: string`
- `tags: var`
- `labelIds: var`
- `labels: var`
- `locked: bool`
- `lockReason: string`

**Сигналы**

- `statusChangeRequested(int newStatus, int newStateReason)`
- `assigneeChangeRequested(var newAssigneeIds)`
- `conversationOpenRequested(string conversationId)`
- `ticketClosed()`

**Функции**

- `ticketTypeLabel(type)`

### Panels/ — Панели

<a id="type-imtdeskgui-TicketListPanel"></a>
#### `TicketListPanel`

Панель тикетов: область интерфейса (список, боковая или нижняя панель).

- **Файл:** [Panels/TicketListPanel.qml](imtdeskgui/Panels/TicketListPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `ticketsModel: var`
- `currentTicketId: string`
- `statusFilter: int`

**Сигналы**

- `ticketSelected(string ticketId)`
- `newTicketRequested()`

### Dialogs/ — Диалоги

<a id="type-imtdeskgui-EntityContextTicketsDialog"></a>
#### `EntityContextTicketsDialog`

Модальный диалог сущности. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/EntityContextTicketsDialog.qml](imtdeskgui/Dialogs/EntityContextTicketsDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `entityType: string`
- `entityId: string`
- `entityDisplayName: string`
- `hasValidEntityContext: bool` — только чтение
- `resolvedEntityDisplayName: string` — только чтение
- `loading: bool`
- `ticketsPageSize: int`
- `cardPadding: real` — только чтение
- `cardRadius: real` — только чтение
- `cardColor: string` — только чтение
- `cardBorderColor: string` — только чтение
- `pageBgColor: string` — только чтение
- `accentColor: string` — только чтение
- `sectionLabelColor: string` — только чтение

**Сигналы**

- `ticketCreated()`
- `ticketCreateFailed(string message)`

**Функции**

- `reloadTickets()`
- `openTicket(ticketId)`
- `createTicket(title, description)`
- `appendTicketItems(itemsModel)`

<a id="type-imtdeskgui-TicketCreateDialog"></a>
#### `TicketCreateDialog`

Модальный диалог тикетов. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/TicketCreateDialog.qml](imtdeskgui/Dialogs/TicketCreateDialog.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `prefillContent: string`
- `linkedMessageId: string`
- `linkedConversationId: string`

**Сигналы**

- `ticketCreated(var ticketData)`
- `cancelled()`

**Функции**

- `open(prefill, messageId, conversationId)`
- `cancel()`
- `submitTicket()`

### Editors/ — Редакторы

<a id="type-imtdeskgui-TicketEditor"></a>
#### `TicketEditor`

Редактор тикетов — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/TicketEditor.qml](imtdeskgui/Editors/TicketEditor.qml)
- **Базовый тип:** [`DocumentViewBase`](#type-imtdocgui-DocumentViewBase)

**Свойства**

- `ticketData: TicketData`
- `isNewIssue: bool`
- `commentComp: Component`
- `currentUserId: string` — только чтение
- `accessLevel: string` — только чтение
- `canEdit: bool` — только чтение
- `canLock: bool` — только чтение
- `canComment: bool` — только чтение
- `canEditCoreTicketFields: bool` — только чтение
- `pendingAttachments: var`
- `uploadsInProgress: int`
- `pendingEntityRefs: var`
- `pendingAssignees: var`
- `chatHintDurationMs: int` — только чтение
- `chatHintHeightPx: int` — только чтение
- `minCommentInputHeightPx: int` — только чтение

**Сигналы**

- `commentSubmitted(string commentText)`

**Функции**

- `formatTimestamp(isoStr)`
- `formatCommentHtml(content)`
- `normalizeUserId(userId)`
- `isSameUserId(leftId, rightId)`
- `isImageAttachment(fileName)`
- `copyTextToClipboard(text, successHint)`
- `getCollectionCount(collection)`
- `getCollectionItem(collection, index)`
- `formatChatExportText()`
- `chatParticipants()`
- `addComment(commentText, attachmentsList)`
- `editComment(messageId, newContent)`
- `deleteComment(messageId)`
- `removeCommentAttachment(messageId, attachmentIndex)`
- `uploadAttachment(fileObj, fileName, localPreview)`
- `findComboIndex(combo, targetId, fallback)`
- `updateGui()`
- `updateModel()`

---

<a id="imtdocgui"></a>
## `imtdocgui` — Документо-ориентированные рабочие области

Каркас для приложений, работающих с документами: рабочие области одиночного и множественного документа, история и ревизии, отмена/повтор (undo/redo), сервисы документа и делегаты команд рабочей области.

**Импорт:** `import imtdocgui 1.0` · **Файлов:** 30

### Pages/ — Страницы

<a id="type-imtdocgui-DocumentWorkspacePageView"></a>
#### `DocumentWorkspacePageView`

Страница-контейнер документа: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/DocumentWorkspacePageView.qml](imtdocgui/Pages/DocumentWorkspacePageView.qml)
- **Базовый тип:** [`PageContainer`](imtgui/View/PageContainer.qml)
- **Наследники:** [`CollectionWorkspacePageView`](#type-imtcolgui-CollectionWorkspacePageView), [`MultiDocWorkspacePageView`](#type-imtdocgui-MultiDocWorkspacePageView), [`SingleDocumentWorkspacePageView`](#type-imtdocgui-SingleDocumentWorkspacePageView)

**Свойства**

- `documentManager: DocumentService`
- `documentManagerView: Item`

**Функции**

- `checkCreation()`
- `setDocumentName(documentId, name)`
- `onMenuModelChanged(model)`

<a id="type-imtdocgui-MultiDocWorkspacePageView"></a>
#### `MultiDocWorkspacePageView`

Страница-контейнер нескольких документов: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/MultiDocWorkspacePageView.qml](imtdocgui/Pages/MultiDocWorkspacePageView.qml)
- **Базовый тип:** [`DocumentWorkspacePageView`](#type-imtdocgui-DocumentWorkspacePageView)

**Функции**

- `setDocumentName(documentId, name)`

<a id="type-imtdocgui-SingleDocumentWorkspacePageView"></a>
#### `SingleDocumentWorkspacePageView`

Страница-контейнер одиночного документа: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/SingleDocumentWorkspacePageView.qml](imtdocgui/Pages/SingleDocumentWorkspacePageView.qml)
- **Базовый тип:** [`DocumentWorkspacePageView`](#type-imtdocgui-DocumentWorkspacePageView)
- **Наследники:** [`AdministrationUi`](#type-imtauthgui-AdministrationUi), [`SearchPageSingleDocumentView`](#type-imtdocgui-SearchPageSingleDocumentView)

**Свойства**

- `visualStatusProvider: alias`

### Views/ — Представления

<a id="type-imtdocgui-DocumentHistoryView"></a>
#### `DocumentHistoryView`

Визуальное представление документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/DocumentHistoryView.qml](imtdocgui/Views/DocumentHistoryView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `elementWidth: int`
- `contentHeight: alias`
- `flickable: alias`
- `documentId: string`
- `activeRevision: int`
- `collectionId: string`
- `ok: bool`

**Функции**

- `sendRequest()`
- `getHeaders()`

<a id="type-imtdocgui-DocumentView"></a>
#### `DocumentView`

Визуальное представление документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/DocumentView.qml](imtdocgui/Views/DocumentView.qml)
- **Базовый тип:** [`DocumentViewBase`](#type-imtdocgui-DocumentViewBase)

<a id="type-imtdocgui-DocumentViewBase"></a>
#### `DocumentViewBase`

Базовое представление документа — общий каркас, от которого наследуются конкретные представления.

- **Файл:** [Views/DocumentViewBase.qml](imtdocgui/Views/DocumentViewBase.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)
- **Наследники:** [`DocumentView`](#type-imtdocgui-DocumentView), [`TenantEditor`](#type-imtauthgui-TenantEditor), [`TicketEditor`](#type-imtdeskgui-TicketEditor), [`UserView`](#type-imtauthgui-UserView)

**Свойства**

- `documentId: string`
- `documentTypeId: string`
- `documentManager: var`
- `documentManagerView: Item`
- `representationController: DocumentRepresentationController`

**Сигналы**

- `documentSaved()`

<a id="type-imtdocgui-MultiDocWorkspaceView"></a>
#### `MultiDocWorkspaceView`

Визуальное представление нескольких документов: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/MultiDocWorkspaceView.qml](imtdocgui/Views/MultiDocWorkspaceView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `documentManager: DocumentService`
- `popupWidth: int`
- `visualStatusProvider: ObjectVisualStatusProvider`
- `contentColor: color`
- `tabPanelColor: color`
- `tabDelegateDecorator: alias`
- `defaultDocumentIcon: string`

**Сигналы**

- `viewLoaded(string viewId, var view)`

**Функции**

- `startLocalLoading()`
- `stopLocalLoading()`
- `setCurrentTabIndex(index)`
- `openErrorDialog(message)`
- `setDocumentName(documentId, name)`
- `addFixedView(viewComp, name, id, forceFocus, pinned, icon)`

<a id="type-imtdocgui-MultiDocumentCollectionView"></a>
#### `MultiDocumentCollectionView`

Представление коллекции нескольких документов: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/MultiDocumentCollectionView.qml](imtdocgui/Views/MultiDocumentCollectionView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `showStandardLoading: bool`
- `collectionView: CollectionView`
- `documentManager: DocumentServiceBase`
- `collectionTabId: string`
- `tabVisible: bool`

**Сигналы**

- `startLoading(string documentId)`
- `stopLoading(string documentId)`

**Функции**

- `updateTabName(documentId)`
- `setCurrentTabIndex(index)`
- `onTryCloseDirtyDocument(documentId, callback)`
- `setCollectionViewComp(name, collectionViewComp)`

<a id="type-imtdocgui-SearchPageSingleDocumentView"></a>
#### `SearchPageSingleDocumentView`

Визуальное представление одиночного документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/SearchPageSingleDocumentView.qml](imtdocgui/Views/SearchPageSingleDocumentView.qml)
- **Базовый тип:** [`SingleDocumentWorkspacePageView`](#type-imtdocgui-SingleDocumentWorkspacePageView)

<a id="type-imtdocgui-SingleDocumentCollectionView"></a>
#### `SingleDocumentCollectionView`

Представление коллекции одиночного документа: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/SingleDocumentCollectionView.qml](imtdocgui/Views/SingleDocumentCollectionView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `showStandardLoading: bool`
- `collectionView: CollectionView`
- `documentManager: DocumentServiceBase`
- `reopenAfterCloseDocumentId: string`
- `reopenAfterCloseTypeId: string`
- `reopenAfterCloseObjectId: string`

**Сигналы**

- `startLoading(string documentId)`
- `stopLoading(string documentId)`

**Функции**

- `setCollectionViewComp(name, collectionViewComp)`

<a id="type-imtdocgui-SingleDocumentWorkspaceContentView"></a>
#### `SingleDocumentWorkspaceContentView`

Визуальное представление одиночного документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/SingleDocumentWorkspaceContentView.qml](imtdocgui/Views/SingleDocumentWorkspaceContentView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `documentManager: DocumentServiceBase`
- `documentId: string`
- `documentTypeId: string`
- `showStandardLoading: bool`

**Сигналы**

- `viewLoaded(string documentId, var view)`
- `viewUnloaded(string documentId)`
- `startLoading(string documentId)`
- `stopLoading(string documentId)`

<a id="type-imtdocgui-SingleDocumentWorkspaceShellView"></a>
#### `SingleDocumentWorkspaceShellView`

Визуальное представление одиночного документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/SingleDocumentWorkspaceShellView.qml](imtdocgui/Views/SingleDocumentWorkspaceShellView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `documentManager: DocumentServiceBase`
- `objectId: string`
- `objectTypeId: string`
- `createNew: bool`
- `proposedSourceDocumentId: string`
- `showStandardLoading: bool`
- `headerVisible: bool`
- `documentNameInputEnabled: bool`
- `documentNameResolver: var`
- `documentId: alias` — только чтение
- `state: alias` — только чтение
- `lastErrorMessage: alias` — только чтение

**Сигналы**

- `documentReady(string documentId)`
- `closed(string documentId)`

**Функции**

- `retry()`
- `closeDocument()`

<a id="type-imtdocgui-SingleDocumentWorkspaceView"></a>
#### `SingleDocumentWorkspaceView`

Визуальное представление одиночного документа: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/SingleDocumentWorkspaceView.qml](imtdocgui/Views/SingleDocumentWorkspaceView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `titleSeparation: string`
- `initialItemTitleVisible: bool`
- `documentManager: DocumentService`
- `visualStatusProvider: ObjectVisualStatusProvider`

**Функции**

- `openErrorDialog(message)`
- `addInitialItem(viewComp, name)`
- `addPage(viewComp, name)`

### Panels/ — Панели

<a id="type-imtdocgui-DocumentHistoryPanel"></a>
#### `DocumentHistoryPanel`

Панель документа: область интерфейса (список, боковая или нижняя панель).

- **Файл:** [Panels/DocumentHistoryPanel.qml](imtdocgui/Panels/DocumentHistoryPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `opened: bool`
- `contentWidth: int`
- `collapsedWidth: int`
- `contentHeight: alias`
- `documentId: string`
- `collectionId: alias`
- `editorFlickable: Flickable`

**Функции**

- `getHeaders()`

### Dialogs/ — Диалоги

<a id="type-imtdocgui-DocumentHistoryDialog"></a>
#### `DocumentHistoryDialog`

Модальный диалог документа. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/DocumentHistoryDialog.qml](imtdocgui/Dialogs/DocumentHistoryDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `documentId: string`

<a id="type-imtdocgui-DocumentRevisionDialog"></a>
#### `DocumentRevisionDialog`

Модальный диалог документа. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/DocumentRevisionDialog.qml](imtdocgui/Dialogs/DocumentRevisionDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `collectionId: string`
- `documentId: string`
- `selectedRevision: int`
- `currentRevision: int`
- `revisionsModel: BaseModel`

**Сигналы**

- `revisionModelReceived()`
- `receiveRevisionModelStarted()`
- `receiveRevisionModelFailed()`

**Функции**

- `getRevisionList()`
- `getHeaders()`

### Delegates/ — Делегаты

<a id="type-imtdocgui-DocCollectionViewDelegate"></a>
#### `DocCollectionViewDelegate`

Делегат коллекции: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/DocCollectionViewDelegate.qml](imtdocgui/Delegates/DocCollectionViewDelegate.qml)
- **Базовый тип:** [`CollectionViewCommandsDelegateBase`](#type-imtcolgui-CollectionViewCommandsDelegateBase)

**Свойства**

- `documentManager: DocumentServiceBase`
- `documentManagerId: string`
- `documentConfigs: var`

**Функции**

- `registerDocumentType(typeId, typeName)`
- `addDocumentView(typeId, viewTypeId, editorComp, controllerComp)`
- `registerDocumentTypes()`
- `getRegisteredDocumentTypeIds()`
- `getDocumentTypeName(typeId)`
- `onEdit()`
- `onNew()`
- `onRevision()`

<a id="type-imtdocgui-DocumentCollectionViewDelegate"></a>
#### `DocumentCollectionViewDelegate`

Делегат документа: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/DocumentCollectionViewDelegate.qml](imtdocgui/Delegates/DocumentCollectionViewDelegate.qml)
- **Базовый тип:** [`CollectionViewCommandsDelegateBase`](#type-imtcolgui-CollectionViewCommandsDelegateBase)
- **Наследники:** [`ProductCollectionViewCommandsDelegate`](#type-imtlicgui-ProductCollectionViewCommandsDelegate), [`RoleCollectionViewCommandsDelegate`](#type-imtauthgui-RoleCollectionViewCommandsDelegate)

**Свойства**

- `documentManager: DocumentService`
- `documentTypeId: string`
- `viewTypeId: string`
- `documentManagerId: string`
- `documentTypeIds: var`
- `documentViewTypeIds: var`
- `documentViewsComp: var`
- `documentDataControllersComp: var`
- `documentValidatorsComp: var`
- `isSingleDocumentMode: bool`

**Функции**

- `updateDocumentService()`
- `registerDocumentViews()`
- `getDocumentTypeIdIndex(typeId)`
- `getHeaders()`
- `createNewObject(typeId)`
- `openDocumentEditor(objectId, typeId, documentName)`
- `onEdit()`
- `onRevision()`
- `onNew()`

<a id="type-imtdocgui-DocumentWorkspaceCommandsDelegate"></a>
#### `DocumentWorkspaceCommandsDelegate`

Делегат команд документа: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/DocumentWorkspaceCommandsDelegate.qml](imtdocgui/Delegates/DocumentWorkspaceCommandsDelegate.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentPtr: Item`
- `documentHistoryDialogComp: Component`

**Сигналы**

- `commandActivated(string commandId)`

**Функции**

- `commandHandle(commandId)`
- `onClose()`
- `onSave()`
- `onHistory()`

<a id="type-imtdocgui-DocumentWorkspaceCommandsDelegateBase"></a>
#### `DocumentWorkspaceCommandsDelegateBase` · _не экспортируется как тип (загружается по пути/внутренний)_

Визуальный компонент документа.

- **Файл:** [Delegates/DocumentWorkspaceCommandsDelegateBase.qml](imtdocgui/Delegates/DocumentWorkspaceCommandsDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `view: Item`

**Сигналы**

- `commandActivated(string commandId)`

**Функции**

- `commandHandle(commandId)`

### Controllers/ — Контроллеры

<a id="type-imtdocgui-DocumentDataController"></a>
#### `DocumentDataController`

Невизуальный контроллер документа: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/DocumentDataController.qml](imtdocgui/Controllers/DocumentDataController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`GqlDocumentDataController`](#type-imtguigql-GqlDocumentDataController)

**Свойства**

- `documentModel: var`
- `documentId: string`
- `documentName: string`
- `documentDescription: string`
- `typeId: string`
- `hasRemoteChanges: bool`
- `documentModelComp: Component`

**Сигналы**

- `saved(string id, string name)`
- `error(string message, string type)`
- `modelChanged()`

**Функции**

- `getDocumentId()`
- `getDocumentName()`
- `getDocumentTypeId()`
- `getDocumentModel()`
- `getDocumentDescription()`
- `updateDocumentModel()`
- `insertDocument()`
- `saveDocument()`
- `createDocumentModel()`

<a id="type-imtdocgui-DocumentRepresentationController"></a>
#### `DocumentRepresentationController`

Невизуальный контроллер документа: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/DocumentRepresentationController.qml](imtdocgui/Controllers/DocumentRepresentationController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentId: string`
- `representationModel: var`
- `view: ViewBase`

**Сигналы**

- `startUpdateDocument(string documentId)`
- `updateDocumentFailed(string documentId, string message)`
- `documentUpdated(string documentId)`
- `startUpdateRepresentation(string documentId, var representation)`
- `representationUpdated(string documentId, var representation)`
- `updateRepresentationFailed(string documentId, string message)`

**Функции**

- `updateRepresentationFromDocument()`
- `updateDocumentFromRepresentation()`

<a id="type-imtdocgui-UndoRedoManager"></a>
#### `UndoRedoManager`

Менеджер отмены/повтора: управляет состоянием и жизненным циклом связанных объектов.

- **Файл:** [Controllers/UndoRedoManager.qml](imtdocgui/Controllers/UndoRedoManager.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `autoTracking: bool`

**Сигналы**

- `modelChanged()`
- `undo()`
- `redo()`

**Функции**

- `getCurrentStateModel()`
- `getAvailableUndoSteps()`
- `getAvailableRedoSteps()`
- `resetUndo()`
- `doUndo()`
- `doRedo(steps)`
- `doListShift(steps, fromList, toList)`
- `registerModel(model)`
- `unregisterModel()`
- `modelIsRegistered()`
- `isTransaction()`
- `beginChanges()`
- `endChanges()`
- `makeChanges()`
- `setBlockingUpdateModel(blockingUpdateModel)`
- `commandHandle(commandId)`
- `setStandardModel(model)`
- `getStandardModel()`
- `onModelChanged()`

### Providers/ — Провайдеры

<a id="type-imtdocgui-CommandsProvider"></a>
#### `CommandsProvider`

Провайдер данных команд: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/CommandsProvider.qml](imtdocgui/Providers/CommandsProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `commandsModel: TreeItemModel`
- `commandId: string`
- `uuid: string`
- `additionInputParams: var`
- `modelCommands: GqlModel`

**Сигналы**

- `commandModeChanged(string commandId, bool newMode)`
- `commandsModelDataChanged(string commandId, bool newMode)`

**Функции**

- `updateModel()`
- `updateGui()`
- `clearGui()`
- `setCommandIsEnabled(commandId, isEnabled)`
- `setCommandIcon(commandId, icon)`
- `setCommandName(commandId, commandName)`
- `setCommandNotification(commandId, notification)`
- `commandIsEnabled(commandId)`
- `commandExists(commandId)`
- `getCommandIndex(commandId)`
- `mergeModelWith(externModel)`
- `setAdditionalProperties(model)`

### Services/ — Сервисы

<a id="type-imtdocgui-DocumentService"></a>
#### `DocumentService`

Сервис документа: предоставляет операции над сущностью и управляет её жизненным циклом.

- **Файл:** [Services/DocumentService.qml](imtdocgui/Services/DocumentService.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `defaultDocumentName: string`
- `documentsCount: int`
- `activeView: Item`
- `typeId: string`
- `documentsModel: ListModel`
- `singleDocumentDataComp: Component`
- `defaultDataController: Component`
- `internal: QtObject`

**Сигналы**

- `documentClosed(string documentId)`
- `documentAdded(string documentId)`
- `documentSaved(string documentId)`
- `documentSavingStarted(string documentId)`
- `documentSavingFailed(string documentId, string message)`
- `documentIsDirtyChanged(string documentId, bool isDirty)`
- `documentOpeningStarted(string documentId)`
- `documentOpened(string documentId)`
- `documentOpeningFailed(string documentId, string message)`
- `tryCloseDirtyDocument(string documentId, var callback)`
- `documentTypeIdRegistered(string documentTypeId)`

**Функции**

- `getActiveView()`
- `getTypeId()`
- `getDocumentTypeId(documentId)`
- `getSupportedDocumentTypeIds()`
- `getDocumentName(documentId)`
- `registerDocumentValidator(documentTypeId, validatorComp)`
- `registerDocumentDataController(documentTypeId, dataControllerComp)`
- `unRegisterDocumentDataController(documentTypeId)`
- `registerDocumentView(documentTypeId, viewComp)`
- `unRegisterDocumentTypeId(documentTypeId)`
- `getDocumentViewComp(documentTypeId)`
- `getDocumentViewCompByDocumentId(documentId)`
- `getDocumentValidator(documentTypeId)`
- `documentIsRegistered(documentTypeId)`
- `dataControllerIsRegistered(documentTypeId)`
- `getDocumentDataController(documentTypeId)`
- `insertNewDocument(documentTypeId, name)`
- `getDocumentData(index)`
- `getDocumentDataById(documentId)`
- `createTemplateDocument(documentId, documentTypeId)`
- `openDocument(documentId, documentTypeId, name)`
- `addDocumentToModel(documentId, name, documentTypeId, documentData)`
- `saveDocument(documentId)`
- `onDocumentSaved(documentId)`
- `getDocumentIndexByDocumentId(documentId)`
- `getDocumentDataByView(view)`
- `documentIsNew(documentId)`
- `documentIsDirty(documentId)`
- `saveDirtyDocuments(beQuiet, ignoredPtr)`
- `closeDocumentByIndex(documentIndex, force)`
- `closeDocument(documentId, force)`
- `documentIsValid(documentData, data)`
- `closeAllDocuments()`
- `getOpenedDocumentIds()`
- `setupDocumentView(documentId, view)`
- `setBlockUndoManager(documentId, isBlock)`
- `clearUndoManager(documentId)`

<a id="type-imtdocgui-DocumentServiceBase"></a>
#### `DocumentServiceBase`

Базовый сервис документа — общая основа, от которой наследуются конкретные сервисы.

- **Файл:** [Services/DocumentServiceBase.qml](imtdocgui/Services/DocumentServiceBase.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`GqlBasedCollectionDocumentService`](#type-imtguigql-GqlBasedCollectionDocumentService)

**Свойства**

- `getOpenedDocumentListOverride: var`
- `openDocumentOverride: var`
- `createDocumentOverride: var`
- `saveDocumentOverride: var`
- `closeDocumentOverride: var`
- `doUndoOverride: var`
- `doRedoOverride: var`
- `getUndoInfoOverride: var`

**Сигналы**

- `startGetOpenedDocumentList()`
- `openedDocumentListReceived(var documentListInfo)`
- `openedDocumentListReceiveFailed(string message)`
- `startOpenDocument(string documentId, string typeId)`
- `documentOpened(string documentId, string typeId)`
- `openDocumentFailed(string documentId, string message)`
- `startCloseDocument(string documentId)`
- `documentClosed(string documentId)`
- `closeDocumentFailed(string documentId, string message)`
- `startSaveDocument(string documentId)`
- `saveDocumentFailed(string documentId, string message)`
- `documentSaved(string documentId)`
- `startCreateDocument(string typeId)`
- `createDocumentFailed(string typeId, string message)`
- `documentCreated(string documentId, string typeId)`
- `startUndo(string documentId, int steps)`
- `undoDone(string documentId)`
- `undoFailed(string documentId, string message)`
- `startRedo(string documentId, int steps)`
- `redoDone(string documentId)`
- `redoFailed(string documentId, string message)`
- `startResetUndo(string documentId)`
- `resetUndoDone(string documentId)`
- `resetUndoFailed(string documentId, string message)`
- `requestDocumentName(string documentId, string typeId)`
- `documentNameChanged(string documentId, string oldName, string newName)`
- `documentIsDirtyChanged(string documentId, bool isDirty)`
- `startUndoInfoReceive(string documentId)`
- `undoInfoReceived(string documentId, int availableUndoSteps, int availableRedoSteps, bool isDirty)`
- `undoInfoReceiveFailed(string documentId, string message)`
- `documentManagerChanged(string typeOperation, string objectId, string documentId, string documentName)`
- `documentDataLoaded(string documentId)`
- `startUpdateRepresentation(string documentId, var representation)`
- `documentRepresentationUpdated(string documentId, var representation)`
- `updateRepresentationFailed(string documentId, string message)`
- `updateDocumentFailed(string documentId, string message)`
- `documentGuiUpdated(string documentId, var representation)`
- `tryCloseDirtyDocument(string documentId, var callback)`
- `documentViewRegistered(string typeId, string viewTypeId)`
- `documentAlreadyOpened(string documentId, string typeId)`
- `documentReady(string documentId)`

**Функции**

- `openDocument(objectTypeId, objectId)`
- `createDocument(typeId, proposedSourceDocumentId)`
- `saveDocument(documentId)`
- `setDocumentSaveNameResolver(documentId, resolver)`
- `clearDocumentSaveNameResolver(documentId)`
- `resolveDocumentNameForSave(documentId, fallbackName)`
- `closeDocument(documentId)`
- `doUndo(documentId, steps)`
- `doRedo(documentId, steps)`
- `resetUndo(documentId)`
- `getDefaultDocumentName()`
- `getUndoInfo(documentId)`
- `getOpenedDocumentList()`
- `getOpenedDocumentIds()`
- `registerDocumentViewData(documentTypeId, viewTypeId, viewEditorComp, representationControllerComp)`
- `getDocumentEditorFactory(documentTypeId, viewTypeId)`
- `getDocumentRepresentationControllerFactory(documentTypeId, viewTypeId)`
- `documentIsNew(documentId)`
- `setDocumentIsNew(documentId, isNew)`
- `getSupportedDocumentTypeIds()`
- `getDocumentIdByView(view)`
- `getSupportedDocumentViewTypeIds(documentTypeId)`
- `getDocumentTypeId(documentId)`
- `documentIsOpened(documentId)`
- `setDocumentObjectId(documentId, objectId)`
- `getDocumentIdByObjectId(objectId)`
- `getDocumentObjectId(documentId)`
- `getDocumentIndexByDocumentId(documentId)`
- `setDocumentName(documentId, name)`
- `getDocumentName(documentId)`
- `documentIsDirty(documentId)`
- `documentIsLoading(documentId)`
- `setDocumentIsLoading(documentId, isLoading)`
- `setDocumentIsDirty(documentId, isDirty)`
- `getViewTypeIdByViewFactory(documentTypeId, viewFactory)`
- `getDocumentViewInstance(documentId, viewTypeId)`
- `onViewInstanceCreated(documentId, view, viewTypeId)`
- `getDocumentServiceActiveView()`
- `setDocumentServiceActiveView(view)`
- `setAutoNamedTypeId(typeId, hasProvider)`
- `hasDocumentNameProvider(typeId)`
- `handleDocumentOpened(documentId, objectId, objectTypeId, documentName, hasNameProvider, isDirty)`
- `handleDocumentCreated(documentId, objectTypeId, documentName, hasNameProvider, proposedObjectId, isDirty)`
- `handleSaveDocumentResult(documentId, status, message, documentName)`
- `handleCloseDocumentResult(documentId, status)`
- `handleUndoResult(documentId, status)`
- `handleRedoResult(documentId, status)`

<a id="type-imtdocgui-MainDocumentService"></a>
#### `MainDocumentService` · **singleton**

Сервис документа: предоставляет операции над сущностью и управляет её жизненным циклом.

- **Файл:** [Services/MainDocumentService.qml](imtdocgui/Services/MainDocumentService.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentManagers: var`
- `documents: var`

**Сигналы**

- `documentOpened(string typeId, string documentId, string documentTypeId)`
- `tryRegisterDocumentService(string typeId, var callback)`

**Функции**

- `clear()`
- `getDocumentService(typeId)`
- `getDocumentDataByView(viewRef)`
- `unregisterDocumentService(documentManager)`
- `registerDocumentService(typeId, documentManager)`
- `openDocument(typeId, documentId, documentTypeId)`
- `saveDirtyDocuments()`
- `dirtyDocumentsExists()`
- `closeAllDocuments()`
- `closeDocument()`

### Models/ — Модели и данные

<a id="type-imtdocgui-DocumentDecorator"></a>
#### `DocumentDecorator`

Декоратор документа: расширяет и настраивает поведение базового компонента.

- **Файл:** [Models/DocumentDecorator.qml](imtdocgui/Models/DocumentDecorator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentId: string`
- `documentName: string`
- `documentTypeId: string`
- `documentManager: var`
- `registeredViews: var`
- `registeredRepresentation: var`
- `documentManagerConnections: Connections`

**Сигналы**

- `viewRegistered(var view, var representationController, bool updateRepresentation)`

**Функции**

- `onCommandActivated(commandId)`
- `onStartUpdateRepresentation(documentId, representation)`
- `onRepresentationUpdated(documentId, representation)`
- `onUpdateRepresentationFailed(documentId, message)`
- `onUpdateDocumentFailed(documentId, message)`
- `onGuiUpdated(view, model)`
- `onGuiVisibleChanged(view, visible)`
- `onModelDataChanged(view, model)`
- `registerView(view, representationController, updateRepr)`
- `onUndo()`
- `onRedo()`
- `onSave()`
- `updateRepresentationForAllViews()`
- `updateDocumentForAllViews()`

<a id="type-imtdocgui-DocumentValidator"></a>
#### `DocumentValidator`

Валидатор документа: проверяет корректность введённых данных.

- **Файл:** [Models/DocumentValidator.qml](imtdocgui/Models/DocumentValidator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentModel: var`

**Функции**

- `isValid(data)`

<a id="type-imtdocgui-SingleDocumentTypeRegistrar"></a>
#### `SingleDocumentTypeRegistrar`

Регистратор одиночного документа: регистрирует типы/обработчики в системе.

- **Файл:** [Models/SingleDocumentTypeRegistrar.qml](imtdocgui/Models/SingleDocumentTypeRegistrar.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentManager: var`
- `views: var`

**Функции**

- `registerAll()`

---

<a id="imtcolgui"></a>
## `imtcolgui` — Коллекции и представления коллекций

Базовые строительные блоки для коллекций: провайдеры и контроллеры данных, структура и фильтры коллекции, табличные и сеточные представления, делегаты элементов, а также блок метаданных (MetaInfo).

**Импорт:** `import imtcolgui 1.0` · **Файлов:** 33

### Pages/ — Страницы

<a id="type-imtcolgui-CollectionWorkspacePageView"></a>
#### `CollectionWorkspacePageView`

Страница-контейнер рабочей области: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/CollectionWorkspacePageView.qml](imtcolgui/Pages/CollectionWorkspacePageView.qml)
- **Базовый тип:** [`DocumentWorkspacePageView`](#type-imtdocgui-DocumentWorkspacePageView)

**Функции**

- `setDocumentName(documentId, name)`

### Views/ — Представления

<a id="type-imtcolgui-CollectionView"></a>
#### `CollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/CollectionView.qml](imtcolgui/Views/CollectionView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `viewId: alias`
- `collectionId: alias`
- `commandsViewComp: alias`
- `commandsView: alias`
- `commandsControllerComp: alias`
- `commandsController: alias`
- `dataControllerComp: alias`
- `dataController: alias`
- `commandsDelegateComp: alias`
- `commandsDelegate: alias`
- `collectionFilter: alias`
- `documentCollectionFilter: alias`
- `table: alias`
- `readOnly: alias`
- `filterMenu: alias`
- `filterMenuVisible: alias`
- `hasFilter: alias`
- `hasSort: alias`
- `hasPagination: alias`
- `elementsCount: alias`
- `pagination: alias`
- `metaInfoView: alias`
- `tableRowDelegate: alias`
- `scrollbarItem: alias`
- `scrollbarItemHoriz: alias`
- `contentColor: alias`
- `visibleMetaInfo: bool`
- `headerRightClickEnabled: bool`
- `commandsPanelVisible: bool`
- `loadingDataAfterHeadersReceived: bool`
- `backgroundUpdatesEnabled: bool`
- `loadingIndicatorDelay: int`
- `canResetFilters: alias`
- `metaInfoWidth: int`
- `contentHeight: alias`

**Сигналы**

- `selectedIndexChanged(int index)`
- `elementsChanged()`
- `headersChanged()`
- `tableDecoratorChanged()`
- `selectionChanged(var selectedIds, var selectedIndexes)`
- `checkedItemsChanged()`
- `rightButtonMouseClicked(int mouseX, int mouseY)`
- `tableViewParamsAccepted()`
- `commandActivated(string commandId)`

**Функции**

- `registerFilter(filterId, filterModel)`
- `setFilterIsEnabled(filterId, enabled)`
- `clearAllFilters(beQuiet)`
- `doubleClicked(id, index)`
- `setAlertPanel(alertPanelComp)`
- `doUpdateGui()`
- `doUpdateModel()`
- `removeElements(elementIds)`
- `removeElementSet(filter)`
- `restoreObjects(elementIds)`
- `restoreObjectSet(filter)`
- `duplicateElements(elementIds, name)`
- `setElementName(elementId, name)`
- `setElementDescription(elementId, description)`
- `setElementEnabled(elementId, enabled)`
- `onEdit(id, index)`
- `getSelectedIds()`
- `hasActiveFilter()`
- `registerFieldFilterDelegate(filterId, filterDelegateComp)`
- `unregisterFieldFilterDelegate(filterId)`
- `setFilterDependency(filterId, dependsOnFilterId)`
- `registerMetaInfoViewDelegate(metaInfoId, viewComp)`

<a id="type-imtcolgui-CollectionViewBase"></a>
#### `CollectionViewBase`

Базовое представление коллекции — общий каркас, от которого наследуются конкретные представления.

- **Файл:** [Views/CollectionViewBase.qml](imtcolgui/Views/CollectionViewBase.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `collectionId: string`
- `table: alias`
- `hasPagination: bool`
- `hasFilter: bool`
- `hasSort: bool`
- `canResetFilters: bool`
- `headerRightClickEnabled: bool`
- `isMultiCheckable: alias`
- `filterMenu: alias`
- `loading: alias`
- `filterMenuVisible: bool`
- `pagination: alias`
- `elementsCount: alias`
- `tableRowDelegate: alias`
- `scrollbarItem: alias`
- `scrollbarItemHoriz: alias`
- `collectionFilter: CollectionFilter`
- `documentCollectionFilter: DocCollectionFilter`
- `selectionManager: IdSelectionManager`
- `dataControllerComp: Component`
- `dataController: var`
- `activeFilter: bool`
- `contentHeight: int`

**Сигналы**

- `selectedIndexChanged(int index)`
- `tableViewParamsAccepted()`
- `elementsChanged()`
- `headersChanged()`
- `tableDecoratorChanged()`
- `filterChanged(string filterId, var filterValue)`
- `selectionChanged(var ids, var indexes)`
- `checkedItemsChanged()`
- `rightButtonMouseClicked(int mouseX, int mouseY)`
- `doubleClicked(string id, int index)`

**Функции**

- `onModelChanged()`
- `getSelectedIds()`
- `hasActiveFilter()`
- `registerFieldFilterDelegate(filterId, filterDelegateComp)`
- `setFilterDependency(filterId, dependsOnFilterId)`

<a id="type-imtcolgui-GridCollectionView"></a>
#### `GridCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/GridCollectionView.qml](imtcolgui/Views/GridCollectionView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `uuid: string`
- `documentManagerPtr: var`
- `documentName: string`
- `contentMargins: int`
- `baseGridCollectionView: alias`
- `itemName: string`
- `isUsedDocumentService: bool`
- `visibleMetaInfo: bool`
- `hasMetaInfo: bool`
- `editorPath: string`
- `commandsDelegatePath: string`
- `commandUpdateGui: string`
- `commandsDelegate: alias`
- `commandsId: alias`
- `commands: alias`
- `gridCellWidth: alias`
- `gridCellHeight: alias`
- `gridContentHeight: alias`
- `gridContentY: alias`
- `gridBottomMargin: alias`
- `hasExtention: alias`
- `openST: alias`
- `grid: alias`
- `gridCount: alias`
- `gridCountInLine: alias`
- `gridRowCount: alias`
- `gridSelectedRow: alias`
- `gridIsLastRow: alias`
- `gridAddHeight: alias`
- `gridCellHeightMin: alias`
- `gridCellHeightMax: alias`
- `gridCacheBuffer: alias`
- `extendingInfoComp: alias`
- `gridDelegateMargin: alias`
- `gridIndicatorMainColor: alias`
- `gridIndicatorCoverColor: alias`
- `gridIndicatorCoverOpacity: alias`
- `gridIndicatorVisible: alias`
- `gridElementsDelegate: alias`
- `gridElementsModel: alias`
- `selectedIndex: int`
- `gridDecoratorPath: alias`
- `metaInfo: alias`
- `paginationCurrentIndex: int`
- `paginationPageSize: alias`
- `paginationCountAllElements: alias`
- `paginationCountElements: int`
- `hasPagination: alias`
- `backgroundColor: string`

**Функции**

- `updateGui()`
- `selectItem(id, name)`
- `findIndexById(id, nameId)`

<a id="type-imtcolgui-GridCollectionViewBase"></a>
#### `GridCollectionViewBase`

Базовое представление коллекции — общий каркас, от которого наследуются конкретные представления.

- **Файл:** [Views/GridCollectionViewBase.qml](imtcolgui/Views/GridCollectionViewBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commands: alias`
- `isWeb: bool`
- `hasExtention: bool`
- `openST: bool`
- `gridElementsDelegate: alias`
- `gridElementsModel: alias`
- `gridCellWidth: alias`
- `gridCellHeight: alias`
- `gridContentHeight: alias`
- `gridCacheBuffer: alias`
- `gridContentY: alias`
- `gridBottomMargin: alias`
- `gridCount: alias`
- `gridCountInLine: int`
- `gridRowCount: int`
- `gridSelectedRow: int`
- `gridIsLastRow: bool`
- `gridSelectedIndexInRow: int`
- `gridAddHeight: real`
- `gridCellHeightMin: int`
- `gridCellHeightMax: int`
- `gridDelegateMargin: int`
- `gridIndicatorMainColor: string`
- `gridIndicatorCoverColor: alias`
- `gridIndicatorCoverOpacity: alias`
- `gridIndicatorVisible: bool`
- `extendingInfoComp: alias`
- `gridDecoratorPath: alias`
- `modelFilter: alias`
- `pagination: alias`
- `paginationCurrentIndex: int`
- `paginationPageSize: alias`
- `paginationCountAllElements: alias`
- `paginationCountElements: int`
- `commandsId: string`
- `grid: alias`
- `selectedIndex: int`
- `hasPagination: bool`
- `backgroundColor: string`

**Сигналы**

- `selectedItem(string id, string name)`
- `selectedIndexChangedSignal(int index)`
- `elementsChanged()`

<a id="type-imtcolgui-ItemSelectElementView"></a>
#### `ItemSelectElementView`

Представление одного поля/элемента выбора внутри формы или строки.

- **Файл:** [Views/ItemSelectElementView.qml](imtcolgui/Views/ItemSelectElementView.qml)
- **Базовый тип:** [`ElementView`](imtgui/View/ElementView.qml)
- **Наследники:** [`GqlBasedItemSelectElementView`](#type-imtguigql-GqlBasedItemSelectElementView)

**Свойства**

- `items: var`
- `editable: bool`
- `label: string`
- `addButtonText: string`
- `filterPlaceholder: string`
- `dataProvider: var`
- `emptyText: string`
- `showCount: bool`
- `nonRemovableIds: var`
- `accentColor: string` — только чтение
- `accentBgLight: string` — только чтение
- `accentBorderLight: string` — только чтение

**Сигналы**

- `itemRemoved(int index, var itemData)`
- `selectionChanged(var selectedItems)`
- `popupClosed()`

<a id="type-imtcolgui-MessageCollectionView"></a>
#### `MessageCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/MessageCollectionView.qml](imtcolgui/Views/MessageCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

**Свойства**

- `filterRightMargin: int`
- `clearLogEnabled: bool`
- `clearLogCommandId: string`
- `clearLogInputComp: Component`
- `clearLogPayloadComp: Component`

**Функции**

- `getHeaders()`
- `requestClearLog()`

<a id="type-imtcolgui-SchemeView"></a>
#### `SchemeView`

Визуальное представление схемы: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/SchemeView.qml](imtcolgui/Views/SchemeView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `objectsModel: BaseModel`
- `selectedIndex: int`
- `contentHeight: real`
- `contentY: real`
- `originY: real`
- `contentWidth: real`
- `contentX: real`
- `originX: real`
- `scaleStep: real`
- `maximumObjectWidth: real`
- `objectFontSize: int`
- `objectSecondaryFontSize: int`
- `strokeSecondaryText: bool`
- `autoFit: bool`

**Сигналы**

- `copySignal(int index)`
- `pasteSignal(int index)`
- `deleteSignal(int index)`
- `renameSignal(int index)`
- `revertSignal()`
- `modelDataChanged()`
- `objectMoveFinished()`

**Функции**

- `designSchemeChanged(scheme)`
- `appSizeChanged(params)`
- `setAutoFit(autoFit)`
- `zoomIn()`
- `zoomOut()`
- `resetZoom()`
- `zoomToFit()`
- `requestPaint()`
- `findModelIndex(id)`
- `goInside()`
- `correctPosition(addX, addY)`
- `correctPositionScaled(addX, addY, scale_)`
- `copyObjectFunc(index)`
- `pasteObjectFunc()`
- `deleteObjectFunc(index)`

### Dialogs/ — Диалоги

<a id="type-imtcolgui-FilterableSelectPopup"></a>
#### `FilterableSelectPopup`

Всплывающее окно фильтруемого выбора. Появляется по месту (у элемента) для быстрого выбора или подсказки.

- **Файл:** [Dialogs/FilterableSelectPopup.qml](imtcolgui/Dialogs/FilterableSelectPopup.qml)
- **Базовый тип:** [`PopupView`](imtgui/View/PopupView.qml)

**Свойства**

- `dataProvider: QtObject`
- `debounceInterval: int`
- `idRole: string`
- `textRole: string`
- `itemWidth: int`
- `itemHeight: int`
- `maxVisibleItems: int`
- `textSize: int`
- `fontColor: string`
- `preselectedIds: var`
- `knownItems: var`
- `showCheckBox: bool`
- `showSeparator: bool`
- `filterPlaceholder: string`
- `headerComponent: Component`
- `showSelectedGroup: bool`
- `maxSelectedGroupItems: int`
- `embedded: bool`
- `delegate: Component`

**Сигналы**

- `itemSelected(string itemId, int index)`
- `selectionChanged(var selectedIds)`
- `requestClose()`

**Функции**

- `getItemId(index)`
- `getItemText(index)`
- `getItem(index)`
- `started()`
- `closePopup()`
- `handleItemClick(itemId, index)`
- `contentYCorrection(down_)`

### Delegates/ — Делегаты

<a id="type-imtcolgui-CollectionStructureCommandsDelegateBase"></a>
#### `CollectionStructureCommandsDelegateBase`

Визуальный компонент коллекции.

- **Файл:** [Delegates/CollectionStructureCommandsDelegateBase.qml](imtcolgui/Delegates/CollectionStructureCommandsDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `treeView: TreeViewGql`
- `collectionStructureBase: Item`
- `documentManagerPtr: Item`
- `commandsProvider: CommandsProvider`
- `commandId: string`
- `descriptionFieldId: string`
- `contextMenuModel: var`
- `autoUpdateAfterChanges: bool`
- `removeGqlModel: alias`
- `renameGqlModel: alias`
- `setDescriptionGqlModel: alias`
- `setDescriptionDialogComp: alias`
- `removeDialogTitle: string`
- `removeMessage: string`
- `removeNodeGqlCommand: string`
- `removeObjectGqlCommand: string`
- `renameGqlCommand: string`
- `newNodeGqlCommand: string`
- `setDescriptionGqlCommand: string`
- `renameDialog: Component`
- `newNodeDialog: Component`

**Сигналы**

- `selectionChanged()`
- `commandActivated(string commandId)`
- `renamed(string id, string newName)`
- `descriptionSetted(string id, string description)`
- `removed(string id)`

**Функции**

- `test()`
- `openPopupMenu(x, y)`
- `onEdit()`
- `onRemove()`
- `onNewDocument()`
- `onNewNode()`
- `onRename()`
- `onSetDescription()`
- `commandHandle(commandId)`

<a id="type-imtcolgui-CollectionViewCommandsDelegateBase"></a>
#### `CollectionViewCommandsDelegateBase`

Вспомогательный объект коллекции.

- **Файл:** [Delegates/CollectionViewCommandsDelegateBase.qml](imtcolgui/Delegates/CollectionViewCommandsDelegateBase.qml)
- **Базовый тип:** [`ViewCommandsDelegateBase`](imtgui/View/ViewCommandsDelegateBase.qml)
- **Наследники:** [`DocCollectionViewDelegate`](#type-imtdocgui-DocCollectionViewDelegate), [`DocumentCollectionViewDelegate`](#type-imtdocgui-DocumentCollectionViewDelegate), [`PackageCollectionViewCommandsDelegate`](#type-imtlicgui-PackageCollectionViewCommandsDelegate)

**Свойства**

- `collectionView: Item`
- `descriptionFieldId: string`
- `collectionId: string`
- `removeDialogTitle: string`
- `removeMessage: string`
- `contextMenuModel: TreeItemModel`
- `loadingInParts: bool`
- `renameDialogComp: alias`
- `removeDialogComp: alias`
- `importFileDialog: alias`
- `exportFileDialog: alias`
- `setDescriptionDialogComp: alias`
- `exportContextMenuWidth: int`
- `importObjectTypeId: string`
- `canRename: bool`
- `canSetDescription: bool`
- `showingDisableObjects: bool`
- `importDialogMimeTypes: var`
- `exportDialogMimeTypes: var`
- `exportContextMenuItems: var`

**Сигналы**

- `renamed(string id, string newName)`
- `descriptionSetted(string id, string description)`
- `removed(string id)`
- `selectionChanged(var selectedIds, var selectedIndexes)`
- `beginImport(string fileName, string filePath, int fileSize)`
- `exportFinished()`

**Функции**

- `setupExtensionsModel()`
- `getContextMenuModel()`
- `updateBaseCommandsAccent()`
- `updateCustomCommandsAccent()`
- `updateItemSelection(selectedItems)`
- `updateStateBaseCommands(selection, commandsController, elementsModel)`
- `updateStateCustomCommands(selection, commandsController, elementsModel)`
- `setupContextMenu()`
- `onEdit()`
- `onImport()`
- `onExport(params)`
- `onRevision()`
- `onRestore(params)`
- `onRestoreAll(params)`
- `onDuplicate(params)`
- `onImportDialogResult(name, fileData, mimeType)`
- `onExportDialogResult(objectId, mimeType)`
- `getDialogNameFilterIndex(nameFilters, fileExt)`
- `getExtensionFromNameFilter(nameFilters, index)`
- `getFormatFromMimeType(mimeType)`
- `onRemove(params)`
- `onRemoveAll()`
- `onNew()`
- `onRename()`
- `onSetDescription()`
- `commandHandle(commandId, params)`

<a id="type-imtcolgui-CollectionViewDelegate"></a>
#### `CollectionViewDelegate`

Делегат коллекции: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/CollectionViewDelegate.qml](imtcolgui/Delegates/CollectionViewDelegate.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `collectionView: CollectionView`

**Функции**

- `removeObject(objectIds)`
- `renameObject(objectId, newName)`
- `setDescriptionObject(objectId, description)`
- `updateObject(objectId)`
- `openDocumentEditor(objectId)`
- `setupContextMenu(menu)`
- `createNewObject(typeId, objectName, description)`
- `updateItemSelection(selectedItems, selectedTypeId)`

<a id="type-imtcolgui-GridExtendingBaseDelegate"></a>
#### `GridExtendingBaseDelegate`

Делегат сетки: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/GridExtendingBaseDelegate.qml](imtcolgui/Delegates/GridExtendingBaseDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `rootItem: Item`
- `minHeight: int`
- `selected: bool`
- `rowNumber: int`
- `belowSelectedRow: bool`
- `inLastRow: bool`

**Функции**

- `setOpenST()`

<a id="type-imtcolgui-MetaInfoDateDelegate"></a>
#### `MetaInfoDateDelegate`

Делегат метаданных: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/MetaInfoDateDelegate.qml](imtcolgui/Delegates/MetaInfoDateDelegate.qml)
- **Базовый тип:** [`MetaInfoViewDelegateBase`](#type-imtcolgui-MetaInfoViewDelegateBase)

<a id="type-imtcolgui-MetaInfoStringListDelegate"></a>
#### `MetaInfoStringListDelegate`

Делегат метаданных: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/MetaInfoStringListDelegate.qml](imtcolgui/Delegates/MetaInfoStringListDelegate.qml)
- **Базовый тип:** [`MetaInfoViewDelegateBase`](#type-imtcolgui-MetaInfoViewDelegateBase)

<a id="type-imtcolgui-MetaInfoTextDelegate"></a>
#### `MetaInfoTextDelegate`

Делегат метаданных: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/MetaInfoTextDelegate.qml](imtcolgui/Delegates/MetaInfoTextDelegate.qml)
- **Базовый тип:** [`MetaInfoViewDelegateBase`](#type-imtcolgui-MetaInfoViewDelegateBase)

<a id="type-imtcolgui-MetaInfoViewDelegateBase"></a>
#### `MetaInfoViewDelegateBase`

Базовый делегат метаданных — основа для конкретных делегатов элементов.

- **Файл:** [Delegates/MetaInfoViewDelegateBase.qml](imtcolgui/Delegates/MetaInfoViewDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`MetaInfoDateDelegate`](#type-imtcolgui-MetaInfoDateDelegate), [`MetaInfoStringListDelegate`](#type-imtcolgui-MetaInfoStringListDelegate), [`MetaInfoTextDelegate`](#type-imtcolgui-MetaInfoTextDelegate)

**Свойства**

- `metaInfoView: var`
- `viewData: string`

### Controllers/ — Контроллеры

<a id="type-imtcolgui-CollectionCommandsController"></a>
#### `CollectionCommandsController`

Невизуальный контроллер коллекции: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/CollectionCommandsController.qml](imtcolgui/Controllers/CollectionCommandsController.qml)
- **Базовый тип:** [`CommandsController`](imtcontrols/Models/CommandsController.qml)

**Свойства**

- `contextMenuCommandsModel: TreeItemModel`

<a id="type-imtcolgui-CollectionDataController"></a>
#### `CollectionDataController`

Невизуальный контроллер коллекции: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/CollectionDataController.qml](imtcolgui/Controllers/CollectionDataController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`GqlBasedCollectionDataController`](#type-imtguigql-GqlBasedCollectionDataController)

**Свойства**

- `collectionId: string`

**Сигналы**

- `collectionHeadersReceived(var headersModel)`
- `collectionHeadersReceiveFailed(var headersModel)`
- `objectReceived(var object)`
- `objectReceiveFailed(string message)`
- `objectAdded(string id)`
- `objectAddFailed(string message)`
- `objectUpdated(string id)`
- `objectUpdateFailed(string message)`
- `objectsRemoved(var ids)`
- `removeObjectsFailed(string message)`
- `objectsRestored(var ids)`
- `restoreObjectsFailed(string message)`
- `listObjectsReceived(var listObjects)`
- `listObjectsReceiveFailed(string message)`
- `objectRenamed(string id, string newName)`
- `objectRenameFailed(string message)`
- `objectDescriptionSetted(string id, string description)`
- `objectSetDescriptionFailed(string message)`
- `subCollectionCreated(var subCollection)`
- `subCollectionCreateFailed(string message)`

**Функции**

- `getCollectionHeaders()`
- `getObjectData(objectId)`
- `insertNewObject(typeId, name, description, objectData, objectId)`
- `setObjectData(objectId, newObjectData)`
- `getElements(count, offset, filter)`
- `removeObjects(objectIds)`
- `removeObjectSet(filter)`
- `restoreObjects(objectIds)`
- `restoreObjectSet(filter)`
- `setObjectName(objectId, name)`
- `setObjectDescription(objectId, description)`
- `createSubCollection(offset, count, selectionParams)`

<a id="type-imtcolgui-CollectionStructureCommandsObserver"></a>
#### `CollectionStructureCommandsObserver`

Наблюдатель коллекции: отслеживает изменения и уведомляет подписчиков.

- **Файл:** [Controllers/CollectionStructureCommandsObserver.qml](imtcolgui/Controllers/CollectionStructureCommandsObserver.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commandsProvider: var`

**Функции**

- `commandsModelLoaded()`

### Providers/ — Провайдеры

<a id="type-imtcolgui-CollectionDataProvider"></a>
#### `CollectionDataProvider`

Провайдер данных коллекции: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/CollectionDataProvider.qml](imtcolgui/Providers/CollectionDataProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`CachedAccountCollection`](#type-imtauthgui-CachedAccountCollection), [`CachedFeatureCollection`](#type-imtlicgui-CachedFeatureCollection), [`CachedLicenseCollection`](#type-imtlicgui-CachedLicenseCollection), [`CachedProductCollection`](#type-imtlicgui-CachedProductCollection), [`GroupCollectionDataProvider`](#type-imtauthgui-GroupCollectionDataProvider), [`RoleCollectionDataProvider`](#type-imtauthgui-RoleCollectionDataProvider), [`UserCollectionDataProvider`](#type-imtauthgui-UserCollectionDataProvider)

**Свойства**

- `collectionModel: TreeItemModel`
- `notificationModel: TreeItemModel`
- `commandId: string`
- `subscriptionCommandId: string`
- `completed: bool`
- `fields: var`
- `textFilteringInfoIds: var`
- `distinctSelectionFields: var`
- `orderType: string`
- `sortByField: string`
- `state: alias`
- `offset: int`
- `count: int`
- `filter: CollectionFilter`
- `itemsInfoModel: GqlRequestSender`
- `subscriptionClient: SubscriptionClient`

**Сигналы**

- `modelUpdated(var data)`
- `failed(string message)`

**Функции**

- `updateModel(offset)`
- `clearModel()`
- `setCustomInputParams(inputParams)`
- `getData(objectId, value)`
- `getHeaders()`

<a id="type-imtcolgui-CollectionStructureDataProvider"></a>
#### `CollectionStructureDataProvider`

Провайдер данных коллекции: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/CollectionStructureDataProvider.qml](imtcolgui/Providers/CollectionStructureDataProvider.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `collectionModel: TreeItemModel`
- `commandId: string`
- `completed: bool`
- `fields: var`
- `stateModel: alias`
- `offset: int`
- `count: int`
- `filterModel: TreeItemModel`
- `itemsInfoModel: GqlModel`

**Сигналы**

- `modelUpdated()`
- `failed()`

**Функции**

- `updateModel(inputParams)`
- `onModelUpdated()`
- `getData(objectId, value)`

<a id="type-imtcolgui-FilterableSelectDataProvider"></a>
#### `FilterableSelectDataProvider`

Провайдер данных фильтруемого выбора: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/FilterableSelectDataProvider.qml](imtcolgui/Providers/FilterableSelectDataProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`FilterableSelectGqlDataProvider`](#type-imtguigql-FilterableSelectGqlDataProvider)

**Свойства**

- `collectionId: string`
- `pageSize: int`
- `multiSelect: bool`
- `items: var` — только чтение
- `isInitialLoading: bool` — только чтение
- `isPageLoading: bool` — только чтение
- `hasMore: bool` — только чтение
- `error: var` — только чтение

**Сигналы**

- `dataChanged()`
- `errorOccurred(var error)`
- `selectionChanged()`

**Функции**

- `fetch(filter)`
- `refetch()`
- `fetchMore()`
- `retry()`
- `isItemSelected(id)`
- `getSelectedIds()`
- `getSelectedItems()`
- `getSelectedCount()`
- `hasUnresolvedItems()`
- `getSelectedItemText(id)`
- `getSelectedItemTypeId(id)`
- `toggleItem(id, item)`
- `setSingleSelection(id, item)`
- `clearSelection()`
- `setPreselectedIds(ids)`
- `addKnownItem(id, item)`
- `executeRequest(requestId, count, offset, filter)`
- `executeFetchByIds(requestId, ids)`
- `onRequestSuccess(requestId, rawItems)`
- `onRequestError(requestId, message, code)`
- `onFetchByIdsSuccess(requestId, resolvedItems)`
- `onFetchByIdsError(requestId, message)`

### Models/ — Модели и данные

<a id="type-imtcolgui-CollectionFilter"></a>
#### `CollectionFilter`

Фильтр коллекции: набор условий отбора элементов коллекции.

- **Файл:** [Models/CollectionFilter.qml](imtcolgui/Models/CollectionFilter.qml)
- **Базовый тип:** `ComplexCollectionFilter`
- **Наследники:** [`MessageCollectionFilter`](#type-imtcolgui-MessageCollectionFilter)

**Свойства**

- `fieldSortingInfoFact: Component`
- `groupFilterFact: Component`
- `fieldFilterFact: Component`
- `baseModelFact: Component`
- `logicalOperation: var`
- `filterOperation: var`
- `valueType: var`

**Сигналы**

- `filterChanged()`
- `cleared(bool beQiuet)`
- `fieldFilterAdded(string fieldId, var fieldValue)`
- `fieldFilterRemoved(string fieldId)`

**Функции**

- `createBaseModel()`
- `getTextFilter()`
- `setTextFilter(filter)`
- `setSortingInfo(infoId, sortOrder)`
- `getSortingInfo()`
- `setFilteringInfoIds(filteringInfoIds)`
- `createGroupFilter(operation, fields, groups)`
- `addGroupFilter(group)`
- `createFieldFilter(fieldId, value, valueType, operations)`
- `addFieldFilter(field)`
- `removeFieldFilter(field)`
- `removeFilterByFieldId(fieldId)`
- `removeGroupFilter(group)`
- `setTimeFilter(timeFilter)`
- `clearAllFilters(beQiuet)`
- `clearSortFilter()`
- `clearFieldsFilter()`
- `clearTimeFilter()`
- `clearTextFilter()`
- `clearDistinctFilter()`
- `timeFilterIsEmpty()`
- `textFilterIsEmpty()`
- `fieldsFilterIsEmpty()`
- `isEmpty()`

<a id="type-imtcolgui-CollectionStructure"></a>
#### `CollectionStructure`

Структура коллекции: описание состава и иерархии данных.

- **Файл:** [Models/CollectionStructure.qml](imtcolgui/Models/CollectionStructure.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `uuid: string`
- `documentManagerPtr: DocumentService`
- `documentName: string`
- `contentMargins: int`
- `baseCollectionView: alias`
- `isUsedDocumentService: bool`
- `visibleMetaInfo: bool`
- `hasFilter: bool`
- `commandId: string`
- `documentTypeId: string`
- `commandsDelegatePath: string`
- `commandsDelegate: alias`
- `treeView: alias`
- `filterMenu: alias`
- `filterMenuVisible: alias`
- `modelFilter: alias`
- `notificationModel: TreeItemModel`
- `contextMenuModel: ListModel`
- `commandsProvider: alias`
- `hasRemoteChanges: bool`
- `localizationChanged: bool`

**Сигналы**

- `elementsChanged()`
- `headersChanged()`
- `filterDecoratorLoaded()`

**Функции**

- `onLocalizationChanged(language)`
- `getDocumentName()`
- `receiveRemoteChanges()`
- `filterMenuActivate()`
- `fillContextMenuModel()`
- `getHeaders()`
- `updateGui()`
- `updateModel()`
- `selectItem(id, name)`
- `getEditorPath()`
- `getEditorCommandId()`
- `onCommandsModelChanged()`
- `setAlertPanel(alertPanelComp)`

<a id="type-imtcolgui-CollectionStructureBase"></a>
#### `CollectionStructureBase`

Визуальный компонент коллекции.

- **Файл:** [Models/CollectionStructureBase.qml](imtcolgui/Models/CollectionStructureBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commands: alias`
- `commandId: string`
- `tableElementsDelegate: alias`
- `treeView: var`
- `hasFilter: bool`
- `filterMenu: alias`
- `filterMenuItem: alias`
- `filterMenuVisible: alias`
- `modelFilter: alias`

**Сигналы**

- `selectedItem(string id, string name)`
- `selectedIndexChanged(int index)`
- `elementsChanged()`
- `selectionChanged(var selection)`
- `filterDecoratorLoaded()`

**Функции**

- `onLocalizationChanged(language)`
- `onFilterClosed()`
- `onTextFilterChanged(index, text)`
- `updateModels()`
- `updateBranch(nodeId)`

<a id="type-imtcolgui-CollectionStructureBaseGqlModels"></a>
#### `CollectionStructureBaseGqlModels`

Вспомогательный объект коллекции.

- **Файл:** [Models/CollectionStructureBaseGqlModels.qml](imtcolgui/Models/CollectionStructureBaseGqlModels.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `getItemsGqlCommand: string`
- `getObjectViewGqlCommand: string`
- `objectViewEditorPath: string`
- `objectViewEditorCommandsId: string`
- `commandId: string`
- `additionInputParams: var`
- `notificationModel: TreeItemModel`
- `rootItem: Item`
- `fieldsData: var`
- `itemsInfoModel: GqlModel`
- `objectViewModel: GqlModel`

**Сигналы**

- `itemsInfoGqlStateChanged(string state)`
- `itemsReceived(var selectIndex, var items)`

**Функции**

- `updateModels()`
- `updateItemsModel(selectIndex)`

<a id="type-imtcolgui-CollectionViewBaseGqlModels"></a>
#### `CollectionViewBaseGqlModels`

Вспомогательный объект коллекции.

- **Файл:** [Models/CollectionViewBaseGqlModels.qml](imtcolgui/Models/CollectionViewBaseGqlModels.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `getHeadersGqlCommand: string`
- `getItemsGqlCommand: string`
- `getObjectViewGqlCommand: string`
- `objectViewEditorPath: string`
- `objectViewEditorCommandsId: string`
- `commandId: string`
- `table: var`
- `additionInputParams: var`
- `headers: TreeItemModel`
- `items: TreeItemModel`
- `notificationModel: TreeItemModel`
- `rootItem: Item`
- `pagination: Item`
- `fieldsData: var`
- `headerInfoModel: GqlModel`
- `itemsInfoModel: GqlModel`
- `objectViewModel: GqlModel`

**Сигналы**

- `itemsInfoGqlStateChanged(string state)`

**Функции**

- `updateModels()`
- `updateItemsModel()`

<a id="type-imtcolgui-DocCollectionFilter"></a>
#### `DocCollectionFilter`

Фильтр коллекции: набор условий отбора элементов коллекции.

- **Файл:** [Models/DocCollectionFilter.qml](imtcolgui/Models/DocCollectionFilter.qml)
- **Базовый тип:** `Doc.DocumentCollectionFilter`

**Свойства**

- `s_activeState: string` — только чтение
- `s_inactiveState: string` — только чтение
- `s_disabledState: string` — только чтение

**Сигналы**

- `filterChanged()`

**Функции**

- `setDocumentId(documentId)`
- `clear(beQuiet)`
- `addDocumentState(state)`
- `containsState(state)`
- `isEmpty()`

<a id="type-imtcolgui-MessageCollectionFilter"></a>
#### `MessageCollectionFilter`

Фильтр коллекции: набор условий отбора элементов коллекции.

- **Файл:** [Models/MessageCollectionFilter.qml](imtcolgui/Models/MessageCollectionFilter.qml)
- **Базовый тип:** [`CollectionFilter`](#type-imtcolgui-CollectionFilter)

**Свойства**

- `filterableHeadersModel: TreeItemModel`

**Функции**

- `setMessageStatusFilter(messageKey, status)`

<a id="type-imtcolgui-MessageCollectionFilterDecorator"></a>
#### `MessageCollectionFilterDecorator`

Декоратор коллекции: расширяет и настраивает поведение базового компонента.

- **Файл:** [Models/MessageCollectionFilterDecorator.qml](imtcolgui/Models/MessageCollectionFilterDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](imtcontrols/Base/DecoratorBase.qml)

**Свойства**

- `segmentedButton: alias`
- `filtermenu: alias`
- `complexFilter: CollectionFilter`
- `filterRightMargin: int`
- `clearLogVisible: bool`

**Сигналы**

- `clearLogRequested()`

**Функции**

- `checkWidth()`
- `updateFilter()`

<a id="type-imtcolgui-MessageFilterItem"></a>
#### `MessageFilterItem`

Фильтр сообщений: набор условий отбора элементов коллекции.

- **Файл:** [Models/MessageFilterItem.qml](imtcolgui/Models/MessageFilterItem.qml)
- **Базовый тип:** [`Row`](https://doc.qt.io/qt-6/qml-qtquick-row.html)

**Свойства**

- `icon: alias`
- `text: alias`
- `checked: alias`

<a id="type-imtcolgui-MetaInfo"></a>
#### `MetaInfo`

Визуальный компонент метаданных.

- **Файл:** [Models/MetaInfo.qml](imtcolgui/Models/MetaInfo.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `elementMetaInfo: ElementMetaInfo`
- `elementHeight: int`
- `contentVisible: bool`
- `registeredViewDelegates: var`

**Функции**

- `startLoading()`
- `stopLoading()`
- `registerViewDelegate(parameterTypeId, viewComp)`

---

<a id="imtguigql"></a>
## `imtguigql` — GraphQL/SDL-инфраструктура и удалённые данные

Инфраструктурный слой поверх GraphQL и SDL: отправка запросов и подписок, WebSocket-соединения, GraphQL-версии контроллеров, провайдеров и сервисов данных, удалённые коллекции, выбор серверных путей и файлов.

**Импорт:** `import imtguigql 1.0` · **Файлов:** 41

### Pages/ — Страницы

<a id="type-imtguigql-GqlCollectionDocManagerPageView"></a>
#### `GqlCollectionDocManagerPageView`

Страница-контейнер коллекции: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/GqlCollectionDocManagerPageView.qml](imtguigql/Pages/GqlCollectionDocManagerPageView.qml)
- **Базовый тип:** [`PageContainer`](imtgui/View/PageContainer.qml)

**Свойства**

- `documentManager: DocumentServiceBase`

**Функции**

- `checkCreation()`
- `setDocumentName(documentId, name)`

<a id="type-imtguigql-GqlSingleDocCollectionPageView"></a>
#### `GqlSingleDocCollectionPageView`

Страница-контейнер коллекции: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/GqlSingleDocCollectionPageView.qml](imtguigql/Pages/GqlSingleDocCollectionPageView.qml)
- **Базовый тип:** [`PageContainer`](imtgui/View/PageContainer.qml)

**Свойства**

- `documentManager: DocumentServiceBase`

**Функции**

- `checkCreation()`
- `setDocumentName(documentId, name)`

<a id="type-imtguigql-GqlSingleDocumentPageView"></a>
#### `GqlSingleDocumentPageView`

Страница-контейнер одиночного документа: связывает данные страницы с её визуальным наполнением в рабочей области.

- **Файл:** [Pages/GqlSingleDocumentPageView.qml](imtguigql/Pages/GqlSingleDocumentPageView.qml)
- **Базовый тип:** [`PageContainer`](imtgui/View/PageContainer.qml)

**Свойства**

- `collectionId: string`
- `objectId: string`
- `objectTypeId: string`
- `createNew: bool`
- `documentManager: DocumentServiceBase`

**Функции**

- `checkCreation()`
- `setDocumentName(documentId, name)`
- `openObject(typeId, id)`
- `createObject(typeId)`

### Views/ — Представления

<a id="type-imtguigql-CollectionRepresentation"></a>
#### `CollectionRepresentation`

Представление данных коллекции: сопоставляет модель с её отображением.

- **Файл:** [Views/CollectionRepresentation.qml](imtguigql/Views/CollectionRepresentation.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `collectionId: string`
- `gqlGetListCommandId: string`
- `gqlImportCommandId: string`
- `gqlExportCommandId: string`
- `elementsModel: TreeItemModel`
- `headersModel: var`
- `notificationModel: TreeItemModel`
- `additionalFieldIds: var`
- `requestedFields: var`

**Сигналы**

- `removed()`
- `elementsRemoved(var elementIds)`
- `elementSetRemoved()`
- `elementSetRemoveFailed(string message)`
- `renamed(string objectId, string newName)`
- `imported(string objectId)`
- `exported(string fileName, string data)`
- `descriptionSetted(string objectId, string description)`
- `headersReceived(var headers)`
- `elementsReceived(var elements)`
- `objectMetaInfoReceived(var metaInfoData)`
- `objectMetaInfoReceiveFailed(string message)`
- `visualStatusReceived(string objectId, string icon, string text, string description)`
- `visualStatusReceiveFailed(string objectId, string errorMessage)`
- `elementsDuplicated()`
- `elementsDuplicateFailed(string message)`
- `beginUpdate()`
- `endUpdate()`

**Функции**

- `createSelectionParams(collectionFilter, documentFilter)`
- `importObject(typeId, name, description, b64encoded, mimeType)`
- `exportObject(objectId, mimeType, additionalParamsObj)`
- `getObjectMetaInfo(objectId)`
- `getObjectVisualStatus(objectId, typeId)`
- `duplicateElements(elementIds, name)`
- `addAdditionalInputParams(inputParams)`
- `updateModel()`
- `getCollectionHeaders()`
- `updateElements(count, offset, paramsSet)`
- `clearElements()`
- `getElementsRepresentation()`
- `removeElements(elementIds)`
- `removeElementSet(collectionFilter)`
- `setElementName(objectId, name)`
- `setElementDescription(objectId, description)`
- `restoreObjectSet(collectionFilter)`
- `restoreObjects(objectIds)`
- `getHeaders()`
- `getRequestedFields()`

<a id="type-imtguigql-ComboBoxGqlSimpleElementView"></a>
#### `ComboBoxGqlSimpleElementView`

Представление одного поля/элемента GraphQL внутри формы или строки.

- **Файл:** [Views/ComboBoxGqlSimpleElementView.qml](imtguigql/Views/ComboBoxGqlSimpleElementView.qml)
- **Базовый тип:** [`ComboBoxElementView`](imtgui/View/ComboBoxElementView.qml)

**Свойства**

- `gqlCommandId: string`
- `subscriptionCommandId: string`
- `fields: var`
- `filter: var`

**Функции**

- `getHeaders()`

<a id="type-imtguigql-GqlBasedItemSelectElementView"></a>
#### `GqlBasedItemSelectElementView`

Представление одного поля/элемента GraphQL внутри формы или строки.

- **Файл:** [Views/GqlBasedItemSelectElementView.qml](imtguigql/Views/GqlBasedItemSelectElementView.qml)
- **Базовый тип:** [`ItemSelectElementView`](#type-imtcolgui-ItemSelectElementView)

**Свойства**

- `collectionId: string`

<a id="type-imtguigql-RemoteCollectionView"></a>
#### `RemoteCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/RemoteCollectionView.qml](imtguigql/Views/RemoteCollectionView.qml)
- **Базовый тип:** [`CollectionView`](#type-imtcolgui-CollectionView)
- **Наследники:** [`ConversationCollectionView`](#type-imtchatgui-ConversationCollectionView), [`FeatureCollectionView`](#type-imtlicgui-FeatureCollectionView), [`LicenseCollectionView`](#type-imtlicgui-LicenseCollectionView), [`MessageCollectionView`](#type-imtcolgui-MessageCollectionView), [`ProductCollectionView`](#type-imtlicgui-ProductCollectionView), [`RoleCollectionView`](#type-imtauthgui-RoleCollectionView), [`TenantCollectionView`](#type-imtauthgui-TenantCollectionView), [`TicketCollectionView`](#type-imtdeskgui-TicketCollectionView), [`UserActionCollectionView`](#type-imtauthgui-UserActionCollectionView), [`UserCollectionView`](#type-imtauthgui-UserCollectionView), [`UserGroupCollectionView`](#type-imtauthgui-UserGroupCollectionView)

**Свойства**

- `hasRemoteChanges: bool`
- `tableViewParamsStoredServer: bool`
- `showRemoteChangesAlert: bool`
- `gqlGetListCommandId: string`
- `additionalFieldIds: var`
- `requestedFields: var`
- `subscriptionCommandId: alias`

**Сигналы**

- `removed(string objectId)`
- `collectionChanged(var changeInfo)`
- `collectionInserted(var changeInfo)`
- `collectionUpdated(var changeInfo)`
- `collectionRemoved(var changeInfo)`
- `collectionLocalChanged(var changeInfo)`
- `collectionRemoteChanged(var changeInfo)`

**Функции**

- `getHeaders()`
- `receiveRemoteChanges()`
- `handleSubscription(dataModel, changeInfo)`

<a id="type-imtguigql-ServerPathPickerElementView"></a>
#### `ServerPathPickerElementView`

Представление одного поля/элемента серверного пути внутри формы или строки.

- **Файл:** [Views/ServerPathPickerElementView.qml](imtguigql/Views/ServerPathPickerElementView.qml)
- **Базовый тип:** [`ElementView`](imtgui/View/ElementView.qml)

**Свойства**

- `path: string`
- `paths: var`
- `pathKind: int`
- `multiSelect: bool`
- `titleText: string`
- `browseEnabled: bool`
- `readOnlyPath: bool`
- `placeHolderText: string`
- `extensions: var`
- `textInputValidator: var`
- `showErrorWhenInvalid: bool`
- `errorText: string`

**Сигналы**

- `pathEdited()`
- `pathsEdited(var paths)`
- `editingFinished()`

**Функции**

- `getHeaders()`

### Dialogs/ — Диалоги

<a id="type-imtguigql-FileSystemBrowserDialog"></a>
#### `FileSystemBrowserDialog`

Модальный диалог файловой системы. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/FileSystemBrowserDialog.qml](imtguigql/Dialogs/FileSystemBrowserDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `startPath: string`
- `pathKind: int`
- `multiSelect: bool`
- `titleText: string`
- `clientIdHeader: string`
- `serviceIdHeader: string`
- `extensionFilter: string`
- `hasExtensionFilter: bool` — только чтение
- `currentPath: string`
- `hasParent: bool`
- `parentPath: string`
- `atDrivesRoot: bool` — только чтение
- `selectedPath: string`
- `selectedPaths: var`
- `isLoading: bool`
- `errorText: string`
- `hasMore: bool`
- `pageSize: int`
- `loadedCount: int`
- `totalCount: int`
- `filterText: string`
- `sortColumn: string`
- `sortAscending: bool`
- `checkColumnWidth: int`
- `sizeColumnWidth: int`
- `dateColumnWidth: int`
- `filterFieldWidth: int`
- `scrollGutter: int`
- `revealIndex: int`
- `filterFieldFocused: bool`

**Сигналы**

- `pathSelected(string selectedPath)`
- `pathsSelected(var paths)`

**Функции**

- `isDirectoryEntry(entryType)`
- `isConfirmableLeaf(entryType)`
- `isSelectableEntry(entryType)`
- `isListableEntry(entryType)`
- `iconNameFor(entryType)`
- `formatBytes(byteValue)`
- `formatSize(sizeValue, entryType)`
- `formatDate(isoDate)`
- `resolveHeaders()`
- `normalizePath(path)`
- `parentDirOf(path)`
- `currentFolderName()`
- `isPathSelected(path)`
- `findEntryByPath(path)`
- `findDisplayIndexByPath(path)`
- `selectionIsValid()`
- `setSingleSelection(path)`
- `toggleSelection(path)`
- `selectRange(fromIndex, toIndex)`
- `onRowClicked(index, path, modifiers)`
- `pageAllSelected()`
- `pageAnySelected()`
- `headerCheckState()`
- `toggleSelectAll()`
- `sortMark(columnId)`
- `sortByToSdl()`
- `toggleSort(columnId)`
- `applyFilterFromField(text)`
- `fillEntriesInput(path, append)`
- `browse(path, append)`
- `onLoadFailed(message)`
- `retryLastAttempt()`
- `loadMore()`
- `browseUp()`
- `payloadHasParentPath(payload)`
- `applyPayload(payload)`
- `applySelectionAfterLoad(reveal)`
- `refocusSelection()`
- `activateEntry(index)`
- `moveSelectionTo(index)`
- `moveSelection(delta)`
- `activateCurrent()`
- `handleKey(event)`
- `selectionKindHint()`
- `itemCountLabel()`
- `selectionCountLabel()`
- `extensionFilterLabel()`
- `extensionFilterHint()`
- `statusLabel()`
- `updateOkEnabled()`
- `computeTitle()`
- `navigateInitial()`

<a id="type-imtguigql-PopupMenuDialogGql"></a>
#### `PopupMenuDialogGql`

Вспомогательный объект GraphQL.

- **Файл:** [Dialogs/PopupMenuDialogGql.qml](imtguigql/Dialogs/PopupMenuDialogGql.qml)
- **Базовый тип:** [`PopupMenuDialog`](imtcontrols/Dialogs/PopupMenuDialog.qml)

**Свойства**

- `commandId: string`
- `dataProviderState: string`
- `filterText: string`
- `comboDecorator: var`
- `textField_: var`
- `hasLoading: bool`

**Сигналы**

- `editSignal()`
- `clearSignal()`

**Функции**

- `setY()`
- `requestNextBatch()`
- `setTextFocus(focus)`
- `onKeyboardUp()`
- `onKeyboardDown()`

<a id="type-imtguigql-PopupMenuDialogGqlLegacy"></a>
#### `PopupMenuDialogGqlLegacy`

Визуальный компонент GraphQL.

- **Файл:** [Dialogs/PopupMenuDialogGqlLegacy.qml](imtguigql/Dialogs/PopupMenuDialogGqlLegacy.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `model: var`
- `uuid: string`
- `root: Item`
- `rootItem: Item`
- `itemWidth: int`
- `itemHeight: int`
- `filterHeight: int`
- `offset: int`
- `delegateRadius: int`
- `textSize: int`
- `count: int`
- `commandId: string`
- `filterName: string`
- `fontColor: string`
- `filterText: alias`
- `excludeFilterPart: string`
- `endListStatus: bool`
- `hiddenBackground: bool`
- `canClose: bool`
- `canUpdateModel: bool`
- `preventFirstLoading: bool`
- `doNotCorrectPosition: bool`
- `additionalFilter: string`
- `modelFilterAlias: alias`
- `elementsCount: int`
- `pauseDuration: int`
- `contentHeight: real`
- `pinned: bool`
- `ready: bool`
- `nameId: string`
- `delegate: Component`
- `properties: TreeItemModel`
- `gettedParams: TreeItemModel`
- `filterIdsModel: TreeItemModel`
- `countVisibleItem: int`
- `selectedIndex: int`
- `hoverBlocked: bool`
- `notSetProperties: bool`
- `isUpwards: bool`

**Сигналы**

- `finished(string commandId, int index)`
- `endList()`
- `textEdited()`
- `started()`
- `clearSignal()`
- `propertiesChangedSignal()`
- `destructionSignal()`

**Функции**

- `onBackgroundClicked()`
- `updateModel()`
- `onAppSizeChanged(parameters)`
- `setTextFocus(focus)`
- `createAdditionalQuery()`
- `contentYCorrection(down_)`

<a id="type-imtguigql-RemoteCollectionViewDialog"></a>
#### `RemoteCollectionViewDialog`

Модальный диалог коллекции. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/RemoteCollectionViewDialog.qml](imtguigql/Dialogs/RemoteCollectionViewDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `collectionId: string`
- `collectionView: RemoteCollectionView`

### Editors/ — Редакторы

<a id="type-imtguigql-SelectableCollectionEditor"></a>
#### `SelectableCollectionEditor`

Редактор коллекции — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/SelectableCollectionEditor.qml](imtguigql/Editors/SelectableCollectionEditor.qml)
- **Базовый тип:** [`ElementView`](imtgui/View/ElementView.qml)

**Свойства**

- `collectionId: string`
- `targetTitle: string`
- `sourceTitle: string`
- `selectedIds: var`
- `targetRequestedFields: var`
- `targetCollectionView: RemoteCollectionView`
- `sourceCollectionView: RemoteCollectionView`
- `stackView: StackView`

**Сигналы**

- `selectionChanged()`

**Функции**

- `setSourceAdditionalFilters(sourceCollectionView)`
- `updateSourceCollection()`
- `setTargetAdditionalFilters(collectionFilter)`
- `updateTargetCollection()`
- `addElementsFromSourceCollection()`

### Inputs/ — Поля ввода

<a id="type-imtguigql-ComboBoxGql"></a>
#### `ComboBoxGql`

Вспомогательный объект GraphQL.

- **Файл:** [Inputs/ComboBoxGql.qml](imtguigql/Inputs/ComboBoxGql.qml)
- **Базовый тип:** [`ComboBox`](imtcontrols/Inputs/ComboBox.qml)

**Свойства**

- `dataProvider: alias`
- `filter: alias`
- `commandId: alias`
- `excludeFilterPart: string`
- `filterText: string`
- `endListStatus: bool`
- `closeOnFinished: bool`
- `keepFilterText: bool`
- `hasLoading: bool`
- `modelUpdateDelay: int`
- `popupDecorator: Component`

**Сигналы**

- `editSignal()`
- `clearSignal()`
- `closeAction()`

**Функции**

- `openPopupMenu()`
- `updateModel(offset)`
- `onMouseAreaClicked()`
- `setTextFilter(text)`
- `setCustomFilters(inputParams)`

<a id="type-imtguigql-ComboBoxGqlLegacy"></a>
#### `ComboBoxGqlLegacy`

Визуальный компонент GraphQL.

- **Файл:** [Inputs/ComboBoxGqlLegacy.qml](imtguigql/Inputs/ComboBoxGqlLegacy.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `model: var`
- `properties: TreeItemModel`
- `gettedParams: TreeItemModel`
- `filterIdsModel: TreeItemModel`
- `borderColor: color`
- `backgroundColor: color`
- `currentText: string`
- `textCentered: bool`
- `menuVisible: bool`
- `isColor: bool`
- `backVisible: bool`
- `hiddenBackground: bool`
- `canClose: bool`
- `closeEmpty: bool`
- `complexModel: bool`
- `openST: bool`
- `preventFirstLoading: bool`
- `radius: int`
- `currentIndex: int`
- `offset: int`
- `count: int`
- `countVisibleItem: int`
- `delegateRadius: int`
- `itemHeight: int`
- `filterHeiht: int`
- `textSize: int`
- `textDelegateSize: int`
- `dialogsCountPrev: int`
- `dialogsCount: int`
- `commandId: string`
- `filterName: string`
- `filterText: string`
- `fontColor: string`
- `fontColorTitle: string`
- `keepFilterText: bool`
- `inFocus: bool`
- `pauseDuration: int`
- `excludeFilterPart: string`
- `canUpdateModel: bool`
- `additionalFilter: string`
- `nameId: string`
- `selectedIndex: int`
- `hoverBlocked: bool`
- `doNotCorrectPosition: bool`
- `readOnly: bool`
- `isUpwards: bool`
- `delegate: Component`
- `popupComp: alias`
- `gradient: alias`
- `image: alias`
- `imageSource: alias`
- `imageRotation: alias`
- `tooltipText: alias`
- `tooltipItem: alias`

**Сигналы**

- `setCurrentText(var modelll, int index)`
- `clicked()`
- `finished(string commandId, int index)`
- `editSignal()`
- `closeSignal(var model_)`
- `clearSignal()`
- `closeEmptySignal()`

**Функции**

- `popupDestruction()`
- `openPopupMenu()`
- `closeFunc()`

<a id="type-imtguigql-ComboBoxGqlSimple"></a>
#### `ComboBoxGqlSimple`

Вспомогательный объект GraphQL.

- **Файл:** [Inputs/ComboBoxGqlSimple.qml](imtguigql/Inputs/ComboBoxGqlSimple.qml)
- **Базовый тип:** [`ComboBox`](imtcontrols/Inputs/ComboBox.qml)

**Свойства**

- `gqlCommandId: alias`
- `subscriptionCommandId: alias`
- `fields: alias`
- `filter: alias`

**Функции**

- `updateModel()`
- `getHeaders()`

<a id="type-imtguigql-ServerPathPicker"></a>
#### `ServerPathPicker`

Пикер серверного пути: элемент выбора значения из набора.

- **Файл:** [Inputs/ServerPathPicker.qml](imtguigql/Inputs/ServerPathPicker.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `path: string`
- `paths: var`
- `pathKind: int`
- `multiSelect: bool`
- `titleText: string`
- `browseEnabled: bool`
- `readOnlyPath: bool`
- `placeHolderText: string`
- `extensions: var`

**Сигналы**

- `pathEdited()`
- `pathsEdited(var paths)`

**Функции**

- `getHeaders()`

### Controllers/ — Контроллеры

<a id="type-imtguigql-GqlBasedCollectionDataController"></a>
#### `GqlBasedCollectionDataController`

Невизуальный контроллер коллекции: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/GqlBasedCollectionDataController.qml](imtguigql/Controllers/GqlBasedCollectionDataController.qml)
- **Базовый тип:** [`CollectionDataController`](#type-imtcolgui-CollectionDataController)

**Свойства**

- `getCollectionHeadersInput: GetCollectionHeadersInput`
- `getCollectionHeadersRequest: GqlSdlRequestSender`
- `getObjectDataInput: GetObjectDataInput`
- `getObjectDataRequest: GqlSdlRequestSender`
- `insertNewObjectInput: InsertNewObjectInput`
- `insertNewObjectRequest: GqlSdlRequestSender`
- `setObjectDataInput: SetObjectDataInput`
- `setObjectDataRequest: GqlSdlRequestSender`
- `removeElementsInput: RemoveElementsInput`
- `removeElementsRequest: GqlSdlRequestSender`
- `removeElementSetInput: RemoveElementSetInput`
- `removeElementSetRequest: GqlSdlRequestSender`
- `restoreObjectsInput: RestoreObjectsInput`
- `restoreObjectsRequest: GqlSdlRequestSender`
- `restoreObjectSetInput: RestoreObjectSetInput`
- `restoreObjectSetRequest: GqlSdlRequestSender`
- `setObjectNameInput: SetObjectNameInput`
- `setObjectNameRequest: GqlSdlRequestSender`
- `setObjectDescriptionInput: SetObjectDescriptionInput`
- `setObjectDescriptionRequest: GqlSdlRequestSender`
- `createSubCollectionInput: CreateSubCollectionInput`
- `createSubCollectionRequest: GqlSdlRequestSender`

**Функции**

- `getHeaders()`
- `getCollectionHeaders()`
- `getObjectData(objectId)`
- `insertNewObject(typeId, name, description, objectData, objectId)`
- `setObjectData(objectId, newObjectData)`
- `getElements(count, offset, filter)`
- `removeObjects(objectIds)`
- `removeObjectSet(selectionParams)`
- `restoreObjects(objectIds)`
- `restoreObjectSet(selectionParams)`
- `setObjectName(objectId, name)`
- `createSubCollection(offset, count, selectionParams)`

<a id="type-imtguigql-GqlBasedCommandsController"></a>
#### `GqlBasedCommandsController`

Невизуальный контроллер GraphQL: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/GqlBasedCommandsController.qml](imtguigql/Controllers/GqlBasedCommandsController.qml)
- **Базовый тип:** [`CommandsController`](imtcontrols/Models/CommandsController.qml)

**Свойства**

- `commandsRequest: GqlSdlRequestSender`

**Функции**

- `getCommands()`
- `getHeaders()`

<a id="type-imtguigql-GqlBasedDataModelController"></a>
#### `GqlBasedDataModelController`

Невизуальный контроллер GraphQL: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/GqlBasedDataModelController.qml](imtguigql/Controllers/GqlBasedDataModelController.qml)
- **Базовый тип:** [`DataModelController`](imtcontrols/Models/DataModelController.qml)

**Свойства**

- `gqlCommandId: string`
- `responseModel: var`
- `inputModel: var`
- `gqlRequest: GqlRequest`

**Функции**

- `saveDataModel(params, documentId, documentName, documentDescription, documentTypeId)`
- `prepareInputModel(params, documentId, documentName, documentDescription)`
- `getHeaders()`
- `prepareResultModel()`

<a id="type-imtguigql-GqlBasedOptionManager"></a>
#### `GqlBasedOptionManager`

Менеджер GraphQL: управляет состоянием и жизненным циклом связанных объектов.

- **Файл:** [Controllers/GqlBasedOptionManager.qml](imtguigql/Controllers/GqlBasedOptionManager.qml)
- **Базовый тип:** [`OptionManager`](imtcontrols/Models/OptionManager.qml)

**Свойства**

- `inputModel: alias`
- `responseModel: alias`
- `getCommandId: alias`

**Функции**

- `requestDataModel(params)`

<a id="type-imtguigql-GqlDocumentDataController"></a>
#### `GqlDocumentDataController`

Невизуальный контроллер документа: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/GqlDocumentDataController.qml](imtguigql/Controllers/GqlDocumentDataController.qml)
- **Базовый тип:** [`DocumentDataController`](#type-imtdocgui-DocumentDataController)

**Свойства**

- `gqlGetCommandId: string`
- `gqlAddCommandId: string`
- `gqlUpdateCommandId: string`
- `subscriptionCommandId: string`
- `subscriptionClient: SubscriptionClient`
- `gqlUpdateModel: GqlRequestSender`
- `gqlGetModel: GqlRequestSender`
- `gqlAddModel: GqlRequestSender`

**Функции**

- `updateDocumentModel()`
- `insertDocument()`
- `saveDocument()`
- `getHeaders()`
- `createGetInputParams()`
- `createUpdateInputParams()`
- `createAddInputParams()`

<a id="type-imtguigql-GqlRequestDocumentDataController"></a>
#### `GqlRequestDocumentDataController`

Невизуальный контроллер документа: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/GqlRequestDocumentDataController.qml](imtguigql/Controllers/GqlRequestDocumentDataController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `documentId: string`
- `documentName: string`
- `documentDescription: string`
- `typeId: string`
- `documentModelComp: Component`
- `documentModel: var`
- `gqlGetCommandId: string`
- `gqlAddCommandId: string`
- `gqlUpdateCommandId: string`
- `subscriptionCommandId: string`
- `getRequestInputParam: var`
- `addRequestInputParam: var`
- `updateRequestInputParam: var`
- `subscriptionClient: SubscriptionClient`
- `gqlUpdateModel: GqlRequest`
- `gqlGetModel: GqlRequest`
- `gqlAddModel: GqlRequest`

**Сигналы**

- `beforeSaveModel()`
- `beforeInsertModel()`
- `saved(string id, string name)`
- `error(string message, string type)`
- `modelChanged()`

**Функции**

- `getDocumentId()`
- `getDocumentName()`
- `getDocumentTypeId()`
- `getDocumentModel()`
- `getDocumentDescription()`
- `parseGqlErrors(responseObj)`
- `setupGetRequestInputData()`
- `setupAddRequestInputData()`
- `setupUpdateRequestInputData()`
- `updateDocumentModel()`
- `insertDocument()`
- `saveDocument()`
- `createDocumentModel()`
- `getHeaders()`
- `getBodyForGetModel()`

<a id="type-imtguigql-RemoteFileController__"></a>
#### `RemoteFileController__`

Невизуальный контроллер удалённых данных: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/RemoteFileController.qml](imtguigql/Controllers/RemoteFileController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `state: string`
- `downloadedFileLocation: string`
- `downloadedFilePath: string`
- `json: string`
- `prefix: string`

**Сигналы**

- `progress(real bytesLoaded, real bytesTotal)`
- `fileDeleted(var url)`
- `fileDownloaded(string filePath)`
- `fileUploaded(var url)`
- `fileExists(var url)`
- `fileDownloadFailed()`
- `fileUploadFailed()`

**Функции**

- `sendFile(fileUrl)`
- `getFile(fileHash, fileName)`
- `deleteFile(fileHash, fileUrl)`

<a id="type-imtguigql-TableViewParamController"></a>
#### `TableViewParamController`

Невизуальный контроллер: инкапсулирует бизнес-логику, координирует запросы и состояние.

- **Файл:** [Controllers/TableViewParamController.qml](imtguigql/Controllers/TableViewParamController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `tableViewParams: TableViewParams`
- `tableId: string`
- `getModelQuery: GqlModel`
- `saveModelQuery: GqlModel`

**Сигналы**

- `updated()`

**Функции**

- `getModel()`
- `saveModel()`
- `clearModel()`

### Providers/ — Провайдеры

<a id="type-imtguigql-FilterableSelectGqlDataProvider"></a>
#### `FilterableSelectGqlDataProvider`

Провайдер данных GraphQL: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/FilterableSelectGqlDataProvider.qml](imtguigql/Providers/FilterableSelectGqlDataProvider.qml)
- **Базовый тип:** [`FilterableSelectDataProvider`](#type-imtcolgui-FilterableSelectDataProvider)

**Свойства**

- `getSelectableItemsInput: GetSelectableItemsInput`
- `getSelectableItemsRequest: GqlSdlRequestSender`

**Функции**

- `getHeaders()`
- `executeRequest(requestId, count, offset, filter)`
- `executeFetchByIds(requestId, ids)`

<a id="type-imtguigql-GqlBasedDataModelProvider"></a>
#### `GqlBasedDataModelProvider`

Провайдер данных GraphQL: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/GqlBasedDataModelProvider.qml](imtguigql/Providers/GqlBasedDataModelProvider.qml)
- **Базовый тип:** [`DataModelProvider`](imtcontrols/Models/DataModelProvider.qml)

**Свойства**

- `getCommandId: string`
- `responseModel: var`
- `inputModel: var`
- `gqlRequest: GqlRequest`

**Функции**

- `requestDataModel(paramsObj)`
- `prepareInputModel(paramsObj)`
- `prepareDataModel()`
- `getHeaders()`

<a id="type-imtguigql-GqlBasedObjectVisualStatusProvider"></a>
#### `GqlBasedObjectVisualStatusProvider`

Провайдер данных GraphQL: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/GqlBasedObjectVisualStatusProvider.qml](imtguigql/Providers/GqlBasedObjectVisualStatusProvider.qml)
- **Базовый тип:** [`ObjectVisualStatusProvider`](imtgui/View/ObjectVisualStatusProvider.qml)

**Свойства**

- `collectionId: string`
- `objectVisualStatusInput: ObjectVisualStatusInput`
- `getVisualStatusInfoRequest: GqlSdlRequestSender`

**Функции**

- `getVisualStatus(id, typeId)`
- `getHeaders()`

<a id="type-imtguigql-GqlBasedPermissionsProvider"></a>
#### `GqlBasedPermissionsProvider`

Провайдер данных прав доступа: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/GqlBasedPermissionsProvider.qml](imtguigql/Providers/GqlBasedPermissionsProvider.qml)
- **Базовый тип:** [`PermissionsProvider`](#type-imtauthgui-PermissionsProvider)

**Функции**

- `requestPermissions(tenantId)`
- `requestUserPermissions()`
- `requestOrganizationPermissions(tenantId, userId)`

<a id="type-imtguigql-MetaInfoProvider"></a>
#### `MetaInfoProvider`

Провайдер данных метаданных: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/MetaInfoProvider.qml](imtguigql/Providers/MetaInfoProvider.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `getMetaInfoGqlCommand: string`
- `metaInfoModel: TreeItemModel`
- `state: alias`

**Функции**

- `getMetaInfo(objectId)`
- `getHeaders()`

<a id="type-imtguigql-SdlObjectProvider"></a>
#### `SdlObjectProvider` · _не экспортируется как тип (загружается по пути/внутренний)_

Провайдер данных SDL: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/SdlObjectProvider.qml](imtguigql/Providers/SdlObjectProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

### Services/ — Сервисы

<a id="type-imtguigql-GqlBasedCollectionDocumentService"></a>
#### `GqlBasedCollectionDocumentService`

Сервис документа: предоставляет операции над сущностью и управляет её жизненным циклом.

- **Файл:** [Services/GqlBasedCollectionDocumentService.qml](imtguigql/Services/GqlBasedCollectionDocumentService.qml)
- **Базовый тип:** [`DocumentServiceBase`](#type-imtdocgui-DocumentServiceBase)

**Свойства**

- `collectionId: string`
- `documentManagerSubscription: SubscriptionClient`
- `undoManagerSubscription: SubscriptionClient`
- `collectionIdInput: CollectionId`
- `objectIdInput: ObjectId`
- `documentIdInput: DocumentId`
- `saveDocumentInput: SaveDocumentInput`
- `documentTypeIdInput: DocumentTypeId`
- `collectionUndoRedoInput: CollectionUndoRedoInput`
- `undoRedoInput: UndoRedoInput`
- `getOpenedDocumentListRequest: GqlSdlRequestSender`
- `openDocumentRequest: GqlSdlRequestSender`
- `createDocumentRequest: GqlSdlRequestSender`
- `saveDocumentRequest: GqlSdlRequestSender`
- `closeDocumentRequest: GqlSdlRequestSender`
- `doUndoRequest: GqlSdlRequestSender`
- `doRedoRequest: GqlSdlRequestSender`
- `getUndoInfoRequest: GqlSdlRequestSender`

**Функции**

- `getHeaders()`
- `getOpenedDocumentList()`
- `openDocument(typeId, documentId)`
- `createDocument(typeId, proposedSourceDocumentId)`
- `openOrCreateByObjectId(typeId, objectId, proposedSourceDocumentId)`
- `saveDocument(documentId, documentName)`
- `closeDocument(documentId)`
- `doUndo(documentId, steps)`
- `doRedo(documentId, steps)`
- `resetUndo(documentId)`
- `getUndoInfo(documentId)`

### Api/ — Сеть и API

<a id="type-imtguigql-GqlRequest"></a>
#### `GqlRequest` · _не экспортируется как тип (загружается по пути/внутренний)_

Вспомогательный объект GraphQL.

- **Файл:** [Api/GqlRequest.qml](imtguigql/Api/GqlRequest.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`GqlSdlRequestSender`](#type-imtguigql-GqlSdlRequestSender)

**Свойства**

- `json: string`
- `state: string`

**Функции**

- `setGqlQuery(gqlData, headers)`

<a id="type-imtguigql-GqlRequestSender"></a>
#### `GqlRequestSender`

Отправитель запросов GraphQL: формирует запрос, отправляет его и обрабатывает ответ.

- **Файл:** [Api/GqlRequestSender.qml](imtguigql/Api/GqlRequestSender.qml)
- **Базовый тип:** `GqlModel`

**Свойства**

- `gqlCommandId: string`
- `requestType: int`

**Функции**

- `onResult(data)`
- `onError(message, type)`
- `getHeaders()`
- `send(params)`
- `createQueryParams(query, params)`

<a id="type-imtguigql-GqlSdlRequestSender"></a>
#### `GqlSdlRequestSender`

Отправитель запросов GraphQL: формирует запрос, отправляет его и обрабатывает ответ.

- **Файл:** [Api/GqlSdlRequestSender.qml](imtguigql/Api/GqlSdlRequestSender.qml)
- **Базовый тип:** [`GqlRequest`](#type-imtguigql-GqlRequest)

**Свойства**

- `gqlCommandId: string`
- `requestType: int`
- `sdlObject: var`
- `sdlObjectComp: Component`
- `inputObjectComp: Component`
- `inputParams: var`

**Сигналы**

- `finished(int status)`

**Функции**

- `addInputParam(id, value)`
- `onResult(data)`
- `onError(message, type)`
- `getHeaders()`
- `send(sdlInputObject)`
- `createQueryParams(query)`
- `getRequestedFields()`

<a id="type-imtguigql-RemoteCollectionChangeListener"></a>
#### `RemoteCollectionChangeListener`

Слушатель событий коллекции: реагирует на изменения на сервере (подписка).

- **Файл:** [Api/RemoteCollectionChangeListener.qml](imtguigql/Api/RemoteCollectionChangeListener.qml)
- **Базовый тип:** [`SubscriptionClient`](#type-imtguigql-SubscriptionClient)

**Свойства**

- `collectionId: string`
- `currentUserId: string`

**Сигналы**

- `changed(var changeInfo, var rawData)`
- `inserted(var changeInfo, var rawData)`
- `updated(var changeInfo, var rawData)`
- `removed(var changeInfo, var rawData)`
- `localChanged(var changeInfo, var rawData)`
- `remoteChanged(var changeInfo, var rawData)`
- `parseError(var changeInfo, var rawData)`
- `unknownOperation(var changeInfo, var rawData)`

<a id="type-imtguigql-SubscriptionClient"></a>
#### `SubscriptionClient`

Клиент подписок: устанавливает соединение и обменивается данными с сервером.

- **Файл:** [Api/SubscriptionClient.qml](imtguigql/Api/SubscriptionClient.qml)
- **Базовый тип:** `GqlModel`
- **Наследники:** [`RemoteCollectionChangeListener`](#type-imtguigql-RemoteCollectionChangeListener), [`WebSocketConnectionChecker`](#type-imtguigql-WebSocketConnectionChecker)

**Свойства**

- `subscriptionId: string`
- `gqlCommandId: string`
- `state: string`
- `autoSubscribe: bool`
- `ok: bool`

**Сигналы**

- `messageReceived(var data)`

**Функции**

- `deliverReady()`
- `getGqlQuery()`
- `registerSubscription()`
- `unRegisterSubscription()`
- `getHeaders()`

<a id="type-imtguigql-SubscriptionManager"></a>
#### `SubscriptionManager`

Менеджер подписок: управляет состоянием и жизненным циклом связанных объектов.

- **Файл:** [Api/SubscriptionManager.qml](imtguigql/Api/SubscriptionManager.qml)
- **Базовый тип:** [`WebSocket`](https://doc.qt.io/qt-6/qml-qtwebsockets-websocket.html)

**Свойства**

- `socketModel: TreeItemModel`
- `subscriptionModel: var`
- `authenticationTokenHeaderId: string` — только чтение
- `timer: Timer`

**Функции**

- `reconnect()`
- `handleSubscriptionError(subscriptionId, rawMessage)`
- `onAccessTokenRefreshed(parameters)`
- `registerSubscriptionEvent(parameters)`
- `clear()`
- `registerSubscriptionToServer()`
- `registerSubscription(query, subscriptionClient, headers)`
- `unRegisterSubscription(subscriptionClient)`

<a id="type-imtguigql-WebSocketConnectionChecker"></a>
#### `WebSocketConnectionChecker`

Проверка соединения подключения: контролирует доступность канала связи.

- **Файл:** [Api/WebSocketConnectionChecker.qml](imtguigql/Api/WebSocketConnectionChecker.qml)
- **Базовый тип:** [`SubscriptionClient`](#type-imtguigql-SubscriptionClient)

**Свойства**

- `status: int`
- `subscriptionManager: SubscriptionManager`

**Функции**

- `registerSubscription()`
- `unRegisterSubscription()`

<a id="type-imtguigql-XmlHttpRequestProxy"></a>
#### `XmlHttpRequestProxy`

Прокси: посредник для выполнения запросов/передачи данных.

- **Файл:** [Api/XmlHttpRequestProxy.qml](imtguigql/Api/XmlHttpRequestProxy.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Сигналы**

- `ok(string gqlData, var gqlRequestRef)`
- `forbidden(string gqlData, var gqlRequestRef)`
- `unauthorized(string gqlData, var gqlRequestRef)`
- `internalError(string gqlData, var gqlRequestRef)`

**Функции**

- `handleResponse(gqlBody, xhr, gqlRequestRef)`

### Base/ — Базовые компоненты

<a id="type-imtguigql-ClientStyle"></a>
#### `ClientStyle`

Стиль/оформление: набор визуальных констант и правил.

- **Файл:** [Base/ClientStyle.qml](imtguigql/Base/ClientStyle.qml)
- **Базовый тип:** [`StyleBase`](imtstyle/StyleBase.qml)

**Функции**

- `getDesignScheme(theme)`

---

<a id="imtchatgui"></a>
## `imtchatgui` — Чат и обмен сообщениями

Компоненты чата: панель и лента переписки, ввод сообщений, пузыри сообщений, индикатор набора текста, поиск, карточки ссылок на сущности и создание бесед.

**Импорт:** `import imtchatgui 1.0` · **Файлов:** 12

### Views/ — Представления

<a id="type-imtchatgui-ChatNotificationBanner"></a>
#### `ChatNotificationBanner`

Баннер чата: информационная полоса-уведомление вверху экрана.

- **Файл:** [Views/ChatNotificationBanner.qml](imtchatgui/Views/ChatNotificationBanner.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `senderName: string`
- `messagePreview: string`
- `conversationId: string`

**Сигналы**

- `bannerTapped(string conversationId)`

**Функции**

- `show(sender, preview, convId)`
- `dismiss()`

<a id="type-imtchatgui-ChatView"></a>
#### `ChatView`

Визуальное представление чата: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/ChatView.qml](imtchatgui/Views/ChatView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `conversationId: string`
- `conversationName: string`
- `messagesModel: var`
- `isTyping: bool`
- `typingUserName: string`

**Сигналы**

- `messageSent(string content, var entityRefs, var attachmentIds)`
- `ticketCreateRequested(string messageId, string content)`

**Функции**

- `showNotification(senderName, preview)`

<a id="type-imtchatgui-ConversationCollectionView"></a>
#### `ConversationCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/ConversationCollectionView.qml](imtchatgui/Views/ConversationCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtchatgui-EntityReferenceCard"></a>
#### `EntityReferenceCard`

Карточка сущности: компактное визуальное представление одной сущности.

- **Файл:** [Views/EntityReferenceCard.qml](imtchatgui/Views/EntityReferenceCard.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `entityType: string`
- `entityId: string`
- `displayName: string`
- `previewData: string`
- `compact: bool`

**Сигналы**

- `removeRequested()`

**Функции**

- `entityTypeIcon(type)`

<a id="type-imtchatgui-MessageBubble"></a>
#### `MessageBubble`

Пузырь сообщения сообщений: элемент ленты переписки.

- **Файл:** [Views/MessageBubble.qml](imtchatgui/Views/MessageBubble.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `messageId: string`
- `senderId: string`
- `senderName: string`
- `content: string`
- `timestamp: string`
- `status: int`
- `isOwn: bool`
- `hasEntityRefs: bool`
- `reactions: var`

**Сигналы**

- `createTicketRequested(string messageId, string content)`
- `reactionAdded(string messageId, string reaction)`

<a id="type-imtchatgui-TypingIndicator"></a>
#### `TypingIndicator`

Индикатор набора текста: визуальный признак состояния/процесса.

- **Файл:** [Views/TypingIndicator.qml](imtchatgui/Views/TypingIndicator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `userName: string`
- `dots: string`

### Panels/ — Панели

<a id="type-imtchatgui-ChatPanel"></a>
#### `ChatPanel`

Панель чата: область интерфейса (список, боковая или нижняя панель).

- **Файл:** [Panels/ChatPanel.qml](imtchatgui/Panels/ChatPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `conversationsModel: var`
- `currentConversationId: string`

**Сигналы**

- `conversationSelected(string conversationId)`
- `newConversationRequested()`

### Dialogs/ — Диалоги

<a id="type-imtchatgui-ConversationCreateDialog"></a>
#### `ConversationCreateDialog`

Модальный диалог беседы. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/ConversationCreateDialog.qml](imtchatgui/Dialogs/ConversationCreateDialog.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Сигналы**

- `conversationCreated(var convData)`
- `cancelled()`

**Функции**

- `open()`
- `cancel()`
- `submit()`

<a id="type-imtchatgui-EntitySearchPopup"></a>
#### `EntitySearchPopup`

Всплывающее окно сущности. Появляется по месту (у элемента) для быстрого выбора или подсказки.

- **Файл:** [Dialogs/EntitySearchPopup.qml](imtchatgui/Dialogs/EntitySearchPopup.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `searchQuery: string`
- `triggerChar: string`
- `triggerPos: int`
- `searchResults: var`

**Сигналы**

- `entitySelected(string entityType, string entityId, string displayName)`

**Функции**

- `open()`
- `close()`
- `performSearch()`

### Editors/ — Редакторы

<a id="type-imtchatgui-ConversationEditor"></a>
#### `ConversationEditor`

Редактор беседы — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/ConversationEditor.qml](imtchatgui/Editors/ConversationEditor.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `conversationData: ConversationData`

**Функции**

- `updateGui()`
- `updateModel()`

### Inputs/ — Поля ввода

<a id="type-imtchatgui-ChatSearchBar"></a>
#### `ChatSearchBar`

Строка поиска чата: поле для фильтрации содержимого по тексту.

- **Файл:** [Inputs/ChatSearchBar.qml](imtchatgui/Inputs/ChatSearchBar.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `placeholderText: string`
- `searchText: string`

**Сигналы**

- `searchTextChanged(string text)`

<a id="type-imtchatgui-MessageInput"></a>
#### `MessageInput`

Поле ввода сообщений: элемент для ручного ввода значения.

- **Файл:** [Inputs/MessageInput.qml](imtchatgui/Inputs/MessageInput.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `conversationId: string`
- `pendingEntityRefs: var`

**Сигналы**

- `messageReady(string content, var entityRefs, var attachmentIds)`
- `typingStarted()`
- `typingStopped()`

**Функции**

- `sendMessage()`

---

<a id="imtlicgui"></a>
## `imtlicgui` — Лицензирование (продукты, функции, лицензии)

Управление лицензиями: представления и редакторы продуктов, функций и лицензий, делегаты команд, провайдеры зависимостей функций и кэш коллекций.

**Импорт:** `import imtlicgui 1.0` · **Файлов:** 17

### Views/ — Представления

<a id="type-imtlicgui-FeatureCollectionView"></a>
#### `FeatureCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/FeatureCollectionView.qml](imtlicgui/Views/FeatureCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtlicgui-LicenseCollectionView"></a>
#### `LicenseCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/LicenseCollectionView.qml](imtlicgui/Views/LicenseCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtlicgui-ProductCollectionView"></a>
#### `ProductCollectionView`

Представление коллекции коллекции: список/таблица элементов с выбором, фильтрацией и командами.

- **Файл:** [Views/ProductCollectionView.qml](imtlicgui/Views/ProductCollectionView.qml)
- **Базовый тип:** [`RemoteCollectionView`](#type-imtguigql-RemoteCollectionView)

<a id="type-imtlicgui-ProductView"></a>
#### `ProductView`

Визуальное представление продуктов: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/ProductView.qml](imtlicgui/Views/ProductView.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `allFeaturesModel: TreeItemModel`
- `licensesModel: TreeItemModel`
- `productFeaturesViewModel: TreeItemModel`
- `tableView: alias`
- `productData: ProductData`

**Функции**

- `onLocalizationChanged(language)`
- `onFeaturesChanged()`
- `updateHeaders()`
- `updateModel()`
- `updateGui()`
- `updateFeaturesGui()`
- `addFeature(featureId)`
- `removeFeature(featureId)`

### Dialogs/ — Диалоги

<a id="type-imtlicgui-FeaturesDialog"></a>
#### `FeaturesDialog`

Модальный диалог функций. Открывается поверх основного интерфейса для ввода данных или подтверждения действия.

- **Файл:** [Dialogs/FeaturesDialog.qml](imtlicgui/Dialogs/FeaturesDialog.qml)
- **Базовый тип:** [`Dialog`](imtcontrols/Dialogs/Dialog.qml)

**Свойства**

- `selectedIndexes: var`
- `featuresModel: TreeItemModel`
- `tableModel: TreeItemModel`
- `excludeFeatureIds: var`

**Сигналы**

- `featureAdded(var modelIndex)`

**Функции**

- `updateGui()`

### Editors/ — Редакторы

<a id="type-imtlicgui-FeatureEditor"></a>
#### `FeatureEditor`

Редактор функций — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/FeatureEditor.qml](imtlicgui/Editors/FeatureEditor.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `dependenciewViewModel: TreeItemModel`
- `featureId: string`
- `tableView: alias`
- `featureData: FeatureData`

**Функции**

- `onLocalizationChanged(language)`
- `getAllParents(selectedIndex)`
- `updateTreeViewGui()`
- `updateGui()`
- `updateModel()`
- `updateHeaders()`

<a id="type-imtlicgui-LicenseEditor"></a>
#### `LicenseEditor`

Редактор лицензий — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/LicenseEditor.qml](imtlicgui/Editors/LicenseEditor.qml)
- **Базовый тип:** [`ViewBase`](imtgui/View/ViewBase.qml)

**Свойства**

- `allFeaturesModel: TreeItemModel`
- `featuresModel: TreeItemModel`
- `licensesModel: TreeItemModel`
- `licenseData: LicenseDefinitionData`

**Функции**

- `onLocalizationChanged(language)`
- `updateModel()`
- `updateGui()`
- `updateLicensesGui()`
- `updateLicensesModel()`
- `updateFeaturesModel()`
- `updateFeaturesGui()`
- `updateLicensesHeaders()`
- `updateHeaders()`

### Delegates/ — Делегаты

<a id="type-imtlicgui-FeatureEditorCommandsDelegate"></a>
#### `FeatureEditorCommandsDelegate`

Делегат команд функций: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/FeatureEditorCommandsDelegate.qml](imtlicgui/Delegates/FeatureEditorCommandsDelegate.qml)
- **Базовый тип:** [`ViewCommandsDelegateBase`](imtgui/View/ViewCommandsDelegateBase.qml)

<a id="type-imtlicgui-PackageCollectionViewCommandsDelegate"></a>
#### `PackageCollectionViewCommandsDelegate`

Делегат команд коллекции: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/PackageCollectionViewCommandsDelegate.qml](imtlicgui/Delegates/PackageCollectionViewCommandsDelegate.qml)
- **Базовый тип:** [`CollectionViewCommandsDelegateBase`](#type-imtcolgui-CollectionViewCommandsDelegateBase)

<a id="type-imtlicgui-PackageViewItemDelegate"></a>
#### `PackageViewItemDelegate`

Делегат пакетов: шаблон отрисовки одного элемента списка/таблицы.

- **Файл:** [Delegates/PackageViewItemDelegate.qml](imtlicgui/Delegates/PackageViewItemDelegate.qml)
- **Базовый тип:** [`TreeViewItemDelegateBase`](imtcontrols/Views/TreeViewItemDelegateBase.qml)

**Свойства**

- `featureData: FeatureData`

**Сигналы**

- `error(string message)`

**Функции**

- `featureIsValid(featureId, featureName)`
- `canRename(featureId)`

<a id="type-imtlicgui-ProductCollectionViewCommandsDelegate"></a>
#### `ProductCollectionViewCommandsDelegate`

Делегат команд коллекции: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/ProductCollectionViewCommandsDelegate.qml](imtlicgui/Delegates/ProductCollectionViewCommandsDelegate.qml)
- **Базовый тип:** [`DocumentCollectionViewDelegate`](#type-imtdocgui-DocumentCollectionViewDelegate)

**Свойства**

- `force: bool`

**Функции**

- `updateItemSelection(selectedItems)`
- `onImport()`
- `onImportDialogResult(name, fileData, mimeType)`
- `onRename()`

<a id="type-imtlicgui-ProductViewCommandsDelegate"></a>
#### `ProductViewCommandsDelegate`

Делегат команд продуктов: описывает доступные действия (кнопки/пункты меню) для элемента.

- **Файл:** [Delegates/ProductViewCommandsDelegate.qml](imtlicgui/Delegates/ProductViewCommandsDelegate.qml)
- **Базовый тип:** [`ViewCommandsDelegateBase`](imtgui/View/ViewCommandsDelegateBase.qml)

**Свойства**

- `featuresDialogComp: Component`

### Providers/ — Провайдеры

<a id="type-imtlicgui-FeaturesDependenciesProvider"></a>
#### `FeaturesDependenciesProvider`

Провайдер данных функций: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/FeaturesDependenciesProvider.qml](imtlicgui/Providers/FeaturesDependenciesProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `model: TreeItemModel`

**Функции**

- `getAllDependencies(featureId)`
- `getDependencies(featureId)`
- `getAllDependentFeaturesByFeatureId(featureId)`
- `getAllDependenciesRecursive(featureId, dependenciesModel, retVal)`
- `getAllDependentFeaturesRecursive(featureId, retVal)`

<a id="type-imtlicgui-LicensesProvider"></a>
#### `LicensesProvider`

Провайдер данных лицензий: загружает данные с сервера и поставляет их представлениям.

- **Файл:** [Providers/LicensesProvider.qml](imtlicgui/Providers/LicensesProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `model: TreeItemModel`
- `completed: bool`
- `licensesModel: GqlModel`

**Сигналы**

- `failed()`

**Функции**

- `updateModel()`
- `getLicenseName(productId, licenseId)`

### Models/ — Модели и данные

<a id="type-imtlicgui-CachedFeatureCollection"></a>
#### `CachedFeatureCollection` · **singleton**

Кэшируемая коллекция коллекции (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedFeatureCollection.qml](imtlicgui/Models/CachedFeatureCollection.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)

<a id="type-imtlicgui-CachedLicenseCollection"></a>
#### `CachedLicenseCollection` · **singleton**

Кэшируемая коллекция коллекции (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedLicenseCollection.qml](imtlicgui/Models/CachedLicenseCollection.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)

<a id="type-imtlicgui-CachedProductCollection"></a>
#### `CachedProductCollection` · **singleton**

Кэшируемая коллекция коллекции (singleton): единый разделяемый источник данных, доступный всему приложению.

- **Файл:** [Models/CachedProductCollection.qml](imtlicgui/Models/CachedProductCollection.qml)
- **Базовый тип:** [`CollectionDataProvider`](#type-imtcolgui-CollectionDataProvider)

**Свойства**

- `hardwareProductsModel: TreeItemModel`
- `softwareProductsModel: TreeItemModel`

**Сигналы**

- `hardwareProductsModelReady()`
- `softwareProductsModelReady()`

**Функции**

- `getLicensesModel(productId)`
- `getProductIdByName(productName)`
- `getProductIdByLicenseId(licenseId)`

---

<a id="imtgeogui"></a>
## `imtgeogui` — Адреса и географические структуры

Небольшой набор компонентов для работы с адресами: дерево адресов, редактор адреса и представление адресной структуры.

**Импорт:** `import imtgeogui 1.0` · **Файлов:** 3

### Views/ — Представления

<a id="type-imtgeogui-AddressTreeView"></a>
#### `AddressTreeView`

Визуальное представление адреса: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/AddressTreeView.qml](imtgeogui/Views/AddressTreeView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `addressListCommandId: string`
- `subscriptionCommandId: string`
- `searchNameId: string`
- `idParam: string`
- `nameParam: string`
- `fullAddressParam: string`
- `latitudeParam: string`
- `longitudeParam: string`
- `typeIdParam: string`
- `parentIdsParam: string`
- `hasChildrenParam: string`
- `idsToOpen: string`
- `textColor: string`
- `searchFields: var`
- `searchComp: alias`
- `searchCompModel: alias`
- `treeViewModel: alias`
- `treeView: alias`
- `treeLoading: alias`
- `moveToAnim: alias`
- `addressTreeInputObjectComp: Component`
- `addressTreeSdlObjectComp: Component`
- `additionalDelegateComp: Component`
- `addressTreeRequest: GqlSdlRequestSender`

**Сигналы**

- `searchFinished(string itemId, int index)`
- `treeClicked(int index)`
- `treeViewSelectedIndexChanged(int index)`
- `searchFieldClear()`
- `treeViewInserted(int index)`

**Функции**

- `setCustomFilters(inputParams)`
- `setExternalAddress(address, parentIds)`
- `getFirstIdToOpen()`
- `removeFirstIdToOpen()`
- `openNestedTree(ids, startIndex)`
- `openReaction(index)`
- `handleSubscription(dataModel)`

<a id="type-imtgeogui-StructureAddressView"></a>
#### `StructureAddressView`

Визуальное представление адреса: отображает данные и обрабатывает действия пользователя.

- **Файл:** [Views/StructureAddressView.qml](imtgeogui/Views/StructureAddressView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `addressModel: TreeItemModel`
- `selectedItemAddressModel: TreeItemModel`
- `addedItemAddressModel: TreeItemModel`
- `addressTypeNames: var`
- `parentId: string`
- `parentName: string`
- `isEdit: bool`
- `addressListCommandId: string`
- `searchNameId: string`
- `idParam: string`
- `nameParam: string`
- `fullAddressParam: string`
- `latitudeParam: string`
- `longitudeParam: string`
- `typeIdParam: string`
- `parentIdsParam: string`
- `hasChildrenParam: string`
- `textColor: string`
- `searchComp: alias`
- `addressTreeRequest: alias`
- `treeView: alias`
- `treeLoading: alias`
- `treeBody: alias`
- `addressTreeInputObjectComp: Component`
- `addressTreeSdlObjectComp: Component`
- `addAddressRequest: GqlRequestSender`
- `updateAddressRequest: GqlRequestSender`
- `removeAddressRequest: GqlSdlRequestSender`

**Функции**

- `getAddressTypeName(index)`
- `setCustomFilters(inputParams)`
- `setDataToAddressModel(typeId, typeValue, nameValue, itemId, parentId, latitude, longitude, index)`

### Editors/ — Редакторы

<a id="type-imtgeogui-AddressEditor"></a>
#### `AddressEditor`

Редактор адреса — форма для создания и изменения сущности с валидацией ввода.

- **Файл:** [Editors/AddressEditor.qml](imtgeogui/Editors/AddressEditor.qml)
- **Базовый тип:** [`Column`](https://doc.qt.io/qt-6/qml-qtquick-column.html)

**Свойства**

- `model: var`
- `isValid: bool`
- `addressTypeNames: var`
- `typesModel: TreeItemModel`

**Функции**

- `updateGui()`

---

<a id="imt3dgui"></a>
## `imt3dgui` — 3D-сцены (обёртки над Qt Quick 3D)

Тонкие QML-обёртки над типами Qt Quick 3D (и модулей Helpers/Particles3D), сгруппированные по назначению: сцена и узлы, камеры, источники света, материалы, текстуры, окружение, геометрия, инстансинг, скелетная анимация, частицы, эффекты постобработки и вспомогательные утилиты.

**Импорт:** `import imt3dgui 1.0` · **Файлов:** 54

### Scene/ — Сцена и узлы

<a id="type-imt3dgui-Loader3D"></a>
#### `Loader3D`

Асинхронная загрузка 3D-компонента — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Loader3D.qml](imt3dgui/Scene/Loader3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `sourceComponent: var`
- `source: string`
- `asynchronous: bool`
- `active: bool`
- `item: var`
- `Null: int` — только чтение
- `Ready: int` — только чтение
- `Loading: int` — только чтение
- `Error: int` — только чтение
- `status: int`

<a id="type-imt3dgui-Model"></a>
#### `Model`

3D-модель (меш + материалы) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Model.qml](imt3dgui/Scene/Model.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `source: string`
- `materials: var`
- `castsShadows: bool`
- `receivesShadows: bool`
- `geometry: var`
- `instancing: var`
- `instanceTable: var`
- `skeleton: var`
- `morphTargets: var`
- `skin: var`
- `pickable: int`
- `depthBias: int`

<a id="type-imt3dgui-Node3D"></a>
#### `Node3D`

Узел сцены (трансформации) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Node3D.qml](imt3dgui/Scene/Node3D.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`AmbientLight`](#type-imt3dgui-AmbientLight), [`Attractor3D`](#type-imt3dgui-Attractor3D), [`AxisHelper`](#type-imt3dgui-AxisHelper), [`CustomCamera`](#type-imt3dgui-CustomCamera), [`DirectionalLight`](#type-imt3dgui-DirectionalLight), [`FrustumCamera`](#type-imt3dgui-FrustumCamera), [`Gravity3D`](#type-imt3dgui-Gravity3D), [`InstanceModel`](#type-imt3dgui-InstanceModel), [`Joint`](#type-imt3dgui-Joint), [`Loader3D`](#type-imt3dgui-Loader3D), [`Model`](#type-imt3dgui-Model), [`OrthographicCamera`](#type-imt3dgui-OrthographicCamera), [`ParticleEmitter3D`](#type-imt3dgui-ParticleEmitter3D), [`ParticleSystem3D`](#type-imt3dgui-ParticleSystem3D), [`PerspectiveCamera`](#type-imt3dgui-PerspectiveCamera), [`PointLight`](#type-imt3dgui-PointLight), [`ReflectionProbe`](#type-imt3dgui-ReflectionProbe), [`Repeater3D`](#type-imt3dgui-Repeater3D), [`Skeleton`](#type-imt3dgui-Skeleton), [`SpotLight`](#type-imt3dgui-SpotLight), [`Wander3D`](#type-imt3dgui-Wander3D)

**Свойства**

- `position: vector3d`
- `eulerRotation: vector3d`
- `rotation: quaternion`
- `scale: vector3d`
- `pivot: vector3d`
- `visible3D: bool`
- `opacity3D: real`
- `objectName3D: string`
- `staticFlags: int`
- `forward: vector3d` — только чтение
- `up: vector3d` — только чтение
- `right: vector3d` — только чтение
- `childNodes: list<QtObject>`

**Функции**

- `mapPositionToScene(localPos)`
- `mapPositionFromScene(scenePos)`
- `mapDirectionToScene(localDir)`
- `mapDirectionFromScene(sceneDir)`

<a id="type-imt3dgui-PickResult"></a>
#### `PickResult`

Результат выбора (пикинга) объекта — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/PickResult.qml](imt3dgui/Scene/PickResult.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `objectHit: var`
- `distance: real`
- `position: vector3d`
- `normal: vector3d`
- `uvPosition: vector2d`
- `scenePosition: vector3d`
- `sceneNormal: vector3d`

<a id="type-imt3dgui-Render3D"></a>
#### `Render3D`

Управление рендерингом 3D — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Render3D.qml](imt3dgui/Scene/Render3D.qml)
- **Базовый тип:** [`Canvas`](https://doc.qt.io/qt-6/qml-qtquick-canvas.html)

**Свойства**

- `camera: var`
- `environment: var`
- `clearColor: color`
- `sceneNodes: list<QtObject>`

<a id="type-imt3dgui-Repeater3D"></a>
#### `Repeater3D`

Повторитель 3D-объектов по модели — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Repeater3D.qml](imt3dgui/Scene/Repeater3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `source: var`
- `count: int`
- `depthSortingEnabled: bool`

<a id="type-imt3dgui-Scene3DView"></a>
#### `Scene3DView`

Представление 3D-сцены — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/Scene3DView.qml](imt3dgui/Scene/Scene3DView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `cameraDistance: real`
- `rotationX: real`
- `rotationY: real`
- `backgroundColor: color`

<a id="type-imt3dgui-View3D"></a>
#### `View3D`

Область отображения 3D-сцены — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Scene/View3D.qml](imt3dgui/Scene/View3D.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `camera: var`
- `environment: var`
- `importScene: var`
- `renderMode: real`
- `ready3D: bool`
- `clearColor: color`
- `renderFormat: int`
- `samples: int`
- `renderStats: var`
- `sceneNodes: list<QtObject>`

**Сигналы**

- `ready()`
- `sceneGraphChanged()`

**Функции**

- `pick(x, y)`
- `mapFrom3DScene(scenePos)`
- `mapTo3DScene(viewPos)`

### Cameras/ — Камеры

<a id="type-imt3dgui-CustomCamera"></a>
#### `CustomCamera`

Камера с произвольной матрицей проекции — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Cameras/CustomCamera.qml](imt3dgui/Cameras/CustomCamera.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `projection: matrix4x4`

<a id="type-imt3dgui-FrustumCamera"></a>
#### `FrustumCamera`

Камера с настраиваемой пирамидой видимости — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Cameras/FrustumCamera.qml](imt3dgui/Cameras/FrustumCamera.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `clipNear: real`
- `clipFar: real`
- `top: real`
- `bottom: real`
- `left: real`
- `right: real`

<a id="type-imt3dgui-OrthographicCamera"></a>
#### `OrthographicCamera`

Ортографическая камера — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Cameras/OrthographicCamera.qml](imt3dgui/Cameras/OrthographicCamera.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `clipNear: real`
- `clipFar: real`
- `horizontalMagnification: real`
- `verticalMagnification: real`

<a id="type-imt3dgui-PerspectiveCamera"></a>
#### `PerspectiveCamera`

Перспективная камера — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Cameras/PerspectiveCamera.qml](imt3dgui/Cameras/PerspectiveCamera.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `fieldOfView: real`
- `clipNear: real`
- `clipFar: real`
- `frustumCullingEnabled: bool`
- `lookAtNode: var`

### Lights/ — Источники света

<a id="type-imt3dgui-AmbientLight"></a>
#### `AmbientLight`

Фоновое (ambient) освещение — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Lights/AmbientLight.qml](imt3dgui/Lights/AmbientLight.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `color: color`
- `brightness: real`

<a id="type-imt3dgui-DirectionalLight"></a>
#### `DirectionalLight`

Направленный источник света — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Lights/DirectionalLight.qml](imt3dgui/Lights/DirectionalLight.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `ShadowMapQualityLow: int` — только чтение
- `ShadowMapQualityMedium: int` — только чтение
- `ShadowMapQualityHigh: int` — только чтение
- `ShadowMapQualityVeryHigh: int` — только чтение
- `color: color`
- `brightness: real`
- `castsShadow: bool`
- `shadowMapQuality: int`
- `shadowBias: real`
- `shadowMapFar: real`
- `bakeMode: int`
- `scope: var`
- `shadowColor: color`
- `shadowFactor: real`

<a id="type-imt3dgui-PointLight"></a>
#### `PointLight`

Точечный источник света — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Lights/PointLight.qml](imt3dgui/Lights/PointLight.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `color: color`
- `brightness: real`
- `constantFade: real`
- `linearFade: real`
- `quadraticFade: real`
- `castsShadow: bool`
- `shadowMapQuality: int`
- `shadowBias: real`
- `shadowMapFar: real`
- `bakeMode: int`
- `scope: var`

<a id="type-imt3dgui-SpotLight"></a>
#### `SpotLight`

Прожектор (конусный свет) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Lights/SpotLight.qml](imt3dgui/Lights/SpotLight.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `color: color`
- `brightness: real`
- `coneAngle: real`
- `innerConeAngle: real`
- `castsShadow: bool`
- `shadowMapQuality: int`
- `shadowBias: real`
- `shadowMapFar: real`
- `bakeMode: int`
- `scope: var`

### Materials/ — Материалы

<a id="type-imt3dgui-CustomMaterial"></a>
#### `CustomMaterial`

Пользовательский материал (шейдеры) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Materials/CustomMaterial.qml](imt3dgui/Materials/CustomMaterial.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Opaque: int` — только чтение
- `Transparent: int` — только чтение
- `vertexShader: string`
- `fragmentShader: string`
- `shadingMode: int`
- `destinationBlend: int`
- `sourceBlend: int`
- `alwaysDirty: bool`
- `textureProperties: var`

<a id="type-imt3dgui-DefaultMaterial"></a>
#### `DefaultMaterial`

Базовый материал — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Materials/DefaultMaterial.qml](imt3dgui/Materials/DefaultMaterial.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `diffuseColor: color`
- `specularTint: color`
- `shininess: real`
- `opacity: real`
- `emissiveColor: color`
- `diffuseMap: var`
- `normalMap: var`

<a id="type-imt3dgui-PrincipledMaterial"></a>
#### `PrincipledMaterial`

PBR-материал (Principled) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Materials/PrincipledMaterial.qml](imt3dgui/Materials/PrincipledMaterial.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Default: int` — только чтение
- `Mask: int` — только чтение
- `Blend: int` — только чтение
- `Opaque: int` — только чтение
- `baseColor: color`
- `metalness: real`
- `roughness: real`
- `opacity: real`
- `emissiveColor: color`
- `baseColorMap: var`
- `metalnessMap: var`
- `roughnessMap: var`
- `normalMap: var`
- `emissiveMap: var`
- `occlusionMap: var`
- `heightMap: var`
- `alphaMode: int`
- `alphaCutoff: real`
- `normalStrength: real`
- `specularAmount: real`
- `specularTint: color`
- `clearcoatAmount: real`
- `clearcoatRoughnessAmount: real`
- `transmissionFactor: real`
- `pointSize: real`
- `lineWidth: real`

<a id="type-imt3dgui-SpecularGlossyMaterial"></a>
#### `SpecularGlossyMaterial`

Материал Specular/Glossy (PBR) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Materials/SpecularGlossyMaterial.qml](imt3dgui/Materials/SpecularGlossyMaterial.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `albedoColor: color`
- `specularColor: color`
- `glossiness: real`
- `opacity: real`
- `emissiveColor: color`
- `albedoMap: var`
- `specularMap: var`
- `glossinessMap: var`
- `normalMap: var`
- `emissiveMap: var`
- `occlusionMap: var`

### Textures/ — Текстуры

<a id="type-imt3dgui-CubeMapTexture"></a>
#### `CubeMapTexture`

Кубическая карта (cubemap) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Textures/CubeMapTexture.qml](imt3dgui/Textures/CubeMapTexture.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `source: string`

<a id="type-imt3dgui-ProceduralTexture"></a>
#### `ProceduralTexture`

Процедурная текстура — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Textures/ProceduralTexture.qml](imt3dgui/Textures/ProceduralTexture.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `width: int`
- `height: int`
- `RGBA8: int` — только чтение
- `RGBA16F: int` — только чтение
- `RGBA32F: int` — только чтение
- `format: int`
- `tilingModeHorizontal: int`
- `tilingModeVertical: int`

**Сигналы**

- `textureDataUpdated()`

<a id="type-imt3dgui-Texture"></a>
#### `Texture`

Текстура — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Textures/Texture.qml](imt3dgui/Textures/Texture.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `ClampToEdge: int` — только чтение
- `MirroredRepeat: int` — только чтение
- `Repeat: int` — только чтение
- `UV0: int` — только чтение
- `UV1: int` — только чтение
- `Environment: int` — только чтение
- `LightProbe: int` — только чтение
- `None: int` — только чтение
- `Linear: int` — только чтение
- `Nearest: int` — только чтение
- `source: string`
- `tilingModeHorizontal: int`
- `tilingModeVertical: int`
- `flipU: bool`
- `flipV: bool`
- `scaleU: real`
- `scaleV: real`
- `pivotU: real`
- `pivotV: real`
- `rotationUV: real`
- `mappingMode: int`
- `minFilter: int`
- `magFilter: int`
- `mipFilter: int`
- `generateMipmaps: bool`

### Environment/ — Окружение

<a id="type-imt3dgui-ExtendedSceneEnvironment"></a>
#### `ExtendedSceneEnvironment`

Расширенное окружение (эффекты кадра) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Environment/ExtendedSceneEnvironment.qml](imt3dgui/Environment/ExtendedSceneEnvironment.qml)
- **Базовый тип:** [`SceneEnvironment`](#type-imt3dgui-SceneEnvironment)

**Свойства**

- `aoEnabled: bool`
- `aoStrength: real`
- `aoDistance: real`
- `aoSoftness: real`
- `aoSampleRate: int`
- `aoBias: real`
- `glowEnabled: bool`
- `glowStrength: real`
- `glowIntensity: real`
- `glowBloom: real`
- `glowBlendMode: int`
- `glowQualityHigh: int`
- `depthOfFieldEnabled: bool`
- `depthOfFieldFocusDistance: real`
- `depthOfFieldFocusRange: real`
- `depthOfFieldBlurAmount: real`
- `vignetteEnabled: bool`
- `vignetteStrength: real`
- `vignetteColor: color`
- `vignetteRadius: real`
- `lensFlareEnabled: bool`
- `lensFlareBloomScale: real`
- `lensFlareBloomBias: real`
- `lensFlareDistortion: real`
- `colorAdjustmentsEnabled: bool`
- `colorAdjustmentsBrightness: real`
- `colorAdjustmentsContrast: real`
- `colorAdjustmentsSaturation: real`

<a id="type-imt3dgui-Fog"></a>
#### `Fog`

Туман сцены — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Environment/Fog.qml](imt3dgui/Environment/Fog.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `enabled: bool`
- `color: color`
- `density: real`
- `depthNear: real`
- `depthFar: real`
- `depthEnabled: bool`
- `heightMin: real`
- `heightMax: real`
- `heightEnabled: bool`
- `leastIntenseY: real`
- `mostIntenseY: real`
- `transmitCurve: real`
- `transmitMinimum: real`

<a id="type-imt3dgui-ReflectionProbe"></a>
#### `ReflectionProbe`

Зонд отражений — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Environment/ReflectionProbe.qml](imt3dgui/Environment/ReflectionProbe.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `Infinite: int` — только чтение
- `Box: int` — только чтение
- `parallaxCorrection: bool`
- `boxSize: int`
- `boxOffset: vector3d`
- `quality: int`
- `refreshInterval: real`
- `timeSlicing: bool`

<a id="type-imt3dgui-SceneEnvironment"></a>
#### `SceneEnvironment`

Параметры окружения сцены — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Environment/SceneEnvironment.qml](imt3dgui/Environment/SceneEnvironment.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`ExtendedSceneEnvironment`](#type-imt3dgui-ExtendedSceneEnvironment)

**Свойства**

- `Color: int` — только чтение
- `SkyBox: int` — только чтение
- `Transparent: int` — только чтение
- `NoTonemap: int` — только чтение
- `TonemapModeLinear: int` — только чтение
- `TonemapModeAces: int` — только чтение
- `TonemapModeFilmic: int` — только чтение
- `NoAA: int` — только чтение
- `SSAA: int` — только чтение
- `MSAA: int` — только чтение
- `Medium: int` — только чтение
- `High: int` — только чтение
- `VeryHigh: int` — только чтение
- `clearColor: color`
- `backgroundMode: int`
- `lightProbe: var`
- `tonemapMode: int`
- `antialiasingMode: int`
- `antialiasingQuality: int`
- `fog: var`
- `effects: var`
- `skyBoxCubeMap: var`
- `probeExposure: real`
- `probeHorizon: real`
- `depthTestEnabled: bool`
- `depthPrePassEnabled: bool`

### Geometry/ — Геометрия

<a id="type-imt3dgui-Geometry"></a>
#### `Geometry`

Пользовательская геометрия меша — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Geometry/Geometry.qml](imt3dgui/Geometry/Geometry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Position: int` — только чтение
- `Normal: int` — только чтение
- `TexCoord0: int` — только чтение
- `TexCoord1: int` — только чтение
- `Tangent: int` — только чтение
- `Binormal: int` — только чтение
- `JointIndex: int` — только чтение
- `JointWeight: int` — только чтение
- `Color: int` — только чтение
- `UnsignedInt16: int` — только чтение
- `UnsignedInt32: int` — только чтение
- `Float32: int` — только чтение
- `name: string`
- `primitiveType: int`
- `vertexData: var`
- `indexData: var`
- `stride: int`
- `attributes: var`

<a id="type-imt3dgui-GridGeometry"></a>
#### `GridGeometry`

Геометрия координатной сетки — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Geometry/GridGeometry.qml](imt3dgui/Geometry/GridGeometry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `horizontalLines: int`
- `verticalLines: int`
- `horizontalStep: real`
- `verticalStep: real`

<a id="type-imt3dgui-HeightFieldGeometry"></a>
#### `HeightFieldGeometry`

Геометрия карты высот — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Geometry/HeightFieldGeometry.qml](imt3dgui/Geometry/HeightFieldGeometry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `source: string`
- `smoothShading: bool`
- `extents: vector3d`

### Instancing/ — Инстансинг

<a id="type-imt3dgui-InstanceList"></a>
#### `InstanceList`

Список экземпляров для инстансинга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Instancing/InstanceList.qml](imt3dgui/Instancing/InstanceList.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `instanceCount: int`
- `instances: list<QtObject>`

<a id="type-imt3dgui-InstanceListEntry"></a>
#### `InstanceListEntry`

Элемент списка экземпляров — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Instancing/InstanceListEntry.qml](imt3dgui/Instancing/InstanceListEntry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `position: vector3d`
- `eulerRotation: vector3d`
- `scale: vector3d`
- `color: color`
- `customData: var`

<a id="type-imt3dgui-InstanceModel"></a>
#### `InstanceModel`

Инстансированная модель — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Instancing/InstanceModel.qml](imt3dgui/Instancing/InstanceModel.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `source: string`
- `materials: var`
- `instancing: var`
- `instanceTable: var`
- `castsShadows: bool`
- `receivesShadows: bool`

<a id="type-imt3dgui-InstanceRange"></a>
#### `InstanceRange`

Диапазон случайных экземпляров — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Instancing/InstanceRange.qml](imt3dgui/Instancing/InstanceRange.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `from: int`
- `instanceCount: int`
- `positionMinimum: vector3d`
- `positionMaximum: vector3d`
- `scaleMinimum: vector3d`
- `scaleMaximum: vector3d`
- `eulerRotationMinimum: vector3d`
- `eulerRotationMaximum: vector3d`
- `colorMinimum: color`
- `colorMaximum: color`
- `randomSeed: bool`

<a id="type-imt3dgui-Instancing"></a>
#### `Instancing`

Инстансинг (базовый) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Instancing/Instancing.qml](imt3dgui/Instancing/Instancing.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `instanceCount: int`
- `hasTransparency: bool`
- `depthSortingEnabled: bool`
- `instanceBuffer: var`

### Skeleton/ — Скелет и анимация

<a id="type-imt3dgui-Joint"></a>
#### `Joint`

Сустав (кость) скелета — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Skeleton/Joint.qml](imt3dgui/Skeleton/Joint.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `index: int`
- `skeletonRoot: string`

<a id="type-imt3dgui-MorphTarget"></a>
#### `MorphTarget`

Морф-таргет (морфинг-анимация) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Skeleton/MorphTarget.qml](imt3dgui/Skeleton/MorphTarget.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `weight: real`
- `attributes: var`

<a id="type-imt3dgui-Skeleton"></a>
#### `Skeleton`

Скелет для скиннинга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Skeleton/Skeleton.qml](imt3dgui/Skeleton/Skeleton.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `joints: list<QtObject>`

### Particles/ — Частицы

<a id="type-imt3dgui-Attractor3D"></a>
#### `Attractor3D`

Аттрактор частиц — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/Attractor3D.qml](imt3dgui/Particles/Attractor3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `system: var`
- `particles: var`
- `position3D: vector3d`
- `duration: real`
- `strength: real`
- `enabled: bool`
- `hideAtEnd: bool`
- `useCachedPositions: int`

<a id="type-imt3dgui-Gravity3D"></a>
#### `Gravity3D`

Гравитация для частиц — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/Gravity3D.qml](imt3dgui/Particles/Gravity3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `system: var`
- `particles: var`
- `direction: vector3d`
- `magnitude: real`
- `enabled: bool`

<a id="type-imt3dgui-ModelParticle3D"></a>
#### `ModelParticle3D`

Частицы на основе 3D-модели — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/ModelParticle3D.qml](imt3dgui/Particles/ModelParticle3D.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `maxAmount: int`
- `color: color`
- `colorVariation: real`
- `unifiedColorVariation: bool`
- `fadeInDuration: real`
- `fadeOutDuration: real`
- `delegate: var`
- `particleScale: real`

<a id="type-imt3dgui-ParticleEmitter3D"></a>
#### `ParticleEmitter3D`

Эмиттер частиц — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/ParticleEmitter3D.qml](imt3dgui/Particles/ParticleEmitter3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `Point: int` — только чтение
- `Line: int` — только чтение
- `Area: int` — только чтение
- `system: var`
- `particle: var`
- `shape: int`
- `emitRate: real`
- `lifeSpan: int`
- `lifeSpanVariation: int`
- `particleEndScale: vector3d`
- `particleRotation: vector3d`
- `particleRotationVariation: vector3d`
- `particleRotationVelocity: vector3d`
- `particleRotationVelocityVariation: vector3d`
- `velocity: vector3d`
- `velocityVariation: vector3d`
- `enabled: bool`

<a id="type-imt3dgui-ParticleSystem3D"></a>
#### `ParticleSystem3D`

Система частиц — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/ParticleSystem3D.qml](imt3dgui/Particles/ParticleSystem3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `running: bool`
- `paused: bool`
- `logging: bool`
- `startTime: int`
- `time: real`
- `useRandomSeed: bool`
- `seed: int`
- `particles: list<QtObject>`

<a id="type-imt3dgui-SpriteParticle3D"></a>
#### `SpriteParticle3D`

Спрайтовые частицы — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/SpriteParticle3D.qml](imt3dgui/Particles/SpriteParticle3D.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Normal: int` — только чтение
- `Screen: int` — только чтение
- `maxAmount: int`
- `color: color`
- `colorTable: var`
- `colorVariation: real`
- `unifiedColorVariation: bool`
- `fadeInDuration: real`
- `fadeOutDuration: real`
- `blendMode: int`
- `sprite: var`
- `spriteSequenceFrameCount: int`
- `billboard: bool`
- `particleScale: real`

<a id="type-imt3dgui-Wander3D"></a>
#### `Wander3D`

Блуждание частиц — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Particles/Wander3D.qml](imt3dgui/Particles/Wander3D.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `system: var`
- `particles: var`
- `globalAmount: vector3d`
- `globalPace: vector3d`
- `globalPaceStart: vector3d`
- `uniqueAmount: vector3d`
- `uniquePace: vector3d`
- `uniqueAmountVariation: real`
- `uniquePaceVariation: real`
- `enabled: bool`

### Effects/ — Постобработка и эффекты

<a id="type-imt3dgui-Buffer"></a>
#### `Buffer`

Буфер прохода рендеринга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Effects/Buffer.qml](imt3dgui/Effects/Buffer.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Unknown: int` — только чтение
- `UByte: int` — только чтение
- `UByte4: int` — только чтение
- `Short2: int` — только чтение
- `Float16: int` — только чтение
- `Float32: int` — только чтение
- `name: string`
- `format: int`
- `sizeMultiplier: int`
- `hasSceneLifetime: bool`

<a id="type-imt3dgui-Command"></a>
#### `Command`

Команда прохода рендеринга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Effects/Command.qml](imt3dgui/Effects/Command.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `name: string`
- `destinationName: string`
- `bufferInput: var`

<a id="type-imt3dgui-Effect"></a>
#### `Effect`

Эффект постобработки — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Effects/Effect.qml](imt3dgui/Effects/Effect.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `passes: var`

<a id="type-imt3dgui-Pass"></a>
#### `Pass`

Проход рендеринга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Effects/Pass.qml](imt3dgui/Effects/Pass.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `shaders: var`
- `commands: var`
- `output: var`

<a id="type-imt3dgui-Shader"></a>
#### `Shader`

Шейдер прохода — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Effects/Shader.qml](imt3dgui/Effects/Shader.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `Vertex: int` — только чтение
- `Fragment: int` — только чтение
- `shader: string`
- `stage: int`

### Helpers/ — Вспомогательные

<a id="type-imt3dgui-AxisHelper"></a>
#### `AxisHelper`

Помощник осей координат — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Helpers/AxisHelper.qml](imt3dgui/Helpers/AxisHelper.qml)
- **Базовый тип:** [`Node3D`](#type-imt3dgui-Node3D)

**Свойства**

- `axisScale: real`
- `enableXZGrid: bool`
- `enableXYGrid: bool`
- `enableYZGrid: bool`

<a id="type-imt3dgui-DebugView"></a>
#### `DebugView`

Отладочная панель рендеринга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Helpers/DebugView.qml](imt3dgui/Helpers/DebugView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `source: var`
- `resourceDetailsVisible: bool`
- `wireframeEnabled: bool`

<a id="type-imt3dgui-RenderStats"></a>
#### `RenderStats`

Статистика рендеринга — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Helpers/RenderStats.qml](imt3dgui/Helpers/RenderStats.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `fps: real`
- `frameTime: real`
- `renderTime: real`
- `syncTime: real`
- `maxFrameTime: real`

<a id="type-imt3dgui-WasdController"></a>
#### `WasdController`

Управление камерой (WASD + мышь) — тонкая QML-обёртка над одноимённым типом Qt Quick 3D.

- **Файл:** [Helpers/WasdController.qml](imt3dgui/Helpers/WasdController.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `controlledObject: var`
- `speed: real`
- `shiftSpeed: real`
- `forwardSpeed: real`
- `backSpeed: real`
- `rightSpeed: real`
- `leftSpeed: real`
- `upSpeed: real`
- `downSpeed: real`
- `xSpeed: real`
- `ySpeed: real`
- `mouseEnabled: bool`
- `keysEnabled: bool`
- `inputsNeedProcessing: bool`
- `acceptedButtons: bool`

