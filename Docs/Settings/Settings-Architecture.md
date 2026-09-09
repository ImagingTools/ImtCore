# Настройки: текущая архитектура и план улучшения клиента

Дата: 2026-09-08
Статус: анализ + предложение (код не изменялся)

Документ состоит из трёх частей:

- **Часть I** — как настройки устроены сейчас, на сервере и на клиенте.
- **Часть II** — диагноз: что именно делает систему сложной.
- **Часть III** — подробный план улучшения **клиента**. Сервер (`ParamsSet` +
  `C*RepresentationControllerComp` + SDL + `.acc`) в этой итерации **не трогаем**.

---

# Часть I. Как устроено сейчас

## 1. Общая схема

```
                              СЕРВЕР

  .acc: GeneralPageDataModel               .acc: GeneralPageRepresentationModel
  ComposedParamsSet "General"              ParamSetRepresentationController "General"
    ├─ Design         SelectionParam  <-->   ├─ Design    SelectionParamRepresentationController
    ├─ Language       SelectionParam  <-->   ├─ Language  SelectionParamRepresentationController
    └─ PageViewParams ObjectCollection --->  (контроллера нет -> параметр молча пропускается)
              │                                          │
              │ iprm::IParamsSet                         │ IJsonRepresentationController
              v                                          v
  CUsersSettingsDatabaseDelegateComp            CSettingsControllerComp
  CCompactXmlMemWriteArchive                    OnGetSettings / OnSetSettings
  -> одна XML-строка в UserSettings.Settings                │
                                                            │
                          SDL: ParamsSet { parameters: [Parameter] }
                               Parameter  { id, typeId, name, description, enabled, data }
                                                            │  data = JSON-документ,
                                                            │  упакованный в строку
                                                            v
                              КЛИЕНТ

  UserSettingsController.qml  (GqlSdlRequestSender: GetSettings / SetSettings)
              │
              v
  ParamsSetController -> ParamController.createParamFromJson()
              │
  SettingsController.qml
      supportedParamEditors { typeId -> Component }    15 регистраций
      registeredControllers { paramId -> controller }  регистрация из ApplicationMain
      createRepresentation()  -> собирает фейковый ParamsSet из страниц
              │
  PreferenceDialog -> Preference -> ComposedParamsGui -> XxxParamEditor -> XxxElementView
              │
  ApplicationMain.onParamModelCreated(): ручное сопоставление
      getParameterById("Language")     -> languageProvider.selectionParam.createFromJson(...)
      getParameterById("DesignSchema") -> designProvider.selectionParam.createFromJson(...)
```

## 2. Сервер

### 2.1 Модель данных

Набор настроек — это **композиция ACF-компонентов**, описанная в
[GeneralPageDataModel.acc](../../Partitura/ImtCoreVoce.arp/GeneralPageDataModel.acc):
`ComposedParamsSet "General"` со списками `Parameters[]` / `ParametersId[]`, куда
входят `SelectionParam Design`, `SelectionParam Language` и `ObjectCollection
PageViewParams`. Допустимые значения выбора заданы отдельными элементами
`SelectionConstraints` (`OptionIds[]`, `OptionNames[]`).

### 2.2 Представление

Второе, параллельное дерево —
[GeneralPageRepresentationModel.acc](../../Partitura/ImtCoreVoce.arp/GeneralPageRepresentationModel.acc):
на каждый параметр свой `XxxParamRepresentationController` с продублированными
`ModelId` и `Name`, собранные в `ParamSetRepresentationController` через
`ParamRepresentationControllers[]`.

Конвертацию выполняет
[CParamSetRepresentationController](../../Include/imtserverapp/CParamSetRepresentationController.cpp):
для каждого параметра ищет подходящий контроллер, сериализует его SDL-представление
в `QJsonDocument::Compact` и кладёт результат **строкой** в `Parameter.data`.

Выбор контроллера — перебор с `dynamic_cast`:

```cpp
for (const IJsonRepresentationController* controllerPtr : m_representationControllers){
    if (controllerPtr != nullptr && controllerPtr->IsModelSupported(dataModel)){
        return controllerPtr;   // первый подошедший
    }
}
```

