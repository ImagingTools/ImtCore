# C++-типы и их аналоги в вебе (JQML v4)

На десктопе часть QML-типов приходит из C++: их регистрируют `qmlRegisterType` и `qmlRegisterSingletonInstance` в модуле
`com.imtcore.imtqml` (см. `Impl/JQMLHost/Main.cpp`, `Include/imtqml/CStaticQmlBaseTypeRegistratorComp.cpp`).
В браузере C++ нет, поэтому движок JQML v4 (`Tools/JQML/v4`) содержит **собственные классы на JavaScript с тем же именем и тем же
интерфейсом**. Страница пишет `TreeItemModel { }` или `RemoteFileController { }` и на десктопе, и в вебе, ничего не меняя.

Этот файл отвечает на вопрос: какие C++-типы в вебе уже реализованы, какие нет и как добавить новый.

## Как движок выбирает тип

Для `Name { }` движок (`createItem` в `src/engine.js`) идёт в таком порядке:

1. Встроенные типы движка: классы из `BUILTIN_TYPES` (`src/items.js`) и несколько типов, которые `createItem` создаёт отдельно
   (`TreeItemModel`, `ListModel`, `Loader` и другие). **Аналоги C++-типов находятся здесь.**
2. Тип из QML-файла: по `qmldir` импортированного модуля или по каталогу файла.
3. Неизвестный тип: движок создаёт объект-заглушку (`Placeholder`, для SDL-типов с полями `m_...`). Вызов метода, которого у заглушки
   нет, даёт ошибку `... is not a function`.

Поэтому встроенный тип всегда главнее одноимённого QML-файла. Так, `imtguigql/qmldir` держит QML-версию под именем
`RemoteFileController__`: на десктопе она не мешает C++-типу, а в вебе страница получает класс движка.

## Что реализовано

| C++-тип (`com.imtcore.imtqml`) | Класс в вебе (`src/items.js`) | Что делает в вебе |
|---|---|---|
| `TreeItemModel` (`imtbase::CTreeItemModel`) | `TreeItemModel` | Модель из строк «ключ → значение». Значение: обычное или вложенная модель. Читается из JSON, отдаёт JSON. |
| `GqlModel` (`imtqml::CGqlModel`) | `GqlModel` (наследует `TreeItemModel`) | Заполняет себя ответом GraphQL-запроса через `XMLHttpRequest`. `state`: `Loading`, `Processing`, `Ready`, `Unauthorized`, `Forbidden`, `Error`. Токен и id продукта хранятся на конструкторе `XMLHttpRequest`. |
| `WebSocket` (`imtqml::CQmlWebSocket`) | `WebSocketItem` | Поверх `WebSocket` браузера: `url`, `active`, `status`, `sendTextMessage`, `sendBinaryMessage`, сигналы сообщений. |
| `RemoteFileController` (`imtqml::CRemoteFileController`) | `RemoteFileController` | `sendFile`, `getFile`, `deleteFile`, свойства `state`, `json`, `prefix`, сигналы как у C++-типа. Адрес файла: `../../<prefix>/<имя>`. Скачивание открывает адрес в новой вкладке (`window.open`), браузер сам сохраняет файл. Пока `prefix` пуст, используется `files`. |
| SDL-объекты (`qmlRegisterType<C...Object>` в сгенерированном коде) | `createSdlObject` | Заглушка с полями `m_...` и методами `hasX`, `emplaceX`, `removeX`, `clearX`, `copyFrom`. |

### Что у `TreeItemModel` не реализовано

Реализована часть интерфейса, которой пользуются страницы. Есть: `addTreeModel`, `clear`, `containsKey`, `copy`, `copyItemDataFromModel`,
`copyMe`, `createFromJson`, `getData`, `getItemsCount`, `getKeys`, `getTreeItemModel`, `insertNewItem`, `isValidData`, `refresh`,
`removeItem`, `setData`, `setExternTreeModel`, `toJson`. Методов C++-типа, которых в вебе пока нет:
`beginChanges`, `endChanges`, `clearQueryParams`, `getQueryParam`, `getQueryParams`, `setQueryParam`, `takeQueryParam`,
`copyItemDataToModel`, `getModelFromItem`, `getParent`, `setParent`, `insertNewItemWithParameters`, `isArray`, `setIsArray`,
`isEqualWithModel`, `isTreeModel`, `removeData`, `setUpdateEnabled`, `swapItems`.

