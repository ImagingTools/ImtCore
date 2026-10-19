# Правила организации Permissions в решениях на базе ImtCore и Lisa

> **Назначение.** Единый свод правил: как описывать, привязывать, проверять и тестировать права доступа (permissions) в продуктах на ImtCore и Lisa.
> **Источник.** Код `ItDevelopment`: ImtCore `442be502b9` (2026-10-15), Lisa `a598f56` (PR #28 «Full-permissions»), ProLife `d4d9a910`, Puma `e8b53c6`; файлы Partitura (`.acc`), `*Features.h` продуктов, QML и тесты. Пути ниже даны относительно `ItDevelopment/`.
> **Состояние реализации.** Ветки `feature/permission-requirements` (ImtCore), `feature/organization-permissions-by-path` (Puma), `feature/permission-model-tests` (ProLife), `feature/permission-paths` (Lisa), 2026-10-19: права только путями, новые деревья Lisa/ProLife и организации, ревизии в каждой сущности.
> **Как читать.** Разделы 3–12 — правила, **целевое** поведение. Там, где код пока ведёт себя иначе, стоит ссылка на раздел 13. Раздел 13 — расхождения с кодом и открытые вопросы.

---

## 1. Роли компонентов

| Компонент | Что отвечает за permissions |
|---|---|
| **Lisa** | Каталог продуктов, фич и лицензий. Здесь редактируется **дерево фич продукта** (`FeatureEditor`), из него генерируется `<Product>Features.h` (генератор `ImtCore/Include/imtlicgql/CProductInfoFileGeneratorComp`). Сама Lisa — тоже продукт `Lisa` со своим деревом. |
| **ImtCore** | Рантайм: модель (`imtlic`, `imtauth`), проверка (`imtservergql`, `imtserverapp`, `imtauthgql`), GUI (`imtauthgui`, `imtlicgui`), готовые компоненты Partitura (`ImtCoreVoce.arp`, `ImtAuthVoce.arp`, `ImtUserAdministrationVoce.arp`). |
| **Puma** | Сервис учётных записей (пользователи, роли, группы, организации). Продуктовые серверы получают пользователя и его права через него (`AdminId = su`). |
| **Продукт** (Lisa, ProLife, NeoPro, Acula, Agentino, IotPlatform) | Описывает своё дерево фич, декларирует права команд серверной части и страниц GUI в своих `.acc`. |

## 2. Сквозная схема

```
Lisa FeatureEditor ──generate──► <Product>Features.h ──FillProduct()──► IProductInfo (дерево фич продукта)
                                                                              │
Роли / группы / локальные права (только листья) ──► Puma                     │
        права в продукте, фильтр по организации                               ▼
                                              CAuthenticationManagerComp: создаёт IGqlContext
                                              (user, tenant, product, scopes PAT, IsTenantOwner)
                                                                              │
                                              действующие права = (права ∩ scopes PAT)
                                              + включённые requirements, ∩ права организации
                                                                              │
              ┌───────────────────────────────────────────────────────────────┤
              ▼                                                               ▼
  Серверные команды (GraphQL)                                    GUI: страницы и команды
  CPermissibleGqlRequestHandlerComp                              CCommandsController / PagesController
  ICommandPermissionsProvider + IPermissionChecker               отдают только доступные элементы
              │                                                               │
              └──────────────── imtauth::HasPermission ◄──────────────────────┘
                                (единое правило сравнения)
```

**Принцип:** границу безопасности создаёт сервер. GUI (скрытие страниц, кнопок) — удобство, а не защита.

## 3. Описание permissions (дерево фич)

**R3.1. Единственный источник — дерево фич продукта в Lisa.** `<Product>Features.h` генерируется, руками не правится. Сервер при старте вызывает `FillProduct(*IProductInfo)` (`Lisa/Impl/LisaServer/Main.cpp`); для GraphQL-запроса `ProductPermissions` дерево читается из ресурса `:/Features/<Product>Features` (компонент `SingleProductPermission`). Lisa принимает `<Product>Features.h` и обратно — импортом продукта (как XML): файл создаёт новый продукт; продукт с тем же `ProductId` не импортируется, корень, который уже есть в каталоге фич под другим uuid, тоже. Фичи из файла записываются в каталог: новые добавляются, существующие переписываются версией из файла (диалог импорта предупреждает об этом).

**R3.2. Узел-permission:** `SetIsPermission(true)`, `SetOptional(false)`. Permission — право действия в приложении, а не продаваемая возможность; продаваемые фичи (`IsPermission=false`, `Optional=true`) в дерево прав не попадают. Во всех продуктах сейчас 100 % узлов — permission.

**R3.3. Идентификатор узла (`FeatureId`):** латиница, PascalCase, без `/`, уникален в пределах родителя. Сущность называет область, поэтому id внутри неё короткие (`View`, `Edit`, `Add`) и повторяются в разных областях — правом является только **полный путь** (раздел 4). Путь хранится в БД ролей и пользователей, переименование узла требует миграции. Отображаемое имя — через `QT_TRANSLATE_NOOP("Feature", "...")`.

**R3.4. Типовая структура области** (Lisa и ProLife, по образцу дерева организации R3.6):

```
/<Area>Management                     группа области (FeatureManagement, SensorManagement, ...)
├─ ViewPage                           лист: страница области (меню, страница)
├─ View                               лист: данные (коллекция, документы, команды)
├─ ViewAll, ViewHistory → View        листья, где они есть (ProLife)
├─ Edit → View                        группа: любое изменение
│  ├─ Add                             лист: создание
│  ├─ Change                          лист или группа полей: Change/{Description, SerialNumber, ...}
│  └─ Remove                          лист: удаление
└─ Revisions                          ревизии документов этой области
   ├─ View → <Area>/View
   ├─ Restore → Revisions/View
   └─ Delete → Revisions/View
```

**`ViewPage` и `View` независимы**, requirements между ними нет: данные области можно показать без её страницы (таблица датчиков в чужом редакторе, список выбора, API), а страница без `View` откроется пустой. Странице нужен `ViewPage`, данным — `View`, роль выдаёт оба, когда нужны оба.

Особые действия области — листья рядом с `View` (`/SensorManagement/Bind`, `/SensorManagement/CreateLicenseFile`) или внутри `Edit` (`/LicenseManagement/Edit/Split`, `/LicenseManagement/Edit/Revoke`). Администрирование: `/Administration/{Users, Roles, Groups}/{ViewPage, View, ViewHistory → View, Edit → View/{Add, Change, Remove}, Revisions}`. Рабочее место: `/WorkspaceManagement/{ViewPage, ViewUserActions, ViewAllUserActions → ViewUserActions}` (в ProLife ещё `ViewAnalytics`), поиск: `/SearchManagement/ViewPage` — у них нет своей коллекции. Отдельной сквозной группы ревизий больше нет (R6.7).

**R3.5. У пользователя и роли хранятся только листья** (полным путём). Группа — только способ сгруппировать листья: её нельзя выдать. Requirements на группу дают её листья, а не саму группу (раздел 5). Путь группы встречается только в **проверках** (R4.4).

**R3.6. Дерево организации — отдельное.** Права внутри организации задаёт `imtauth::COrganizationsPermissionsProviderComp`, а не продукт. Не смешивать с продуктовым деревом. Один корень, области по шаблону R3.4:

```
/OrganizationManagement
├─ Edit/{ChangeName, ChangeDescription}
├─ Members/{View, Edit → Members/View /{Invite, Exclude, ChangeRole}}
├─ Roles/{View, Edit → Roles/View /{Add, Change, Remove}}
├─ Groups/{View, Edit → Groups/View /{Add, Change, Remove}}
├─ Permissions/{View, Change → Permissions/View}
└─ Connections/{View, ViewCode, Edit → Connections/View /{Connect, Remove}}
```

Права этого дерева, как и продуктовые, хранятся и проверяются только полными путями (раздел 4).

**Два дерева `/OrganizationManagement` с разными задачами.**
- **Дерево организации** (выше) — статические права участника: что пользователь может делать **внутри** организации. Хранятся в членстве (`ITenantMembership::GetOrganizationPermissions`), проверяются сервером в самих командах (`imtauthgql::HasTenantManageAccess`, `CanManageMembers`), клиент получает их как `currentUserOrganizationPermissions`. Владелец, создатель и суперпользователь проходят без проверки листа.
- **Продуктовое дерево** (ProLife) отвечает только за видимость страницы, данных и команд:

```
/OrganizationManagement
├─ ViewPage        страница «Организации»
├─ View            данные и все команды организации (чтение, изменение, участники, связи, ...)
├─ Add             создание организации
├─ Remove          удаление организации
└─ Revisions/{View → View, Restore, Delete}
```

Внутренние команды организации объявлены продуктовым `/OrganizationManagement/View`; какое именно действие разрешено, решает лист участника:

| Команды | Лист участника |
|---|---|
| изменение организации (`UpdateTenant`) | `Edit/ChangeName` или `Edit/ChangeDescription` |
| `SetTenantActive`, `RemoveTenant` | только владелец, создатель, суперпользователь |
| участники и приглашения: чтение | `Members/View` (своё членство — без права) |
| участники и приглашения: приглашение, исключение, смена роли | любой лист `Members/Edit/*` (13.23) |
| `GetTenantPermissions` | `Permissions/View` или `Roles/View` |
| права участника другого пользователя (`GetOrganizationPermissions`) | `Permissions/View` |
| изменение прав участника (`UpdateMembershipPermissions`) | любой лист `Members/Edit/*` (13.23) |
| связи, запросы, предложения, договоры, гранты, сообщения, заявки — чтение | `Connections/View` |
| код подключения | `Connections/ViewCode` |
| создание, принятие, отклонение, отмена; договоры, гранты, сообщения, заявки — изменение; код подключения — изменение | `Connections/Edit/Connect` |
| удаление связи | `Connections/Edit/Remove` |

## 4. Формат идентификатора и правило сравнения

**Единственный формат — полный путь узла:** `/Administration/Users/Edit/Change` (`imtlic::CalculateFeaturePath`), всегда с ведущим `/`. «Голый» `FeatureId` (`ChangeUser`) правом **не является**: у пользователя он ничего не даёт, в проверке ничему не соответствует, requirements от него не раскрываются. Хранимые данные в голых id нужно перевести в пути (13.5).

**R4.1. Единое правило — `imtauth::HasPermission(права пользователя, что проверяем)`** (`ImtCore/Include/imtauth/imtauth.h`), на клиенте — `PermissionsController.hasPermission` (QML, см. 13.3). Любая проверка «есть ли право» идёт только через него; `contains`/`indexOf` на списке прав недопустимы.

**Дерево для всех примеров** — условное, то же, что в юнит-тестах `PermissionRequirementsTest` (в продуктах дерево устроено по R3.4; длинные id выбраны, чтобы примеры читались без расшифровки):

```
/Administration
├─ ViewAdministration          группа
│  ├─ ViewUsers                лист
│  └─ ViewRoles                лист
└─ EditAdministration          группа
   ├─ EditUser                 группа
   │  ├─ ChangeUser            лист
   │  └─ RemoveUser            лист
   └─ EditRole                 группа
      └─ ChangeRole            лист
/RevisionManagement            группа
├─ ViewRevisions               лист
└─ ExportRevision              лист
/Reports/ViewUsers             лист (тот же id ViewUsers под другим родителем)
```

**Правило сравнения**

| У пользователя (лист) | Проверяем | Результат |
|---|---|---|
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser/ChangeUser` | **да** — тот же лист |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser` | **да** — проверка группы = «есть хотя бы один лист внутри» |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration` | **да** — то же на любой глубине |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `/Administration/EditAdministration/EditUser/RemoveUser` | нет — соседний лист |
| `/Administration/ViewAdministration/ViewUsers` | `/Administration/EditAdministration` | нет — в этой группе у пользователя листьев нет |
| `/Administration/ViewAdministration/ViewUsers` | `/Reports/ViewUsers` | нет — тот же id под другим родителем — другое право |
| `ChangeUser` (голый, старые данные) | `/Administration/EditAdministration/EditUser/ChangeUser` | нет — голый id не право |
| `/Administration/EditAdministration/EditUser/ChangeUser` | `ChangeUser` (голый) | нет — проверка голого id всегда отказ |
| `/Administration/EditAdministration/EditUser` (группа, записанная как право) | `/Administration/EditAdministration/EditUser/ChangeUser` | нет — группа не выдаётся (R3.5) и листа не покрывает |
| любое | пустая строка | нет |

**R4.2.** Граница пути — символ `/`: `/Administration/EditAdministration/EditUser` покрывает `.../EditUser/ChangeUser`, но не `.../EditUsers` (другой узел с похожим именем).
**R4.3.** Всё — только полными путями: `.acc`, `Features.h`, C++, QML, requirements, scopes PAT, права ролей и пользователей. Общий компонент ImtCore, которому путь продукта заранее неизвестен, получает его от хоста свойством (`revisionsPermissionPath`, `permissionPath`), а не проверяет голый id. Декларация голым id не открывает команду никому, кроме суперпользователя и владельца организации (13.21).
**R4.4.** Путь группы допустим в **проверке** (например, `PagePermissions` содержит `/FeatureManagement/EditFeature`) и означает «у пользователя есть любой лист внутри». Выдать группу нельзя (R3.5).

## 5. Requirements — что право включает

**Смысл.** `requirements` узла — список прав, которые он **включает**. «`ChangeUser` → `ViewUsers`» значит: у кого есть `ChangeUser`, у того есть и `ViewUsers`. Обратно не действует: `ViewUsers` не даёт `ChangeUser`. Включённые права в роли не хранятся — они вычисляются при проверке.

**R5.1. Четыре вида зависимости.** Узел-источник — лист или группа, цель — лист или группа. В правах пользователя всегда оказываются только листья (R3.5).

| Зависимость | Пример | Что значит |
|---|---|---|
| лист → лист | `ChangeUser` → `ViewUsers` | у кого `ChangeUser`, у того и `ViewUsers` |
| группа → лист | группа `EditUser` → `ViewUsers` | любой лист `EditUser` (`ChangeUser`, `RemoveUser`, …) включает `ViewUsers` |
| лист → группа | `ChangeUser` → группа `ViewAdministration` | `ChangeUser` включает **все** листья `ViewAdministration` |
| группа → группа | группа `EditAdministration` → группа `ViewAdministration` | любой лист `EditAdministration` включает все листья `ViewAdministration` |

**R5.2. Как вычисляется** — на лету, при каждой проверке прав, по дереву продукта (`IProductInfo`) из контекста запроса:
1. Исходный набор — листья пользователя в продукте запроса после фильтра по организации; для PAT — их пересечение со scopes токена (раздел 10).
2. Каждый лист включает свои requirements и requirements всех групп, в которых он лежит.
3. Цель-лист даёт этот лист, цель-группа — все листья группы на момент проверки.
4. Полученные листья раскрываются так же, пока набор растёт: транзитивно, вместе с requirements их групп. Циклы допустимы.
5. Итог ограничивается правами организации (`GetTenantPermissions()`).
6. Это и есть **действующие права**; проверка — `HasPermission(действующие права, что проверяем)` (раздел 4).

Без `IProductInfo` в контексте requirements не раскрываются, и пользователь получает только выданные листья. Наличие `FeatureContainer` в `AuthenticationManager` проверяется при старте сервера.

**R5.3. Направление записи: «более сильное право → что оно включает»**: `Edit*` → `View*`, никогда наоборот. Удобнее всего одна запись на группе: группа `EditUser` → `ViewUsers` действует для `ChangeUser`, `RemoveUser`, `AddUser`. Так записаны деревья Lisa, ProLife и организации; NeoPro, IotPlatform, Acula — ещё в обратную сторону (13.1).

**R5.4. Ограничения** (проверяют `FeatureEditor` и генератор `Features.h`):
- цель — существующий узел того же продукта, записанный полным путём; внешнее требование в `LicenseEditor` помечается «from outside this product»;
- узел не может зависеть **от себя, от своих предков и от своих потомков**.

**R5.5. Группа-цель раскрывается в момент проверки.** Новый лист, добавленный в группу-цель, сразу получают все, у кого есть зависимость от этой группы, хотя их роли не менялись. Поэтому `FeatureEditor` показывает у каждого узла итог («`ChangeUser` включает: `ViewUsers`, `ViewRoles`») и предупреждает, когда лист добавляют в группу, на которую есть зависимости.

**R5.6. Клиент получает от сервера действующие права** (выданные + включённые), иначе кнопки, открытые включёнными правами, будут скрыты. Редактор ролей показывает включённые права отдельно, неактивными: убрать `ViewUsers`, пока у роли есть `ChangeUser`, нельзя.

### Примеры

Дерево — из раздела 4. В таблицах имена — сокращения полных путей: `ChangeUser` = `/Administration/EditAdministration/EditUser/ChangeUser`, группа `EditUser` = `/Administration/EditAdministration/EditUser` и т. д. `→` читается как «включает».

**Лист → лист**

| У пользователя | Requirements | Проверяем | Результат |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ChangeUser` | **да** — право выдано |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **да** — включено правом `ChangeUser` |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | `ViewRoles` | нет — включён только `ViewUsers` |
| `ViewUsers` | `ChangeUser` → `ViewUsers` | `ChangeUser` | нет — View не даёт Edit |
| `ChangeUser` | `ChangeUser` → `ViewUsers` | группа `ViewAdministration` | **да** — включённый `ViewUsers` лежит в группе |
| `ChangeUser` | `ChangeUser` → `ViewUsers`, `ViewUsers` → `ViewRevisions` | `ViewRevisions` | **да** — включение транзитивно |

**Группа → лист**

| У пользователя | Requirements | Проверяем | Результат |
|---|---|---|---|
| `RemoveUser` | группа `EditUser` → `ViewUsers` | `ViewUsers` | **да** — зависимость группы действует для её листа |
| `ChangeRole` | группа `EditUser` → `ViewUsers` | `ViewUsers` | нет — `ChangeRole` не в `EditUser` |

**Лист → группа**

| У пользователя | Requirements | Проверяем | Результат |
|---|---|---|---|
| `ChangeUser` | `ChangeUser` → группа `ViewAdministration` | `ViewRoles` | **да** — включены все листья группы |
| `ChangeUser` | `ChangeUser` → группа `ViewAdministration` | `/Reports/ViewUsers` | нет — лист с тем же id вне группы не включён |
| `ViewRoles` | `ChangeUser` → группа `ViewAdministration` | `ChangeUser` | нет — обратно не действует |

**Группа → группа**

| У пользователя | Requirements | Проверяем | Результат |
|---|---|---|---|
| `ChangeRole` | группа `EditAdministration` → группа `ViewAdministration` | `ViewUsers` | **да** — любой лист Edit-ветки включает все листья View-ветки |
| `ViewUsers` | группа `EditAdministration` → группа `ViewAdministration` | `ChangeRole` | нет — обратно не действует |
| `ChangeUser` | группа `EditAdministration` → группа `ViewAdministration`, группа `ViewAdministration` → `ViewRevisions` | `ViewRevisions` | **да** — включённые листья приносят requirements своей группы |
| `ChangeUser` | то же | `ExportRevision` | нет — включён только `ViewRevisions` |

**Границы и особые случаи**

| У пользователя | Requirements | Проверяем | Результат |
|---|---|---|---|
| `ChangeUser`; PAT со scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `ViewUsers` | **да** — включение раскрывается от scopes токена |
| `ChangeUser`, `RemoveUser`; PAT со scopes `[ChangeUser]` | `ChangeUser` → `ViewUsers` | `RemoveUser` | нет — вне scopes |
| `ChangeUser`; организации разрешён `ChangeUser`, но не `ViewUsers` | `ChangeUser` → `ViewUsers` | `ViewUsers` | нет — граница организации. Редактор прав организации такой набор не сохранит: он сам добавляет включённые листья (раздел 10) |
| `ChangeUser`; в группу `ViewAdministration` добавили новый лист `ViewGroups` | `ChangeUser` → группа `ViewAdministration` | `ViewGroups` | **да** — сразу после обновления дерева, без изменения ролей (R5.5) |
| `ChangeUser` (голый id, старые данные) | `ChangeUser` → `ViewUsers` | `ViewUsers` | нет — голый id не право и ничего не включает; старые данные переводятся в пути отдельно (13.5) |
| `ChangeUser`; в контексте нет `IProductInfo` | `ChangeUser` → `ViewUsers` | `ViewUsers` | нет — requirements не раскрыты, это ошибка конфигурации (R5.2) |

**Запрещённые зависимости** (генератор и `FeatureEditor` их отклоняют, R5.4)

| Зависимость | Почему нельзя |
|---|---|
| `ChangeUser` → `ChangeUser` | зависимость от себя |
| `ChangeUser` → группа `EditUser` | зависимость от своего родителя: `ChangeUser` получил бы `RemoveUser` и `AddUser` |
| `ChangeUser` → группа `/Administration` | зависимость от предка: раздала бы всю область, включая `ChangeRole` |
| группа `EditUser` → `ChangeUser` | зависимость от своего потомка |
| группа `EditAdministration` → группа `EditUser` | зависимость от своего потомка: `ChangeRole` получил бы `RemoveUser` |

## 6. Привязка permissions к серверным командам

### 6.1. Компоненты (Partitura, пакет `ImtServerAppPck`)

| Компонент | Назначение |
|---|---|
| `SingleCommandPermissionsProvider` | Один `CommandId` + список `CommandPermissions`. |
| `CommandPermissionsProviderJoiner` | Склеивает набор провайдеров в один (`CommandPermissionsProvider[]`); при совпадении `CommandId` побеждает **первый** провайдер (13.9). |
| `ObjectCollectionPermissions` (`ImtAuthVoce`) | Готовое сопоставление стандартных команд коллекции (GetObjectData, InsertNewObject, …) с правами. |
| `CPermissibleGqlRequestHandlerComp` и наследники | Обработчик с атрибутами `CommandPermissions` (провайдер) и `PermissionChecker`. |
| `ImtAuthPck/CheckPermissions` / `CheckPermissionOr` | Чекер **«все»** (AND) / **«любое»** (OR). |

### 6.2. Алгоритм проверки (`imtservergql::CheckRequestPermissions`)

1. Провайдер не подключён → **разрешить** (13.10).
2. Нет контекста или пользователя → **отказать**.
3. Суперпользователь (`IsAdmin`) или владелец организации (`IsTenantOwner`) → **разрешить** (для PAT см. 13.16).
4. Команды нет в провайдере → **отказать** (`denyUndeclared = true`).
5. Чекер не подключён или список прав команды пуст → **разрешить**.
6. В запросе есть заголовок `permissionPath` (6.4) → путь должен **совпадать с одним из прав команды или лежать внутри него**, и он должен быть у пользователя среди действующих прав.
7. Иначе чекер сверяет **действующие** права пользователя (раздел 5) с декларированным списком.

### 6.3. Правила

- **R6.1.** Каждая команда обработчика, наследующего `CPermissibleGqlRequestHandlerComp`, объявляется в провайдере. Необъявленная команда отклоняется.
- **R6.2.** Всегда подключать **оба** атрибута (`CommandPermissions` и `PermissionChecker`). Если любой не задан — обработчик открыт для всех (fail-open, см. `ImtCore/Docs/GQL_CONTEXT_THREAD_PROPAGATION_BUG.md`).
- **R6.3.** Публичные команды (логин, токен, обновление сессии, регистрация) не оформляются как permissible.
- **R6.4.** Соглашение «действие → право» (R3.4): чтение → `<Area>/View`; создание → `<Area>/Edit/Add`; изменение, переименование, описание → `<Area>/Edit/Change` (или лист поля `Edit/Change/<Field>`); удаление → `<Area>/Edit/Remove`. Образец — Lisa `ProductPermissions.acc`. Экспорт → `<Area>/View`, импорт → `<Area>/Edit/Add`, восстановление удалённого → `<Area>/Edit/Remove` (13.21).
- **R6.5.** Серверная команда обычно получает **одно** право; при нескольких — AND (`CheckPermissions`). Элементы GUI (страницы, кнопки) проверяются чекером **OR** (`Command.acc`, `Page.acc` используют `CheckPermissionOr`).
- **R6.6.** Для стандартных коллекций использовать `ObjectCollectionPermissions`: `GetObjectData/Headers/ElementIds/MetaInfo/…` → `<Area>/View`, `InsertNewObject` → `<Area>/Edit/Add`, `SetObjectData/Name/Description` → `<Area>/Edit/Change`, `RemoveElements(Set)` → `<Area>/Edit/Remove`.
- **R6.7. Ревизии — подгруппа `Revisions` каждой области** (R3.4). Контроллер ревизий (`CDocumentRevisionControllerComp`, удалённый `CRemoteDocumentRevisionControllerComp`) получает атрибут `RevisionPermissionPaths` — по одному пути на каждую коллекцию `CollectionIds`, в том же порядке (`Devices` → `/SensorManagement/Revisions`). Команда требует лист своего действия в области коллекции: `GetRevisionInfoList` → `Revisions/View`, `RestoreRevision` → `Revisions/Restore`, `DeleteRevision` → `Revisions/Delete` (`imtservergql::CheckRevisionPermissions`). Коллекция без пути и любая другая команда (`BackupRevision`, `ExportRevision` — заглушки) отклоняются. Ревизии одной сущности не открывают ревизии другой. Клиентские компоненты истории (`DocumentHistoryView`, `DocumentRevisionDialog`, делегаты коллекций) получают путь области от хоста свойством `revisionsPermissionPath`.
- **R6.8.** В декларациях прав не использовать `*`: чекер OR пропускает с ним любого пользователя (13.15).

### 6.4. `permissionPath`

Многоразовый компонент (панель тикетов, чат, `ClientStyle`) встраивается в разные места. Хост передаёт ему `permissionPath` (QML-свойство → HTTP-заголовок `permissionPath`), путь всегда начинается с `/` (при отсутствии добавляется). Использовать, когда одна команда обслуживает несколько контекстов с разными правами.

Пример (условный): команда `TicketsList` объявлена с правами `/Support/ViewTickets` и `/Devices/ViewDeviceTickets`.

| Заголовок `permissionPath` | Действующие права пользователя | Результат |
|---|---|---|
| `/Devices/ViewDeviceTickets` | `/Devices/ViewDeviceTickets` | **да** — путь объявлен для команды и есть у пользователя |
| `/Devices/ViewDeviceTickets` | `/Support/ViewTickets` | нет — команда выполняется в контексте устройств, право поддержки там не действует |
| `/Devices` | `/Devices/ViewDeviceTickets` | нет — путь шире объявленного права; иначе подошёл бы любой лист `/Devices` (13.12) |

## 7. Страницы и команды GUI

- **Страницы.** `PagePermissions` = `['<Area>/ViewPage']` — только лист страницы (R3.4). Команды и данные коллекции, в том числе `GetCommands`, остаются на `<Area>/View`: коллекция может показываться и без страницы. Страница Administration открывается любым из `/Administration/{Users, Roles, Groups}/ViewPage`, её вложенные страницы — своим `ViewPage` (`AdministrationView.qml`). Пример — `Lisa/Partitura/LisaQmlVoce.arp/Pages.acc`.
- **Команды коллекции и документа.** Атрибуты `NewPermissions`, `EditPermissions`, `SavePermissions`, `UndoPermissions`, `RedoPermissions`, `ClosePermissions` в `StandardCollectionCommands` / `StandardDocumentCommands`. Образец (Lisa, Features): `New` → `/FeatureManagement/Edit/Add`; `Edit` → `Edit/Change | Edit/Remove | View`; `Save/Undo/Redo` → `Edit/Add | Edit/Remove | Edit/Change`; `Close` → любое, включая `View`.
- **Фильтрация на сервере.** `CCommandsControllerComp` и контроллер страниц отдают клиенту только элементы, доступные по **действующим** правам. Это обязательный путь для **меню, страниц и командных панелей**.
- **Локальные проверки в QML.** `PermissionsController.checkPermission(id)` (синглтон `imtauthgui`): суперпользователь (`login === "su"`) → `true`, иначе `hasPermission(permissions, path)` по действующим правам, полученным с сервера (R5.6), по тому же правилу, что и на сервере (раздел 4): только пути, проверка группы — «есть лист внутри».
- **R7.1.** GUI-проверка дублирует, но не заменяет серверную. Любое действие, доступное через кнопку, защищено серверной командой.
- **R7.2.** Один компонент — один постоянный набор прав. Не кодировать права в QML «в обход» `.acc`: страницы и команды получают их из Partitura.

## 8. Откуда берутся права пользователя

```
права(product, tenant) =  права ролей пользователя в продукте
                          (локальные права роли + родительские роли, транзитивно)
                       ∪ права групп пользователя (родительские группы, роли групп)
                       ∪ локальные права пользователя
   → только роли/группы/права, привязанные к текущей организации
   → (для PAT) ∩ scopes токена
   → + права, включённые requirements (раздел 5), ∩ права организации
   = действующие права
```

- **R8.1. Права привязаны к продукту** (`IRole::GetProductId`, `IUserBaseInfo::GetRoles(productId)`, `GetPermissions(productId)`). Продукт запроса определяет сервер по своей конфигурации, а не клиент (13.13). Роли другого продукта не открывают команды этого продукта.
- **R8.2. Организация (tenant).** Для организации учитываются только роли, группы и права, **привязанные** к ней; для «Без организации» — только непривязанные (глобальные). Родительская роль/группа другой организации ничего не даёт. Делегированная роль (cross-org delegation) учитывается только при совпадении её продукта (`imtauthgql::AdaptUserForTenant`, `GetEffectiveUserPermissions`, `GetTenantUserPermissions`). Это правило действует и при проверке команд, в том числе в самой Puma (13.14).
- **R8.3. Циклы ролей и групп запрещены:** родительская роль/группа, замыкающая цикл, отклоняется с ошибкой (`CreatesRoleCycle`, `CreatesGroupCycle`).
- **R8.4. Запреты (`prohibitions`)** хранятся, но при вычислении прав **не применяются** (13.8). Использовать нельзя.
- **R8.5.** Для удалённых потребителей (ProLife и др.) права, полученные из ролей, **материализуются** в сериализуемую копию пользователя (`CUserCollectionControllerComp::CreateAdaptedObjectData`), иначе PAT-пользователь без ролей даёт пустой набор.
- **R8.6.** Пользователь видит назначенные ему роли и группы независимо от прав: чтение **собственных** ролей (включая делегированные) и групп пропускается без проверки. Чужие роли и группы — только по правам (для групп сейчас не так, 13.7).

## 9. Привилегированные субъекты

| Субъект | Как определяется | Поведение |
|---|---|---|
| **Суперпользователь** | `CUserInfoComp::IsAdmin()`: `UserId == AdminId` (в композициях `AdminId = su`). Клиент определяет по `login == "su"`. | Обходит все проверки прав команд; сервер отдаёт ему **пустой** список прав. |
| **Владелец организации** | `ITenantInfo::GetOwnerId() == userId` для текущей организации (кэшируется на токен). | Обходит проверки в рамках своей организации. |

**R9.1.** Суперпользователя не определять по списку прав — он пустой и неотличим от пользователя без прав. В тестах для этого есть `permissions: ['*']` (`GuiTestKit/fixtures/defineUsers.js` отказывается принимать `su` без него).
**R9.2.** Только суперпользователь: включение/отключение учётной записи (`IsSuperuserRequest`), изменение чужого профиля (`OnSetProfile`), создание отключённой учётной записи, выдача PAT с любыми scopes.
**R9.3.** Отключённая учётная запись → контекст не создаётся (`CCS_FORBIDDEN`), токены и сессии перестают работать немедленно.
**R9.4.** Обход проверок для привилегированных субъектов не снимает ограничения токена: PAT с узкими scopes остаётся узким и у суперпользователя, и у владельца организации (13.16).

## 10. Организации и PAT

**Организации (tenant).**
- `ITenantInfo::GetTenantPermissions()` — **верхняя граница**: перечень продуктовых прав, доступных организации.
- Роль организации может содержать только права из этого списка (`CRoleDocumentValidatorComp`: «Permission '%1' is not allowed for tenant»).
- Список прав организации согласован с requirements: при добавлении права редактор сам добавляет листья, которые это право включает. Иначе `ChangeUser` без `ViewUsers` дал бы изменение без просмотра.
- `GetProductPermissions(productId, tenantId)` возвращает дерево, ограниченное правами организации; `GetUserPermissions(productId)` — права текущего пользователя (суперпользователю — всё дерево).
- «Глобальные» права в ответе авторизации = права пользователя, не входящие ни в одну из организаций, где он состоит (`CalculateGlobalPermissions`).
- Управление организацией использует отдельное дерево (R3.6); владелец/создатель считаются администраторами организации.

**Personal Access Token.**
- Scopes токена — подмножество прав **создающего**: `HasPermission(действующие права создающего, scope)`; суперпользователь может выдать любые; токен с ограниченными правами не может создать более широкий.
- Scope — путь права или группы (`/OrderManagement/View`, `/OrderManagement`); scope голым id (`ViewOrders`) не покрывает ничего.
- Права PAT = `права пользователя ∩ scopes` по тому же правилу `HasPermission` (13.4); requirements раскрываются **от этого пересечения**, а не от всех прав пользователя: токен со scope `ChangeUser` видит пользователей, но ничего сверх этого (раздел 5). Роли и группы обнуляются; **пустой список scopes → ноль прав** (fail-closed).

## 11. Тестирование

- **Юнит:** `ImtCore/Tests/PermissionRequirementsTest` (25) — правило `HasPermission` (таблица раздела 4, голые id) и раскрытие requirements (таблицы раздела 5: четыре вида зависимостей, транзитивность, циклы, граница организации, продукт запроса, запрещённые зависимости). `ImtCore/Tests/PermissionCheckTest` (38) — проверка серверной команды целиком (`CheckRequestPermissions`): кто проверяется, декларации, все виды requirements, продукт и организация, `permissionPath`, элементы GUI, ревизии (`CheckRevisionPermissions`). Новые кейсы проверены мутациями: каждый ломается, если вернуть сравнение голых id или проверку ревизий по группе либо по чужой коллекции.
- **API (Postman):** Puma `TenantApiPostman` — папка 20 «Organization permissions by path» (права организации по путям, голый id и группа ничего не дают); ProLife `ProLifeApiPostman` — папка 11 «Permission Model» (продукт сервера, scopes PAT путями и голым id, совпадение `GetPermissions` с решением сервера).
- **Серверные API-тесты:** для каждой защищённой команды — пользователь без права получает отказ; для каждого requirement — право включает свою цель, а цель не даёт обратно источник.
- **GUI (`GuiTestKit`).** Матриц «права → страницы» **не ведём**: клиент строится из прав пользователя, поэтому тест спрашивает его самого — `page.isAvailable()`, `page.commands.isAvailable(id)`, `admin.hasSubPage(id)`. Условие пропуска — только `requires(user, permission)` (не `!user.can(x)`: «не удалось определить» ≠ «нет права»).
- **Фикстуры пользователей** перечисляют листья **так, как их называет сервер** (`GetProductPermissions(productId)`). `seed.js` сверяет список с живым деревом — устаревший id роняет прогон. Включённые права в фикстурах не перечисляются: их добавляет сервер. Не собирать список по `.acc`/QML: `EditFeature` в `PagePermissions` — группа, а не выдаваемое право.

## 12. Чек-лист

**Добавить permission:** (1) создать лист в Lisa `FeatureEditor` с `IsPermission=true` по R3.2–R3.5; (2) при необходимости — requirements по R5.3–R5.4 (обычно на группе `<Area>/Edit` → лист `<Area>/View`); (3) пересоздать `<Product>Features.h`; (4) добавить локализацию (`Feature`); (5) обновить список `FULL_ACCESS` в фикстурах GUI-тестов.

**Защитить команду:** (1) `SingleCommandPermissionsProvider` (`CommandId` = id команды GraphQL, `CommandPermissions` = полный путь листа); (2) добавить в `CommandPermissionsProviderJoiner`; (3) обработчик наследует `CPermissibleGqlRequestHandlerComp`, подключены `CommandPermissions` и `PermissionChecker`; (4) если нужна привязка к контексту — `permissionPath` (6.4); (5) тест: пользователь без права получает отказ.

**Страница/кнопка:** `PagePermissions` — лист `<Area>/ViewPage`; `*Permissions` в `Standard*Commands` — пути листьев данных (`<Area>/View`, `<Area>/Edit/*`).

**Ревизии новой коллекции:** подгруппа `<Area>/Revisions` в дереве (R3.4) → путь `<Area>/Revisions` в `RevisionPermissionPaths` контроллера ревизий на позиции коллекции в `CollectionIds` (R6.7) → `revisionsPermissionPath` у QML-хоста истории.

**Новый продукт:** собрать дерево в Lisa → `Features.h` → `FillProduct` в `Main.cpp` → компонент `SingleProductPermission` (`ProductId`, `PathToPermissionsFile`) → `FeatureContainer` в `AuthenticationManager` (без него requirements не раскрываются, R5.2) → `AdminId=su`, `PatPrefix`.

## 13. Расхождения с кодом и открытые вопросы

Исходное состояние — ImtCore `442be502b9`; пометка «Исправлено» — в ветках, перечисленных в шапке. «По коду» — вывод по чтению кода, тестом не подтверждён.

| № | Что обнаружено | Где | Что делать |
|---|---|---|---|
| 13.1 | **Механизм верный, данные и группы — нет.** Код: «держишь A ⇒ держишь requirements A» (`imtlic::CollectImpliedPermissions`, вызывается из `IGqlContext::GetImpliedPermissions()`; итог `GetPermissions(productId) ∪ GetImpliedPermissions()` в `imtservergql::GetUserPermissions`). Данные во всех продуктах записаны в обратную сторону: `View*` → группа `Edit*` (Lisa, ProLife, NeoPro, IotPlatform, Acula, `COrganizationsPermissionsProviderComp`). Если вывод работает, роль с одним `View*` получает `Edit*` на сервере. Кроме того: requirement на группу даёт сам путь группы, а не её листья; итог не ограничивается правами организации; для PAT раскрытие идёт от всех прав пользователя, а не от scopes; ограничения R5.4 не проверяются. | `imtlic/CProductInfo.cpp`, `imtgql/CGqlContext.cpp`, `imtservergql/imtservergql.h`, `*Features.h` | Исправлено в коде (раскрытие группы-цели в листья, requirements групп, граница организации, PAT от scopes, запрет зависимостей своей ветки) и в данных Lisa, ProLife и дерева организации: новые деревья (R3.4, R3.6) записаны `Edit → View`. **Открыто:** NeoPro, IotPlatform, Acula, Agentino; аудит существующих ролей. |
| 13.2 | Исправлено: `GetUserPermissions` берёт права продукта запроса (`GetPermissions(productId)`). | `imtservergql/imtservergql.h` | — |
| 13.3 | Клиентский `PermissionsController.hasPermission` **не реализует проверку группы** (лист `.../EditUser/ChangeUser` есть, проверка группы `.../EditUser` → на клиенте `false`, на сервере `true`). | `ImtCore/Qml/imtauthgui/PermissionsController.qml` | Исправлено: только пути, проверка группы — «есть лист внутри». |
| 13.4 | Ограничение PAT по scopes — **точное совпадение** (`scopeSet.contains`), а выдача scopes — через `HasPermission`. Расходятся для голых id. | `imtservergql/CAuthenticationManagerComp.cpp` | Исправлено: через `HasPermission`; голый scope ничего не покрывает. |
| 13.5 | Голые id больше не поддерживаются (R4.3), а хранимые права ролей, пользователей, участников организаций, scopes PAT и права организаций в БД частично записаны голыми id или **старыми путями** (`/SensorManagement/EditSensor/AddSensor`). После развёртывания такие права перестают действовать. `imtauthdb::CPermissionPathMigrationComp` (шаг миграции БД, `imtdb::IMigrationController`, пакет `ImtAuthDbPck`; пока ни в одной композиции не подключён) переводит голый id в путь только по текущему дереву: старые id (`AddSensor`) в новом дереве не встречаются, короткие (`Add`) неоднозначны — оба остаются как есть. Каталог фич в БД Lisa — тоже старое дерево. | БД Puma и Lisa, `imtauthdb/CPermissionPathMigrationComp` | **Открыто, до развёртывания:** миграция БД по таблице «старый id / старый путь → новый путь» (есть для Lisa и ProLife) и обновление каталога Lisa. |
| 13.6 | Непоследовательное сопоставление: `PackagesSetDescription` → `AddFeature` (по R6.4 — `ChangeFeature`, как у `ProductsSetDescription`). | `Lisa/Partitura/LisaVoce.arp/FeaturesPermissions.acc` | Уточнить. |
| 13.7 | `CUserGroupCollectionControllerComp` пропускает `GroupItem`, `GetObjectData`, `GetObjectTypeId` **для любой группы**, а пользователю с делегированным доступом из другой организации разрешает **любую** групповую команду, включая изменение и удаление. | `imtauthgql/CUserGroupCollectionControllerComp.cpp` | Пропускать только чтение собственных групп (R8.6). |
| 13.8 | `prohibitions` хранятся и сериализуются, но в расчёте прав не участвуют (`CRole::GetProhibitions` возвращает пустой список). | `imtauth/CRole.cpp`, `CUserBaseInfo.cpp` | Реализовать или удалить. |
| 13.9 | При совпадении `CommandId` в joiner молча побеждает первый провайдер. | `imtserverapp/CCommandPermissionsProviderJoinerComp.cpp` | Добавить диагностику. |
| 13.10 | Fail-open: неподключённые `CommandPermissions`/`PermissionChecker` открывают команду всем. | `imtservergql::CheckRequestPermissions` | Проверять конфигурацию при старте. |
| 13.11 | Тест `testContainerCannotBeRequirementTarget` назван неверно: его цель `/Products/EditProducts` — лист, а не группа. Прогон собранного `PermissionRequirementsTest.exe` (2026-10-16): 12 прошли, 7 упали — во всех тестах вывода прав `CollectImpliedPermissions` вернул пустой список. Бинарник мог быть собран со старой `imtlic.lib` (параллельная сборка). | `ImtCore/Tests/PermissionRequirementsTest` | Исправлено: тест переписан (25 кейсов), добавлен `PermissionCheckTest` (38), все проходят (раздел 11). |
| 13.12 | `permissionPath` проверяется как «объявленное право лежит внутри пути»: путь-группа `/Devices` проходит для команды, объявленной на `/Devices/ViewDeviceTickets`, и дальше достаточно **любого** листа `/Devices`. Кроме того, путь заменяет AND-список команды одним правом. То же в `IsElementAccessible`. | `imtservergql/imtservergql.h`, `imtserverapp/imtserverapp.h` | Исправлено (`imtauth::IsPermissionPathDeclared`): путь равен объявленному праву или лежит внутри него. |
| 13.13 | Продукт запроса берётся из заголовка клиента `productId`; без заголовка права собираются по **всем** продуктам. Requirements всегда раскрываются от прав всех продуктов (`m_userInfoPtr->GetPermissions()`). | `imtservergql/CAuthenticationManagerComp.cpp`, `imtgql/CGqlContext.cpp` | Исправлено: продукт — из конфигурации сервера (`ProductInfo`), заголовок — только запасной вариант; requirements раскрываются от прав продукта запроса. |
| 13.14 | По коду: в Puma пользователь в контексте не адаптируется к организации (`AdaptUserForTenant` вызывается только при выдаче данных), поэтому проверка команд в Puma учитывает роли из всех организаций пользователя. В продуктовых серверах пользователь приходит из Puma уже адаптированным. | `imtservergql/CAuthenticationManagerComp.cpp`, Puma `PumaServerBase.acc` | Подтвердить тестом; адаптировать пользователя при создании контекста. |
| 13.15 | `"*"` в списке прав элемента или команды пропускает любого пользователя через `CheckPermissionOr`. | `imtauth/CCheckPermissionOrComp.cpp` | Запретить в декларациях (R6.8). |
| 13.16 | Шаг 3 алгоритма (суперпользователь/владелец → разрешить) выполняется до любых проверок прав. Если у PAT владельца организации выставлен `IsTenantOwner`, токен с узкими scopes получает полный доступ. Не проверялось. | `imtservergql::CheckRequestPermissions` | Проверить; обход не должен снимать scopes (R9.4). |
| 13.17 | `CGuiElementRepresentationControllerComp` без `UserInfo` считает пользователя суперпользователем (показывает всё) и берёт права по всем продуктам; владелец организации видит отфильтрованный GUI, хотя сервер его пропускает. | `imtserverapp/CGuiElementRepresentationControllerComp.cpp` | Низкий приоритет: GUI не граница безопасности. |
| 13.18 | Дерево продукта обходится заново на каждый запрос (`CollectProductRequirements` в контексте запроса, с глубокими копиями фич). | `imtlic/CProductInfo.cpp` | Requirements раскрываются на лету (R5.2), но разобранное дерево продукта держать в кэше, а не копировать на каждый запрос. |
| 13.19 | Права участника организации проверялись мимо R4.1: точным `contains` по голым id (`CanManageMembers`, назначение прав участнику, `HasTenantManageAccess`, признак администратора организации) и голыми id групп (`EditOrganization`, `EditOrganizationMember`) в `CTenantCollectionDocumentServiceComp`. Голый id группы давал всё управление участниками, а права, записанные путями, не действовали. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp`, `TenantAccess.cpp`, `CTenantCollectionDocumentServiceComp.cpp`, `CTenantManagerControllerComp.cpp`, `CCrossOrgGrantCollectionDocumentServiceComp.cpp` | Исправлено в ветке `feature/permission-requirements`: проверки через `HasPermission` по полным путям листьев дерева организации (`imtauthgql/TenantAccess.h`); группа дерева правом не является (R3.5). Дерево организации перестроено (R3.6) и отдаётся редактору прав участника полными путями (`BuildPermissionGroupsFromProvider`), а не голыми id. |
| 13.20 | Ревизии были одной сквозной группой `RevisionManagement`: `ViewRevisions` открывал историю **всех** сущностей, а клиент проверял голый `ViewRevisions`. | `CDocumentRevisionControllerComp`, `CRemoteDocumentRevisionControllerComp`, `ImtCoreVoce.arp/RevisionPermissions.acc`, `imtdocgui` | Исправлено: подгруппа `Revisions` в каждой области (R3.4, R6.7), `RevisionPermissionPaths` у контроллеров ревизий Puma, ProLife, Lisa и клиента Puma; `RevisionPermissions.acc` удалён. Открыто: команды ревизий объявлены как `Query`, восстановление и удаление должны быть `Mutation`. |
| 13.21 | Экспорт, импорт и восстановление сущностей и «показать все» (`ViewAll*`) были объявлены голыми id без узла в дереве и были доступны только суперпользователю и владельцу организации. | `ImtUserAdministrationVoce.arp/{User,Role,Group,Tenant}Permissions.acc`, ProLife `{Account,Software,Order,Device}Permissions.acc`, `*Page.acc` | Исправлено: экспорт → `<Area>/View`, импорт → `<Area>/Edit/Add`, восстановление → `<Area>/Edit/Remove` (у организаций — `/OrganizationManagement/Add` и `/Remove`), `ViewAll*` → `<Area>/ViewAll`. Голым остаётся только `ViewServerPage` (страница настроек сервера ProLife) — намеренно, она доступна только суперпользователю. |
| 13.22 | В `PagesController.acc` страница Administration была объявлена несуществующим путём `/Administration/EditAdministration/EditRole/AddGroup`, а права просмотра ролей в списке не было. | `ImtCore/Partitura/ImtGraphQlVoce.arp/PagesController.acc` | Исправлено: страница открывается любым из `/Administration/{Users, Roles, Groups}/ViewPage` (R3.4). |
| 13.23 | Лист участника не различает действия: приглашение, исключение и смена роли участника, а также изменение его прав (`UpdateMembershipPermissions`) разрешаются **любым** листом `Members/Edit/*`; `Permissions/Change` в проверках не используется. | `imtauthgql/CTenantMembershipManagerControllerComp.cpp` (`CanManageMembers`) | Проверять лист своего действия. |
| 13.24 | Роли, группы и пользователи организации в редакторе организации редактируются командами `Get/Update{Role,Group,User}{From}Representation`, объявленными только продуктовыми `/Administration/{Roles,Groups,Users}/...`. Права участника `Roles/*`, `Groups/*` эти команды не открывают (продуктовые пути организации в их декларациях указывали на несуществующие узлы ещё до этой работы и удалены). | `ImtAuthVoce.arp/{Role,User,UserGroup}CollectionDocumentService.acc`, сервисы документов `imtauthgql` | Решить: проверять в контексте организации лист участника `Roles/Edit/Change` и т. д. в самих сервисах. |
| 13.25 | Страница поиска объявлена `*` (видна всем, 13.15), лист `/SearchManagement/ViewPage` ProLife в декларациях не используется: страница общая для продуктов, у Lisa такого листа нет. | `ImtGraphQlVoce.arp/PagesController.acc` (`SearchPage`) | Объявить страницу листом продукта в композиции каждого продукта. |

## 14. Текущее состояние решений

| Продукт | Узлов permission | Записей requirements | Формат | Направление | Файл |
|---|---|---|---|---|---|
| Lisa | 74 | 28 | путь, дерево R3.4 | `Edit` → `View`, ревизии → `View` — верное | `Lisa/Impl/LisaServer/LisaFeatures.h` |
| ProLife | 139 | 43 | путь, дерево R3.4 | `Edit` → `View`, `ViewAll`/`ViewHistory` → `View`, ревизии → `View` — верное | `ProLife/Impl/ProLifeServer/ProLifeFeatures.h` |
| IotPlatform | 89 | 7 | путь | `View*` → `Edit*` | `IotPlatform/Impl/IotPlatformServerExe/IotPlatformFeatures.h` |
| NeoPro | 35 | 4 | путь | `View*` → `Edit*` | `NeoPro/Impl/NeoProServer/NeoProFeatures.h` |
| Acula | 21 | 4 | путь | `View*` → `Edit*` | `Acula/Impl/AculaServer/AculaFeatures.h` |
| Agentino | 18 | 0 | — | — | `Agentino/Impl/AgentinoServer/AgentinoFeatures.h` |

## 15. Ключевые файлы

| Тема | Файл |
|---|---|
| Правило сравнения | `ImtCore/Include/imtauth/imtauth.h` (`HasPermission`) |
| Проверка команд | `ImtCore/Include/imtservergql/imtservergql.h`, `CPermissibleGqlRequestHandlerComp.*` |
| Чекеры | `ImtCore/Include/imtauth/CCheckPermissionCompBase.cpp`, `CCheckPermissionOrComp.cpp` |
| Элементы GUI | `ImtCore/Include/imtserverapp/imtserverapp.h` (`IsElementAccessible`), `imtservergql/CCommandsControllerComp.cpp` |
| Контекст, requirements | `ImtCore/Include/imtgql/CGqlContext.*`, `imtlic/CProductInfo.cpp`, `imtlic/IFeatureInfo.h` |
| Контекст запроса, PAT, организация | `ImtCore/Include/imtservergql/CAuthenticationManagerComp.cpp` |
| Права пользователя | `imtauth/CUserBaseInfo.cpp`, `CUserInfo.cpp`, `CRole.cpp`, `imtauthgql/imtauthgql.h` |
| Дерево прав GraphQL | `ImtCore/Sdl/imtauth/1.0/Permissions.sdl`, `imtauthgql/CPermissionsProviderComp.cpp` |
| Права команд (Partitura) | `ImtCore/Partitura/ImtCoreVoce.arp/*Permissions.acc`, `ImtAuthVoce.arp/ObjectCollectionPermissions.acc`, `Lisa/Partitura/LisaVoce.arp/*Permissions.acc` |
| Клиент | `ImtCore/Qml/imtauthgui/PermissionsController.qml`, `PermissionsProvider.qml`, `Qml/imtlicgui/FeatureEditor.qml` |
| Ревизии | `ImtCore/Include/imtservergql/imtservergql.h` (`CheckRevisionPermissions`), `CDocumentRevisionControllerComp.*`, `imtclientgql/CRemoteDocumentRevisionControllerComp.*`, `Qml/imtdocgui/DocumentHistoryView.qml` |
| Дерево организации | `ImtCore/Include/imtauth/COrganizationsPermissionsProviderComp.cpp`, `imtauthgql/TenantAccess.h` |
| Тесты | `ImtCore/Tests/PermissionRequirementsTest`, `ImtCore/Tests/PermissionCheckTest`, Puma `Tests/TenantApiPostman`, ProLife `Tests/ProLifeApiPostman`, `ImtCore/Tests/GuiTestKit`, `Lisa/Tests/LisaGui` |
