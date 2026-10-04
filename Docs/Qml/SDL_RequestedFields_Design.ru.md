# SDL: декларативная выборка GraphQL-полей в QML

## Статус и связь с PR #942

Это проект следующего этапа развития QML-клиента, а не описание уже реализованного API.
PR [#942](https://github.com/ImagingTools/ImtCore/pull/942) исправляет генерацию и разбор
серверного `RequestInfo`. Этот документ предлагает клиентскую сторону: как задавать
выборку без строковых имён полей и ручного построения вложенных `GqlObject`.

Добавление документа не меняет генератор, QML-runtime, сервер и текущие запросы.
Все примеры с `*Fields`, `requestedFields` в payload и `getGqlFields()` ниже описывают
предлагаемый API. Названия типов и полей в примерах `GetTenant` и файлового браузера
взяты из существующих SDL-схем ImtCore.

Целевой вариант: отдельный лёгкий QML-файл `<Type>Fields.qml` для каждого выходного
объектного типа и его вложенных типов. Выборка подключается внутри `sdlObjectComp`.
Inline components, JS-фабрики на экране и свойства вида `requestedFields.idRequested`
не нужны.

## 1. Как работает текущий код

Основные исходники:

- [GqlSdlRequestSender.qml](../../Qml/imtguigql/GqlSdlRequestSender.qml): создаёт запрос и разбирает ответ.
- [GraphQLRequest.js](../../Qml/web/GraphQLRequest.js): собирает текст GraphQL из `GqlObject`.
- [CollectionRepresentation.qml](../../Qml/imtguigql/CollectionRepresentation.qml): отдельный механизм выборки строк коллекции.
- [CQmlCodeGeneratorComp.cpp](../../Include/imtsdlgenqml/CQmlCodeGeneratorComp.cpp): генерирует объекты данных на основе `BaseClass`.
- [CQmlCodeMetaGeneratorComp.cpp](../../Include/imtsdlgenqml/CQmlCodeMetaGeneratorComp.cpp): генерирует метаданные и ресурсы.
- [CGqlWrapClassCodeGeneratorComp.cpp](../../Include/imtsdlgencpp/CGqlWrapClassCodeGeneratorComp.cpp): генерирует серверный `RequestInfo`.

Сейчас `GqlSdlRequestSender.send()` вызывает `getRequestedFields()` и передаёт
ненулевой результат в один вызов `query.AddField()`. По умолчанию функция возвращает
`null`. Она ожидает один `GqlObject`, а не массив корневых полей.

Для нескольких корневых полей разработчик может добавлять их вручную в
`createQueryParams()`. Но эта функция вызывается только при отсутствии переданного
`sdlInputObject` и `inputObjectComp`. Выборка начинает зависеть от способа передачи
входных параметров, хотя эти вещи независимы.

Объект из `sdlObjectComp` сейчас создаётся в `onResult()`, после получения ответа.
Следовательно, прочитать выборку внутри payload до отправки запроса без изменения
времени его создания нельзя.

У `CollectionRepresentation.getRequestedFields()` другой контракт: массив строк.
Он составляется из `id`, `name`, `typeId`, заголовков и `additionalFieldIds` и затем
вставляется в `items`. Этот механизм не описывает произвольное дерево вложенных полей.

### Что меняется после #930 и #942

После исправлений сервер действительно учитывает часть requested fields, поэтому
зависимость экрана от незапрошенного поля становится видимой. Нельзя решать это
автоматическим запросом всех полей: так возвращается вычисление дорогих данных,
от которого избавлялся PR #930.

В текущем C++-генераторе обязательные поля SDL (`!`) получают флаг `true` независимо
от выборки, а флаги по умолчанию также равны `true`. Поэтому нельзя обещать, что
невыбранное клиентом обязательное поле никогда не будет вычислено или возвращено.
Новый QML-механизм формирует явный корректный запрос, но сам по себе эту серверную
семантику не меняет.

## 2. Новая структура

| Артефакт | Ответственность |
| --- | --- |
| `GetTenantPayload` | Данные ответа: `m_tenant`, `m_errorMessage`; nullable-ссылка на выборку |
| `GetTenantPayloadFields` | Выборка корневых полей результата команды |
| `TenantDataFields` | Выборка полей объекта `tenant` |
| `TenantMemberEntryFields` | Одна выборка типа элемента массива `members` |
| `TenantInvitationEntryFields` | Одна выборка типа элемента массива `pendingInvitations` |
| `GqlRequestedFields` | Один общий лёгкий базовый QML-тип: сборка и проверка выборки |
| `GqlSdlRequestSender` | Создание payload до отправки, чтение выборки, транспорт и заполнение ответа |

`GqlRequestedFields` предлагается добавить один раз в модуль `imtguigql`.
Остальные `*Fields` генерируются из SDL рядом с обычными типами данных.

Правила структуры:

1. Скаляр, enum и массив скаляров/enum представлены `bool <field>Requested` с binding к `selection.allScalarFieldsRequested`.
2. Объект и массив объектов представлены `<ElementType>Fields <field>: null`.
3. Ненулевая объектная выборка включает ветку. Отдельный `<field>Requested` для неё не нужен.
4. Пустая вложенная выборка является ошибкой, а не командой запросить все поля.
5. Пустая корневая выборка или отсутствие `requestedFields` для объектного ответа является ошибкой.
6. Флаг `false` означает отсутствие поля в запросе, а не требование вернуть `null`.
7. Обязательность SDL не включает клиентский флаг автоматически.
8. Одноимённые типы в разных модулях разрешаются через существующие SDL-импорты.
9. Общий `allScalarFieldsRequested` по умолчанию `false`; без его изменения скалярные флаги также остаются `false`.
10. `allScalarFieldsRequested: true` включает все скалярные поля только текущего объекта; явное значение конкретного флага переопределяет его binding.

Классификация определяется SDL-типом поля, а не значением ответа или QML-типом
контейнера данных. Массив сам по себе не превращает скаляр в объект:

| SDL-поле | Свойство в `Fields` | Действие `allScalarFieldsRequested` |
| --- | --- | --- |
| `String`, `ID`, числа, Boolean и поддержанные скалярные типы | `bool <field>Requested` | Включает поле |
| Enum | `bool <field>Requested` | Включает поле |
| `[String]`, `[ID]`, `[Enum]` | Один `bool <field>Requested` на массив | Включает поле без дочернего selection set |
| Объект | `<Type>Fields <field>: null` | Не создаёт и не включает ветку |
| Массив объектов | `<ElementType>Fields <field>: null` | Не создаёт и не включает ветку |
| Union или массив union | `<UnionType>Fields <field>: null` | Не выбирает варианты union автоматически |

Вложенные объекты, массивы объектов и варианты union имеют собственные выборки.
Значение общего флага родителя не распространяется на них. Модификаторы `!` не
меняют клиентское представление выборки.

Для поля `isActive` имя флага получается `isActiveRequested`, без удвоения `is`.
Для `GetTenantPayload` не генерируется `idRequested`: у этого типа нет SDL-поля `id`.
Оно есть у вложенного `TenantData`.

## 3. Реальная схема GetTenant

Источник: [Tenants.sdl](../../Sdl/imtauth/1.0/Tenants.sdl).
Ниже приведена используемая часть схемы; определение запроса находится в её `Query`.

```graphql
type TenantMemberEntry {
    id: ID!
    name: String!
}

type TenantInvitationEntry {
    id: ID!
    userId: ID!
    userName: String
    status: String!
    invitedByUserId: ID
    invitedByName: String
    createdAt: String
    expiresAt: String
}

type TenantData {
    id: ID
    name: String
    description: String
    ownerId: ID
    creatorId: ID
    isActive: Boolean
    createdAt: String
    updatedAt: String
    currentUserId: ID
    currentUserOrganizationPermissions: [ID]
    members: [TenantMemberEntry]
    pendingInvitations: [TenantInvitationEntry]
    tenantPermissions: [ID]
    parentTenantId: ID
    isSystemTenant: Boolean
}

input GetTenantInput {
    tenantId: ID!
}

type GetTenantPayload {
    tenant: TenantData
    errorMessage: String
}

type Query {
    GetTenant(input: GetTenantInput): GetTenantPayload!
}
```

## 4. GetTenant: было и будет

### Было: ручная сборка на текущем API

Это пример использования существующего API для реального `GetTenant`, а не цитата
готового экрана. Типы и поля взяты из схемы.

```qml
GqlSdlRequestSender {
    id: getTenantRequest
    gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenant

    sdlObjectComp: Component {
        GetTenantPayload {
            id: payload

            onFinished: {
                root.applyTenant(payload.m_tenant, payload.m_errorMessage)
            }
        }
    }

    function createQueryParams(query){
        let inputObject = Gql.GqlObject("input")
        inputObject.fromObject(getTenantInput)
        query.AddParam(inputObject)

        let tenantObject = Gql.GqlObject("tenant")
        tenantObject.InsertField("id")
        tenantObject.InsertField("name")
        tenantObject.InsertField("isActive")

        let membersObject = Gql.GqlObject("members")
        membersObject.InsertField("id")
        membersObject.InsertField("name")
        tenantObject.InsertFieldObject(membersObject)

        let invitationsObject = Gql.GqlObject("pendingInvitations")
        invitationsObject.InsertField("id")
        invitationsObject.InsertField("userName")
        invitationsObject.InsertField("status")
        tenantObject.InsertFieldObject(invitationsObject)

        query.AddField(tenantObject)
        query.AddField(Gql.GqlObject("errorMessage"))
    }
}
```

В этом варианте вызывается `getTenantRequest.send()` без аргумента. Если заменить
его на `send(getTenantInput)`, текущий sender не вызовет `createQueryParams()` и
вручную добавленные поля пропадут.

### Будет: выборка внутри payload

Ниже полный пример поверхности экрана. `applyTenant()` условно обозначает
обработчик экрана, а не существующий публичный метод ImtCore.

```qml
import QtQuick 2.12
import imtguigql 1.0
import imtauthTenantsSdl 1.0

Item {
    id: root

    property string tenantId: ""
    property string tenantName: ""
    property string errorMessage: ""

    function loadTenant(){
        getTenantInput.m_tenantId = root.tenantId
        getTenantRequest.send(getTenantInput)
    }

    function applyTenant(tenant, message){
        root.errorMessage = message
        root.tenantName = tenant !== null ? tenant.m_name : ""
    }

    GetTenantInput {
        id: getTenantInput
    }

    GqlSdlRequestSender {
        id: getTenantRequest
        gqlCommandId: ImtauthTenantsSdlCommandIds.s_getTenant

        sdlObjectComp: Component {
            GetTenantPayload {
                id: payload

                requestedFields: GetTenantPayloadFields {
                    errorMessageRequested: true

                    tenant: TenantDataFields {
                        idRequested: true
                        nameRequested: true
                        isActiveRequested: true

                        members: TenantMemberEntryFields {
                            idRequested: true
                            nameRequested: true
                        }

                        pendingInvitations: TenantInvitationEntryFields {
                            idRequested: true
                            userNameRequested: true
                            statusRequested: true
                        }
                    }
                }

                onFinished: {
                    root.applyTenant(payload.m_tenant, payload.m_errorMessage)
                }
            }
        }
    }
}
```

Sender формирует такой GraphQL-запрос; значение `tenantId` здесь иллюстративное:

```graphql
query GetTenant {
    GetTenant(input: { tenantId: "tenant-123" }) {
        tenant {
            id
            name
            isActive
            members { id name }
            pendingInvitations { id userName status }
        }
        errorMessage
    }
}
```

В запрос не включаются `description`, `ownerId`, `tenantPermissions` и другие
невыбранные поля. Это описание запроса, не гарантия строгой серверной проекции для
всех типов: существующие ограничения `RequestInfo` перечислены ниже.

Чтобы убрать приглашения, достаточно не объявлять `pendingInvitations`.
Чтобы получить массив ID прав, включается `tenantPermissionsRequested: true`.
Не нужно создавать элементы массива, `BaseModel` или экземпляры `TenantData` для
описания выборки.

## 5. Полный пример нового GetTenantPayload

Это проект сгенерированного объекта данных по структуре текущего генератора.
Новое свойство только одно: `requestedFields`. Остальные методы продолжают работать
с данными. Обращения к экземпляру идут через `id`.

```qml
import QtQuick 2.12
import imtcontrols 1.0

BaseClass {
    id: getTenantPayload

    readonly property string __typename: "GetTenantPayload"

    property TenantData m_tenant: null
    property string m_errorMessage: ""

    property GetTenantPayloadFields requestedFields: null

    Component.onCompleted: {
        getTenantPayload._internal.removed = ["m_tenant", "m_errorMessage"]
    }

    function hasTenant(){
        return getTenantPayload.m_tenant !== undefined
            && getTenantPayload.m_tenant !== null
    }

    function hasErrorMessage(){
        return getTenantPayload.m_errorMessage !== undefined
            && getTenantPayload.m_errorMessage !== null
    }

    function emplaceTenant(typename){
        getTenantPayload.m_tenant = getTenantPayload.createComponent(
            "m_tenant", typename
        ).createObject(getTenantPayload)
        getTenantPayload.m_tenant.owner = getTenantPayload
        getTenantPayload._internal.removeAt("m_tenant")
    }

    function removeTenant(){
        getTenantPayload.removeKey("m_tenant")
    }

    function removeErrorMessage(){
        getTenantPayload.removeKey("m_errorMessage")
    }

    function getJSONKeyForProperty(propertyId){
        switch (propertyId){
            case "m_tenant": return "tenant"
            case "m_errorMessage": return "errorMessage"
            case "__typename": return "__typename"
        }
    }

    function createElement(propertyId, typename){
    }

    function createComponent(propertyId, typename){
        switch (propertyId){
            case "m_tenant":
                return Qt.createComponent(
                    "qrc:/qml/imtauthTenantsSdl/TenantData.qml"
                )
        }
    }

    function createMe(){
        return Qt.createComponent("GetTenantPayload.qml").createObject()
    }

    function getPropertyType(propertyId){
        switch (propertyId){
            case "m_tenant": return "TenantData"
            case "m_errorMessage": return "string"
        }
    }
}
```

`fromObject()`, `toJson()`, уведомления и `finished` наследуются от
[BaseClass.qml](../../Qml/imtcontrols/Base/BaseClass.qml). Выборка не попадает в JSON,
`getPropertyType()` или `_internal.removed`: это не поле SDL-данных.
`copyMe()` и JSON-копирование копируют данные, но не конфигурацию запроса.

Свойство `requestedFields` добавляется только в типы, непосредственно используемые
как объектный результат операций. Если один тип является и результатом операции,
и вложенным объектом, свойство будет у всех экземпляров этого типа, но по умолчанию
останется `null`. Это одна ссылка, а не полный набор флагов.

## 6. Полные примеры файлов Fields

### Общий GqlRequestedFields

Это один новый вручную поддерживаемый тип в `imtguigql`, а не файл для каждой схемы.
Он централизует проверку пустых выборок, распространение ошибок и защиту от циклов
экземпляров. Не используется `throw`, сложный синтаксис JS или inline components.

Контракт `getGqlFields(path, ancestors)`:

- Успех: `{ fields: [...], errorMessage: "" }`.
- Ошибка: `{ fields: [], errorMessage: "..." }`.
- Каждый элемент `fields` является `GqlObject`, в том числе скалярный лист.
- Порядок полей соответствует порядку SDL, не порядку присваиваний на экране.
- Sender не отправляет частично построенный запрос при любой ошибке.

```qml
import QtQuick 2.12
import imtguigql 1.0

QtObject {
    id: root

    property string sdlTypeName: ""
    property bool allScalarFieldsRequested: false

    function getGqlFields(path, ancestors){
        let fieldPath = path === undefined ? root.sdlTypeName : path
        let parentObjects = ancestors === undefined ? [] : ancestors
        let result = { fields: [], errorMessage: "" }

        if (parentObjects.indexOf(root) !== -1){
            result.errorMessage = "Selection object cycle at " + fieldPath
            return result
        }

        root.appendGqlFields(result, fieldPath, parentObjects.concat([root]))

        if (result.errorMessage === "" && result.fields.length === 0){
            result.errorMessage = "Empty selection at " + fieldPath
        }

        if (result.errorMessage !== ""){
            result.fields = []
        }

        return result
    }

    function appendGqlFields(result, path, ancestors){
        result.errorMessage = "Selection builder is not generated for " + path
    }

    function addScalar(result, fieldId, requested){
        if (result.errorMessage === "" && requested){
            result.fields.push(Gql.GqlObject(fieldId))
        }
    }

    function addObject(result, fieldId, selection, path, ancestors){
        if (result.errorMessage !== "" || selection === null){
            return
        }

        let nested = selection.getGqlFields(path + "." + fieldId, ancestors)
        if (nested.errorMessage !== ""){
            result.errorMessage = nested.errorMessage
            return
        }

        let objectField = Gql.GqlObject(fieldId)
        for (let index = 0; index < nested.fields.length; index++){
            objectField.InsertFieldObject(nested.fields[index])
        }

        result.fields.push(objectField)
    }
}
```

Использование `InsertFieldObject()` для листьев допустимо в текущем JS-сборщике:
скалярный `GqlObject("id")` имеет пустой список детей и сериализуется просто как `id`.
Это позволяет единообразно собирать разные уровни дерева.

### GetTenantPayloadFields

```qml
import QtQuick 2.12
import imtguigql 1.0

GqlRequestedFields {
    id: selection

    sdlTypeName: "GetTenantPayload"

    property TenantDataFields tenant: null
    property bool errorMessageRequested: selection.allScalarFieldsRequested

    function appendGqlFields(result, path, ancestors){
        selection.addObject(result, "tenant", selection.tenant, path, ancestors)
        selection.addScalar(result, "errorMessage", selection.errorMessageRequested)
    }
}
```

### TenantDataFields

Все поля соответствуют существующему `TenantData`, включая массивы скаляров.

```qml
import QtQuick 2.12
import imtguigql 1.0

GqlRequestedFields {
    id: selection

    sdlTypeName: "TenantData"

    property bool idRequested: selection.allScalarFieldsRequested
    property bool nameRequested: selection.allScalarFieldsRequested
    property bool descriptionRequested: selection.allScalarFieldsRequested
    property bool ownerIdRequested: selection.allScalarFieldsRequested
    property bool creatorIdRequested: selection.allScalarFieldsRequested
    property bool isActiveRequested: selection.allScalarFieldsRequested
    property bool createdAtRequested: selection.allScalarFieldsRequested
    property bool updatedAtRequested: selection.allScalarFieldsRequested
    property bool currentUserIdRequested: selection.allScalarFieldsRequested
    property bool currentUserOrganizationPermissionsRequested: selection.allScalarFieldsRequested
    property TenantMemberEntryFields members: null
    property TenantInvitationEntryFields pendingInvitations: null
    property bool tenantPermissionsRequested: selection.allScalarFieldsRequested
    property bool parentTenantIdRequested: selection.allScalarFieldsRequested
    property bool isSystemTenantRequested: selection.allScalarFieldsRequested

    function appendGqlFields(result, path, ancestors){
        selection.addScalar(result, "id", selection.idRequested)
        selection.addScalar(result, "name", selection.nameRequested)
        selection.addScalar(result, "description", selection.descriptionRequested)
        selection.addScalar(result, "ownerId", selection.ownerIdRequested)
        selection.addScalar(result, "creatorId", selection.creatorIdRequested)
        selection.addScalar(result, "isActive", selection.isActiveRequested)
        selection.addScalar(result, "createdAt", selection.createdAtRequested)
        selection.addScalar(result, "updatedAt", selection.updatedAtRequested)
        selection.addScalar(result, "currentUserId", selection.currentUserIdRequested)
        selection.addScalar(result, "currentUserOrganizationPermissions", selection.currentUserOrganizationPermissionsRequested)
        selection.addObject(result, "members", selection.members, path, ancestors)
        selection.addObject(result, "pendingInvitations", selection.pendingInvitations, path, ancestors)
        selection.addScalar(result, "tenantPermissions", selection.tenantPermissionsRequested)
        selection.addScalar(result, "parentTenantId", selection.parentTenantIdRequested)
        selection.addScalar(result, "isSystemTenant", selection.isSystemTenantRequested)
    }
}
```

### TenantMemberEntryFields

```qml
import QtQuick 2.12
import imtguigql 1.0

GqlRequestedFields {
    id: selection

    sdlTypeName: "TenantMemberEntry"

    property bool idRequested: selection.allScalarFieldsRequested
    property bool nameRequested: selection.allScalarFieldsRequested

    function appendGqlFields(result, path, ancestors){
        selection.addScalar(result, "id", selection.idRequested)
        selection.addScalar(result, "name", selection.nameRequested)
    }
}
```

### TenantInvitationEntryFields

```qml
import QtQuick 2.12
import imtguigql 1.0

GqlRequestedFields {
    id: selection

    sdlTypeName: "TenantInvitationEntry"

    property bool idRequested: selection.allScalarFieldsRequested
    property bool userIdRequested: selection.allScalarFieldsRequested
    property bool userNameRequested: selection.allScalarFieldsRequested
    property bool statusRequested: selection.allScalarFieldsRequested
    property bool invitedByUserIdRequested: selection.allScalarFieldsRequested
    property bool invitedByNameRequested: selection.allScalarFieldsRequested
    property bool createdAtRequested: selection.allScalarFieldsRequested
    property bool expiresAtRequested: selection.allScalarFieldsRequested

    function appendGqlFields(result, path, ancestors){
        selection.addScalar(result, "id", selection.idRequested)
        selection.addScalar(result, "userId", selection.userIdRequested)
        selection.addScalar(result, "userName", selection.userNameRequested)
        selection.addScalar(result, "status", selection.statusRequested)
        selection.addScalar(result, "invitedByUserId", selection.invitedByUserIdRequested)
        selection.addScalar(result, "invitedByName", selection.invitedByNameRequested)
        selection.addScalar(result, "createdAt", selection.createdAtRequested)
        selection.addScalar(result, "expiresAt", selection.expiresAtRequested)
    }
}
```

## 7. Изменения GqlSdlRequestSender

### Выборка независима от input

Все три способа передачи входных данных сохраняются: аргумент `send(input)`,
`inputObjectComp` и `createQueryParams()`. Выборка строится отдельным шагом в любом
случае. `createQueryParams()` больше не добавляет выходные поля.

Смысл новой части `send()` после создания payload и входных параметров:

```javascript
let selectionResult = payload.requestedFields.getGqlFields(root.gqlCommandId)
if (selectionResult.errorMessage !== ""){
    root.onError(selectionResult.errorMessage, "Error")
    return
}

for (let index = 0; index < selectionResult.fields.length; index++){
    query.AddField(selectionResult.fields[index])
}

root.setGqlQuery(query.GetQuery(), headers)
```

Это фрагмент алгоритма, не полная реализация sender. До него проверяются компонент,
созданный payload и ненулевой `requestedFields`; ошибка также завершает запрос через
`onError()`/`finished(-1)`, а не оставляет экран в состоянии загрузки.

`getRequestedFields()` как переопределяемый экраном метод sender удаляется при
миграции. Его не следует сохранять как вторую конкурирующую ветку сборки запросов.

### Время жизни объекта ответа

| Этап | Предлагаемое поведение |
| --- | --- |
| `send()` | Создать новый pending payload из `sdlObjectComp` с parent = sender |
| До HTTP | Прочитать и проверить его выборку; собрать запрос |
| Во время запроса | Сохранить pending payload; не пересоздавать его в `onResult()` |
| Успешный ответ | Опубликовать этот объект как `sdlObject`, затем вызвать его `fromObject()` |
| После заполнения | Обработчик payload `onFinished` получает заполненные `m_*`; sender сообщает `finished(1)` |
| Ошибка HTTP/GQL/выборки | Очистить и уничтожить pending payload; завершить запрос ошибкой |
| Повторная отправка | Создать новый payload, чтобы не сохранить старые значения отсутствующих полей |

Для одного sender предлагается один активный запрос. Второй `send()` до завершения
первого отклоняется и не должен завершать или уничтожать первый запрос. Проверка
активности должна действовать уже до `setGqlQuery()`, а не полагаться исключительно
на транспортное состояние `Loading`.

Последний успешный `sdlObject` сохраняется при неудачной следующей загрузке. При
успешной замене его освобождение выполняется после обработки нового ответа.
Контракт для потребителей: borrowed-ссылки на payload и его дочерние модели действуют
до следующей успешной замены или уничтожения sender. Если экран должен удерживать
модель дольше, нужны копирование данных или отдельное явное владение. Существующие
обработчики, присваивающие `m_*` в долгоживущие модели экранов, необходимо проверить
перед введением этого правила, а не уничтожать их данные незаметно.

Раннее создание меняет момент `Component.onCompleted` у payload. Такие обработчики
не должны считать, что данные уже получены. Обработку ответа оставляем в `onFinished`.
Текущий режим с заранее переданным `sdlObject` также требует отдельной проверки
миграции, чтобы владение и наличие `requestedFields` не определялись неявно.

## 8. Реальный пример миграции файлового браузера

Источники: [FileSystemBrowserDialog.qml](../../Qml/imtguigql/FileSystemBrowserDialog.qml)
и [FileSystem.sdl](../../Sdl/imtbase/1.0/FileSystem.sdl).

### Было

Текущий `createQueryParams()` добавляет и input, и поля:

```qml
function createQueryParams(query){
    var gqlObject = Gql.GqlObject("input")
    gqlObject.fromObject(getEntriesInput)
    query.AddParam(gqlObject)

    query.AddField(Gql.GqlObject("path"))
    query.AddField(Gql.GqlObject("parentPath"))
    query.AddField(Gql.GqlObject("totalCount"))
    query.AddField(Gql.GqlObject("hasMore"))

    var entriesFields = Gql.GqlObject("entries")
    entriesFields.InsertField("name")
    entriesFields.InsertField("path")
    entriesFields.InsertField("entryType")
    entriesFields.InsertField("size")
    entriesFields.InsertField("lastModified")
    entriesFields.InsertField("totalBytes")
    entriesFields.InsertField("freeBytes")
    query.AddField(entriesFields)
}
```

### Будет

```qml
GqlSdlRequestSender {
    id: requestSender
    context: root.context
    gqlCommandId: ImtbaseFileSystemSdlCommandIds.s_getFileSystemEntries

    sdlObjectComp: Component {
        GetFileSystemEntriesPayload {
            id: payload

            requestedFields: GetFileSystemEntriesPayloadFields {
                pathRequested: true
                parentPathRequested: true
                totalCountRequested: true
                hasMoreRequested: true

                entries: FileSystemEntryFields {
                    nameRequested: true
                    pathRequested: true
                    entryTypeRequested: true
                    sizeRequested: true
                    lastModifiedRequested: true
                    totalBytesRequested: true
                    freeBytesRequested: true
                }
            }

            onFinished: {
                root.applyPayload(payload)
            }
        }
    }

    function getHeaders(){
        return root.resolveHeaders()
    }

    function onError(message, type){
        root.onLoadFailed(message)
    }
}
```

В `browse()` вызывается `requestSender.send(getEntriesInput)` вместо `send()`;
переопределение `createQueryParams()` удаляется. Заголовки и context сохраняются.
Сборка выборки и очистка pending payload должны выполняться sender независимо от
переопределённого экраном `onError()`.

## 9. Вложенность, динамические флаги и особые типы

### Все скалярные поля без перечисления

Общий флаг позволяет не перечислять десятки однотипных полей. Он объявляется в
`GqlRequestedFields` один раз, а сгенерированные скалярные bool-свойства привязаны
к нему через `id` своего объекта. Сборщик по-прежнему читает конечное значение
каждого флага, а не выполняет `allScalarFieldsRequested || fieldRequested`:
такое OR-выражение сломало бы явные исключения `false`.

У `TenantMemberEntry` сейчас два скалярных поля, но тот же пример будет работать
без изменений при добавлении новых скалярных полей в SDL.

Запросить все поля `members`, не раскрывая другие ветки tenant:

```qml
TenantDataFields {
    idRequested: true
    nameRequested: true

    members: TenantMemberEntryFields {
        allScalarFieldsRequested: true
    }
}
```

Получится:

```graphql
tenant {
    id
    name
    members { id name }
}
```

Вообще не запрашивать `members`:

```qml
TenantDataFields {
    idRequested: true
    nameRequested: true
}
```

Это эквивалентно явному `members: null` и формирует `tenant { id name }`.
`members: TenantMemberEntryFields {}` без выбранных флагов всё ещё является
ошибкой пустой выборки; для выбора всех полей нужен явный общий флаг.

Все скалярные поля приглашения, кроме дат:

```qml
TenantInvitationEntryFields {
    allScalarFieldsRequested: true

    createdAtRequested: false
    expiresAtRequested: false
}
```

Если эта выборка подключена как `pendingInvitations`, получится:

```graphql
pendingInvitations {
    id
    userId
    userName
    status
    invitedByUserId
    invitedByName
}
```

Общий флаг может быть `false`, а отдельный флаг `true`: это обычная точечная
выборка. Явное значение или binding конкретного поля заменяет его стандартный
binding к общему флагу. При программном присваивании этот binding тоже снимается;
для возврата к исходным настройкам можно пересоздать объект выборки. Переключение
общего флага не должно отменять явно заданные исключения.

Новое скалярное поле SDL автоматически попадёт в выборку с общим флагом `true`.
Это сознательный контракт «все скалярные поля», а не бесплатная оптимизация:
скаляр тоже может требовать дорогого вычисления. Для чувствительных списков
остаётся предпочтительной явная выборка необходимых полей.

### Массивы скаляров и объектов

```qml
TenantDataFields {
    allScalarFieldsRequested: true
    tenantPermissionsRequested: false

    members: TenantMemberEntryFields {
        allScalarFieldsRequested: true
    }
}
```

Это выбирает все скалярные поля tenant, включая массив
`currentUserOrganizationPermissions: [ID]`, но исключает массив
`tenantPermissions`. Поле `members` получает свою выборку всех скаляров,
а `pendingInvitations` остаётся `null` и не включается в запрос.

Для скалярного массива нет bool-флагов на каждый элемент и нет дочернего блока
GraphQL. Для массива объектов одна выборка применяется ко всем элементам.
Размер массива, пустой массив, значение `null` и допустимость nullable-элементов
ответа не влияют на выборку: она строится до получения данных.

Модификаторы `[Type]`, `[Type!]`, `[Type]!` и `[Type!]!` отличаются требованиями
к данным, но не способом выбора полей. В существующем генераторе данные массивов
скаляров/enum представлены `var`, а объектов/union - `BaseModel`; в `Fields`
модель массива создавать не нужно. Проверки nullable-значений ответа остаются
ответственностью существующей десериализации, не общего флага выборки.

### Реактивные флаги

Обычные bool-свойства могут использовать bindings. Например, расширенные сведения
запрашиваются по настройке экрана:

```qml
tenant: TenantDataFields {
    idRequested: true
    nameRequested: true
    descriptionRequested: root.showDetails
}
```

Sender читает текущее дерево при каждом `send()`. Изменение флага после отправки
не меняет уже отправленный запрос и не инициирует новый запрос автоматически.
Кэшировать сериализованную выборку без учёта этих изменений нельзя.

Для отключения всей объектной ветки её свойство устанавливается в `null`.
Дополнительный флаг включения и неявное раскрытие всех дочерних полей не вводятся.
Если экран меняет ветку программно, он также управляет временем жизни созданного
для неё объекта: присваивание `null` само по себе не уничтожает `QObject`.

### Рекурсивные типы

Для SDL `TreeNode { name: String childNodes: [TreeNode] }` генерируется один
`TreeNodeFields` с `nameRequested`, привязанным к `allScalarFieldsRequested`,
и `TreeNodeFields childNodes: null`. Общий флаг по умолчанию `false`.
Конечная выборка двух уровней описывается обычной вложенностью:

```qml
TreeNodeFields {
    nameRequested: true
    childNodes: TreeNodeFields {
        nameRequested: true
        childNodes: TreeNodeFields {
            nameRequested: true
        }
    }
}
```

Нельзя останавливать клиентский сборщик лишь потому, что SDL-тип уже встречался:
это запретило бы корректные запросы конечной глубины. Проверяется цикл объектов
выборки по идентичности экземпляра на текущем пути, а не повтор имени типа.
Общий дочерний объект в двух независимых ветках не считается циклом.
`allScalarFieldsRequested: true` на рекурсивном типе выбирает только его
скаляры: он не создаёт `childNodes` и не запускает рекурсивное раскрытие.

В #942 серверное `RequestInfo` не раскрывает повтор типа на пути. Следовательно,
сформировать конечный рекурсивный GraphQL-запрос клиент может, но получить точные
серверные флаги для всех его глубоких полей пока нельзя. Это отдельная серверная
задача, не решаемая генерацией `*Fields`.

### Union

Для union требуется контейнер выборок его вариантов. Например, для существующей
тестовой схемы `Content = TextContent | ImageContent` предлагается `ContentFields`
со свойствами `TextContentFields textContent: null` и
`ImageContentFields imageContent: null`:

```qml
ContentFields {
    textContent: TextContentFields {
        allScalarFieldsRequested: true
    }
    imageContent: ImageContentFields {
        allScalarFieldsRequested: true
    }
}
```

Он должен сформировать:

```graphql
content {
    __typename
    ... on TextContent { text }
    ... on ImageContent { url }
}
```

`__typename` добавляется автоматически, чтобы существующая десериализация могла
выбрать конкретный тип. Отсутствие выбранных вариантов считается ошибкой даже при
автоматически добавленном `__typename`.

У union нет собственного набора обычных скалярных полей. Его общий флаг не
выбирает все варианты и не передаётся им. Чтобы запросить все скаляры каждого
варианта, нужно явно объявить каждый вариант и включить флаг внутри него, как
в примере. Вложенные объекты этих вариантов по-прежнему выбираются отдельно;
исключения конкретных скаляров работают так же, как у других `*Fields`.

Для `content: Content` и `content: [Content]` используются один и тот же
`ContentFields` и одинаковый GraphQL selection set. Во втором случае
`__typename` нужен для каждого ненулевого элемента ответа, а не для контейнера
массива. Не объявленный вариант не получает фрагмент; если сервер вернёт такой
конкретный тип, выборка предоставит только `__typename`, без полей других типов.

В JS-сборщике сейчас нет структурного API для inline fragments. Его надо добавить
отдельно; нельзя выдавать приведённый union-пример за работающий с текущим runtime.
И нельзя кодировать фрагмент строкой, подставляемой вместо обычного имени поля.
Серверный генератор также не строит вложенные bool-флаги вариантов union: выборка
вариантов не означает, что #942 уже обеспечивает их детальное вычисление по флагам.

### Что уже поддерживается, а что требует доработки

| Часть механизма | Состояние существующей реализации |
| --- | --- |
| Классификация enum и массивов | QML-генератор уже различает enum, скалярные массивы и массивы объектов; `Fields` должен использовать SDL-классификацию |
| Выборка скаляров и вложенных объектов | Текущий JS-сериализатор умеет собирать такие `GqlObject`; генерация `*Fields` и интеграция sender ещё нужны |
| `allScalarFieldsRequested` и исключения | Новое свойство общего builder и новые bindings сгенерированных флагов; сейчас в runtime этого API нет |
| Ответы union и массивов union | `BaseClass.fromObject()` и сгенерированные `createComponent()`/`createElement()` используют `__typename` для выбора типа |
| Inline fragments в C++ | C++-сборщик уже поддерживает `... on Type` |
| Inline fragments в JS/web | Нужен структурный API фрагментов в `GraphQLRequest.js` |
| Детальные серверные флаги union | #942 отслеживает само поле, но не создаёт вложенные флаги полей вариантов |
| Обязательные серверные поля | Их флаги остаются `true` независимо от клиентской выборки |
| Глубокие рекурсивные серверные флаги | Повтор типа на пути не раскрывается в текущем `RequestInfo` |

Поддержка выбора и десериализации не равна гарантии серверной проекции и
вычисления только выбранных данных. Union-фрагменты, обязательные поля и
рекурсивные типы требуют отдельных интеграционных проверок; смену конкретного
union-типа между ответами тоже нужно проверить с новым жизненным циклом payload.

### Коллекции с динамическими колонками

Typed `*Fields` хорошо подходит фиксированным SDL-payload. Но существующие
`headersModel` и `additionalFieldIds` нельзя автоматически заменить статической
выборкой и потерять зависимость от видимых колонок.

Для миграции коллекций нужен отдельный контракт: сгенерированный setter флага по
SDL field ID с проверкой имени и затем тот же сборщик `*Fields`. Перед новым
построением выборки из колонок создаётся чистый объект или явно сбрасываются
предыдущие флаги. Неизвестное поле является ошибкой. Без этого шага нельзя удалять
действующий механизм `CollectionRepresentation`.

Корневые scalar/enum-ответы и непосредственные массивы результатов не имеют
обычного payload-объекта. Их поддержка требует отдельного изменения контракта
sender и сериализации запроса без объектного selection set; такие операции не
считаются поддержанными только за счёт `*Fields`.

## 10. Генерация, ресурсы и имена

1. Найти объектные выходные типы query, mutation и subscription и достижимые из них типы.
2. Сгенерировать один `*Fields` на тип, а не на каждую команду или путь вложенности.
3. Типы, используемые только как input, не получают `*Fields`.
4. Для импортированного типа использовать `*Fields` из его исходного SDL-модуля, не дублировать файл в импортирующем модуле.
5. Добавить `requestedFields: null` в типы корневых объектных результатов.
6. Зарегистрировать `*Fields` как обычные типы, не singleton, в `qmldir`, `.qrc` и метаданных генерации.
7. Обновить списки выходных файлов/зависимостей сборки; новые файлы должны попадать и в Qt, и в web-сборку.
8. Добавить общий `GqlRequestedFields` в ресурсы и `qmldir` модуля `imtguigql`.
9. Проверять коллизии имён до записи файлов.
10. Генерировать binding каждого скалярного флага к общему `allScalarFieldsRequested`, а не константу `false`; объектные ветки оставлять `null`.

Пример новых записей `qmldir` для модуля tenants:

```text
GetTenantPayloadFields 1.0 GetTenantPayloadFields.qml
TenantDataFields 1.0 TenantDataFields.qml
TenantMemberEntryFields 1.0 TenantMemberEntryFields.qml
TenantInvitationEntryFields 1.0 TenantInvitationEntryFields.qml
```

Нельзя молча перезаписать настоящий SDL-тип `TenantDataFields`, если он уже есть.
Аналогично поле `membersRequested` в схеме может столкнуться с флагом, производным
от другого поля `members`. Коллизия со служебными именами базового builder или
общим `allScalarFieldsRequested` или QML-идентификаторами тоже требует явной
ошибки генерации. Автоматический
непредсказуемый rename не должен менять публичный API незаметно.

Реализация должна обходить зависимости с учётом типов, общих для нескольких
операций. Циклические типы требуют конечного набора файлов, а не рекурсивной
текстовой генерации вложенных структур.

## 11. Оптимизация и совместимость с web

Почему отдельные `*Fields` предпочтительнее флагов внутри обычных SDL-типов:

- На каждой строке ответа не появляются bool-флаги всех её полей.
- Выборка не создаёт `BaseModel`, данные, сериализацию и транзакции `BaseClass`.
- Объектные ветки по умолчанию `null`; экземпляры создаются лишь для объявленных веток.
- Одна выборка массива применяется ко всем элементам независимо от размера ответа.
- Сборщик проходит только включённые объектные ветки и bool-свойства посещённых типов.
- Сгенерированный код знает поле и тип; не требуется runtime-обход всех `m_*` или восстановление схемы из значений данных.

Стоимость не нулевая: больше файлов/ресурсов и типов в web-bundle, один объект
выборки на выбранную ветку и проверка скалярных флагов каждого посещённого типа.
Для типа с большим числом полей проход остаётся линейным по числу его флагов,
а не только по числу выбранных скаляров. Предложенные примеры не являются
бенчмарком и не доказывают конкретный выигрыш времени.

Общий флаг добавляет одно bool-свойство на объект выборки и bindings его
скалярных флагов. Они существуют только в `*Fields`, а не в каждой строке ответа.
Нельзя оптимизировать сборку веткой «общий флаг true - вывести все поля» и
пропустить проверку отдельных флагов: это проигнорирует явные исключения.

Сборку можно ускорять после измерений, но не путём глобального mutable singleton
выборки: два sender с разными полями не должны влиять друг на друга. Кэширование
в рамках sender допустимо только с надёжной инвалидизацией при изменении
`allScalarFieldsRequested`, любого отдельного флага, дочерней ветки и `sdlObjectComp`.

Для v3 используются обычные внешние QML-типы, `QtObject`, простые свойства,
методы и явные `id`. Не нужны `component Fields: QtObject`, вложенные имена типов
вида `TenantData.Fields`, arrow callbacks в декларациях или блоки `prop: { ... }`.

При этом совместимость нельзя считать проверенной по виду синтаксиса. Нужны
компиляция и runtime-проверка Qt и v3, особенно для nullable-рекурсивных ссылок,
импортов сгенерированных типов и вызова переопределённого `appendGqlFields()`.
Для общего флага отдельно проверяются bindings к унаследованному свойству,
их явное переопределение на экране и переключение значения между отправками.

## 12. Миграция и проверки реализации

Предлагаемая последовательность:

1. Добавить общий builder и генерацию `*Fields`, обновить эталоны QML-генератора.
2. Проверить `GetTenant` с вложенными массивами на обеих платформах.
3. Перевести sender на выборку внутри payload и новый жизненный цикл.
4. Перевести существующие `GqlSdlRequestSender` в ImtCore и потребляющих репозиториях.
5. Перевести файловый браузер, убрав поля из `createQueryParams()`.
6. Отдельно добавить union и контракт динамических колонок; затем мигрировать соответствующие экраны.

При интеграции нельзя включить строгую ошибку отсутствующей выборки и оставить
в том же релизе немигрированные sender. Внутренний API меняется явно; старый
переопределяемый `getRequestedFields()` не остаётся скрытым fallback.

Минимальные проверки:

| Случай | Ожидаемый результат |
| --- | --- |
| Несколько корневых полей | Каждый `GqlObject` добавлен отдельно; нет обёртки с именем команды |
| Только скаляры, enum, массив ID | Выводятся обычные имена полей без дочернего блока |
| Общий флаг по умолчанию | Все скалярные флаги `false`, все объектные ветки `null` |
| Общий флаг `true` | Выбраны все скаляры, enum и массивы скаляров/enum текущего типа |
| Общий флаг с исключением `false` | Исключённое поле отсутствует даже при общем флаге `true` |
| Общий флаг родителя | Не включает дочерние объекты, массивы объектов и варианты union |
| Переключение общего флага | Меняет флаги с сохранёнными bindings; не отменяет явные значения и bindings поля |
| Вложенный объект и массив объектов | Правильные блоки выборки на каждом уровне |
| Поле с тем же именем, что и родитель | Сборка не обрывается; выбираются и соседние поля |
| Флаг выключен / ветка `null` | Поле отсутствует в тексте запроса |
| Пустой корень / пустая вложенная ветка | Ошибка до HTTP с полным путём поля |
| Неверное свойство в QML | Диагностика Qt; в v3 проверить, что неизвестное свойство не игнорируется |
| Конечная рекурсивная выборка | Создаётся без автоматического бесконечного раскрытия |
| Цикл экземпляров выборки | Ошибка builder; общий дочерний объект в разных ветках допустим |
| Разные способы передачи input | Одинаковая выборка независимо от ветки сборки аргументов |
| Повторный запрос с более узкой выборкой | Нет старых значений в новом payload |
| Транспортная/GQL/локальная ошибка | Pending-объект освобождён, вызов завершён, экран не зависает |
| Второй send во время первого | Не портит первый запрос и его payload |
| Обработчики payload | `Component.onCompleted` до HTTP, `onFinished` после заполнения ответа |
| Сериализация/копирование данных | Флаги и вложенные `*Fields` не появляются в JSON данных |
| Union | `__typename` и inline fragments после отдельного расширения runtime |
| Массив union | Та же выборка вариантов; правильный конкретный тип каждого элемента по `__typename` |
| Union с общим флагом вариантов | Все скаляры выбранных вариантов с учётом исключений; вложенные объекты не раскрываются |
| Рекурсивный тип с общим флагом | Выбраны скаляры текущего уровня без создания рекурсивных веток |
| Qt и v3 | Одинаковый запрос; корректные импорты, ресурсы и работа bindings |

Клиентские запросы для поддержанных нерекурсивных типов нужно прогнать через
`CGqlRequest` и сгенерированный серверный wrapper из тестов #942. Проверять
следует как выбранные, так и исключённые необязательные поля; обязательные поля
сравниваются с текущей серверной семантикой, а не с предположением «все false».

## Итог

Экран объявляет `requestedFields: GetTenantPayloadFields { ... }` внутри
`GetTenantPayload` в `sdlObjectComp`. Все флаги и вложенность определены SDL и
задаются короткими обычными QML-свойствами. `allScalarFieldsRequested` позволяет
выбрать все скаляры текущего типа без перечисления с явными исключениями;
объекты, массивы объектов и варианты union по-прежнему объявляются отдельно.
Данные ответа остаются в `m_*`,
выборка живёт отдельно, новые файлы не создают дополнительных моделей на каждую
строку. Sender собирает выборку до отправки и заполняет тот же pending payload
после ответа. Документ фиксирует этот целевой контракт, но не заявляет, что он
уже реализован в PR #942.