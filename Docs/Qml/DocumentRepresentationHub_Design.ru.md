# Единая точка работы с репрезентациями документа

## Статус

Это проект, а не описание существующего API. Код по нему не написан и не запускался.
Все новые имена условные: `DocumentRepresentationHub`, `DocumentRepresentationBranch`,
`Document`, `readAfter`, `onBranchEvent`, `modelPath`.

Проект опирается на PR [#1048](https://github.com/ImagingTools/ImtCore/pull/1048)
(на 2026-11-04 он открыт): метку правки `changeId`, `changeSet` с полными путями и общие
отправители запросов. Текущая работа клиента описана в
[DocumentService_Client_Architecture.ru.md](DocumentService_Client_Architecture.ru.md).

Локальный `DocumentService` (вариант A) проект не затрагивает.

## Содержание

1. [Зачем это нужно](#1-зачем-это-нужно)
2. [Основные решения](#2-основные-решения)
3. [Состав системы](#3-состав-системы)
4. [Document](#4-document)
5. [DocumentRepresentationHub](#5-documentrepresentationhub)
6. [DocumentRepresentationBranch](#6-documentrepresentationbranch)
7. [N view и M веток](#7-n-view-и-m-веток)
8. [Чтение после своих изменений](#8-чтение-после-своих-изменений)
9. [Общение веток](#9-общение-веток)
10. [Примеры](#10-примеры)
11. [Что заменяется](#11-что-заменяется)
12. [Порядок перехода](#12-порядок-перехода)
13. [Риски и что надо проверить](#13-риски-и-что-надо-проверить)
14. [Открытые вопросы](#14-открытые-вопросы)

## 1. Зачем это нужно

Клиент сейчас работает так: у каждого view свой `DocumentRepresentationController` и своя
модель, `DocumentDecorator` связывает их с документом, а хосты собирают состояние документа
из сигналов сервиса.

Что мешает:

| Проблема | Факт |
|---|---|
| Состояние документа нельзя привязать к свойствам | В `DocumentServiceBase` 46 сигналов и 62 функции, состояние доступно только через функции |
| Хосты повторяют одну логику | Около 44 обработчиков в `MultiDocumentCollectionView`, около 41 в `SingleDocumentCollectionView`, 11 в `DocumentViewBase` |
| Кэши из-за порядка событий | `cachedDocumentNames`, `cachedDocumentObjectIds`, `pendingDataLoaded`, вкладки `opening:`, `cancelledOpenings` |
| Координация растёт | После #1048 `DocumentDecorator` занимает около 820 строк: 8 параллельных массивов на view, очередь команд, таймеры на 3 и 30 секунд |
| Запись идёт на view, а не на документ | Два view могут писать одновременно, а порядок HTTP-запросов не гарантирован |
| Чтение после правки решается грубо | `requestRepresentationOnUpdate` работает по принципу «всё или ничего» и не зависит от того, что изменилось; другие view перечитываются всегда |
| Логика чтения разбросана | `requestRepresentationOnUpdate`, `readBackOnRefusal`, `AssignmentsRefresher`, ручные чтения в `TenantEditor` и `TicketCollectionView` |

В продуктах у каждого типа документа сейчас ровно один view. Два view есть только у демо
`ContactInfo` в Gallery. Зато `TenantEditor` держит до 8 страниц на одной `TenantData`, то есть
одна модель на несколько экранов уже работает на практике.

## 2. Основные решения

1. **Единая точка на документ.** Вся работа с репрезентациями идёт через один hub.
2. **N view на M веток.** Ветка создаётся по компоненту репрезентации, а не по view. M = 1 даёт
   одну разветвлённую репрезентацию, M = N соответствует сегодняшней схеме. Серверные контракты
   при этом не меняются.
3. **Своя правка определяется по `changeId`.** Механизм взят из #1048.
4. **Одна запись на документ за раз.** Чтения ждут окончания записей.
5. **Чтение после изменений — это правило ветки.** Функция `readAfter` решает, что читать,
   с учётом события и `changeSet`.
6. **Ветки общаются только через hub.** Прямых ссылок и подключений между ними нет.
7. **Вложенных веток нет.** Иерархия живёт в модели и view. У каждого пути данных один
   владелец-ветка.

## 3. Состав системы

```
DocumentService     реестр типов, список документов, транспорт, разбор уведомлений
  └─ Document       наблюдаемое состояние и команды одного документа
       └─ DocumentRepresentationHub      очередь записей, changeId, общение веток
            ├─ Branch "device"        ← General, Licenses, Settings (m_settings)
            ├─ Branch "connections"   ← Connections
            └─ Branch "history"       ← History
```

| Кто | Отвечает за |
|---|---|
| `DocumentService` | Реестр типов и view, список документов, транспорт, разбор уведомлений |
| `Document` | Состояние и команды документа для хостов и редакторов |
| `DocumentRepresentationHub` | Порядок запросов между ветками, `changeId` и эхо, ожидание для `Save`, `Undo`, `Redo`, доставка событий между ветками |
| `DocumentRepresentationBranch` | Модель, что устарело, что не отправлено, отправка своих запросов, применение ответа |
| `DocumentViewBase` | `updateGui` и `updateModel`, как сейчас; связь редактора с веткой |

`DocumentRepresentationBranch` живёт в `imtdocgui` и не зависит от транспорта. Команды и
отправители находятся в `GqlDocumentRepresentationBranch` из `imtguigql`. Примеры ниже для
краткости используют одно имя.

`ViewBase` общесистемный, его структура не меняется. Логика связи с документом находится в
`DocumentViewBase`, поэтому на него надо перевести `RoleView`, `UserGroupView`,
`ConversationEditor` и email-редактор из Gallery.

## 4. Document

`Document` заменяет набор сигналов и функций вида `getDocumentName(documentId)`.

- **Свойства:** `id`, `typeId`, `objectId`, `name`, `isNew`, `isDirty`, `isReadOnly`,
  `isDataReady`, `isBusy`, `undoSteps`, `redoSteps`.
- **Команды:** `save()`, `undo()`, `redo()`, `close()`, `reload()`.
- **События:** только настоящие события: `saved`, `closed`, `lost`, `failed(message)`.

**Открытие.** `service.openDocument()` сразу создаёт `Document`, ещё без `id`. Вкладка привязана
к объекту, поэтому кэши, префикс `opening:` и `cancelledOpenings` не нужны. Если пользователь
закрыл вкладку до ответа, документ запоминает это и закрывается на сервере после ответа.
Уведомление `DocumentOpened` содержит `objectId`, поэтому пришедшее раньше ответа уведомление
находит тот же объект. Документы из `GetOpenedDocumentList` создаёт сам сервис.

**Привязка хоста.** Вместо десятков обработчиков:

```qml
Loading {
	visible: tabRoot.document.isBusy
}
```

Имя и признак изменений вкладки привязываются так же.

`isBusy` учитывает открытие, первую загрузку, сохранение, undo/redo и закрытие. Фоновые чтения
в него не входят, иначе индикатор мигал бы при каждой правке.

**Сохранение.** `save()` проверяет `acceptsSave()` у видимых view, вызывает `doUpdateModel()`,
определяет имя документа, ждёт очередь записей и отправляет `SaveDocument`. Отказ записи отменяет
сохранение. Чтения `Save` не ждёт: правки на сервере уже применены, а чтение только обновляет экран.

## 5. DocumentRepresentationHub

Hub заменяет `DocumentDecorator`.

**Создание веток.** Hub создаёт все ветки при создании документа из реестра типа. Подождать
первого view нельзя: `SingleDocumentWorkspaceContentView` создаёт только первый view, и остальных
веток не было бы, а другая ветка может к ним обратиться. Создание ветки не вызывает чтения.

**Правила очереди:**

1. На документ идёт одна запись за раз, из какой бы ветки она ни была.
2. Чтения идут только при пустой очереди записей, поэтому чтение не вернёт состояние до своей правки.
3. На ветку идёт не больше одного чтения за раз. Чтения разных веток могут идти параллельно.
4. `Save`, `Undo` и `Redo` ждут очередь записей и выполняются в порядке вызова. Отказ записи их отменяет.
5. Чтения ждут и запись, вызванную тем же событием: сначала запись, потом чтение.

**Метка `changeId`:**

- Перед отправкой записи hub создаёт UUID и запоминает его как свой. Поэтому порядок прихода
  ответа и эха не важен.
- Метка хранится на документ, а не на view. Хранятся последние несколько меток, например 16:
  таймер на 3 секунды не нужен, а опоздавшее эхо тоже распознается.
- При отказе метка удаляется, и эхо этой правки считается чужим.
- `DocumentChanged` с чужой меткой или без метки помечает устаревшими все ветки. Это другие
  сессии, undo/redo и откаты. Что именно изменилось, уведомление не сообщает, поэтому ветки
  отфильтровать нельзя. `changeSet` есть в серверном событии, его можно передать в SDL позже.
- Метка попадает к серверу заголовком `documentChangeId` через `DocumentUpdateRequestSender`.

**Сторож запроса.** Ответа на 401 нет, пока запрос не будет отправлен повторно. Поэтому страховка
на 30 секунд остаётся, но как сторож одного запроса, а не отдельное состояние каждой правки.

**Основной интерфейс:**

| Функция | Назначение |
|---|---|
| `getBranch(branchId)` | Ветка документа по имени |
| `reload(branchId, fields)` | Перечитать ветку через общую очередь; пустой `fields` означает всю ветку |
| `post(messageId, params)` | Событие предметной области для всех веток |
| `hasPendingWrites()` | Есть ли неотправленные или неподтверждённые записи |

## 6. DocumentRepresentationBranch

Ветка — это одна модель и всё, что нужно, чтобы держать её в согласии с сервером. View она не знает.
Другие ветки знает только по `branchId` через hub. Порядок запросов она не решает.

**Объявляет автор:**

| Свойство или функция | Назначение |
|---|---|
| `branchId` | Постоянное имя ветки |
| `readCommandId`, `writeCommandId` | Команды; без команды записи ветка только читает |
| `readPermissionPath`, `writePermissionPath` | Права на чтение и запись могут различаться, как у устройств ProLife |
| `loadOnCreate` | Читать новый документ с сервера вместо значений по умолчанию |
| `modelComp` | Компонент модели и объекта ответа |
| `dependsOn` | Ветки, данные которых нужны этой ветке |
| `createWriteInput(changeSet)` | Собирает данные для записи |
| `readAfter(event, changeSet)` | Правило чтения после собственных событий ветки |
| `onBranchEvent(source, event, changeSet)` | Реакция на события других веток |

**Заполняет hub:** `documentId`, `collectionId`, `hub`, `isActive` (видим ли хотя бы один её view),
`wasNew` (документ был новым до этого сохранения).

**Внутреннее состояние:**

| Поле | Смысл |
|---|---|
| `model` | Текущая модель, общая для всех view ветки |
| `isLoaded` | Модель хотя бы раз получена, либо новый документ работает на значениях по умолчанию |
| `pendingChangeSet` | Правки, ещё не отправленные на сервер, или `null` |
| `staleFields` | Что устарело: ничего, список полей или вся модель |
| `requestKind` | Текущий запрос: нет, чтение или запись |
| `readFailed`, `errorMessage` | Последнее чтение не удалось |
| `applying` | Идёт применение ответа; изменения модели в этот момент не считаются правками |

**Сигналы для hub и view:** `edited(changeSet)`, `refreshed(fieldIds)`, `modelReplaced()`,
`failed(message)`. Пустой `fieldIds` означает всю ветку.

**Транспорт.** Используются `DocumentRepresentationRequestSender` и `DocumentUpdateRequestSender`
из #1048. Они гарантируют ответ на любую ошибку, кроме 401.

### Правка

Ветка один раз подписывается на `modelChanged` корня своей модели:

```qml
function onModelChanged(changeSet){
	if (root.applying){
		return
	}

	root.addPendingChanges(changeSet)
	root.edited(changeSet)
}
```

Подписка именно на корень важна по двум причинам:

- `changeSet` приходит с полными путями, даже если правка сделана во view на поддереве;
- правка считается один раз. Если слушать `modelDataChanged` у view, каждый view на этой модели
  сообщит об одной и той же правке.

По сигналу `edited` hub ставит запись в очередь, спрашивает `readAfter` у этой ветки,
вызывает `onBranchEvent` у остальных и обновляет соседние view.

Ветка без `writeCommandId` правки игнорирует и пишет ошибку в журнал, чтобы очередь и `Save`
не ждали запись, которой не будет.

### Запись

1. Hub убеждается, что других записей нет, и вызывает `sendWrite(changeId)`.
2. Ветка забирает `pendingChangeSet`, вызывает `createWriteInput` и отправляет запрос с меткой.
3. Правки, сделанные за это время, копятся в новом `pendingChangeSet`.
4. Ответ получает hub, а дальше решает, что делать с очередью и чтением.

### Чтение

```qml
function canRead(){
	return root.isActive && root.staleFields !== null
			&& root.requestKind === "" && !root.readFailed
}
```

Ответ применяется так:

| Ситуация | Что делает ветка |
|---|---|
| Появились новые правки | Отбрасывает ответ, ветка остаётся устаревшей и прочитается после записи |
| Полное чтение | Заменяет или обновляет модель и сообщает `refreshed([])` |
| Частичное чтение | Копирует только запрошенные поля и сообщает `refreshed(fieldIds)` |

Весь шаг идёт под `applying = true`. Список полей передаётся явно, потому что повторное заполнение
списка в `changeSet` не попадает. Частичный запрос собирается тем же способом, что и нынешний
`__addAssignmentsQuery`.

Несколько причин читать объединяются: `["licenses"]` и `["permissions"]` дают оба поля,
а список и вся модель дают всю модель. Поэтому серия событий даёт один запрос.

### Ошибки

- **Ошибка чтения:** ставится `readFailed`, чтобы ветка не повторяла запрос бесконечно. Сброс
  происходит при следующем показе view или вызове `reload()`.
- **Отказ в записи:** hub спрашивает `readAfter(Refused)`. Если нужно чтение, `pendingChangeSet`
  сбрасывается: те правки сделаны поверх отклонённой. При `readNone` очередь отправляется дальше.
- **401:** см. сторож запроса в разделе 5.

### Жизненный цикл

1. Hub создаёт ветку вместе с документом.
2. Новый документ без `loadOnCreate`: модель сразу считается загруженной. Иначе ветка устарела и прочитается.
3. Открытый документ: ветка ждёт `isDataReady` и читается, когда становится активной.
4. Уничтожение view не влияет на ветку: она хранит модель.
5. Закрытие документа уничтожает ветку.

## 7. N view и M веток

Регистрация остаётся прежней, добавляется необязательный путь к поддереву:

```qml
addDocumentView(typeId, viewTypeId, editorComp, branchComp, modelPath)
```

- Один компонент ветки у нескольких view даёт один экземпляр ветки на документ.
- Без `modelPath` view получает всю модель, с ним — её поддерево. Код редактора не меняется, потому
  что путь в `changeSet` считается от объекта, на который подписан получатель.
- Запись всегда идёт через корень ветки.
- Правка из одного view обновляет другие view этой ветки локально, без запроса.
- Скрытый view обновляет GUI перед показом. Иначе его `updateModel` вернёт старые значения поверх
  чужой правки.

**Когда нужна отдельная ветка:**

| Ситуация | Что использовать |
|---|---|
| View работает с частью тех же данных | `modelPath` |
| Тяжёлая часть той же модели | Частичное чтение, если решена проблема полной записи (раздел 13) |
| Другой тип SDL или другой модуль | Отдельная ветка |
| Другие права на запись | Отдельная ветка |
| Данные должны меняться атомарно | Одна ветка |
| Отдельная сущность со своим сохранением | Отдельный документ |

ProLife не может дополнить тип SDL из ImtCore, поэтому продукт, добавляя свой view к типу из ImtCore,
создаёт для него свою ветку.

**Вложенные ветки не поддерживаются.** Если дочерняя ветка лежит внутри модели родителя, у данных
два владельца: запись родителя отправит незагруженные поля ребёнка, чтение родителя перезапишет его
неотправленные правки, а `Save` и отказы получат вложенные ожидания. Глубокие view обслуживает
`modelPath`, например `"m_settings.m_network"`.

## 8. Чтение после своих изменений

`readAfter(event, changeSet)` возвращает один из трёх результатов: `readNone`, `readAll` или список
полей. События: `Edited`, `Saved`, `Refused`.

Поведение по умолчанию (`defaultReadAfter`):

| Событие | Результат |
|---|---|
| Своя правка | Ничего |
| Сохранение | Ничего |
| Отказ в записи | Вся ветка, очередь правок сбрасывается |

Правила ветки не управляют чтением после чужих изменений и undo/redo: такие ветки устаревают всегда.

| Задача | Правило |
|---|---|
| Сервер пересчитывает часть данных | При затронутом пути вернуть список полей |
| Сервер дополняет документ сам | При `Edited` вернуть `readAll` |
| Новый документ получает данные после сохранения | При `Saved` и `wasNew` вернуть `readAll` |
| Отказ без перечитывания | При `Refused` вернуть `readNone` |

Редактор может запросить чтение сам:

```qml
root.hub.reload("tenant", ["members"])
```

Чтение встанет в общую очередь и дождётся своих правок.

Редактор, которому нужно обновить только часть экрана, переопределяет `updateGuiFields`. По умолчанию
она вызывает полный `doUpdateGui()`:

```qml
function updateGuiFields(fieldIds){
	roleView.updateAssignments()
}
```

Так ввод в поле имени не затрётся, когда придут пересчитанные права.

## 9. Общение веток

Все обращения идут через hub. Прямые связи не дают единого порядка запросов, живут дольше документа,
скрывают циклы и требуют для теста всех соседних веток.

| Что нужно | Как | Запрос на сервер |
|---|---|---|
| Узнать об изменении в другой ветке | `onBranchEvent(source, event, changeSet)` | Нет |
| Прочитать значение другой ветки | `hub.getBranch("device").model` | Нет |
| Попросить другую ветку перечитать данные | `branch.markStale(fields)` | Чтение в порядке очереди |
| Изменить чужие данные только на клиенте | `branch.applyLocal(fieldIds, function)` | Нет |
| Настоящая правка чужих данных | Запись в `branch.model` | Запись со своим `changeId` |
| Событие предметной области | `hub.post(messageId, params)` | Нет |

События `onBranchEvent`: `Edited`, `Refreshed`, `Saved`, `Message`. По умолчанию правка другой ветки
помечает эту устаревшей. Переопределив функцию, ветка берёт решение на себя.

**Правила:**

1. Реакция на данные с сервера идёт только через `applyLocal`. Настоящая правка в ответ на `Refreshed`
   сделала бы документ «грязным» сразу после открытия.
2. `applyLocal` не вызывает `Edited` и дальше не распространяется. Цепочка настоящих правок останавливается,
   когда значения перестают меняться. Hub ограничивает её глубину и пишет ошибку в журнал.
3. Сначала ветка обновляет своё состояние, затем hub рассылает событие. Получатель видит согласованные данные.
4. Атомарности между ветками нет: правка двух веток — это две мутации и два шага undo, а одну из них сервер
   может отклонить.
5. Значение из `applyLocal` уйдёт на сервер со следующей записью ветки. Данные только для показа не должны
   лежать в записываемой модели.
6. Перед чтением чужих данных надо проверить `isLoaded`. Ветка из `dependsOn` считается активной вместе
   с зависимой веткой и прочитается, а готовность сообщит событие `Refreshed`. Цикл в `dependsOn` не приводит
   к зависанию, но обе ветки будут читаться всегда.

## 10. Примеры

### 10.1. Один view, одна ветка

Заявка вместо нынешних примерно 75 строк контроллера. Редактор не меняется.

```qml
Component {
	id: ticketBranchComp

	DocumentRepresentationBranch {
		id: ticketBranch

		branchId: "ticket"
		readCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_getTicketRepresentation
		writeCommandId: ImtdeskTicketCollectionDocumentServiceSdlCommandIds.s_updateTicketFromRepresentation
		readPermissionPath: container.permissionPath
		writePermissionPath: container.permissionPath
		loadOnCreate: true

		modelComp: Component {
			TicketData {
				m_status: "Open"
				m_priority: "Medium"
			}
		}

		property UpdateTicketInput updateInput: UpdateTicketInput {}

		function createWriteInput(changeSet){
			ticketBranch.updateInput.m_documentId = ticketBranch.documentId
			ticketBranch.updateInput.m_ticket = ticketBranch.model
			return ticketBranch.updateInput
		}

		function readAfter(event, changeSet){
			if (event === DocumentRepresentationBranch.Saved && ticketBranch.wasNew){
				return ticketBranch.readAll
			}

			return ticketBranch.defaultReadAfter(event, changeSet)
		}
	}
}

addDocumentView("Ticket", "TicketEditor", ticketEditorComp, ticketBranchComp)
```

### 10.2. Роль: частичное чтение после правки

При смене родителей сервер пересчитывает унаследованные права. Правило заменяет `AssignmentsRefresher`:

```qml
function readAfter(event, changeSet){
	if (event === DocumentRepresentationBranch.Edited
			&& ChangeSetHelper.isAffected(changeSet, "parentRoles")){
		return ["parentRoles", "permissions", "accessWarnings"]
	}

	return roleBranch.defaultReadAfter(event, changeSet)
}
```

### 10.3. Несколько view на одной ветке

```qml
addDocumentView("Role", "General", roleGeneralEditorComp, roleBranchComp)
addDocumentView("Role", "Permissions", rolePermissionsEditorComp, roleBranchComp)
```

Правка в `General` не вызывает чтения для `Permissions`: тот обновит GUI из той же модели при показе.

### 10.4. Разные ветки или поддерево

Демо из Gallery сейчас использует разные ветки: у view разные типы SDL и команды.

```qml
addDocumentView("ContactInfo", "ContactInfoEditor", contactInfoEditorComp, contactInfoBranchComp)
addDocumentView("ContactInfo", "ContactEmailEditor", emailEditorComp, emailBranchComp)
```

Если бы `ContactInfoData` содержал вложенный `m_email`, хватило бы одной ветки:

```qml
addDocumentView("ContactInfo", "ContactInfoEditor", contactInfoEditorComp, contactInfoBranchComp)
addDocumentView("ContactInfo", "ContactEmailEditor", emailEditorComp, contactInfoBranchComp, "m_email")
```

### 10.5. Полный пример: 5 view и 3 ветки

Условный документ: такого в продуктах нет. Реальны `DeviceData`, `UpdateDeviceInput` и пути прав,
остальные типы и команды придуманы.

| View | Ветка | Модель view |
|---|---|---|
| `General` | `device` | вся `DeviceData` |
| `Licenses` | `device` | вся `DeviceData` |
| `Settings` | `device` | поддерево `m_settings` |
| `Connections` | `connections` | своя `DeviceConnectionsData` |
| `History` | `history` | своя `DeviceHistoryData`, только чтение |

```qml
Component.onCompleted: {
	registerDocumentType("Device", qsTr("Device"))

	addDocumentView("Device", "General", generalEditorComp, deviceBranchComp)
	addDocumentView("Device", "Licenses", licensesEditorComp, deviceBranchComp)
	addDocumentView("Device", "Settings", settingsEditorComp, deviceBranchComp, "m_settings")
	addDocumentView("Device", "Connections", connectionsEditorComp, connectionsBranchComp)
	addDocumentView("Device", "History", historyViewComp, historyBranchComp)
}
```

Основная ветка. Лицензии пересчитываются, если сменилась модель устройства. Новое подключение тоже их меняет:

```qml
Component {
	id: deviceBranchComp

	DocumentRepresentationBranch {
		id: deviceBranch

		branchId: "device"
		readCommandId: ProlifeDeviceCollectionDocumentServiceSdlCommandIds.s_getDeviceRepresentation
		writeCommandId: ProlifeDeviceCollectionDocumentServiceSdlCommandIds.s_updateDeviceFromRepresentation
		readPermissionPath: "/SensorManagement/View"
		writePermissionPath: "/SensorManagement/Edit/Change"

		modelComp: Component {
			DeviceData {}
		}

		property UpdateDeviceInput updateInput: UpdateDeviceInput {}

		function createWriteInput(changeSet){
			deviceBranch.updateInput.m_documentId = deviceBranch.documentId
			deviceBranch.updateInput.m_deviceData = deviceBranch.model
			return deviceBranch.updateInput
		}

		function readAfter(event, changeSet){
			if (event === DocumentRepresentationBranch.Edited
					&& ChangeSetHelper.isAffected(changeSet, "model")){
				return ["licenses"]
			}

			return deviceBranch.defaultReadAfter(event, changeSet)
		}

		function onBranchEvent(source, event, changeSet){
			if (source.branchId === "connections" && event === DocumentRepresentationBranch.Edited){
				deviceBranch.markStale(["licenses"])
			}
		}
	}
}
```

Подключения. Устаревают, только если изменилась сеть. Им нужны данные основной ветки:

```qml
Component {
	id: connectionsBranchComp

	DocumentRepresentationBranch {
		id: connectionsBranch

		branchId: "connections"
		dependsOn: ["device"]
		readCommandId: DeviceConnectionsCommandIds.s_getConnectionsRepresentation
		writeCommandId: DeviceConnectionsCommandIds.s_updateConnectionsFromRepresentation
		readPermissionPath: "/SensorManagement/View"
		writePermissionPath: "/SensorManagement/Connections"

		modelComp: Component {
			DeviceConnectionsData {}
		}

		property UpdateConnectionsInput updateInput: UpdateConnectionsInput {}

		function createWriteInput(changeSet){
			connectionsBranch.updateInput.m_documentId = connectionsBranch.documentId
			connectionsBranch.updateInput.m_connections = connectionsBranch.model
			return connectionsBranch.updateInput
		}

		function onBranchEvent(source, event, changeSet){
			if (source.branchId === "device" && event === DocumentRepresentationBranch.Edited
					&& ChangeSetHelper.isAffected(changeSet, "settings.network")){
				connectionsBranch.markStale([])
			}
		}

		function getProtocols(){
			let device = connectionsBranch.hub.getBranch("device")
			return device.isLoaded ? device.model.m_supportedProtocols : []
		}
	}
}
```

История. Команды записи нет. Новая запись появляется только после сохранения:

```qml
Component {
	id: historyBranchComp

	DocumentRepresentationBranch {
		id: historyBranch

		branchId: "history"
		readCommandId: DeviceHistoryCommandIds.s_getHistoryRepresentation
		readPermissionPath: "/SensorManagement/View"

		modelComp: Component {
			DeviceHistoryData {}
		}

		function onBranchEvent(source, event, changeSet){
			if (event === DocumentRepresentationBranch.Saved){
				historyBranch.markStale([])
			}
		}
	}
}
```

Редакторы веток не видят:

```qml
DocumentViewBase {
	id: settingsEditor

	property DeviceSettingsData settings: model

	function updateGui(){
		portInput.text = settingsEditor.settings.m_port
	}

	function updateModel(){
		settingsEditor.settings.m_port = portInput.text
	}
}
```

Что происходит при действиях (открыт `General`, остальные view скрыты):

| Действие | Запись | Чтение сейчас | При показе view |
|---|---|---|---|
| Открытие документа | — | `device` | `connections` и `history` читаются при первом показе, остальные — без запросов |
| Правка имени в `General` | `device` | — | `Licenses` и `Settings` обновляют GUI из модели, без запроса |
| Смена модели устройства | `device` | поле `licenses` | `Licenses` показывает новые данные, без запроса |
| Правка порта в `Settings` | вся модель `device` | — | `General` и `Licenses` обновляют GUI из модели |
| Правка `settings.network` | `device` | — | `Connections` читается при показе |
| Правка в `Connections` | `connections` | поле `licenses` после записи | `Licenses` показывает новые данные, без запроса |
| `Save` | `SaveDocument` после очереди | — | `History` читается при показе |
| `Undo` или чужая правка | — | ветка видимого view | Другие ветки читаются при показе; view той же ветки обновляют GUI без запроса |

Число запросов. Сценарий: открыть документ, пройти все 5 view, поправить имя в `General`, пройти их ещё раз.

| | Каждому view своя модель (#1048) | Hub с ветками |
|---|---|---|
| Первый обход | 5 чтений | 3 чтения |
| Правка имени | 1 запись | 1 запись |
| Второй обход | 4 чтения | 0 чтений |
| **Итого** | **10 запросов** | **4 запроса** |

Это расчёт по правилам, а не замер.

### 10.6. Порядок запросов на примере роли

Пользователь меняет родительскую роль, затем сразу имя и нажимает `Save`:

| Шаг | Что происходит |
|---|---|
| 1 | Правка `parentRoles`: hub выдаёт метку `A` и отправляет запись |
| 2 | Правка `name`, пока запись идёт: встаёт в очередь |
| 3 | `Save`: ждёт очередь |
| 4 | Приходит эхо с меткой `A`: метка своя, чтения по эху нет |
| 5 | Ответ на `A`: уходит запись `name` с меткой `B`; частичное чтение из `readAfter` ждёт её |
| 6 | Ответ и эхо на `B`: очередь записей пуста |
| 7 | Уходят чтение `parentRoles`, `permissions`, `accessWarnings` и `SaveDocument` |

Чтение не затрагивает ввод в имя. Если другая сессия поменяет роль на шаге 5, придёт чужая метка `C`:
все ветки станут устаревшими, а полное чтение после записей заменит частичное.

## 11. Что заменяется

| Сейчас | В новой системе |
|---|---|
| `DocumentDecorator` | `DocumentRepresentationHub` |
| `DocumentRepresentationController` | `DocumentRepresentationBranch` |
| `registeredViews`, `registeredRepresentation`, `viewConnections` | Ветка и связанные с ней view |
| `updateCounters`, внешние вызовы `setBlockingUpdateModel` | `applying` и `requestKind` |
| `requestUpdateViews`, `deferredViews` | `isActive` и `staleFields` |
| `queuedUpdates` | `pendingChangeSet` |
| `failedViews` | `readFailed` |
| `ownUpdates`, таймер на 3 секунды | Последние метки в hub |
| `pendingCommands` | Очередь команд в hub |
| `requestRepresentationOnUpdate`, `readBackOnRefusal`, `AssignmentsRefresher` | `readAfter` |
| Сигналы состояния и функции вида `getDocumentName(id)` | Свойства `Document` |
| `cachedDocumentNames`, `cachedDocumentObjectIds`, `pendingDataLoaded`, `opening:`, `cancelledOpenings` | `Document`, создаваемый сразу при открытии |
| Обработчики индикаторов в хостах | Привязка к `document.isBusy` |

Из #1048 остаются без замены: серверная часть `changeId`, `changeSet` с `ChangeSetHelper`, оба отправителя
запросов. Его сценарные тесты подходят как приёмка для hub.

## 12. Порядок перехода

Каждый этап — отдельный PR с зелёными тестами `Tests/QmlTest/imtdocgui`:

1. **Основа.** Влить #1048.
2. **Ветки и hub.** Ввести `DocumentRepresentationHub` и `DocumentRepresentationBranch` вместо координации
   в `DocumentDecorator` и перевести контроллеры ImtCore и `Device` из ProLife. Хосты не меняются.
3. **Document.** Ввести `Document`, старые функции сервиса временно делегируют ему. Перевести `DocumentViewBase`.
4. **Хосты.** Привязать их к `Document`, перенести восстановление списка открытых документов в сервис.
5. **Очистка.** Удалить временные функции, лишние сигналы и механизм `*Override`, который используется только
   в тестах. Обновить `DocumentService_Client_Architecture.ru.md`.

Каждый этап проверяется и в Qt, и в JQML.

## 13. Риски и что надо проверить

| Что | Почему важно | Что сделать |
|---|---|---|
| Частичная загрузка и полная запись | Незагруженные поля при полной записи могут затереть данные | Проверить, оставляет ли `Update…FromRepresentation` отсутствующие поля нетронутыми. Пока нет ответа, ветка загружается и пишется целиком |
| Экономия частичного чтения | Обязательные поля SDL вычисляются всегда, а обработчик может считать всё независимо от выборки | Проверить обработчики, начиная с `GetTenantRepresentation` |
| Запись модели внутри `updateGui` | При подписке на корень защита `ViewBase` не сработает. Hub включает `applying` на время обновления view, но редактор может вызвать `doUpdateGui` сам | Найти такие места в редакторах |
| Замена объекта модели при чтении | #1048 по умолчанию подставляет полученный объект. Ветка должна переподписаться, а hub заново раздать модель view, включая `modelPath` | Выбрать между заменой и копированием, покрыть тестом |
| Несохранённый текст в поле | Полное чтение заменит текст, ещё не попавший в модель. Сейчас при чужой правке происходит то же самое | Оценить, нужно ли это решать |
| Новые документы | Чтение после правки может заменить клиентские значения по умолчанию, например `m_id` | Проверить на типах без `loadOnCreate` |
| Права на общей ветке | Один `writePermissionPath` на ветку | Проверить документы с разными правами на поля |
| Ошибочные зависимости | Если ветка не отметит себя устаревшей, а сервер изменил её данные, экран покажет старое значение | Отказываться от умолчания только при уверенности |
| Потеря токена при 401 | Что будет, если обновить токен не удалось, не проверено | Проверить сторож запроса на таком сценарии |
| Перечисления в QML-типе | Запись `DocumentRepresentationBranch.Edited` может не поддерживаться в JQML | Проверить, при необходимости заменить строковыми константами |
| Тестирование N view | Реальный документ с разными view только в демо | Покрыть тестами, проверить на первом продуктовом сценарии |

## 14. Открытые вопросы

1. **Вливать ли #1048 как есть и строить hub поверх него.** Рекомендация: да. Сервер, `changeSet` и отправители
   останутся, заменится только координация в декораторе. Минус: её придётся переписать вскоре после вливания.
2. **Нужна ли частичная запись в первой версии.** Рекомендация: нет. Общая ветка загружается и пишется целиком,
   как `TenantEditor` сейчас. Ленивой загрузки страниц при этом не будет.
3. **Какое общение веток нужно сразу.** Рекомендация: `onBranchEvent`, `markStale`, `getBranch` и `dependsOn`.
   `applyLocal` и `post` добавить при первом реальном сценарии: они самые рискованные, а подтверждённых примеров нет.
4. **Какой реальный документ станет первым с разными view.** Без него связь N к M проверяется только на демо.
5. **Удалять ли `CDocumentServiceController`.** В QML ImtCore и ProLife он не используется, ссылки из C++ и других
   репозиториев не проверялись.