## QML-типы, которые движок реализует сам

`BaseClass` и `BaseModel` (`Qml/imtcontrols/Base`) — не C++-типы, а QML-файлы: от них наследуются все сгенерированные SDL-классы, и
каждый объект ответа сервера (строка таблицы, измерение, цвет) — это `BaseClass`. Ответ таблицы на 1500 строк даёт около 10 тысяч
таких объектов, и как QML-объекты они стоили 3 секунды из 3,1. Поэтому в вебе движок исполняет **свои классы** `BaseClass` и
`BaseModel` (`src/items.js`, раздел «BaseClass and BaseModel») вместо файлов. На десктопе по-прежнему работают файлы.

Что это значит для разработки:

- Любое изменение `BaseClass.qml` или `BaseModel.qml` (метод, поведение `fromObject`, `toJson`, уведомления `modelChanged`) нужно
  повторить в классах движка, иначе веб и десктоп разойдутся.
- `node test/sdl-base.mjs` запускает один сценарий над SDL-объектами (`test/sdl-table`) и с классами движка, и с QML-файлами и сравнивает
  результаты, JSON, GraphQL и сообщения об изменениях. Тест упадёт, если классы разошлись.
- `Qt.createComponent('BaseModel.qml')` и `createComponent('...BaseClass.qml')` в вебе возвращают компонент типа движка.
- Отличия от файлов сделаны ради скорости и не видны снаружи: свойства объекта следят за собой в месте записи (а не подключают
  функцию к каждому `mXxxChanged`), элементы массива кладутся в `BaseModel` одним изменением, `_internal` — обычный объект, а не `QtObject`.

## Что не реализовано

Страница, которая использует такой тип, в вебе получает заглушку. Если в QML-файле есть ветка для веба
(`Qt.platform.os === "web"`), она обходит тип, иначе вызов его методов даст ошибку.

| C++-тип | Где используется | Что в вебе |
|---|---|---|
| `GqlRequest` (`imtqml::CGqlRequest`) | `imtguigql/GqlSdlRequestSender.qml` и ещё 3 файла `imtguigql` | Запросы строит JavaScript-библиотека `Gql.GqlRequest(...)` (`imtguigql/qmldir`: `Gql 1.0 ../web/GraphQLRequest.js`). |
| `FileIO` (`imtqml::FileIO`) | `FileExportDialog`, `FileImportDialog`, `CollectionViewCommandsDelegateBase`, `TicketEditor` | Нет файловой системы. Диалоги экспорта и импорта имеют ветки для `web`. |
| `Process` (`imtqml::CQmlProcess`) | `DatabaseBackupController`, `DatabaseAccessSettingsEditor` | Нет. Запуск процессов в браузере невозможен. |
| `DataModelController` (`imtqml::CDataModelController`) | `GqlBasedDataModelController.qml` | Нет аналога. |
| `DocumentServiceController` (`imtqml::CDocumentServiceController`) | В QML-коде этого репозитория не найден | Нет. |
| `NetworkEventInterceptor` (singleton) | `XmlHttpRequestProxy.qml` | Нет. |
| `Render3DView`, `Scene3DView` (`imt3dgui`) | 3D-страницы | Нет. |

## Как добавить аналог нового C++-типа

1. Написать класс в `Tools/JQML/v4/src/items.js` рядом с `RemoteFileController` или `WebSocketItem`:
   `class X extends Item`, в конструкторе `this.$typeName = 'X'` и `this.$nonVisual = true`.
   Свойства объявляются через `this.$defineCustom(имя, значение)`, сигналы через `this.$declareSignal(имя, [параметры])`,
   методы пишутся как обычные методы класса с теми же именами, что у `Q_INVOKABLE` C++-типа.
2. Добавить класс в `BUILTIN_TYPES` (в конце `items.js`).
3. Написать тест в `Tools/JQML/v4/test/` (образец: `test/remote-file-controller.mjs`).
4. Внести тип в таблицу «Что реализовано» этого файла и убрать из таблицы «Что не реализовано».

## Как проверить в работающем приложении

В консоли браузера: `window.qmlWeb.engine.unknownTypes` показывает типы, для которых движку не хватило ни встроенного класса,
ни QML-файла, с числом созданных заглушек.