Если контроллер не найден — параметр `continue`-ится без ошибки
([CParamSetRepresentationController.cpp:66-69](../../Include/imtserverapp/CParamSetRepresentationController.cpp#L66-L69)).

Все `GetTypeId()`, участвующие в настройках, возвращают поле из
`sdl::V1_0::imtbase::CParamTypeIds`, то есть из фиксированного типа `ParamTypeIds`
в [ImtBaseTypes.sdl](../../Sdl/imtbase/1.0/ImtBaseTypes.sdl) (18 полей). Это
важное свойство — см. §III.3.1.

### 2.3 Транспорт

[Settings.sdl](../../Sdl/imtbase/1.0/Settings.sdl):

```graphql
Query    { GetSettings(input: GetSettingsInput): ParamsSet! }
Mutation { SetSettings(input: SetSettingsInput): SetSettingsPayload! }

input GetSettingsInput { userId: ID! }
input SetSettingsInput { userId: ID!  settings: String! }
```

`userId` приходит **от клиента**;
[CSettingsControllerComp](../../Include/imtservergql/CSettingsControllerComp.cpp#L33)
не сверяет его с аутентифицированным пользователем, хотя базовый класс
`CPermissibleGqlRequestHandlerComp` предоставляет `GetUserId(gqlRequest)`, а в
`SettingsController.acc` атрибуты `PermissionChecker`/`CommandPermissions` не
заданы. `CTableViewParamControllerComp`, наоборот, берёт userId из контекста —
поведение непоследовательно.

### 2.4 Хранение

[CUsersSettingsDatabaseDelegateComp](../../Include/imtauthdb/CUsersSettingsDatabaseDelegateComp.cpp)
сериализует весь `IParamsSet` в `iser::CCompactXmlMemWriteArchive` и пишет одной
строкой в `UserSettings(UserId, Settings)`.

### 2.5 Отдельный тракт: TableViewParams

`PageViewParams` — это `imtbase::IObjectCollection` внутри `IParamsSet`.
Representation-контроллера для коллекции нет, поэтому параметр не доезжает до
клиента, и для него сделан **отдельный API мимо SDL**:

- запрос собирается строками в
  [TableViewParamController.qml](../../Qml/imtguigql/TableViewParamController.qml#L33)
  (`Gql.GqlRequest("query", "GetTableViewParams")`);
- на сервере — 202 строки
  [CTableViewParamControllerComp](../../Include/imtservergql/CTableViewParamControllerComp.cpp),
  вручную повторяющие путь «взять/создать UserSettings → достать вложенную
  коллекцию → достать/создать ParamsSet → достать TableViewParam → сконвертировать
  → положить обратно → сохранить», с ветвлением insert/update в двух местах;
- там же костыль совместимости формата (`doc.isArray()` → оборачивание в `items`).

### 2.6 Тот же стек на клиенте

Для локальных настроек клиентское приложение запускает **ту же машинерию**:
[LocalSettings.acc](../../Partitura/ImtQmlVoce.arp/LocalSettings.acc) содержит
`GeneralPageDataModel` + `GeneralPageRepresentationModel` +
`ParamSetRepresentationController`, а
[CObserverQmlComp](../../Include/imtserverapp/CObserverQmlComp.cpp) кладёт JSON в
QML-свойство `localSettings` и принимает обратно через сигнал `saveSettings(QString)`.

## 3. Клиент

### 3.1 Получение и разбор

[UserSettingsController.qml](../../Qml/imtauthgui/UserSettingsController.qml) —
`ParamsSetController` с двумя `GqlSdlRequestSender` (`GetSettings`/`SetSettings`).
Полученный `ParamsSet` уходит в `createParamFromJson(this.toJson())`.

[ParamController.qml](../../Qml/imtgui/Params/ParamController.qml) на каждый
разбор **уничтожает** старый объект модели и создаёт новый из фабрики-компонента,
затем испускает `paramModelCreated()`.

### 3.2 Реестры и сборка диалога

[SettingsController.qml](../../Qml/imtgui/Settings/SettingsController.qml) держит
два словаря и 15 живых `property Component`:

- `supportedParamEditors` — `typeId -> Component`, заполняется 15 вызовами
  `registerParamEditor()` в `Component.onCompleted`;
- `registeredControllers` — `paramId -> {m_paramId, m_paramName, m_controller}`,
  заполняется извне через `registerParamsSetController()`.

Регистрация страниц — императивная и условная, поэтому её приходится выполнять
заново **перед каждым открытием диалога**:

```qml
// ApplicationMain.qml
function fillPreferenceParamsSet(){
    if (Qt.platform.os !== "web"){
        settingsController.registerParamsSetController("Network", qsTr("Network"), clientSettingsController)
    }
    if (application.serverConnected){
        settingsController.registerParamsSetController("General", qsTr("General"), userSettingsController)
        settingsController.registerParamsSetController("About", qsTr("About"), pageAboutProvider)
    }
}

// ThumbnailDecorator.qml
function showPreferencePage(){
    fillPreferenceParamsSet();
    ModalDialogManager.openDialog(preferenceDialogComp, {"paramsSet": settingsController.createRepresentation()});
}
```

`createRepresentation()` собирает **фейковый `ParamsSet`**, каждый элемент
которого — сериализованный в JSON `ParamsSet` страницы.

### 3.3 Отрисовка страницы

[Preference.qml](../../Qml/imtgui/Settings/Preference.qml): слева `ListView` по
страницам, справа `Repeater` по тем же страницам — **все страницы строятся сразу**,
скрытые через `visible`.

[ComposedParamsGui.qml](../../Qml/imtgui/Params/ComposedParamsGui.qml) (178 строк)
рисует одну страницу: `Repeater` по `m_parameters`, для каждого параметра ищет
редактор по `typeId`, грузит его в `elementLoader` либо (если `typeId == s_paramsSet`)
в `groupLoader` с рекурсивным `ComposedParamsGui`. Свойства и размеры
проставляются императивно:

```qml
function itemOnLoaded(item){
    if (item.paramId != undefined){ item.paramId = paramId }
    if (item.name != undefined){ item.name = paramName }
    ...
    if (item.border){ item.border.width = 0 }
    item.width = paramItem.width - 2 * groupElementView.border.width
}
```

Изменение вложенного редактора поднимается вверх конкатенацией пути:
`composedParamsGui.editorModelDataChanged(composedParamsGui.paramId + "/" + paramId, key)`.

### 3.4 Редакторы

[ParamEditorBase.qml](../../Qml/imtgui/Params/ParamEditorBase.qml) — контракт:
`paramId`, `name`, `description`, `readOnly`, `editorModel`, `sourceComp`,
`paramController`, сигнал `editorModelDataChanged`.

19 файлов в `Qml/imtgui/Params/` и 26 контролов `*ElementView` в `imtcontrols`.
`SelectionParamEditor` — 30 строк, `EnableableParamEditor` — 33. **Это хорошая
часть системы**, её менять не нужно.

### 3.5 Сохранение

[PreferenceDialog.qml](../../Qml/imtgui/Settings/PreferenceDialog.qml) копит
`paramIdsChanges`, восстанавливая id страницы **разбором пути по `/`**:

```qml
let mainParamId = paramId
if (paramId.includes("/")){
    let ids = paramId.split("/")
    mainParamId = ids[0]
}
```

На Apply вызывает `settingsController.saveParam(key)` для каждой изменённой
страницы. Cancel не делает ничего — язык и тема уже применены.

### 3.6 Элементы диалога, не являющиеся настройками

Чтобы показать строку текста и кнопку, `PageAboutProvider` изобретает параметры:

```qml
addParam(ParamTypeIdsTypeMetaInfo.s_textView, "ServerVersion", qsTr("Server Version"), "",
         serverVersionText.toJson())
addParam("ServerLog", "ServerLog", qsTr("Server Log"), "", "")
//       ^ выдуманный typeId          ^ пустые данные: параметра нет, нужен только «редактор»
```

```qml
// ServerLogProvider.qml — «редактор параметра», у которого нет параметра
ParamEditorBase {
    typeId: "ServerLog"
    sourceComp: Component { ButtonElementView { ... } }
}
```

`ServerLogProvider` при этом импортирует `QtQuick.Dialogs 1.3` и
`Qt.labs.platform` — нарушение правил QML проекта и неработоспособность в
web-сборке.

## 4. Цена добавления одной настройки сегодня

| # | Шаг | Файл |
|---|---|---|
| 1 | элемент-параметр | `GeneralPageDataModel.acc` |
| 2 | вписать в `Parameters[]` и `ParametersId[]` | там же |
| 3 | representation-контроллер того же типа | `GeneralPageRepresentationModel.acc` |
| 4 | вписать в `ParamRepresentationControllers[]` | там же |
| 5 | *(новый тип)* `CXxxParamRepresentationControllerComp` .h/.cpp | `Include/imtserverapp/` |
| 6 | *(новый тип)* регистрация в пакете + `cmake .` | `Impl/ImtServerAppPck` |
| 7 | *(новый тип)* SDL-тип + запись в `ParamTypeIds` | `ImtBaseTypes.sdl` |
| 8 | *(новый тип)* `XxxParamEditor.qml` | `Qml/imtgui/Params/` |
| 9 | *(новый тип)* `registerParamEditor(...)` | `SettingsController.qml` |
| 10 | прочитать значение: подписаться и вручную скопировать JSON | `ApplicationMain.qml` |
| 11 | локализация имени/описания | `.accl` |

## 5. Инвентарь кода

| Слой | ~LOC |
|---|---|
| Server: `imtserverapp/C*RepresentationController*` (не только настройки) | ~8000 |
| Server: GQL-обработчики настроек | ~1050 |
| Client: `Qml/imtgui/Params/*` | ~1270 |
| Client: `Qml/imtgui/Settings/*` + контроллеры настроек | ~860 |
| Реестры `.acc`/`.accl` | ~800 |

---

# Часть II. Диагноз

## Серверные проблемы (вне текущего объёма работ)

**S1. Двойная сериализация** — `Parameter.data` это JSON-документ внутри строки
внутри GraphQL. Значение проходит 4 сериализации в каждую сторону.

**S2. Тип определяется `dynamic_cast`-ом, а не схемой** — порядок регистрации
влияет на результат; родственные типы различаются только порядком.

**S3. Модель описана дважды** — данные и представление в двух `.acc`, рассинхрон
не даёт ошибки, параметр молча исчезает.

**S4. Схемы настроек нет** — клиент получает только значения: нет дефолтов, нет
списка возможных настроек, нет валидации на уровне набора.

**S5. Что не «параметр» — не пролезает** — отсюда отдельный API и 202 строки
`CTableViewParamControllerComp`.

**S6. Дубликаты и шов безопасности** — два одинаковых `CUserSettingsControllerComp`
(diff = 8 строк), два почти одинаковых интерфейса representation-контроллеров,
`userId` из клиента без проверки.

> Подробное предложение по серверу вынесено в §IV (приложение) и отложено.

## Клиентские проблемы (это и чиним)

| # | Проблема | Где |
|---|---|---|
| C1 | Реестр редакторов защищает от расширения, которого не может быть (§III.3.1) | `SettingsController.qml` |
| C2 | Регистрация страниц императивна и условна → перерегистрация перед каждым открытием | `fillPreferenceParamsSet()` |
| C3 | `createRepresentation()` пересобирает фейковый `ParamsSet`, страницы парсят JSON заново | `SettingsController`, `Preference` |
| C4 | Все страницы строятся сразу (`Repeater` + `visible`) | [Preference.qml:63-71](../../Qml/imtgui/Settings/Preference.qml#L63-L71) |
| C5 | Императивная раскладка: `item.width = ...`, `item.border.width = 0` | [ComposedParamsGui.qml:110-136](../../Qml/imtgui/Params/ComposedParamsGui.qml#L110-L136) |
| C6 | Путь строки склеивается через `/` и разбирается обратно `split("/")` | `ComposedParamsGui`, `PreferenceDialog` |
| C7 | Cancel не откатывает уже применённые язык и тему | `PreferenceDialog` |
| C8 | Не-настройки делаются фейковыми параметрами | `PageAboutProvider`, `ServerLogProvider` |
| C9 | Нет поиска, отката строки, индикации изменённого, диплинка | — |
| C10 | Неизвестный `typeId` → `console.error` и пустое место | [ComposedParamsGui.qml:67](../../Qml/imtgui/Params/ComposedParamsGui.qml#L67) |
| C11 | Список страниц не прокручивается (`height: contentHeight` внутри `Column`) | [Preference.qml:40-46](../../Qml/imtgui/Settings/Preference.qml#L40-L46) |
| C12 | 15 живых `property Component` ради карты типов | `SettingsController.qml` |
| C13 | 395 строк мёртвого кода: `SettingsProvider.qml`, `SettingsObserver.qml` | ссылок нет нигде |

---

# Часть III. План улучшения клиента

## 1. Границы

**Не трогаем:** SDL, все серверные компоненты, `.acc`-реестры,
`UserSettingsController.qml` / `ClientSettingsController.qml` (GQL-обмен),
контракт `ParamEditorBase`, все `XxxParamEditor.qml`, все `*ElementView`,
`TableViewParamController` (это серверный тракт).

**Меняем:** только слой между «пришёл `ParamsSet`» и «нарисована строка».

**Удаляем:** `registerParamEditor`, `registerParamsSetController`,
`createRepresentation`, `saveParam`, `fillPreferenceParamsSet`.

## 2. Что позволяет текущий протокол

`ParamsSet { parameters: [ Parameter { id, typeId, name, description, enabled, data } ] }`

**Возможно без сервера:** поиск, ленивые страницы, честный Cancel, откат строки к
значению на момент открытия, диплинк, агрегированная валидация (ограничения уже
внутри `data`: `ConstrainedInteger/DoubleParam` несут `minValue`/`maxValue`,
`SelectionParam` — `options[].enabled`; редакторы их читают), не-настройки без
фейковых параметров, кастомные редакторы.

**Невозможно без сервера** — и это не надо имитировать:

- «Сброс к заводскому значению»: `defaultValue` не приходит. Заменяется на
  «вернуть значение, которое было при открытии».
- Поиск по ещё не загруженным страницам: схемы нет.
- `Parameter.enabled`: поле есть, но сервер его не заполняет. Клиент готовим
  заранее — пришло `false`, строка неактивна; заработает само.
- Пер-ключевое сохранение: `SetSettings` принимает набор целиком, гранулярность
  остаётся «страница».

## 3. Почему реестры не нужны

### 3.1 `registerParamEditor` — множество типов закрыто по построению

`typeId` приходит с сервера из `GetTypeId()`. Проверено по всем реализациям в
`imtserverapp`: все, что участвуют в настройках, возвращают поле из
`CParamTypeIds`, то есть из фиксированного SDL-типа `ParamTypeIds`. Строковые
литералы (`ComplexFilter`, `DocumentFilter`, `TimeFilter`) принадлежат фильтрам
коллекций, а не настройкам.

Новый тип параметра невозможно ввести, не отредактировав SDL и не написав
серверный компонент. **Открытый реестр защищал от расширения, которого не может
произойти.** Карта `typeId -> Component` — закрытая таблица внутри `imtgui`.

Подтверждение практикой: во всём репозитории `registerParamEditor` вызывается из
прикладного кода ровно один раз — для `"ServerLog"`, и это не тип параметра, а
хак с кнопкой.

### 3.2 `registerParamsSetController` + `fillPreferenceParamsSet` — ручная эмуляция биндинга

Набор страниц зависит от `Qt.platform.os` и `application.serverConnected` —
условий, меняющихся во времени. Именно поэтому функцию вызывают заново перед
каждым открытием диалога, а регистрация молча перезаписывает запись в словаре.
Декларативно это `visible:`.

### 3.3 `createRepresentation` — промежуточная структура ради одного `Repeater`

Фейковый `ParamsSet` нужен только чтобы `Preference.qml` мог рисовать страницы
тем же кодом, что и вложенные параметры. Диалог, читающий страницы напрямую, в
нём не нуждается — вместе с ним уходит пересборка и повторный парсинг JSON на
каждое открытие. Эта же структура — причина фейковых параметров (§I.3.6).

### 3.4 `saveParam(id)` — метод не того объекта

Сохранение страницы — операция страницы, а не поиск в словаре по строке, для
которого диалогу приходится восстанавливать id разбором пути.

## 4. Целевая архитектура: три понятия

| Понятие | Роль | Кто видит |
|---|---|---|
| `SettingsPage` | страница слева в диалоге | прикладной код |
| `SettingsRow` и виды | строка справа | прикладной код |
| `SettingsDraft` | что изменено и что сохранять | только диалог |

Виды строк: `SettingsRow` (настройка), `SettingsInfoRow` (read-only текст),
`SettingsActionRow` (кнопка), `SettingsLinkRow` (переход), `SettingsCustomRow`
(произвольный компонент), `SettingsGroupRow` (заголовок с вложенными).

### Полный пример: вся конфигурация приложения

Заменяет то, что сейчас размазано по `ApplicationMain`, `ThumbnailDecorator`,
`SettingsController`, `PageAboutProvider` и `ServerLogProvider`:

```qml
SettingsPage {
    pageId: "General"
    name: qsTr("General")
    order: 10
    visible: application.serverConnected
    controller: userSettingsController
}

SettingsPage {
    pageId: "Network"
    name: qsTr("Network")
    order: 20
    visible: Qt.platform.os !== "web"
    controller: clientSettingsController

    SettingsRow {
        paramPath: "ServerConnectionInterface"
        editorComp: Component { ServerConnectionParamEditor {} }
    }
}

SettingsPage {
    id: aboutPage

    pageId: "About"
    name: qsTr("About")
    order: 100
    visible: application.serverConnected

    SettingsInfoRow {
        rowId: "ServerVersion"
        name: qsTr("Server Version")
        text: application.serverVersion
    }

    SettingsActionRow {
        rowId: "ServerLog"
        name: qsTr("Server Log")
        buttonText: qsTr("Download")

        onTriggered: {
            serverLogDownloader.download()
        }
    }
}
```

Страница с `controller` показывает строки, пришедшие с сервера. Страница без
`controller` — чисто клиентская. Можно и то, и другое: строки контроллера идут
первыми, объявленные — по `order`. Объявленная `SettingsRow` с `paramPath`,
совпадающим с автоматической строкой, **заменяет** её.

### Что чем заменяется

| Было | Стало |
|---|---|
| `registerParamsSetController(id, name, ctrl)` | `SettingsPage { pageId; name; controller }` |
| `registerParamEditor(typeId, comp)` | закрытая таблица в `imtgui` + `SettingsRow.editorComp` |
| `createRepresentation()` | ничего — диалог читает страницы напрямую |
| `saveParam(id)` | `page.save()`, вызывается из `SettingsDraft.apply()` |
| `fillPreferenceParamsSet()` + `if (...)` | `visible:` на странице |
| фейковый параметр + «редактор» без параметра | `SettingsInfoRow` / `SettingsActionRow` |
| `paramIdsChanges` + `split("/")` | `path` — поле строки |

## 5. Внутреннее устройство

```
SettingsPages.qml (singleton)      страницы регистрируют себя сами;
        ^                          прикладной код к нему не обращается
        | Component.onCompleted / onDestruction
SettingsPage.qml                   pageId, name, order, visible, controller,
        |                          default property list<QtObject> rows, save()
        v
PreferenceDialog.qml
    SettingsPageList.qml           слева: прокрутка, поиск
    Loader (active: страница текущая)
        SettingsPageView.qml       справа: Repeater по строкам
            SettingsRowModel.qml   ParamsSet -> плоский список строк с path
                Loader -> XxxParamEditor -> XxxElementView    (без изменений)
    SettingsDraft.qml              снимок, изменённые страницы, apply/revert, isValid
```

Синглтон `SettingsPages` — тот же приём, что уже применяется в проекте
(`PopupManager.popupContainer = popupContainer` в `ThumbnailDecorator`,
`ModalDialogManager`, `MainDocumentService`). Нового допущения о единственном
экземпляре приложения он не вводит: оно уже сделано в этих трёх местах.

### 5.1 `SettingsPage.qml`

```qml
import QtQuick 2.12
import imtgui 1.0

QtObject {
    id: root

    default property list<QtObject> rows

    property string pageId: ""
    property string name: ""
    property int order: 0
    property bool visible: true
    property var controller: null        // ParamsSetController или null

    function save(){
        if (root.controller){
            root.controller.saveParam()
        }
    }

    Component.onCompleted: {
        SettingsPages.addPage(root)
    }

    Component.onDestruction: {
        SettingsPages.removePage(root)
    }
}
```

`default property list<QtObject> rows` даёт декларативную вложенность и
индексный доступ (`rows.length`, `rows[i]`), не полагаясь на `children` —
в этом проекте `children` не JS-массив и `.slice()` на нём падает.

### 5.2 `SettingsPages.qml` (singleton)

```qml
pragma Singleton

import QtQuick 2.12

QtObject {
    id: root

    property var pages: []

    signal pageRequested(string pageId, string rowPath)

    function addPage(page){
        let list = root.pages.slice()
        list.push(page)
        list.sort(root.comparePages)
        root.pages = list
    }

    function removePage(page){
        let list = []
        for (let i = 0; i < root.pages.length; i++){
            if (root.pages[i] !== page){
                list.push(root.pages[i])
            }
        }
        root.pages = list
    }

    function comparePages(a, b){
        return a.order - b.order
    }

    function visiblePages(){
        let list = []
        for (let i = 0; i < root.pages.length; i++){
            if (root.pages[i].visible){
                list.push(root.pages[i])
            }
        }
        return list
    }

    function showPage(pageId){
        root.pageRequested(pageId, "")
    }

    function showRow(path){
        let ids = path.split("/")
        root.pageRequested(ids[0], path)
    }
}
```

`pages` переприсваивается целиком — мутация массива на месте не испускает
сигнал изменения.

### 5.3 `SettingsRowModel.qml` — плоская модель строк

Самая содержательная часть. На входе — `SettingsPage`, на выходе — плоский
список описаний строк:

```
{ kind,          // "param" | "group" | "info" | "action" | "link" | "custom"
  path,          // "Proxy/Host" — реальное поле, а не склейка
  typeId, name, description, enabled,
  data,          // JSON-строка параметра
  editorComp,    // из SettingsRow.editorComp, иначе из SettingsEditors
  groupLevel,    // отступ
  owner, index,  // владелец (ParamsSet) и позиция — для обратной записи
  rowObject }    // объявленная SettingsRow, если есть
```

Построение:

1. Если у страницы есть `controller` — взять `controller.getParam()` и обойти
   `m_parameters`. Для параметра с `typeId === s_paramsSet` создать вложенный
   `ParamsSet` из его `m_data`, добавить строку `kind: "group"` и обойти
   вложенный набор рекурсивно, увеличив `groupLevel`. Рекурсия происходит
   **один раз при построении модели**, а не через вложенные `ComposedParamsGui`.
2. Добавить объявленные строки страницы: `SettingsRow` с существующим
   `paramPath` заменяет автоматическую строку (подмена редактора/порядка),
   остальные добавляются и сортируются по `order`.

Обратная запись — вместо цепочки вложенных `Connections`:

```qml
function writeBack(row, json){
    row.owner.m_parameters.get(row.index).item.m_data = json

    // поднять изменение до корня страницы
    let owner = row.owner
    while (owner !== root.pageParamsSet){
        let parentInfo = root.ownerInfo(owner)
        parentInfo.owner.m_parameters.get(parentInfo.index).item.m_data = owner.toJson()
        owner = parentInfo.owner
    }

    root.pageChanged(root.page.pageId)
}
```

`ownerInfo` — карта «вложенный набор → (владелец, индекс)», заполняемая при
построении. Это то же самое, что сейчас делает цепочка `onEditorModelDataChanged`
в `ComposedParamsGui`, но в одном месте и без склейки строковых путей.

### 5.4 `SettingsPageView.qml`

`Repeater` по строкам модели, `Loader` на каждую. Свойства передаются
декларативно, через свойства `Loader`, а не присваиваниями в `itemOnLoaded()`:
ширина строки — `width: parent.width`, отступ группы — от `groupLevel`.
Неизвестный `typeId` даёт строку-заглушку с предупреждением, а не пустоту.

### 5.5 `SettingsEditors.qml` — закрытая таблица

Одно место, где `typeId` сопоставляется компоненту редактора, для закрытого
набора `ParamTypeIds`. Никакого публичного `register*`. Частный случай
(свой редактор для конкретного параметра) решается `SettingsRow.editorComp`.

### 5.6 `SettingsDraft.qml`

```
snapshot()            для каждой видимой страницы с controller:
                          originals[pageId] = page.controller.getParam().toJson()
markChanged(pageId)   пометить страницу изменённой
changedPages          список изменённых
isValid               агрегируется из редакторов
apply()               для каждой изменённой страницы -> page.save()
revert()              page.controller.createParamFromJson(originals[pageId])
```

`revert()` переиспользует существующий путь `createParamFromJson` →
`paramModelCreated`, на который уже подписаны `languageProvider` и
`designProvider`. Поэтому Cancel возвращает и язык, и тему — то, чего сейчас
не происходит.

### 5.7 Поиск

Ленивое построение страниц и поиск по всем страницам конфликтуют: поиск требует
имён со всех страниц. Решение — лёгкий индекс: при первом обращении к поиску
один раз строятся модели всех видимых страниц, из них берутся `path`, `name`,
`description`; индекс кэшируется и сбрасывается при изменении набора страниц.
Обычное открытие диалога остаётся ленивым.

### 5.8 Диплинк

`SettingsPages.showPage("Network")` и
`SettingsPages.showRow("Network/ServerConnectionInterface")` испускают
`pageRequested`; `ThumbnailDecorator` открывает диалог, передавая
`initialPageId` / `initialRowPath`. Механизм событий уже есть
(`Events.sendEvent("ShowPreferencePage")` с аргументом).

## 6. Файлы

### Новые (`Qml/imtgui/Settings/`)

| Файл | ~LOC |
|---|---|
| `SettingsPages.qml` (singleton) | 60 |
| `SettingsPage.qml` | 60 |
| `SettingsRow.qml`, `SettingsInfoRow`, `SettingsActionRow`, `SettingsLinkRow`, `SettingsCustomRow`, `SettingsGroupRow` | ~30 каждый |
| `SettingsRowModel.qml` | 110 |
| `SettingsPageView.qml` | 90 |
| `SettingsPageList.qml` | 70 |
| `SettingsDraft.qml` | 90 |
| `SettingsEditors.qml` | 60 |

### Удаляемые

| Файл | LOC | Причина |
|---|---|---|
| `SettingsProvider.qml` | 328 | мёртвый код |
| `SettingsObserver.qml` | 67 | мёртвый код |
| `SettingsController.qml` | 131 | реестры не нужны |
| `ComposedParamsGui.qml` | 178 | заменён `SettingsRowModel` + `SettingsPageView` |
| `Preference.qml` | 120 | заменён `SettingsPageList` + `SettingsPageView` |
| `PageAboutProvider.qml` | 37 | заменён декларативной страницей |
| `ServerLogProvider.qml` | 48 | становится `SettingsActionRow`, без `QtQuick.Dialogs` |

Итого удаляется ~909 строк, добавляется ~700, и из них ничего не является
реестром или промежуточным представлением.

**Остаются без изменений:** `ParamEditorBase.qml`, все `XxxParamEditor.qml`, все
`*ElementView`, `ParamsSetController.qml` и `ParamController.qml` (они разбирают
`Parameter.data`, пока формат «JSON в строке» живёт на сервере).

## 7. Улучшения UX

| # | Улучшение | Закрывает |
|---|---|---|
| U1 | Поиск по имени/описанию/пути с указанием страницы | C9 |
| U2 | Ленивое построение страниц (`Loader.active`) | C4 |
| U3 | Честный Cancel + откат отдельной строки | C7 |
| U4 | Apply активен только при валидном состоянии, ошибка в `descriptionColor` | C9 |
| U5 | Индикация изменённых строк и страниц | C9 |
| U6 | Диплинк `showPage` / `showRow` | C9 |
| U7 | Строка-заглушка при неизвестном `typeId` | C10 |
| U8 | Прокрутка списка страниц | C11 |
| U9 | Декларативные ширины | C5 |
| U10 | Клавиатура: фокус, Esc, Enter | — |
| U11 | Готовность к `Parameter.enabled` | — |

## 8. Миграция внешних приложений

`ApplicationMain` наследуют ~10 приложений вне репозитория. Замена механическая:

```qml
// было
Component.onCompleted: {
    settingsController.registerParamEditor("MyType", myEditorComp)
}
function fillPreferenceParamsSet(){
    settingsController.registerParamsSetController("MyPage", qsTr("My page"), myController)
}

// стало
SettingsPage {
    pageId: "MyPage"
    name: qsTr("My page")
    order: 50
    controller: myController

    SettingsRow {
        paramPath: "MyParam"
        editorComp: myEditorComp
    }
}
```

Риск: QML не проверяется при сборке, поэтому забытый вызов удалённой функции
проявится в рантайме при открытии диалога. Предлагается на один релиз оставить
`SettingsController.qml` **надгробием**: те же четыре функции, ничего не делают,
каждая пишет `console.error` с указанием замены. Это не shim — функциональности
в нём нет, он только делает поломку видимой. Через релиз файл удаляется.
Альтернатива, если все приложения обновляются согласованно, — удалить сразу.

## 9. Порядок работ

**Шаг 1 — уборка.** Удалить `SettingsProvider.qml`, `SettingsObserver.qml` и их
строки в `imtguiqml.qrc`. Поведение не меняется, проверяется существующими
GUI-тестами.

**Шаг 2 — страницы и строки.** `SettingsPages`, `SettingsPage`, виды строк,
`SettingsRowModel`, `SettingsPageView`, `SettingsPageList`, `SettingsEditors`.
`ApplicationMain` переводится на декларации. Удаляются `SettingsController`,
`ComposedParamsGui`, `Preference`, `PageAboutProvider`, `fillPreferenceParamsSet`.
Закрывает C1–C6, C8, C10–C13; U2, U7, U8, U9.

**Шаг 3 — `SettingsDraft`.** Apply / Cancel / валидность / индикация: U3, U4, U5.

**Шаг 4 — поиск, диплинк, клавиатура:** U1, U6, U10.

Шаги 2–4 меняют UI и требуют обновления скриншотных базлайнов (перед прогоном
восстанавливать БД, иначе дрейф данных читается как расхождение). Правки QML
требуют пересборки ресурса `imtguiqml` — иначе изменения не подхватываются.

## 10. Вопросы

1. **Надгробие или чистое удаление** `SettingsController.qml` на один релиз?
   Рекомендую надгробие: поломка становится видимой и называет замену.
2. **Порядок страниц** — числовой `order` с шагом 10 (рекомендую) или явный список?
3. **Инлайн-применение**: язык и тема применяются сразу, остальное по Apply.
   Оставить как есть или сделать явным свойством строки `applyMode`?

---

# Часть IV. Приложение: серверная часть (отложено)

Не входит в текущий объём работ, зафиксировано, чтобы не потерять.

Целевая идея: **настройка = ключ + тип + значение**, схема декларируется одним
файлом вместо двух `.acc`-деревьев.

```json
{ "id": "Language", "type": "enum", "scope": "user",
  "default": "en_US", "optionsProvider": "Languages" }
```

```graphql
Query    { GetSettingsSchema: SettingsSchema!   GetSettings(keys: [ID]): [SettingValue]! }
Mutation { SetSettings(input: SetSettingsInput): SetSettingsPayload! }
```

Ключевые следствия: `userId` уходит из входных аргументов (берётся из GQL-контекста,
закрывает S6); значение — плоская строка вместо JSON-в-строке (S1); тип из схемы,
а не из `dynamic_cast` (S2); один источник истины (S3); дефолты и ограничения
приходят клиенту (S4, а с ними — «сброс к заводскому» и полноценный поиск);
table view params становятся ключом `Views/<tableId>` и отдельный API из 202
строк исчезает (S5).

Серверная реализация — один интерфейс `imtapp::ISettingsStore` и два компонента
(декларативный и мост поверх нынешнего `IParamsSet` на время миграции).
Клиентская часть, описанная в Части III, к этому готова: меняется только
источник значения для строки, `SettingsPage`/`SettingsRow`/`SettingsDraft`
остаются как есть.
