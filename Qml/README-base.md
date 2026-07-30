# ImtCore QML — базовые модули

Документация по фундаментальным QML-модулям ImtCore, на которых строятся прикладные модули (см. [README.md](README.md)). Правила размещения новых компонентов — в [STRUCTURE.md](STRUCTURE.md).

- **`imtcontrols`** — библиотека UI-контролов и базовых классов;
- **`imtstyle`** — стили и оформление (отдельный модуль, не подпапка);
- **`imtqmlutils`** — невизуальные утилиты (математика, валидаторы, модели, события);
- **`imtgraphics2d`** — 2D-графика на Canvas;
- **`imtgui`** — каркас приложения и представлений.

> Обозначения: **singleton** — тип-одиночка; **Базовый тип** — ссылка на компонент в документе, файл в репозитории или документацию Qt; **Наследники** — компоненты этих модулей, расширяющие тип. В списках API приведён только публичный контракт корневого объекта; приватные члены (имена с префиксом `_`/`$`) опущены.

## Модули

| Модуль | Назначение | Компонентов |
|---|---|---|
| [`imtcontrols`](#imtcontrols) | Библиотека UI-контролов | 208 |
| [`imtstyle`](#imtstyle) | Стили и оформление | 4 |
| [`imtqmlutils`](#imtqmlutils) | Утилиты (математика, валидаторы, модели, события) | 17 |
| [`imtgraphics2d`](#imtgraphics2d) | 2D-графика на Canvas (фигуры, графики, матрицы) | 35 |
| [`imtgui`](#imtgui) | Каркас приложения и представлений | 110 |


---

<a id="imtcontrols"></a>
## `imtcontrols` — Библиотека UI-контролов

Фундаментальная библиотека переиспользуемых контролов и визуальных примитивов: кнопки, поля ввода, диалоги и всплывающие окна, таблицы и деревья, меню, диаграммы и индикаторы, а также базовые классы (ControlBase, DecoratorBase). Оформление вынесено в отдельный модуль imtstyle. На неё опираются почти все остальные QML-модули.

**Импорт:** `import imtcontrols 1.0` · **Компонентов:** 208

### Base/ — Базовые классы

<a id="b-imtcontrols-BaseClass"></a>
#### `BaseClass`

Вспомогательный компонент.

- **Файл:** [Base/BaseClass.qml](imtcontrols/Base/BaseClass.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `enableNotifications: bool`
- `owner: var`
- `propertiesIsConnected: bool`

**Сигналы**

- `modelChanged(var changeSet)`
- `finished()`

**Функции**

- `beginChanges()`
- `endChanges()`
- `removeKey(key)`
- `connectProperties()`
- `createMe()`
- `isEqualWithModel(model)`
- `refresh()`
- `copy(item)`
- `copyMe()`
- `copyFrom(item)`
- `copyTo(item)`
- `createComponent(propertyId, typename)`
- `createElement(propertyId, typename)`
- `getJSONKeyForProperty(propertyId)`
- `getProperties()`
- `createFromJson(json)`
- `toJson()`
- `toGraphQL()`
- `fromJSON(json)`
- `escapeSpecialChars(jsonString)`
- `fromObject(sourceObject)`

<a id="b-imtcontrols-BaseModel"></a>
#### `BaseModel`

Модель данных.

- **Файл:** [Base/BaseModel.qml](imtcontrols/Base/BaseModel.qml)
- **Базовый тип:** `ListModel`
- **Наследники:** [`TableHeaders`](#b-imtcontrols-TableHeaders)

**Свойства**

- `owner: var`

**Сигналы**

- `finished()`

**Функции**

- `getProperties(item)`
- `toJson()`
- `toGraphQL()`
- `isEqualWithModel(model)`
- `copyMe()`
- `createFromJson(json)`
- `fromJSON(json)`
- `addElement(element)`
- `appendElement(element)`
- `insertElement(index, element)`
- `removeElement(index)`
- `getItemsCount()`
- `containsKey(key, index)`
- `getData(key, index)`
- `setProperty(index, propName, value)`
- `swapItems(index1, index2)`

<a id="b-imtcontrols-ControlBase"></a>
#### `ControlBase`

Базовый компонент (для наследования).

- **Файл:** [Base/ControlBase.qml](imtcontrols/Base/ControlBase.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)
- **Наследники:** [`BottomPanel`](#b-imtgui-BottomPanel), [`BusyIndicator`](#b-imtcontrols-BusyIndicator), [`Button`](#b-imtcontrols-Button), [`CheckBox`](#b-imtcontrols-CheckBox), [`ComboBox`](#b-imtcontrols-ComboBox), [`CustomTooltip`](#b-imtcontrols-CustomTooltip), [`Dialog`](#b-imtcontrols-Dialog), [`FilterMenu`](#b-imtgui-FilterMenu), [`MenuPanelButton`](#b-imtgui-MenuPanelButton), [`PopupMenuDialog`](#b-imtcontrols-PopupMenuDialog), [`ProgressBar`](#b-imtcontrols-ProgressBar), [`RadioButton`](#b-imtcontrols-RadioButton), [`RangeSlider`](#b-imtcontrols-RangeSlider), [`Slider`](#b-imtcontrols-Slider), [`SpinBox`](#b-imtcontrols-SpinBox), [`SwitchCustom`](#b-imtcontrols-SwitchCustom), [`TabDelegate`](#b-imtgui-TabDelegate), [`TextArea`](#b-imtcontrols-TextArea), [`TextEditCustom`](#b-imtcontrols-TextEditCustom), [`TextField`](#b-imtcontrols-TextField), [`TopCenterPanel`](#b-imtgui-TopCenterPanel), [`TopLeftPanel`](#b-imtgui-TopLeftPanel), [`TopPanel`](#b-imtgui-TopPanel), [`TopRightPanel`](#b-imtgui-TopRightPanel)

**Свойства**

- `decorator: Component`
- `decorator_: var`
- `bindWidthAlias: alias`
- `bindHeightAlias: alias`
- `widthFromDecorator: bool`
- `heightFromDecorator: bool`

**Функции**

- `decoratorChangedFunc()`
- `decoratorChangedFuncBase()`
- `setBindTargets()`

<a id="b-imtcontrols-DecoratorBase"></a>
#### `DecoratorBase`

Базовый компонент (для наследования).

- **Файл:** [Base/DecoratorBase.qml](imtcontrols/Base/DecoratorBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`ButtonDecorator`](#b-imtcontrols-ButtonDecorator), [`DialogDecorator`](#b-imtcontrols-DialogDecorator), [`FilterPanelDecorator`](#b-imtgui-FilterPanelDecorator), [`IconButtonDecorator`](#b-imtcontrols-IconButtonDecorator), [`MenuPanelButtonDecorator`](#b-imtgui-MenuPanelButtonDecorator), [`PopupDecorator`](#b-imtcontrols-PopupDecorator), [`PopupDecoratorGql`](#b-imtcontrols-PopupDecoratorGql), [`PopupDetailedDelegateDecorator`](#b-imtcontrols-PopupDetailedDelegateDecorator), [`RadioButtonDecorator`](#b-imtcontrols-RadioButtonDecorator), [`TabPanelDecorator`](#b-imtgui-TabPanelDecorator), [`TopCenterPanelDecorator`](#b-imtgui-TopCenterPanelDecorator), [`TopLeftPanelDecorator`](#b-imtgui-TopLeftPanelDecorator), [`TopPanelDecorator`](#b-imtgui-TopPanelDecorator), [`TopRightPanelDecorator`](#b-imtgui-TopRightPanelDecorator)

**Свойства**

- `baseElement: var`

**Функции**

- `clearGui()`

<a id="b-imtcontrols-Decorators"></a>
#### `Decorators`

Вспомогательный компонент.

- **Файл:** [Base/Decorators.qml](imtcontrols/Base/Decorators.qml)
- **Базовый тип:** [`StyleComponents`](#b-imtstyle-StyleComponents)

**Свойства**

- `styleId: string`

<a id="b-imtcontrols-DecoratorsQt"></a>
#### `DecoratorsQt`

Визуальный компонент.

- **Файл:** [Base/DecoratorsQt.qml](imtcontrols/Base/DecoratorsQt.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `styleId: string`

<a id="b-imtcontrols-Enums"></a>
#### `Enums` · **singleton**

Вспомогательный компонент.

- **Файл:** [Base/Enums.qml](imtcontrols/Base/Enums.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `ok: int`
- `yes: int`
- `open: int`
- `save: int`
- `cancel: int`
- `close: int`
- `discard: int`
- `apply: int`
- `reset: int`
- `restoreDefaults: int`
- `help: int`
- `saveAll: int`
- `yesToAll: int`
- `no: int`
- `noToAll: int`
- `abort: int`
- `retry: int`
- `ignore: int`
- `noButton: int`
- `popupNoAutoClose: int` — только чтение
- `popupCloseOnPressOutside: int` — только чтение
- `popupCloseOnPressOutsideParent: int` — только чтение
- `popupCloseOnReleaseOutside: int` — только чтение
- `popupCloseOnReleaseOutsideParent: int` — только чтение
- `popupCloseOnEscape: int` — только чтение
- `popupDefaultClosePolicy: int` — только чтение
- `pathKindFile: int` — только чтение
- `pathKindDir: int` — только чтение

<a id="b-imtcontrols-IconTest"></a>
#### `IconTest` · **singleton** · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Base/IconTest.qml](imtcontrols/Base/IconTest.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `mode: QtObject`
- `state: QtObject`

<a id="b-imtcontrols-MainWindow"></a>
#### `MainWindow`

Окно.

- **Файл:** [Base/MainWindow.qml](imtcontrols/Base/MainWindow.qml)
- **Базовый тип:** [`Window`](#b-imtcontrols-Window)

<a id="b-imtcontrols-StateManager"></a>
#### `StateManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Base/StateManager.qml](imtcontrols/Base/StateManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `state: string`
- `states: var`

**Функции**

- `restoreDefaults()`

<a id="b-imtcontrols-WrapItem"></a>
#### `WrapItem`

Вспомогательный компонент.

- **Файл:** [Base/WrapItem.qml](imtcontrols/Base/WrapItem.qml)
- **Базовый тип:** `?`

### Models/ — Модели и данные

<a id="b-imtcontrols-CommandsController"></a>
#### `CommandsController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Models/CommandsController.qml](imtcontrols/Models/CommandsController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `typeId: string`
- `view: Item`
- `commands: GuiElementContainer`
- `pendingChanges: var`

**Сигналы**

- `commandsReceived(string typeId, var commands)`
- `commandsReceiveFailed()`

**Функции**

- `applyPendingChanges()`
- `setCommandProperty(commandId, propertyName, value)`
- `getCommandProperty(commandId, propertyName)`
- `getCommandIds()`
- `getCommandIdsRecursive(commands, retVal)`
- `getCommandByIndex(index)`
- `findCommandId(elements, commandId)`
- `getCommandById(commandId)`
- `commandIsEnabled(commandId)`
- `getCommands()`
- `setCommandIsEnabled(commandId, isEnabled)`
- `setCommandIcon(commandId, icon)`
- `setCommandName(commandId, commandName)`
- `setCommandNotification(commandId, notification)`
- `setCommandVisible(commandId, isVisible)`
- `getCommandIndex(commandId)`
- `setIsToggleable(commandId, toggleable)`
- `isToggleable(commandId)`
- `isToggled(commandId)`
- `setToggled(commandId, toggled)`
- `commandExists(commandId)`

<a id="b-imtcontrols-DataModelController"></a>
#### `DataModelController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Models/DataModelController.qml](imtcontrols/Models/DataModelController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `resultModel: var`

**Сигналы**

- `requestStarted(var params)`
- `resultModelReady(var resultModel)`
- `dataModelSaveFailed(string errorMessage)`

**Функции**

- `saveDataModel(params, documentId, documentName, documentDescription)`

<a id="b-imtcontrols-DataModelProvider"></a>
#### `DataModelProvider`

Провайдер данных.

- **Файл:** [Models/DataModelProvider.qml](imtcontrols/Models/DataModelProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `dataModel: var`

**Сигналы**

- `requestStarted(var params)`
- `dataModelReady(var dataModel)`
- `dataModelLoadFailed(string errorMessage)`

**Функции**

- `requestDataModel(params)`

<a id="b-imtcontrols-GqlModel"></a>
#### `GqlModel` · _не экспортируется (внутренний/по пути)_

Модель данных.

- **Файл:** [Models/GqlModel.qml](imtcontrols/Models/GqlModel.qml)
- **Базовый тип:** [`TreeItemModel`](#b-imtcontrols-TreeItemModel)

**Свойства**

- `authenticationTokenHeaderId: string` — только чтение

**Функции**

- `SetGlobalAccessToken(token)`
- `SetRefreshToken(token)`
- `SetProductId(productId)`
- `GetGlobalAccessToken()`
- `GetProductId()`
- `setGqlQuery(gqlData)`

<a id="b-imtcontrols-ModelIndex"></a>
#### `ModelIndex`

Вспомогательный компонент.

- **Файл:** [Models/ModelIndex.qml](imtcontrols/Models/ModelIndex.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `itemData: var`
- `delegateItem: var`
- `parentIndex: ModelIndex`
- `childModel: var`
- `treeModel: var`
- `index: int`
- `depth: int`

**Функции**

- `getData(value)`
- `getDepth(value)`
- `setData(key, value)`
- `getModel()`
- `getParentModel()`
- `getIndex()`
- `addChildIndex(childIndex)`
- `equal(otherIndex)`
- `equalByArrayIndexes(indexes)`
- `getIndexes()`

<a id="b-imtcontrols-NavigableItem"></a>
#### `NavigableItem`

Визуальный компонент.

- **Файл:** [Models/NavigableItem.qml](imtcontrols/Models/NavigableItem.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `paths: var`
- `parentSegment: string`
- `forwardRest: bool`

**Сигналы**

- `activated(var params, var restPath, string matchedPath)`
- `parentActivated(var params)`

**Функции**

- `segmentIsSupported(segment)`
- `processSegment(segment, params, restPath)`

<a id="b-imtcontrols-NavigationController"></a>
#### `NavigationController` · **singleton**

Контроллер: невизуальная логика и координация.

- **Файл:** [Models/NavigationController.qml](imtcontrols/Models/NavigationController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `maxSize: int`
- `navigableItems: var`
- `pending: var`

**Сигналы**

- `navigatePath(var path, var params, var activeSegments, var resultCallback)`
- `currentIndexChanged(int index)`

**Функции**

- `registerNavigableItem(navigableItem)`
- `setPending(parentSegment, rest, params)`
- `unregisterNavigableItem(navigableItem)`
- `navigate(path, params)`
- `push(path, params)`
- `next()`
- `hasNext()`
- `prev()`
- `hasPrev()`
- `clear()`

<a id="b-imtcontrols-OptionManager"></a>
#### `OptionManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Models/OptionManager.qml](imtcontrols/Models/OptionManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `optionModel: var`

**Сигналы**

- `requestStarted(var params)`
- `optionModelReady(var optionModel)`
- `optionModelLoadFailed(string errorMessage)`

**Функции**

- `insertOption(id, name, description, index)`
- `appendOption(id, name, description)`
- `removeOption(index)`
- `getOptionCount()`
- `requestDataModel(params)`

<a id="b-imtcontrols-OptionsListAdapter"></a>
#### `OptionsListAdapter`

Вспомогательный компонент.

- **Файл:** [Models/OptionsListAdapter.qml](imtcontrols/Models/OptionsListAdapter.qml)
- **Базовый тип:** `OptionsList`

**Свойства**

- `collectionModel: var`
- `idKey: string`
- `nameKey: string`
- `descriptionKey: string`
- `optionFactory: Component`

<a id="b-imtcontrols-OptionsListManager"></a>
#### `OptionsListManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Models/OptionsListManager.qml](imtcontrols/Models/OptionsListManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `optionsList: OptionsList`
- `optionFactoryComp: Component`

**Функции**

- `optionsListIsValid()`
- `getOptionsCount()`
- `getOptionIndexById(id)`
- `getOptionIndexByName(name)`
- `resetOptions()`
- `getOptionId(index)`
- `getOptionName(index)`
- `getOptionDescription(index)`
- `isOptionEnabled(index)`
- `insertOption(id, name, description, index)`
- `removeOption(index)`
- `appendOption(id, name, description)`
- `setOptionName(index, name)`
- `setOptionDescription(index, description)`
- `setOptionEnabled(index, enabled)`

<a id="b-imtcontrols-PopupMenuModel"></a>
#### `PopupMenuModel`

Модель данных.

- **Файл:** [Models/PopupMenuModel.qml](imtcontrols/Models/PopupMenuModel.qml)
- **Базовый тип:** `ListModel`

**Функции**

- `addItem(id, name, icon, enabled)`
- `addSeparator()`

<a id="b-imtcontrols-SortController"></a>
#### `SortController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Models/SortController.qml](imtcontrols/Models/SortController.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `sortModel: TreeItemModel`

**Функции**

- `setHeaderSort(headerId, sortOrder)`
- `getCurrentSort()`
- `getCurrentHeaderId()`
- `isEmpty()`

<a id="b-imtcontrols-TableProperties"></a>
#### `TableProperties`

Набор свойств оформления.

- **Файл:** [Models/TableProperties.qml](imtcontrols/Models/TableProperties.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `selectedItems: var`
- `invisibleItems: var`
- `disableItems: var`

**Сигналы**

- `visibleItemsChanged()`
- `stateItemsChanged()`
- `checkedItemsChanged()`

**Функции**

- `addInvisibleItem(index)`
- `clearInvisibleItems()`
- `clearDisabledItems()`
- `itemIsEnabled(index)`
- `setIsEnabledItem(index, isEnabled)`
- `setIsVisibleItem(index, isVisible)`
- `itemIsVisible(index)`
- `addCheckedItem(index)`
- `addSingleCheckedItem(index)`
- `addCheckedItems(indexes)`
- `removeCheckedItem(index)`
- `clearCheckedItems()`
- `itemIsChecked(index)`

<a id="b-imtcontrols-TreeItemModel"></a>
#### `TreeItemModel` · _не экспортируется (внутренний/по пути)_

Модель данных.

- **Файл:** [Models/TreeItemModel.qml](imtcontrols/Models/TreeItemModel.qml)
- **Базовый тип:** [`JSONListModel`](#b-imtqmlutils-JSONListModel)
- **Наследники:** [`GqlModel`](#b-imtcontrols-GqlModel), [`TableDecorator`](#b-imtcontrols-TableDecorator), [`TableTreeDecorator`](#b-imtcontrols-TableTreeDecorator), [`TableViewParams`](#b-imtcontrols-TableViewParams)

**Свойства**

- `infoPath: var`
- `baseUrl: string`
- `queryParams: var`
- `isArray: bool`
- `isUpdateEnabled: bool`

**Функции**

- `SetUpdateEnabled(flag)`
- `GetItemsCount()`
- `GetTreeItemModel(key, row)`
- `GetData(key, row)`
- `SetData(key, value, row)`
- `RemoveData(key, row)`
- `Clear()`
- `IsValidData(key, row)`
- `SetExternTreeModel(key, value, row)`
- `CopyItemDataFromModel(index, externTreeModel, externIndex)`
- `CopyFrom(externTreeModel)`
- `CopyItemDataToModel(index, model, externIndex)`
- `GetModelFromItem(index)`
- `IsEqualWithModel(externModel)`
- `IsEqual(sourceModel)`
- `Copy(obj)`
- `CopyMe()`
- `GetKeys(index)`
- `ContainsKey(key,row)`
- `SetQueryParam(key,value)`
- `needsReload()`
- `Refresh()`
- `SetIsArray(value)`
- `InsertNewItem(index)`
- `InsertNewItemWithParameters(index, parameters)`
- `RemoveItem(index)`
- `AddTreeModel(key, row)`
- `CreateFromJson(jsonString)`
- `ToJson()`
- `updateTreeItemJSONModel()`

<a id="b-imtcontrols-TreeItemModelConverter"></a>
#### `TreeItemModelConverter`

Визуальный компонент.

- **Файл:** [Models/TreeItemModelConverter.qml](imtcontrols/Models/TreeItemModelConverter.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Функции**

- `convertToListModel(treeItemModel)`
- `convertFromListModel(listModel)`

<a id="b-imtcontrols-TreeItemModelObserver"></a>
#### `TreeItemModelObserver`

Вспомогательный компонент.

- **Файл:** [Models/TreeItemModelObserver.qml](imtcontrols/Models/TreeItemModelObserver.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`SettingsObserver`](#b-imtgui-SettingsObserver)

**Свойства**

- `observedModel: TreeItemModel`
- `private_: QtObject`

**Сигналы**

- `modelChanged(var changeList)`

**Функции**

- `registerModel(model)`
- `observedModelDataChanged()`
- `compare(model1, model2)`
- `compareRecursive(model1, model2, changeList, parentKey)`

### Actions/ — Действия и взаимодействие

<a id="b-imtcontrols-Action"></a>
#### `Action`

Вспомогательный компонент.

- **Файл:** [Actions/Action.qml](imtcontrols/Actions/Action.qml)
- **Базовый тип:** `Shortcut`

**Свойства**

- `icon: Image`
- `text: string`
- `enabled: bool`
- `checkable: bool`
- `checked: bool`
- `group: var`

**Сигналы**

- `triggered(QtObject source)`
- `toggled(QtObject source)`

**Функции**

- `toggle(source)`
- `trigger(source)`

<a id="b-imtcontrols-ActionGroup"></a>
#### `ActionGroup`

Вспомогательный компонент.

- **Файл:** [Actions/ActionGroup.qml](imtcontrols/Actions/ActionGroup.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `exclusive: bool`
- `enabled: bool`
- `actions: var`
- `checkedAction: var` — только чтение

**Сигналы**

- `triggered(var action)`

**Функции**

- `addAction(action)`
- `removeAction(action)`

<a id="b-imtcontrols-ControlArea"></a>
#### `ControlArea`

Визуальный компонент.

- **Файл:** [Actions/ControlArea.qml](imtcontrols/Actions/ControlArea.qml)
- **Базовый тип:** [`MouseArea`](https://doc.qt.io/qt-6/qml-qtquick-mousearea.html)

**Свойства**

- `movingItem: Item`
- `wheelTargetItem: Item`
- `insideMovingItem: bool`
- `coord: var`
- `coordPressed: var`
- `hasWheelControl: bool`
- `scaleStep: real`
- `mainMargin: int`
- `wasMoving: bool`
- `isPressed: bool`
- `propagateWheelEvents: bool`
- `propagateMouseEvents: bool`

**Сигналы**

- `deltaSignal(point delta)`
- `wheelDeltaSignal(int wheelDelta)`
- `positionSignal(point position)`
- `rightClicked(real mouseX, real mouseY)`

**Функции**

- `movingFunction(delta)`
- `checkInsideMovingItem(x_, y_, width_, height_)`
- `checkHoverItem(x_, y_, width_, height_, coord_)`
- `withinBorders(delta, x_, y_, width_, height_)`
- `wheelFunction(wheelDelta)`
- `withinWheelTargetItem()`

<a id="b-imtcontrols-DetailedPopupMenuDelegate"></a>
#### `DetailedPopupMenuDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Actions/DetailedPopupMenuDelegate.qml](imtcontrols/Actions/DetailedPopupMenuDelegate.qml)
- **Базовый тип:** [`PopupMenuDelegate`](#b-imtcontrols-PopupMenuDelegate)
- **Наследники:** [`FilterableComboBoxDelegate`](#b-imtcontrols-FilterableComboBoxDelegate)

**Свойства**

- `description: string`

<a id="b-imtcontrols-FilterableComboBoxDelegate"></a>
#### `FilterableComboBoxDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Actions/FilterableComboBoxDelegate.qml](imtcontrols/Actions/FilterableComboBoxDelegate.qml)
- **Базовый тип:** [`DetailedPopupMenuDelegate`](#b-imtcontrols-DetailedPopupMenuDelegate)

**Свойства**

- `displayText: string`
- `comboBoxRef: ComboBox`

<a id="b-imtcontrols-IdSelectionManager"></a>
#### `IdSelectionManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Actions/IdSelectionManager.qml](imtcontrols/Actions/IdSelectionManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `multiSelect: bool`
- `selectedIds: var`
- `focusedId: string`
- `firstSelectedId: string`

**Сигналы**

- `selectionChanged(var selectedIds)`

**Функции**

- `singleSelect(id)`
- `deselect(ids)`
- `toggleSelect(id)`
- `selectMultiple(ids)`
- `clear()`
- `isSelected(id)`

<a id="b-imtcontrols-IndexSelectionManager"></a>
#### `IndexSelectionManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Actions/IndexSelectionManager.qml](imtcontrols/Actions/IndexSelectionManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `multiSelect: bool`
- `selectedIndexes: var`
- `focusedIndex: int`
- `firstSelectedIndex: int`

**Сигналы**

- `selectionChanged(var selectedIndexes)`

**Функции**

- `arraysEqual(a, b)`
- `emitIfChanged(prev, beQuiet)`
- `singleSelect(index, beQuiet)`
- `toggleSelect(index, beQuiet)`
- `selectMultiple(indexes, beQuiet)`
- `rangeSelect(from, to, beQuiet)`
- `clear(beQuiet)`
- `isSelected(index)`

<a id="b-imtcontrols-LocalizationEvent"></a>
#### `LocalizationEvent`

Вспомогательный компонент.

- **Файл:** [Actions/LocalizationEvent.qml](imtcontrols/Actions/LocalizationEvent.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Сигналы**

- `localizationChanged(string langId)`

<a id="b-imtcontrols-MovingItem"></a>
#### `MovingItem`

Визуальный компонент.

- **Файл:** [Actions/MovingItem.qml](imtcontrols/Actions/MovingItem.qml)
- **Базовый тип:** [`MouseArea`](https://doc.qt.io/qt-6/qml-qtquick-mousearea.html)

**Свойства**

- `globalParent: Item`
- `containerItem: Item`
- `movingItem: Item`
- `mainMargin: int`
- `alwaysVisibleSize: int`
- `coord: var`
- `isPressed: bool`

<a id="b-imtcontrols-PopupMenuDelegate"></a>
#### `PopupMenuDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Actions/PopupMenuDelegate.qml](imtcontrols/Actions/PopupMenuDelegate.qml)
- **Базовый тип:** [`ItemDelegate`](#b-imtcontrols-ItemDelegate)
- **Наследники:** [`DetailedPopupMenuDelegate`](#b-imtcontrols-DetailedPopupMenuDelegate)

**Свойства**

- `selected: bool`
- `isSeparator: bool`
- `containerItem: Item`
- `rootItem: Item`
- `contentLeftMargin: int`

<a id="b-imtcontrols-ResizeItem"></a>
#### `ResizeItem`

Визуальный компонент.

- **Файл:** [Actions/ResizeItem.qml](imtcontrols/Actions/ResizeItem.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `mainSize: int`
- `targetItem: Item`
- `globalParent: Item`
- `gap: int`
- `minSize: int`
- `cursorAreaSize: int`
- `canCorrectCoord: bool`

**Сигналы**

- `sizeChanged(real deltaWidth, real deltaHeight)`

**Функции**

- `resize(item, globalCoord, deltaX, deltaY)`

<a id="b-imtcontrols-TableSelectionManager"></a>
#### `TableSelectionManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Actions/TableSelectionManager.qml](imtcontrols/Actions/TableSelectionManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `table: var`

**Функции**

- `selectMultipleRange(fromIndex, toIndex)`
- `onMouseEvent(mouse, index)`
- `onKeysEvent(event)`

### Buttons/ — Кнопки

<a id="b-imtcontrols-Button"></a>
#### `Button`

Кнопка.

- **Файл:** [Buttons/Button.qml](imtcontrols/Buttons/Button.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)
- **Наследники:** [`ItemDelegate`](#b-imtcontrols-ItemDelegate), [`MenuButton`](#b-imtcontrols-MenuButton), [`ToolButton`](#b-imtcontrols-ToolButton)

**Свойства**

- `text: string`
- `iconSource: string`
- `tooltipText: string`
- `textColor: color`
- `down: bool`
- `hoverEnabled: bool`
- `hovered: bool`
- `checkable: bool`
- `checked: bool`
- `enabled: bool`
- `isMenuButton: bool`
- `focusPolicy: int`
- `focusReason: int`
- `mouseArea: alias`
- `icon: IconProperties`
- `font: FontProperties`

**Сигналы**

- `clicked()`
- `doubleClicked()`
- `pressed()`
- `released()`
- `entered(real mouseX, real mouseY)`
- `exited(real mouseX, real mouseY)`
- `positionChanged(real mouseX, real mouseY)`
- `toggled()`
- `closeTooltip()`

<a id="b-imtcontrols-ButtonDecorator"></a>
#### `ButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/ButtonDecorator.qml](imtcontrols/Buttons/ButtonDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)
- **Наследники:** [`ExternalSegmentDecorator`](#b-imtcontrols-ExternalSegmentDecorator), [`ItemDelegateDecorator`](#b-imtcontrols-ItemDelegateDecorator), [`ToolButtonDecorator`](#b-imtcontrols-ToolButtonDecorator), [`TopButtonDecorator`](#b-imtcontrols-TopButtonDecorator)

**Свойства**

- `maxTextWidth: int`
- `minTextWidth: int`
- `contentSpacing: int`
- `contentLeftMargin: int`
- `contentWidth: alias`
- `decoratorWidth: real`
- `widthDefault: int`
- `icon: alias`
- `arrowIconAlias: alias`
- `font: alias`
- `fontFamily: string`
- `fontSize: int`
- `textColor: alias`
- `color: alias`
- `radius: alias`
- `border: alias`
- `backgroundItem: alias`
- `contentCentered: bool`
- `textIsCropped: bool`
- `maxContentWidth: real`
- `tooltipText: string`
- `enabled: bool`
- `isMenuButton: bool`

**Сигналы**

- `mouseEntered(real mouseX, real mouseY)`
- `mouseExited(real mouseX, real mouseY)`
- `mousePositionChanged(real mouseX, real mouseY)`

**Функции**

- `setAnchors()`
- `closeTooltip()`
- `setDecoratorWidth(widthArg)`

<a id="b-imtcontrols-ExternalSegmentDecorator"></a>
#### `ExternalSegmentDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/ExternalSegmentDecorator.qml](imtcontrols/Buttons/ExternalSegmentDecorator.qml)
- **Базовый тип:** [`ButtonDecorator`](#b-imtcontrols-ButtonDecorator)

**Свойства**

- `isLast: bool`

<a id="b-imtcontrols-IconButtonDecorator"></a>
#### `IconButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/IconButtonDecorator.qml](imtcontrols/Buttons/IconButtonDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `icon: alias`
- `color: alias`
- `border: alias`

<a id="b-imtcontrols-MenuButton"></a>
#### `MenuButton`

Кнопка.

- **Файл:** [Buttons/MenuButton.qml](imtcontrols/Buttons/MenuButton.qml)
- **Базовый тип:** [`Button`](#b-imtcontrols-Button)

**Свойства**

- `menuModel: var`
- `currentIndex: int`
- `isOpen: bool`
- `popup: var`
- `popupMenuWidth: real`
- `itemHeight: int`
- `displayId: string`
- `elementIdName: string`
- `mainClickByModel: bool`
- `delegate: Component`
- `popupMenuComp: Component`

**Сигналы**

- `accepted(string buttonId)`

**Функции**

- `open()`
- `close()`

<a id="b-imtcontrols-RadioBox"></a>
#### `RadioBox` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Buttons/RadioBox.qml](imtcontrols/Buttons/RadioBox.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `model: alias`
- `spacing: alias`
- `selectedIndex: alias`
- `listView: alias`
- `mainMargin: int`

<a id="b-imtcontrols-RadioButton"></a>
#### `RadioButton`

Кнопка.

- **Файл:** [Buttons/RadioButton.qml](imtcontrols/Buttons/RadioButton.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: string`
- `checkState: int`
- `checked: bool`
- `checkSize: int`
- `fontPixelSize: int`
- `fontBold: bool`
- `mainMargin: int`
- `selectedColor: string`
- `autoExclusive: bool`
- `tooltipText: alias`
- `tooltipItem: alias`

**Сигналы**

- `clicked()`

**Функции**

- `clickedFuntion()`
- `onRadioButtonUnchecked(object_)`

<a id="b-imtcontrols-RadioButtonDecorator"></a>
#### `RadioButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/RadioButtonDecorator.qml](imtcontrols/Buttons/RadioButtonDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `color: string`
- `selectedColor: string`
- `fontSize: int`

<a id="b-imtcontrols-SegmentedButton"></a>
#### `SegmentedButton`

Кнопка.

- **Файл:** [Buttons/SegmentedButton.qml](imtcontrols/Buttons/SegmentedButton.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `firstSegmentDecorator: Component`
- `middleSegmentDecorator: Component`
- `lastSegmentdecorator: Component`
- `isExclusive: bool`
- `checkable: bool`
- `compl: bool`
- `helpers_: var`
- `ready: bool`
- `isSet: bool`
- `spacing: int`
- `selectedIndex: int`
- `rowAlias: alias`
- `rowCount: int`
- `readOnly: bool`

**Сигналы**

- `clicked(int index)`

**Функции**

- `setChecked(itemIndex)`
- `createObjectHelper(index)`
- `setDecorator(index)`

<a id="b-imtcontrols-TextButton"></a>
#### `TextButton`

Кнопка.

- **Файл:** [Buttons/TextButton.qml](imtcontrols/Buttons/TextButton.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `active: bool`
- `hasIndicator: bool`
- `legend: string`
- `textWidth: int`
- `fontFamily: string`
- `fontPixelSize: int`
- `fontBold: bool`
- `indicatorHeight: alias`
- `tooltipText: alias`
- `tooltipItem: alias`
- `legendColor: var`
- `indicatorColor: string`

**Сигналы**

- `clicked()`

<a id="b-imtcontrols-TextButtonDecorator"></a>
#### `TextButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/TextButtonDecorator.qml](imtcontrols/Buttons/TextButtonDecorator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `baseButton: var`
- `radius: alias`
- `color: alias`

<a id="b-imtcontrols-ToolButton"></a>
#### `ToolButton`

Кнопка.

- **Файл:** [Buttons/ToolButton.qml](imtcontrols/Buttons/ToolButton.qml)
- **Базовый тип:** [`Button`](#b-imtcontrols-Button)

<a id="b-imtcontrols-ToolButtonDecorator"></a>
#### `ToolButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/ToolButtonDecorator.qml](imtcontrols/Buttons/ToolButtonDecorator.qml)
- **Базовый тип:** [`ButtonDecorator`](#b-imtcontrols-ButtonDecorator)

<a id="b-imtcontrols-TopButtonDecorator"></a>
#### `TopButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Buttons/TopButtonDecorator.qml](imtcontrols/Buttons/TopButtonDecorator.qml)
- **Базовый тип:** [`ButtonDecorator`](#b-imtcontrols-ButtonDecorator)

**Свойства**

- `isEmpty: bool`

<a id="b-imtcontrols-TopButtonDecoratorNew"></a>
#### `TopButtonDecoratorNew` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Buttons/TopButtonDecoratorNew.qml](imtcontrols/Buttons/TopButtonDecoratorNew.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `baseButton: var`
- `textColor: string`
- `textColorDisabled: string`
- `fontSize: real`
- `radius: int`
- `isEmpty: bool`

### Inputs/ — Поля ввода

<a id="b-imtcontrols-BarControl"></a>
#### `BarControl`

Визуальный компонент.

- **Файл:** [Inputs/BarControl.qml](imtcontrols/Inputs/BarControl.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `orientation: int`
- `backgroundColor: string`
- `controlColor: string`
- `radius: alias`
- `value: real`

<a id="b-imtcontrols-Calendar"></a>
#### `Calendar`

Визуальный компонент.

- **Файл:** [Inputs/Calendar.qml](imtcontrols/Inputs/Calendar.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `centered: bool`
- `root: Item`
- `hiddenBackground: bool`
- `noMouseArea: bool`
- `forceFocus: bool`
- `multiSelection: bool`
- `canFlick: bool`
- `hasTopMenu: bool`
- `hasButtonMenu: bool`
- `gridOnly: bool`
- `canShowToday: bool`
- `readOnly: bool`
- `hasDoubleClickReaction: bool`
- `startYear: int`
- `lastYear: int`
- `minDate: var`
- `maxDate: var`
- `selectedIndexMonth: int`
- `selectedIndexYear: int`
- `selectedMonthName: string`
- `selectedDay: int`
- `selectedMonth: int`
- `selectedYear: int`
- `endDay: int`
- `endMonth: int`
- `endYear: int`
- `today: var`
- `todayDay: int`
- `todayMonth: int`
- `todayYear: int`
- `selectedDateExist: bool`
- `endDateExist: bool`
- `startNotFinishDate: bool`
- `maxMonthName: string`
- `listViewContentX: int`
- `monthNames: var`
- `monthNamesEng: var`
- `monthNamesRus: var`
- `dayOfWeek: var`
- `dayOfWeekRus: var`
- `dayOfWeekEng: var`
- `mainColor: string`
- `cellColor: string`
- `selectColor: string`
- `topPanelColor: string`
- `highlightColor: string`
- `fontColor_title: string`
- `fontColor_cell: string`
- `fontSize_cell: int`
- `fontSize_title: int`
- `fontBold_title: bool`
- `weekDayListHeight: int`
- `leftImageSource: alias`
- `rightImageSource: alias`
- `bodyWidth: int`
- `okButtonText: string`
- `cancelButtonText: string`
- `todayButtonText: string`
- `hasMonthCombo: bool`
- `hasYearCombo: bool`
- `shownItemsCountCombo: int`
- `monthCombo: alias`
- `yearCombo: alias`
- `topPanel: alias`
- `topPanelHeight: alias`
- `compl: bool`
- `dateStart: var`
- `dateFinish: var`
- `date: var`
- `dateStartStr: string`
- `dateFinishStr: string`
- `dateStr: string`
- `calendarModel: var`
- `weekDayCellDelegate: Component`
- `dayCellDelegate: Component`

**Сигналы**

- `accepted()`
- `canceled()`
- `doubleClicked()`

**Функции**

- `onBackgroundClicked()`
- `onAppSizeChanged(parameters)`
- `setModel(model_)`
- `setDate(date_)`
- `goToMonth(month, year)`
- `close()`
- `reset()`
- `setMaxMonthName()`
- `fillMonthComboModel()`
- `fillYearComboModel()`
- `lessThanDate(y1, m1 ,d1, y2, m2, d2)`
- `moreThanDate(y1, m1 ,d1, y2, m2, d2)`
- `fillMonthModel(month, year)`
- `decrease()`
- `increase()`
- `acceptFunction()`

<a id="b-imtcontrols-CalendarCellDelegateBase"></a>
#### `CalendarCellDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Inputs/CalendarCellDelegateBase.qml](imtcontrols/Inputs/CalendarCellDelegateBase.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`CalendarCellTextDelegate`](#b-imtcontrols-CalendarCellTextDelegate)

**Свойства**

- `calendarItem: Item`
- `grid: Item`
- `isSelectedDate: bool`
- `isEndDate: bool`
- `isToday: bool`
- `modelDay: real`
- `modelMonth: real`
- `modelYear: real`
- `isMidlleDateLR: bool`

<a id="b-imtcontrols-CalendarCellTextDelegate"></a>
#### `CalendarCellTextDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Inputs/CalendarCellTextDelegate.qml](imtcontrols/Inputs/CalendarCellTextDelegate.qml)
- **Базовый тип:** [`CalendarCellDelegateBase`](#b-imtcontrols-CalendarCellDelegateBase)

<a id="b-imtcontrols-CalendarFunctions"></a>
#### `CalendarFunctions`

Набор общих функций.

- **Файл:** [Inputs/CalendarFunctions.qml](imtcontrols/Inputs/CalendarFunctions.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `monthNames: var`

**Функции**

- `monthName(ind)`
- `isLeapYearFun(year)`
- `getLastDayOfMonth(month,year)`
- `weekDayNumber(number)`
- `fillDayModel(month, year)`
- `format(num)`
- `getDateStr(date)`
- `fillMonthData(month, year)`

<a id="b-imtcontrols-CalendarView"></a>
#### `CalendarView`

Визуальное представление.

- **Файл:** [Inputs/CalendarView.qml](imtcontrols/Inputs/CalendarView.qml)
- **Базовый тип:** [`ListView`](https://doc.qt.io/qt-6/qml-qtquick-listview.html)

**Свойства**

- `canFillModel: bool`
- `canSetIndexes: bool`
- `movementStartX: real`
- `weekDayCellDelegate: Component`
- `dayCellDelegate: Component`
- `radius: int`
- `weekDayListHeight: int`
- `dayOfWeekModel: ListModel`
- `color: string`
- `cellColor: string`
- `fontColor_cell: string`
- `fontSize_cell: int`
- `gridCellWidth: int`
- `gridCellHeight: int`
- `dayOfWeek: var`

<a id="b-imtcontrols-CheckBox"></a>
#### `CheckBox`

Флажок.

- **Файл:** [Inputs/CheckBox.qml](imtcontrols/Inputs/CheckBox.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: string`
- `tooltipText: alias`
- `tooltipItem: alias`
- `mouseArea: alias`
- `borderColor: string`
- `radius: int`
- `isActive: bool`
- `tristate: bool`
- `checkState: int`
- `checkIndicator: string`
- `imageSourceActive: string`
- `imageSourceNotActive: string`
- `fontPixelSize: int`
- `fontBold: bool`
- `isLeftText: bool`
- `mainMargin: int`

**Функции**

- `nextCheckState()`

<a id="b-imtcontrols-CheckBoxMenu"></a>
#### `CheckBoxMenu`

Меню.

- **Файл:** [Inputs/CheckBoxMenu.qml](imtcontrols/Inputs/CheckBoxMenu.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)

**Свойства**

- `model: var`
- `nameId: string`
- `currentText: string`
- `allChecked: bool`
- `radius: int`
- `currentIndex: int`
- `placeHolderText: string`
- `multipleChoiceText: string`
- `borderColor: color`
- `backgroundColor: color`
- `isColor: bool`
- `dialogsCountPrev: int`
- `dialogsCount: int`
- `image: alias`
- `imageSource: alias`
- `imageWidth: alias`
- `imageHeight: alias`
- `imageRotation: alias`
- `imageVisible: alias`
- `checkImageSource: string`
- `textSize: int`
- `fontColor: string`
- `fontColorTitle: string`
- `tooltipText: alias`
- `tooltipItem: alias`
- `isOpen: bool`
- `dataModel: TreeItemModel`
- `retModel: TreeItemModel`
- `menuItem: var`
- `canOpenMenu: bool`
- `hasSearch: bool`
- `hasAllSelection: bool`
- `menuHeight: int`
- `delegateHeight: int`
- `menuItemContent: Component`

**Сигналы**

- `finished()`
- `menuCreated()`
- `changedSignal()`
- `setAllSelected()`
- `setAllNotSelected()`

**Функции**

- `openPopupMenu()`
- `closePopupMenu()`
- `createMenuItem()`
- `setText(model_, role_)`

<a id="b-imtcontrols-ComboBox"></a>
#### `ComboBox`

Выпадающий список.

- **Файл:** [Inputs/ComboBox.qml](imtcontrols/Inputs/ComboBox.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)
- **Наследники:** [`FilterableComboBox`](#b-imtcontrols-FilterableComboBox)

**Свойства**

- `model: var`
- `borderColor: color`
- `backgroundColor: color`
- `nameId: string`
- `displayId: string`
- `currentText: string`
- `placeHolderText: string`
- `compMainColor: string`
- `compSelectedColor: string`
- `fontColor: string`
- `fontColorTitle: string`
- `changeable: bool`
- `isColor: bool`
- `textCentered: bool`
- `hiddenBackground: bool`
- `openST: bool`
- `visibleScrollBar: bool`
- `visibleIcon: bool`
- `moveToEnd: bool`
- `hoverBlocked: bool`
- `isOpen: bool`
- `currentIndex: int`
- `shownItemsCount: int`
- `moveToIndex: int`
- `radius: int`
- `contentLeftMargin: int`
- `textSize: int`
- `itemHeight: int`
- `contentY: real`
- `popup: var`
- `containsMouse: alias`
- `mouseArea: alias`
- `tooltipText: alias`
- `tooltipItem: alias`
- `delegate: Component`
- `popupMenuComp: Component`

**Сигналы**

- `accepted()`
- `activated()`
- `highlighted(int index)`
- `clicked()`
- `finished(string itemId, int index)`

**Функции**

- `updateText()`
- `closePopupMenu()`
- `openPopupMenu()`
- `onMouseAreaClicked()`

<a id="b-imtcontrols-CustomTextEdit"></a>
#### `CustomTextEdit` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Inputs/CustomTextEdit.qml](imtcontrols/Inputs/CustomTextEdit.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)

**Свойства**

- `isDinamicHeight: bool`
- `text: alias`
- `textInputFocus: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `borderWidth: alias`
- `color: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `fontBold: bool`
- `placeHolderOpacity: real`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `setFocus(value)`

<a id="b-imtcontrols-CustomTextField"></a>
#### `CustomTextField`

Поле ввода.

- **Файл:** [Inputs/CustomTextField.qml](imtcontrols/Inputs/CustomTextField.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)
- **Наследники:** [`DateTimePicker`](#b-imtcontrols-DateTimePicker), [`SearchTextInput`](#b-imtcontrols-SearchTextInput)

**Свойства**

- `text: string`
- `acceptableInput: alias`
- `textInputFocus: alias`
- `textInputActiveFocus: alias`
- `textInputMask: alias`
- `textInputValidator: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `borderWidth: alias`
- `color: alias`
- `leftPadding: alias`
- `maximumLength: alias`
- `selectionStart: alias`
- `selectionEnd: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `fontFamily: alias`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `textFieldLeftMargin: int`
- `textFieldRightMargin: int`
- `fontBold: bool`
- `hasActionMenu: bool`
- `autoEditingFinished: bool`
- `editingFinishedTimerInterval: int`
- `placeHolderOpacity: real`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `select(from, to)`
- `selectAll()`
- `setFocus(value)`
- `ensureVisible(pos)`
- `copy()`

<a id="b-imtcontrols-CustomTextField_"></a>
#### `CustomTextField_` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Inputs/CustomTextField_.qml](imtcontrols/Inputs/CustomTextField_.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)

**Свойства**

- `text: alias`
- `acceptableInput: alias`
- `textInputFocus: alias`
- `textInputMask: alias`
- `textInputValidator: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `borderWidth: alias`
- `color: alias`
- `maximumLength: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `fontBold: bool`
- `placeHolderOpacity: real`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `setFocus(value)`
- `ensureVisible(pos)`

<a id="b-imtcontrols-DateInput"></a>
#### `DateInput`

Поле ввода.

- **Файл:** [Inputs/DateInput.qml](imtcontrols/Inputs/DateInput.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `inputHeight: int`
- `fontSize: int`
- `fontColor: string`
- `radius: alias`
- `color: alias`
- `placeHolderText: string`
- `dateRegDay: var`
- `dateRegMonth: var`
- `dateRegExpFull: var`
- `dateRegExp: var`
- `canShowCurrentDate: bool`
- `hasTitle: bool`
- `readOnly: bool`
- `isError: bool`
- `isMonthError: bool`
- `isDayError: bool`
- `errorString: string`
- `errorStringMonth: string`
- `errorStringDay: string`
- `calendarWidth: int`
- `calendarHeight: int`
- `tabKeyItem: Item`
- `calendarItem: Item`
- `inputItem: alias`
- `dateStr: string`
- `selectedDate: var`

**Сигналы**

- `dateChanged(string value)`
- `calendarButtonClicked()`

**Функции**

- `setDate(date)`
- `setDateAsString(str)`
- `getDateAsString()`
- `showCurrentDate()`
- `correctDateFormat(str)`
- `checkDateFormat(str)`
- `checkMonth(month)`
- `checkDay(day, month, year)`
- `isLeapYear(year)`

<a id="b-imtcontrols-DatePicker"></a>
#### `DatePicker`

Пикер (выбор значения).

- **Файл:** [Inputs/DatePicker.qml](imtcontrols/Inputs/DatePicker.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `selectedYear: alias`
- `selectedMonth: alias`
- `selectedDay: alias`
- `currentDayButtonVisible: alias`
- `defaultWidth: alias`
- `moveToEndYear: alias`
- `moveToIndexYear: alias`
- `monthCombo: alias`
- `yearCombo: alias`
- `dayCombo: alias`
- `contentWidth: alias`
- `monthNames: var`
- `startYear: int`
- `lastYear: int`
- `todayYear: int`
- `todayMonth: int`
- `selectedIndexMonth: int`
- `shownItemsCountCombo: int`
- `completed: bool`
- `startWithCurrentDay: bool`
- `startWithFirstDay: bool`
- `hasDay: bool`
- `hasDayCombo: bool`
- `hasMonthCombo: bool`
- `hasYearCombo: bool`
- `hasButtons: bool`
- `openST: bool`
- `endComboOnCurrentMonth: bool`
- `readOnly: bool`
- `textInputBan: bool`
- `iconWidth: int`
- `iconHeight: int`
- `iconUpSource: string`
- `iconDownSource: string`
- `mainMargin: int`
- `textFieldHeight: int`
- `textFieldWidthYear: int`
- `textFieldWidthMonth: int`
- `textFieldWidthDay: int`
- `textFieldRadius: int`
- `textSize: int`
- `textFieldColor: string`
- `fontColor: string`
- `textFieldBorderColor: string`
- `textFieldMargin: int`
- `horizontalAlignment: int`
- `buttonWidth: int`
- `buttonHeight: int`
- `buttonColor: string`
- `buttonHighlighted: int`
- `spinBoxSpacing: int`
- `yearTreeModel: TreeItemModel`
- `monthTreeModel: TreeItemModel`
- `dayTreeModel: TreeItemModel`
- `params: var`

**Сигналы**

- `dateChanged()`

**Функции**

- `getDate()`
- `getDateAsString()`
- `setCurrentDay()`
- `checkDate(year, month, day)`
- `findIndexByValue(treeModel, fieldId, targetValue)`
- `setDateFromDateObject(dateObj)`
- `setDate(year, month, day)`
- `setDateFromString(stringDate)`
- `fillDayModel()`
- `fillMonthModel()`
- `getLastDayOfMonth(month)`
- `isLeapYearFun(year)`

<a id="b-imtcontrols-DateTimeInput"></a>
#### `DateTimeInput`

Поле ввода.

- **Файл:** [Inputs/DateTimeInput.qml](imtcontrols/Inputs/DateTimeInput.qml)
- **Базовый тип:** [`Row`](https://doc.qt.io/qt-6/qml-qtquick-row.html)

**Свойства**

- `canShowCurrentTime: bool`
- `hasTitle: bool`
- `color: string`
- `readOnly: bool`
- `selectedDate: var`
- `dateStr: string`
- `timeStr: string`
- `dateTimeStr: string`
- `dateTimeRegExp: var`
- `calendarWidth: int`
- `calendarHeight: int`
- `timeInputDialogWidth: int`
- `timeInputDialogHeight: int`
- `fontSize: int`
- `dateInputWidth: alias`
- `timeInputWidth: alias`

**Сигналы**

- `dateChanged(string value)`

**Функции**

- `setDateTime(datetime)`
- `setDate(date)`
- `setTime(date)`
- `setDateTimeAsString(str)`
- `setDateAsString(str)`
- `setTimeAsString(str)`

<a id="b-imtcontrols-DateTimePicker"></a>
#### `DateTimePicker`

Пикер (выбор значения).

- **Файл:** [Inputs/DateTimePicker.qml](imtcontrols/Inputs/DateTimePicker.qml)
- **Базовый тип:** [`CustomTextField`](#b-imtcontrols-CustomTextField)

**Свойства**

- `calendarPopup: bool`
- `timeEdit: bool`
- `maximumDate: date`
- `minimumDate: date`
- `displayFormat: string`
- `iconUpSource: string`
- `iconDownSource: string`
- `selectedDate: var`

**Сигналы**

- `dateChanged()`

**Функции**

- `setDate(date)`
- `setDateAsString(dateStr)`
- `getDateAsString()`
- `dateToString(date)`
- `validateDateFormat(dateString)`
- `isValidDate(dateString, hasTime)`
- `dayIncrement()`
- `dayDecrement()`
- `monthIncrement()`
- `monthDecrement()`
- `yearIncrement()`
- `yearDecrement()`
- `hourIncrement()`
- `hourDecrement()`
- `minutesIncrement()`
- `minutesDecrement()`

<a id="b-imtcontrols-FilterableComboBox"></a>
#### `FilterableComboBox`

Выпадающий список.

- **Файл:** [Inputs/FilterableComboBox.qml](imtcontrols/Inputs/FilterableComboBox.qml)
- **Базовый тип:** [`ComboBox`](#b-imtcontrols-ComboBox)

**Свойства**

- `filter: string`
- `filteringFields: var`
- `descriptionFieldId: string`
- `sourceModel: var`
- `textInput: CustomTextField`

**Функции**

- `updateFilterModel()`
- `openPopupMenu()`

<a id="b-imtcontrols-InputActions"></a>
#### `InputActions`

Визуальный компонент.

- **Файл:** [Inputs/InputActions.qml](imtcontrols/Inputs/InputActions.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `canCopy: bool`
- `canCut: bool`
- `canPaste: bool`
- `canRemove: bool`
- `canSelectAll: bool`

**Сигналы**

- `copyClicked()`
- `cutClicked()`
- `pasteClicked()`
- `removeClicked()`
- `selectAllClicked()`

**Функции**

- `open(x, y)`

<a id="b-imtcontrols-PathPicker"></a>
#### `PathPicker`

Пикер (выбор значения).

- **Файл:** [Inputs/PathPicker.qml](imtcontrols/Inputs/PathPicker.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `path: string`

<a id="b-imtcontrols-RangeSlider"></a>
#### `RangeSlider`

Ползунок.

- **Файл:** [Inputs/RangeSlider.qml](imtcontrols/Inputs/RangeSlider.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `backgroundHeight: int`
- `backgroundColor: string`
- `controlColor: string`
- `backgroundOpacity: real`
- `controlWidth: real`
- `controlHeight: real`
- `orientation: int`
- `isVertical: bool`
- `from: real`
- `to: real`
- `mainSize: real`
- `secondSize: real`
- `controlRecXFirst: real`
- `positionFirst: real`
- `valueFirst: real`
- `controlRecXSecond: real`
- `positionSecond: real`
- `valueSecond: real`
- `majorTickInterval: int`
- `minorTickInterval: int`
- `hasTicks: bool`
- `hasIndicator: bool`
- `hasTooltip: bool`
- `majorTickHeight: int`
- `minorTickHeight: int`
- `indicatorHeight: int`
- `ticksPosition: int`
- `indicatorPosition: int`
- `controlCenterY: real`
- `fontSize: int`
- `tooltipText: alias`
- `tooltipItem: alias`
- `marginsChanged: bool`

**Функции**

- `decoratorChangedFunc()`
- `correctPositionParams()`
- `setSizeParams()`

<a id="b-imtcontrols-SearchTextInput"></a>
#### `SearchTextInput`

Поле ввода.

- **Файл:** [Inputs/SearchTextInput.qml](imtcontrols/Inputs/SearchTextInput.qml)
- **Базовый тип:** [`CustomTextField`](#b-imtcontrols-CustomTextField)

**Сигналы**

- `searchChanged()`

**Функции**

- `clear()`

<a id="b-imtcontrols-Slider"></a>
#### `Slider`

Ползунок.

- **Файл:** [Inputs/Slider.qml](imtcontrols/Inputs/Slider.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `backgroundHeight: int`
- `backgroundColor: string`
- `controlColor: string`
- `backgroundOpacity: real`
- `controlRecX: real`
- `controlWidth: real`
- `controlHeight: real`
- `orientation: int`
- `isVertical: bool`
- `mainSize: real`
- `secondSize: real`
- `position: real`
- `from: real`
- `to: real`
- `value: real`
- `majorTickInterval: int`
- `minorTickInterval: int`
- `hasTicks: bool`
- `hasIndicator: bool`
- `majorTickHeight: int`
- `minorTickHeight: int`
- `indicatorHeight: int`
- `ticksPosition: int`
- `indicatorPosition: int`
- `controlCenterY: real`
- `fontSize: int`
- `tooltipText: alias`
- `tooltipItem: alias`
- `marginsChanged: bool`
- `hasTooltip: bool`

**Функции**

- `decoratorChangedFunc()`
- `correctPositionParams()`
- `setSizeParams()`

<a id="b-imtcontrols-SpinBox"></a>
#### `SpinBox`

Вспомогательный компонент.

- **Файл:** [Inputs/SpinBox.qml](imtcontrols/Inputs/SpinBox.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `numberReg: var`
- `startValue: real`
- `from: real`
- `to: real`
- `stepSize: real`
- `value: real`
- `editable: bool`
- `wrap: bool`
- `icon: string`
- `units: string`

**Функции**

- `increase()`
- `decrease()`

<a id="b-imtcontrols-SpinBoxComp"></a>
#### `SpinBoxComp`

Визуальный компонент.

- **Файл:** [Inputs/SpinBoxComp.qml](imtcontrols/Inputs/SpinBoxComp.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `baseElement: var`
- `value: real`
- `numberReg: var`
- `startValue: real`
- `from: real`
- `to: real`
- `stepSize: real`
- `editable: bool`
- `wrap: bool`
- `icon: string`
- `fontColor: string`
- `fontSize: int`
- `textInputEnabled: bool`

<a id="b-imtcontrols-TextArea"></a>
#### `TextArea`

Многострочное поле.

- **Файл:** [Inputs/TextArea.qml](imtcontrols/Inputs/TextArea.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `isDinamicHeight: bool`
- `text: string`
- `textInputFocus: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `fontBold: bool`
- `placeHolderOpacity: real`
- `placeHolderVerticalCentered: bool`
- `editingFinishedInterval: int`
- `notEmitEditingFinishedWhenFocusLost: bool`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `setFocus(value)`
- `openInputAction(pointX, pointY)`

<a id="b-imtcontrols-TextEditCustom"></a>
#### `TextEditCustom`

Вспомогательный компонент.

- **Файл:** [Inputs/TextEditCustom.qml](imtcontrols/Inputs/TextEditCustom.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `isDinamicHeight: bool`
- `text: alias`
- `textInputFocus: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `fontBold: bool`
- `placeHolderOpacity: real`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `setFocus(value)`
- `openInputAction(pointX, pointY)`

<a id="b-imtcontrols-TextField"></a>
#### `TextField`

Поле ввода.

- **Файл:** [Inputs/TextField.qml](imtcontrols/Inputs/TextField.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: alias`
- `acceptableInput: alias`
- `textInputFocus: alias`
- `textInputMask: alias`
- `textInputValidator: alias`
- `readOnly: alias`
- `horizontalAlignment: alias`
- `maximumLength: alias`
- `placeHolderText: string`
- `focusColor: string`
- `fontColor: string`
- `placeHolderFontColor: string`
- `borderColor: color`
- `borderColorConst: string`
- `radius: int`
- `textSize: int`
- `placeHolderTextSize: int`
- `echoMode: int`
- `margin: int`
- `fontBold: bool`
- `placeHolderOpacity: real`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `setFocus(value)`
- `ensureVisible(pos)`
- `openInputAction(pointX, pointY)`

<a id="b-imtcontrols-TextFieldWithTitle"></a>
#### `TextFieldWithTitle`

Визуальный компонент.

- **Файл:** [Inputs/TextFieldWithTitle.qml](imtcontrols/Inputs/TextFieldWithTitle.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `text: alias`
- `maximumLength: alias`
- `borderColor: alias`
- `acceptableInput: alias`
- `textInputValidator: alias`
- `echoMode: alias`
- `font: alias`
- `titleFontFamily: alias`
- `titleFontPixelSize: alias`
- `placeHolderText: string`
- `textInput: alias`
- `spacing: alias`
- `radius: int`
- `readOnly: alias`

**Сигналы**

- `editingFinished()`

<a id="b-imtcontrols-TextInputWithLabel"></a>
#### `TextInputWithLabel`

Визуальный компонент.

- **Файл:** [Inputs/TextInputWithLabel.qml](imtcontrols/Inputs/TextInputWithLabel.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `text: alias`
- `maximumLength: alias`
- `borderColor: alias`
- `acceptableInput: alias`
- `textInputValidator: alias`
- `echoMode: alias`
- `titleFontFamily: alias`
- `placeHolderText: string`
- `textInput: alias`
- `spacing: alias`
- `radius: int`

**Сигналы**

- `editingFinished()`

<a id="b-imtcontrols-TimeInput"></a>
#### `TimeInput`

Поле ввода.

- **Файл:** [Inputs/TimeInput.qml](imtcontrols/Inputs/TimeInput.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `inputHeight: int`
- `fontSize: int`
- `fontColor: string`
- `radius: alias`
- `color: alias`
- `readOnly: bool`
- `timeInputDialogWidth: int`
- `timeInputDialogHeight: int`
- `placeHolderText: string`
- `timeRegExpFull: var`
- `timeRegExp: var`
- `canShowCurrentTime: bool`
- `hasTitle: bool`
- `isError: bool`
- `tabKeyItem: Item`
- `inputItem: alias`
- `selectedTime: string`

**Сигналы**

- `timeChanged(string value)`
- `dialogClosed()`

**Функции**

- `checkDateFormat(str)`
- `correctTimeFormat(str)`
- `setTime(str)`
- `showCurrentTime()`
- `getHours()`
- `getMinutes()`

<a id="b-imtcontrols-TimeInputDialog"></a>
#### `TimeInputDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Inputs/TimeInputDialog.qml](imtcontrols/Inputs/TimeInputDialog.qml)
- **Базовый тип:** [`PopupView`](#b-imtgui-PopupView)

**Свойства**

- `radius: int`
- `color: string`
- `hours: int`
- `minutes: int`
- `readOnly: bool`
- `hoursStr: string`
- `minutesStr: string`
- `time: string`
- `pauseAnimDuration: int`

**Сигналы**

- `accepted(string time)`

**Функции**

- `increaseHourse()`
- `decreaseHourse()`
- `increaseMinutes()`
- `decreaseMinutes()`

<a id="b-imtcontrols-TimeIntervalPicker"></a>
#### `TimeIntervalPicker`

Пикер (выбор значения).

- **Файл:** [Inputs/TimeIntervalPicker.qml](imtcontrols/Inputs/TimeIntervalPicker.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `days: int`
- `hours: int`
- `minutes: int`
- `allCompleted: bool`

**Сигналы**

- `intervalChanged()`

**Функции**

- `getInSeconds()`
- `setDays(day)`
- `setHours(hour)`
- `setMinutes(minute)`

<a id="b-imtcontrols-TimePicker"></a>
#### `TimePicker`

Пикер (выбор значения).

- **Файл:** [Inputs/TimePicker.qml](imtcontrols/Inputs/TimePicker.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `hours: int`
- `minutes: int`
- `allCompleted: bool`

**Сигналы**

- `timeChanged()`

**Функции**

- `getInSeconds()`
- `setHours(hour)`
- `setMinutes(minute)`

### Menu/ — Меню

<a id="b-imtcontrols-Menu"></a>
#### `Menu`

Меню.

- **Файл:** [Menu/Menu.qml](imtcontrols/Menu/Menu.qml)
- **Базовый тип:** [`Popup`](#b-imtcontrols-Popup)

**Свойства**

- `title: string`
- `currentIndex: int`
- `cascade: bool`
- `overlap: int`
- `submenuHoverDelay: int`
- `model: var`
- `delegate: Component`
- `count: int` — только чтение
- `submenusList: var`
- `openSubmenu: var`

**Сигналы**

- `triggered(var menuItem)`

**Функции**

- `itemAt(index)`
- `addItem(menuItem)`
- `addSeparator()`
- `addAction(action)`
- `addMenu(sub)`
- `insertItem(index, menuItem)`
- `removeItem(indexOrItem)`
- `takeItem(index)`
- `popup()`

<a id="b-imtcontrols-MenuBar"></a>
#### `MenuBar`

Панель/строка.

- **Файл:** [Menu/MenuBar.qml](imtcontrols/Menu/MenuBar.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)

**Свойства**

- `menus: alias`
- `currentIndex: int`
- `focusedIndex: int`
- `active: bool` — только чтение

**Функции**

- `addMenu(menu)`

<a id="b-imtcontrols-MenuBarItem"></a>
#### `MenuBarItem`

Визуальный компонент.

- **Файл:** [Menu/MenuBarItem.qml](imtcontrols/Menu/MenuBarItem.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `text: string`
- `menu: var`
- `barIndex: int`
- `menuBar: var`
- `highlighted: bool`

<a id="b-imtcontrols-MenuItem"></a>
#### `MenuItem`

Визуальный компонент.

- **Файл:** [Menu/MenuItem.qml](imtcontrols/Menu/MenuItem.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `text: string`
- `iconSource: string`
- `shortcut: string`
- `enabled: bool`
- `checkable: bool`
- `checked: bool`
- `highlighted: bool`
- `action: var`
- `hasSubmenu: bool`
- `submenu: var`
- `itemHeight: int`
- `isMenuItem: bool` — только чтение

**Сигналы**

- `triggered()`
- `toggled()`

**Функции**

- `trigger()`

<a id="b-imtcontrols-MenuSeparator"></a>
#### `MenuSeparator`

Визуальный компонент.

- **Файл:** [Menu/MenuSeparator.qml](imtcontrols/Menu/MenuSeparator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `isMenuSeparator: bool` — только чтение

### Popup/ — Всплывающие окна

<a id="b-imtcontrols-Overlay"></a>
#### `Overlay`

Вспомогательный компонент.

- **Файл:** [Popup/Overlay.qml](imtcontrols/Popup/Overlay.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `anchor: Item`
- `item: Item` — только чтение
- `popupCount: int` — только чтение

<a id="b-imtcontrols-Popup"></a>
#### `Popup`

Всплывающее окно.

- **Файл:** [Popup/Popup.qml](imtcontrols/Popup/Popup.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)
- **Наследники:** [`Menu`](#b-imtcontrols-Menu)

**Свойства**

- `opened: bool` — только чтение
- `modal: bool`
- `dim: bool`
- `focusOnOpen: bool`
- `font: var`
- `padding: real`
- `topPadding: real`
- `leftPadding: real`
- `rightPadding: real`
- `bottomPadding: real`
- `margins: real`
- `topMargin: real`
- `leftMargin: real`
- `rightMargin: real`
- `bottomMargin: real`
- `implicitWidth: real`
- `implicitHeight: real`
- `implicitContentWidth: real`
- `implicitContentHeight: real`
- `availableWidth: real` — только чтение
- `availableHeight: real` — только чтение
- `contentChildren: alias`
- `contentData: alias`
- `contentItem: Item`
- `background: Item`
- `overlay: Item` — только чтение
- `closePolicy: int`
- `transformOrigin: int`
- `popupScale: real`
- `popupOpacity: real`
- `mirrored: bool`
- `cursorRelative: bool`
- `parentPopup: var`

**Сигналы**

- `aboutToShow()`
- `aboutToHide()`
- `closed()`

**Функции**

- `open()`
- `close()`
- `popupAt(anchor)`
- `forceActiveFocus()`

<a id="b-imtcontrols-PopupAnchors"></a>
#### `PopupAnchors`

Вспомогательный компонент.

- **Файл:** [Popup/PopupAnchors.qml](imtcontrols/Popup/PopupAnchors.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `centerIn: Item`

### Dialogs/ — Диалоги

<a id="b-imtcontrols-CriticalErrorDialog"></a>
#### `CriticalErrorDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/CriticalErrorDialog.qml](imtcontrols/Dialogs/CriticalErrorDialog.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `message: string`

<a id="b-imtcontrols-Dialog"></a>
#### `Dialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/Dialog.qml](imtcontrols/Dialogs/Dialog.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)
- **Наследники:** [`CriticalErrorDialog`](#b-imtcontrols-CriticalErrorDialog), [`ErrorDialog`](#b-imtcontrols-ErrorDialog), [`InputDialog`](#b-imtcontrols-InputDialog), [`MessageDialog`](#b-imtcontrols-MessageDialog), [`MessagePopup`](#b-imtcontrols-MessagePopup), [`PreferenceDialog`](#b-imtgui-PreferenceDialog), [`ProgressDialogManager`](#b-imtcontrols-ProgressDialogManager), [`TableHeaderParamComp`](#b-imtcontrols-TableHeaderParamComp)

**Свойства**

- `decoratorItem: var`
- `title: string`
- `bodySource: string`
- `topPanelSource: string`
- `forceFocus: bool`
- `centered: bool`
- `pinned: bool`
- `hasIcon: bool`
- `modal: bool`
- `uuid: string`
- `root: Item`
- `rootItem: Item`
- `buttonsModel: ListModel`
- `buttonsModelCount: int`
- `notClosingButtons: int`
- `buttonIds: int`
- `backgroundColor: string`
- `radius: int`
- `canMove: bool`
- `canResize: bool`
- `decoratorVisible: bool`
- `escapeEnabled: bool`
- `closeAfterPause: bool`
- `displayDuration: int`
- `topPanelComp: Component`
- `topPanelDefault: Component`
- `contentComp: Component`
- `selfComp: Component`
- `topPanel: Item`
- `contentItem: Item`
- `bodyItem: Item`
- `buttons: Item`

**Сигналы**

- `finished(int buttonId)`
- `started()`
- `localizationChanged(string language)`
- `accepted()`
- `canceled()`
- `applied()`
- `closed()`

**Функции**

- `addButton(id, name, enabled)`
- `insertButton(id, name, enabled, index)`
- `removeButton(id)`
- `clearButtons()`
- `setButtonName(id, name)`
- `buttonIsEnabled(buttonId)`
- `setButtonEnabled(id, enabled)`
- `onLocalizationChanged(language)`
- `setButtonIds()`

<a id="b-imtcontrols-DialogButtons"></a>
#### `DialogButtons`

Визуальный компонент.

- **Файл:** [Dialogs/DialogButtons.qml](imtcontrols/Dialogs/DialogButtons.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `buttons: alias`
- `count: int`

**Сигналы**

- `buttonClicked(int buttonId)`

**Функции**

- `clearButtons()`
- `addButton(buttonObj)`
- `setButtonState(buttonId, state)`
- `getButtonState(buttonId)`

<a id="b-imtcontrols-DialogDecorator"></a>
#### `DialogDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Dialogs/DialogDecorator.qml](imtcontrols/Dialogs/DialogDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `title: string`
- `bodySource: string`
- `topPanelSource: string`
- `baseElementFocus: bool`
- `topPanelComp: Component`

**Функции**

- `onLocalizationChanged(language)`

<a id="b-imtcontrols-DialogManagerView"></a>
#### `DialogManagerView`

Визуальное представление.

- **Файл:** [Dialogs/DialogManagerView.qml](imtcontrols/Dialogs/DialogManagerView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `dialogsModel: ListModel`

<a id="b-imtcontrols-DialogNew"></a>
#### `DialogNew`

Визуальный компонент.

- **Файл:** [Dialogs/DialogNew.qml](imtcontrols/Dialogs/DialogNew.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `contentComp: Component`
- `buttonsModel: ListModel`
- `backgroundColor: string`
- `radius: int`
- `notClosingButtons: string`
- `centered: bool`
- `hasIcon: bool`
- `bodySource: string`
- `topPanelSource: string`

**Сигналы**

- `finished(string buttonId)`
- `accepted()`
- `canceled()`
- `applied()`

**Функции**

- `open()`

<a id="b-imtcontrols-ErrorDialog"></a>
#### `ErrorDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/ErrorDialog.qml](imtcontrols/Dialogs/ErrorDialog.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `message: string`

<a id="b-imtcontrols-FileExportDialog"></a>
#### `FileExportDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/FileExportDialog.qml](imtcontrols/Dialogs/FileExportDialog.qml)
- **Базовый тип:** `FileDialog`

**Свойства**

- `fileExt: string`
- `mimeTypes: var`
- `dataStr: string`

**Функции**

- `getDialogNameFilterIndex(nameFilters, fileExt)`
- `writeData(name)`

<a id="b-imtcontrols-FileImportDialog"></a>
#### `FileImportDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/FileImportDialog.qml](imtcontrols/Dialogs/FileImportDialog.qml)
- **Базовый тип:** `FileDialog`

**Свойства**

- `mimeTypes: var`
- `loadingInParts: bool`

**Сигналы**

- `importDialogResult(string fileName, string fileNameWithExtension, string encodedData, string mimeType)`
- `beginImport(string fileNameWithExt, string filePath, string fileSize)`

**Функции**

- `getDialogNameFilterIndex(nameFilters, fileExt)`

<a id="b-imtcontrols-InputDialog"></a>
#### `InputDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/InputDialog.qml](imtcontrols/Dialogs/InputDialog.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `message: string`
- `inputValue: string`
- `contentWidth: int`
- `placeHolderText: string`
- `selectTextOnStart: bool`

<a id="b-imtcontrols-InputDialogBody"></a>
#### `InputDialogBody`

Визуальный компонент.

- **Файл:** [Dialogs/InputDialogBody.qml](imtcontrols/Dialogs/InputDialogBody.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `message: string`
- `inputValue: string`
- `rootItem: Item`
- `placeHolderText: alias`
- `selectTextOnStart: bool`

<a id="b-imtcontrols-MessageDialog"></a>
#### `MessageDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/MessageDialog.qml](imtcontrols/Dialogs/MessageDialog.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `message: string`

<a id="b-imtcontrols-MessageDialogBody"></a>
#### `MessageDialogBody`

Визуальный компонент.

- **Файл:** [Dialogs/MessageDialogBody.qml](imtcontrols/Dialogs/MessageDialogBody.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `message: string`

<a id="b-imtcontrols-MessagePopup"></a>
#### `MessagePopup`

Всплывающее окно.

- **Файл:** [Dialogs/MessagePopup.qml](imtcontrols/Dialogs/MessagePopup.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `hiddenBackground: bool`
- `noMouseArea: bool`
- `text: string`
- `contentWidth: int`

<a id="b-imtcontrols-ModalDialogManager"></a>
#### `ModalDialogManager` · **singleton**

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Dialogs/ModalDialogManager.qml](imtcontrols/Dialogs/ModalDialogManager.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `backgroundItem: var`
- `activeView: Item`
- `topItem: Item`
- `modalDialogModels: ListModel`
- `count: alias`
- `itemUuidList: var`
- `dialogCallback: var`
- `infoDialog: Component`
- `warningDialog: Component`
- `criticalDialog: Component`
- `confirmationDialogComp: Component`
- `windowComp: Component`

**Сигналы**

- `finished(int result)`

**Функции**

- `openDialog(comp, parameters, mode, callback)`
- `closeDialog(index)`
- `closeByComp(comp)`
- `closeByUuid(uuid)`
- `dialogIsOpened(dialogComp)`
- `showInfoDialog(message)`
- `showWarningDialog(message)`
- `showCriticalDialog(message)`
- `showErrorDialog(message)`
- `onServerConnectionError()`
- `showConfirmationDialog(title, message, callback, choiceIsRequired)`
- `openWindow(comp, parameters)`

<a id="b-imtcontrols-PopupDecorator"></a>
#### `PopupDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Dialogs/PopupDecorator.qml](imtcontrols/Dialogs/PopupDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `radius: int`
- `itemHeight: int`
- `itemWidth: int`
- `shownItemsCount: int`
- `delegate: var`
- `contentY: alias`
- `moveToEnd: bool`
- `moveToIndex: int`
- `model: var`
- `selectedIndex: int`
- `topContentLoaderSourceComp: alias`
- `bottomContentLoaderSourceComp: alias`
- `repeater: alias`
- `isUpwards: bool`

**Функции**

- `contentYCorrection(down_)`

<a id="b-imtcontrols-PopupDecoratorGql"></a>
#### `PopupDecoratorGql`

Вспомогательный компонент.

- **Файл:** [Dialogs/PopupDecoratorGql.qml](imtcontrols/Dialogs/PopupDecoratorGql.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `itemHeight: int`
- `itemWidth: int`
- `selectedIndex: int`
- `shownItemsCount: int`
- `moveToIndex: int`
- `radius: int`
- `moveToEnd: bool`
- `isUpwards: bool`
- `modelLoading: bool`
- `hasLoading: bool`
- `dataProviderState: string`
- `modelLoadingStates: var`
- `model: var`
- `delegate: var`
- `contentY: alias`
- `topContentLoaderSourceComp: alias`
- `bottomContentLoaderSourceComp: alias`
- `repeater: alias`
- `scrollbar: alias`

**Функции**

- `setLoadingAnimVisible(visible_)`
- `setModelStateRecVisible(visible_)`
- `contentYCorrection(down_)`

<a id="b-imtcontrols-PopupItemDelegateDecorator"></a>
#### `PopupItemDelegateDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Dialogs/PopupItemDelegateDecorator.qml](imtcontrols/Dialogs/PopupItemDelegateDecorator.qml)
- **Базовый тип:** [`ItemDelegateDecorator`](#b-imtcontrols-ItemDelegateDecorator)

**Свойства**

- `isSeparator: bool`
- `selected: bool`

<a id="b-imtcontrols-PopupMenuDialog"></a>
#### `PopupMenuDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/PopupMenuDialog.qml](imtcontrols/Dialogs/PopupMenuDialog.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `model: var`
- `root: Item`
- `uuid: string`
- `itemWidth: int`
- `itemHeight: int`
- `radius: int`
- `shownItemsCount: int`
- `textSize: int`
- `fontColor: string`
- `color: string`
- `hasShadow: bool`
- `hiddenBackground: bool`
- `visibleScrollBar: bool`
- `moveToEnd: bool`
- `moveToIndex: int`
- `contentY: var`
- `forceFocus: bool`
- `centered: bool`
- `isUpwards: bool`
- `selectedIndex: int`
- `hoverBlocked: bool`
- `nameId: string`
- `delegate: Component`

**Сигналы**

- `finished(string commandId, int index)`
- `started()`

**Функции**

- `onAppSizeChanged(parameters)`
- `onBackgroundClicked(events)`
- `close()`
- `onEscape()`
- `onReturn()`
- `onKeyboardUp()`
- `onKeyboardDown()`

<a id="b-imtcontrols-ProgressDialogManager"></a>
#### `ProgressDialogManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Dialogs/ProgressDialogManager.qml](imtcontrols/Dialogs/ProgressDialogManager.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `message: string`
- `totalProgressTitle: string`
- `hiddenBackground: bool`
- `noMouseArea: bool`
- `backgroundOpacity: real`
- `taskId: string`
- `description: string`
- `value: real`
- `closeByCancel: bool`
- `closeWhenFinished: bool`
- `isCancelling: bool`
- `isFailed: bool`
- `subtaskModel: TreeItemModel`
- `messageOpened: bool`

**Сигналы**

- `taskCompleted(string id)`
- `taskCanceled(string id)`
- `taskClosed()`
- `subtaskCanceled(string id)`

**Функции**

- `addSubtask(id, description, value, isCancellable)`
- `setSubtaskData(id, description, value, isCancellable)`

<a id="b-imtcontrols-TopPanelDialog"></a>
#### `TopPanelDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Dialogs/TopPanelDialog.qml](imtcontrols/Dialogs/TopPanelDialog.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `title: string`
- `titleFontSize: int`
- `titleFontFamily: string`
- `titleTextColor: color`
- `hasIcon: bool`
- `canClose: bool`
- `iconSource: string`
- `closeButtonIconSource: string`

**Сигналы**

- `closeButtonClicked(int buttonId)`

### Views/ — Представления

<a id="b-imtcontrols-Arrowhead"></a>
#### `Arrowhead`

Визуальный компонент.

- **Файл:** [Views/Arrowhead.qml](imtcontrols/Views/Arrowhead.qml)
- **Базовый тип:** [`Canvas`](https://doc.qt.io/qt-6/qml-qtquick-canvas.html)

**Свойства**

- `gap: int`
- `color: string`
- `isConcave: bool`

<a id="b-imtcontrols-BarChart"></a>
#### `BarChart`

Диаграмма.

- **Файл:** [Views/BarChart.qml](imtcontrols/Views/BarChart.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `visibleElements: bool`
- `maxBarHeight: real`
- `maxValue: real`
- `negativeValue: real`
- `positiveValue: real`
- `elementWidth: real`
- `barWidth: real`
- `borderWidth: real`
- `addToValue: real`
- `minValue: real`
- `color_positive: string`
- `color_negative: string`
- `backgroundColor: string`
- `hasTooltip: bool`
- `isPositiveTooltip: bool`
- `isNegativeTooltip: bool`
- `hasBottomRounding: bool`
- `toolTipColor: alias`
- `toolTipFontColor: alias`
- `toolTipFontSize: alias`
- `shownVal: alias`
- `positiveBarHeight: alias`
- `negativeBarHeight: alias`
- `parentY: real`
- `canChangeFilling: bool`
- `isFilled: bool`
- `isSelected: bool`

**Сигналы**

- `clicked()`

**Функции**

- `closeTooltip()`

<a id="b-imtcontrols-BarChartSimple"></a>
#### `BarChartSimple`

Визуальный компонент.

- **Файл:** [Views/BarChartSimple.qml](imtcontrols/Views/BarChartSimple.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `visibleElements: bool`
- `maxBarHeight: real`
- `maxValue: real`
- `negativeValue: real`
- `positiveValue: real`
- `elementWidth: real`
- `barWidth: real`
- `borderWidth: real`
- `addToValue: real`
- `minValue: real`
- `color_positive: string`
- `backgroundColor: string`
- `hasTooltip: bool`
- `hasBottomRounding: bool`
- `toolTipColor: alias`
- `toolTipFontColor: alias`
- `toolTipFontSize: alias`
- `shownVal: alias`
- `positiveBarHeight: alias`
- `parentY: real`
- `canChangeFilling: bool`
- `isFilled: bool`
- `isSelected: bool`

**Сигналы**

- `clicked()`

**Функции**

- `closeTooltip()`

<a id="b-imtcontrols-BaseText"></a>
#### `BaseText`

Визуальный компонент.

- **Файл:** [Views/BaseText.qml](imtcontrols/Views/BaseText.qml)
- **Базовый тип:** [`Text`](https://doc.qt.io/qt-6/qml-qtquick-text.html)

<a id="b-imtcontrols-BasicTableView"></a>
#### `BasicTableView`

Табличное представление.

- **Файл:** [Views/BasicTableView.qml](imtcontrols/Views/BasicTableView.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)

**Свойства**

- `headerVisible: bool`
- `contentVisible: bool`
- `readOnly: bool`
- `withoutSelection: bool`
- `checkable: bool`
- `rowDelegate: alias`
- `background: alias`
- `columnCount: int`
- `rowCount: int`
- `rowItemHeight: int`
- `headerHeight: int`
- `contentHeight: alias`
- `rowModel: var`
- `columnModel: var`
- `selectedIndex: ModelIndex`
- `tableSelection: TreeViewSelection`
- `tableListView: alias`
- `itemsList: var`
- `flickable: alias`

**Сигналы**

- `selectionChanged()`
- `rowModelDataChanged(var delegate, var prop)`
- `rowAdded()`
- `rowRemoved()`
- `checkedItemsChanged()`

**Функции**

- `getCheckedItems()`
- `checkItem(delegate)`
- `uncheckItem(delegate)`
- `checkAll()`
- `uncheckAll()`
- `getItemsDataAsList()`
- `resetSelection()`
- `singleSelect(item)`
- `addChildItem(modelIndex, rowObj)`
- `removeChildItem(modelIndex)`
- `removeByIndex(modelIndex)`
- `getItemByIndex(index)`
- `getRootItemByIndex(index)`
- `flickableToEnd()`
- `addRow(row)`
- `addColumn(column)`
- `insertColumn(index, column)`
- `insertRow(index, row)`
- `removeColumn(index)`
- `removeRow(index)`

<a id="b-imtcontrols-BasicTreeView"></a>
#### `BasicTreeView`

Древовидное представление.

- **Файл:** [Views/BasicTreeView.qml](imtcontrols/Views/BasicTreeView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `model: var`
- `columns: var`
- `rowHeight: int`
- `indentation: int`
- `cacheBuffer: int`
- `headerHeight: int`
- `minColumnWidth: int`
- `columnResizeHandleWidth: int`
- `allowColumnResize: bool`
- `showHeader: bool`
- `multiSelect: bool`
- `allowDisabledSelection: bool`
- `skipDisabledOrNonCheckableOnCheck: bool`
- `editable: bool`
- `editOnDoubleClick: bool`
- `allowDisabledEditing: bool`
- `tristate: bool`
- `filterText: string`
- `filterRole: var`
- `filterDebounceMs: int`
- `writeBack: var`
- `editingFocusOutPolicy: string`
- `selectedBackgroundColor: string`
- `hoveredBackgroundColor: string`
- `normalTextColor: string`
- `selectedTextColor: string`
- `disabledTextColor: string`
- `headerBackgroundColor: string`
- `gridLineColor: string`
- `editorErrorColor: string`
- `contentListView: var` — только чтение
- `visibleCount: int` — только чтение
- `selectedCount: int` — только чтение
- `editing: bool` — только чтение
- `editingKey: string` — только чтение
- `editingColumn: int` — только чтение
- `currentIndex: var`
- `editingIndex: var`
- `contentHeight: real` — только чтение

**Сигналы**

- `nodeClicked(var index)`
- `nodeDoubleClicked(var index)`
- `selectionChanged()`
- `checkStateChanged(var index, int state)`
- `checkedItemsChanged()`
- `headerClicked(var column)`
- `cellClicked(var index, var column)`
- `cellDoubleClicked(var index, var column)`
- `cellEditStarted(var index, var column)`
- `cellEdited(var index, var column, var value, var oldValue)`
- `cellEditCanceled(var index, var column)`
- `cellEditCommitFailed(var index, var column, var value, string reason)`

**Функции**

- `rebuild()`
- `rebuildTreePreservingState()`
- `captureExpandedState()`
- `rebuildTree()`
- `buildVisibleTree()`
- `writeBackNode(node)`
- `createIndex(node)`
- `columnCount()`
- `columnAt(i)`
- `columnKey(column)`
- `columnTitle(column)`
- `columnWidth(column)`
- `isTreeColumn(column, columnIndex)`
- `isColumnEditable(column)`
- `columnDataPath(column)`
- `columnType(keyValue, column)`
- `cellValue(keyValue, column)`
- `setCellValue(keyValue, column, value)`
- `valueByPath(object, path)`
- `setValueByPath(object, path, value)`
- `comboIndexOf(options, value)`
- `toggleExpanded(keyValue)`
- `expandNode(keyValue)`
- `collapseNode(keyValue)`
- `expandAll()`
- `collapseAll()`
- `select(keyValue)`
- `deselect(keyValue)`
- `toggleSelection(keyValue)`
- `clearSelection()`
- `selectedKeys()`
- `selectedIndexes()`
- `cleanupSelection()`
- `toggleCheckState(keyValue)`
- `setCheckState(keyValue, state)`
- `checkItem(key)`
- `uncheckItem(key)`
- `setCheckStateSilent(keyValue, state)`
- `checkAll()`
- `uncheckAll()`
- `getCheckedKeys()`
- `getCheckedNodes()`
- `allNodes()`
- `isEditingCell(keyValue, columnIndex)`
- `editCurrent(columnIndex)`
- `startEdit(keyValue, columnOrName)`
- `commitCurrentEdit()`
- `cancelCurrentEdit()`
- `beginEditCell(keyValue, columnIndex)`
- `commitEdit(value)`
- `cancelEdit()`
- `revertCell(indexObject, column)`
- `editNextCell(fromColumn)`
- `editPreviousCell(fromColumn)`
- `setNodeEnabled(keyValue, value)`
- `syncVisibleNode(keyValue)`
- `visibleRowOf(keyValue)`
- `nodeForKey(keyValue)`
- `indexForKey(keyValue)`
- `keyForIndex(indexObject)`
- `isExpanded(keyValue)`
- `isSelected(keyValue)`
- `refreshNode(keyValue)`
- `refreshAll()`
- `ensureVisible(keyValue)`
- `expandParents(keyValue)`
- `selectAndEnsureVisible(keyValue)`
- `scrollToCurrent()`
- `setNodeCheckable(keyValue, value)`
- `setNodeVisible(keyValue, value)`
- `setNodeVisibleBatch(updates)`

<a id="b-imtcontrols-BusyIndicator"></a>
#### `BusyIndicator`

Индикатор.

- **Файл:** [Views/BusyIndicator.qml](imtcontrols/Views/BusyIndicator.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Функции**

- `start()`
- `stop()`

<a id="b-imtcontrols-ChangeItemSize"></a>
#### `ChangeItemSize`

Визуальный компонент.

- **Файл:** [Views/ChangeItemSize.qml](imtcontrols/Views/ChangeItemSize.qml)
- **Базовый тип:** [`MouseArea`](https://doc.qt.io/qt-6/qml-qtquick-mousearea.html)

**Свойства**

- `addToSize: int`
- `minWidth: int`
- `minHeight: int`
- `rootItem: Item`
- `coord: var`

<a id="b-imtcontrols-CollapsibleRectangle"></a>
#### `CollapsibleRectangle`

Визуальный компонент.

- **Файл:** [Views/CollapsibleRectangle.qml](imtcontrols/Views/CollapsibleRectangle.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `minHeight: int`
- `maxHeight: int`
- `buttonMargin: int`
- `iconSource: alias`

<a id="b-imtcontrols-ComboBoxCellContentComp"></a>
#### `ComboBoxCellContentComp`

Вспомогательный компонент.

- **Файл:** [Views/ComboBoxCellContentComp.qml](imtcontrols/Views/ComboBoxCellContentComp.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `model: var`

**Сигналы**

- `currentIndexChanged(int index)`

<a id="b-imtcontrols-CustomScrollbar"></a>
#### `CustomScrollbar`

Визуальный компонент.

- **Файл:** [Views/CustomScrollbar.qml](imtcontrols/Views/CustomScrollbar.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `targetItem: var`
- `vertical: bool`
- `minSize: real`
- `secondSize: real`
- `indicatorRadius: int`
- `backgroundColor: string`
- `indicatorColor: string`
- `highlightColor: string`
- `canFade: bool`
- `isMoving: bool`
- `pauseDuration: int`
- `indicatorMargin: int`
- `minimum: real`
- `notUsed: bool`
- `hideNotUsed: bool`
- `koeff: real`
- `koeffVert: real`
- `koeffHoriz: real`
- `visibleState: bool`
- `inSideTarget: bool`
- `alwaysVisible: bool`
- `canDragOutOfBounds: bool`
- `targetContentHeight: int`
- `targetContentWidth: int`
- `decorator: Component`
- `decorator_: var`
- `scrollIndicatorY: real`
- `scrollIndicatorX: real`
- `secondSizeDecorator: real`
- `allowableGapHor: real`
- `allowableGapVert: real`

**Сигналы**

- `contentXSignal(real contentX)`
- `contentYSignal(real contentY)`
- `movingSignal()`

**Функции**

- `setContentPositionFunc(position_)`
- `setVisibleFunc()`

<a id="b-imtcontrols-CustomTooltip"></a>
#### `CustomTooltip`

Всплывающая подсказка.

- **Файл:** [Views/CustomTooltip.qml](imtcontrols/Views/CustomTooltip.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: string`
- `componentWidth: int`
- `tooltipWidth: int`
- `componentMinHeight: int`
- `componentMargin: int`
- `fitToTextWidth: bool`
- `fitToHCenter: bool`
- `componentHeight: int`
- `fontPixelSize: int`
- `fontBold: bool`
- `fontColor: string`
- `textMargin: int`
- `horizontalAlignment: int`
- `radius: int`
- `color: string`
- `borderWidth: int`
- `borderColor: string`
- `openST: bool`
- `waitingDuration: int`
- `delay: int`
- `timeout: int`
- `dialogIndex: int`
- `lineHeight: real`
- `tooltipContentComp: Component`
- `targetItem: Item`

**Функции**

- `openTooltipWithCoord(xX, yY)`
- `open(xX, yY)`
- `openTooltip(xX, yY)`
- `closeTooltip()`
- `show(xX, yY)`
- `hide()`

<a id="b-imtcontrols-Diagram"></a>
#### `Diagram`

Диаграмма.

- **Файл:** [Views/Diagram.qml](imtcontrols/Views/Diagram.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `diagramName: string`
- `legendX: string`
- `legendY: string`
- `colorPositive: string`
- `colorNegative: string`
- `barWidth: int`
- `model: TreeItemModel`
- `axeYValueModel: TreeItemModel`
- `visibleAxeX: bool`
- `visibleAxeY: bool`
- `fontSize: int`
- `hasMinValue: bool`
- `minValue: real`
- `maxValue: real`
- `maxAxeYValue: real`
- `minWidth: real`
- `backgroundColor: string`
- `axeColor: string`
- `titleColor: string`
- `valueColor: string`
- `hasMiddleLine: bool`
- `hasBarFillingChange: bool`
- `hasBigIndicator: bool`
- `bigIndicatorMargin: real`
- `middleLineColor: string`
- `spacingX: int`
- `xElementRotation: real`
- `parentY: real`
- `globalX: real`
- `maxSizeText: alias`
- `selectedIndex: int`
- `compl: bool`
- `indicatorLimits: var`
- `bigIndicatorComp: Component`
- `bigIndicatorItem: Item`
- `hasTooltip: bool`

**Функции**

- `clear()`
- `setMinValue()`
- `setMaxValue()`
- `fillAxeYModel()`
- `setSizeText()`
- `roundDigit(digit, ceil)`

<a id="b-imtcontrols-DiagramIndicator"></a>
#### `DiagramIndicator`

Индикатор.

- **Файл:** [Views/DiagramIndicator.qml](imtcontrols/Views/DiagramIndicator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `root: Item`
- `rootItem: Item`
- `diagramItem: Item`
- `hiddenBackground: bool`
- `noMouseArea: bool`
- `barX: real`
- `barY: real`
- `text: string`
- `coeff: real`
- `textVerticalOffset: real`
- `mainMarginVertical: int`
- `textMargin: int`
- `textObj: alias`

**Функции**

- `onAppSizeChanged(parameters)`

<a id="b-imtcontrols-DrawingContainer"></a>
#### `DrawingContainer`

Контейнер.

- **Файл:** [Views/DrawingContainer.qml](imtcontrols/Views/DrawingContainer.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `edge: int`
- `content: Component`
- `steps: var`
- `stepsLength: int`
- `mainStep: int`
- `currentStep: int`
- `currentStepIndex: int`
- `isOpen: bool`
- `edgeSize: int`
- `controlColor: string`
- `itemWidth: int`
- `itemHeight: int`
- `loaded: bool`

**Функции**

- `setVisible(visible_)`
- `addStep(step)`
- `setSizes()`

<a id="b-imtcontrols-ExpandingContainer"></a>
#### `ExpandingContainer`

Контейнер.

- **Файл:** [Views/ExpandingContainer.qml](imtcontrols/Views/ExpandingContainer.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `startWidth: int`
- `startHeight: int`
- `finishWidth: int`
- `finishHeight: int`
- `duration: int`
- `isOpen: bool`

**Функции**

- `open()`
- `close()`

<a id="b-imtcontrols-Gallery"></a>
#### `Gallery`

Визуальный компонент.

- **Файл:** [Views/Gallery.qml](imtcontrols/Views/Gallery.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `model: TreeItemModel`
- `nameId: string`
- `isOpen: bool`
- `animDuration: int`
- `isFullScreen: bool`

**Сигналы**

- `deleteSignal(int index)`

**Функции**

- `open()`
- `close()`

<a id="b-imtcontrols-InputView"></a>
#### `InputView`

Визуальное представление.

- **Файл:** [Views/InputView.qml](imtcontrols/Views/InputView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `model: var`
- `dataModel: TreeItemModel`
- `fitToWidth: bool`
- `orientation: int`
- `spacing: int`
- `contentHeight: alias`
- `contentWidth: alias`
- `scrollbar: alias`

**Сигналы**

- `modelDataChanged(string role, int index)`

**Функции**

- `setChildParam(param, val, index_)`

<a id="b-imtcontrols-IntervalBar"></a>
#### `IntervalBar`

Панель/строка.

- **Файл:** [Views/IntervalBar.qml](imtcontrols/Views/IntervalBar.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `maxColumnHeight: int`
- `columnWidth: int`
- `barHeight: int`
- `spacing: int`
- `positiveColor: string`
- `negativeColor: string`
- `textColor: string`
- `count: int`
- `maxCount: int`
- `percent: int`
- `percentCritical: int`
- `percentMax: int`
- `percentCoeff: real`
- `value: real`
- `valueName: string`
- `hasPercentText: bool`
- `hasMouseArea: bool`

**Сигналы**

- `clicked()`

**Функции**

- `setCount()`
- `roundPercent(percent_)`

<a id="b-imtcontrols-ItemDelegate"></a>
#### `ItemDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/ItemDelegate.qml](imtcontrols/Views/ItemDelegate.qml)
- **Базовый тип:** [`Button`](#b-imtcontrols-Button)
- **Наследники:** [`PopupMenuDelegate`](#b-imtcontrols-PopupMenuDelegate)

**Свойства**

- `highlighted: bool`

<a id="b-imtcontrols-ItemDelegateDecorator"></a>
#### `ItemDelegateDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Views/ItemDelegateDecorator.qml](imtcontrols/Views/ItemDelegateDecorator.qml)
- **Базовый тип:** [`ButtonDecorator`](#b-imtcontrols-ButtonDecorator)
- **Наследники:** [`PopupItemDelegateDecorator`](#b-imtcontrols-PopupItemDelegateDecorator)

<a id="b-imtcontrols-Loading"></a>
#### `Loading`

Визуальный компонент.

- **Файл:** [Views/Loading.qml](imtcontrols/Views/Loading.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `indicatorSize: int`
- `accentColor: color`
- `background: alias`
- `animPhase: int`

**Функции**

- `start()`
- `stop()`

<a id="b-imtcontrols-PaintedCorner"></a>
#### `PaintedCorner`

Визуальный компонент.

- **Файл:** [Views/PaintedCorner.qml](imtcontrols/Views/PaintedCorner.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `r: real`
- `mainColor: string`
- `lineColor: string`
- `lineWidth: real`

<a id="b-imtcontrols-Piechart"></a>
#### `Piechart`

Визуальный компонент.

- **Файл:** [Views/Piechart.qml](imtcontrols/Views/Piechart.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `segments: var`
- `ring: bool`
- `ringThickness: real`
- `clockwise: bool`
- `rotationAngle: real`
- `showLegend: bool`
- `showPercent: bool`
- `legendClickable: bool`

**Сигналы**

- `chartUpdated()`
- `legendClicked(string id, string label, string color, int value)`

**Функции**

- `getTotalValue()`
- `getPercentText(value)`
- `updateGui()`

<a id="b-imtcontrols-PopupDetailedDelegateDecorator"></a>
#### `PopupDetailedDelegateDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Views/PopupDetailedDelegateDecorator.qml](imtcontrols/Views/PopupDetailedDelegateDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

<a id="b-imtcontrols-ProgressBar"></a>
#### `ProgressBar`

Панель/строка.

- **Файл:** [Views/ProgressBar.qml](imtcontrols/Views/ProgressBar.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `barHeight: int`
- `hasText: bool`
- `hasTitle: bool`
- `title: string`
- `text: string`
- `percent: int`
- `from: real`
- `to: real`
- `value: real`
- `position: real`
- `indeterminate: bool`
- `barIsGradient: bool`
- `barRadius: int`
- `borderColor: string`
- `backgroundColor: string`
- `gradient: Gradient`
- `fontSize: int`

<a id="b-imtcontrols-ProgressRoundIndicator"></a>
#### `ProgressRoundIndicator`

Индикатор.

- **Файл:** [Views/ProgressRoundIndicator.qml](imtcontrols/Views/ProgressRoundIndicator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `percent: real`
- `lineWidth: int`
- `mainColor: string`
- `hasText: bool`
- `hasIcon: bool`
- `imageSource: string`
- `indeterminate: bool`
- `indetMode: string`

<a id="b-imtcontrols-ReorderListQmlManager"></a>
#### `ReorderListQmlManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Views/ReorderListQmlManager.qml](imtcontrols/Views/ReorderListQmlManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `reorderModel: BaseModel`
- `list: Item`
- `fromIndex: int`
- `toIndex: int`
- `selectedId: string`
- `enteredInfo: var`

**Сигналы**

- `reloadModel()`

**Функции**

- `clearSelection()`
- `moveItemDown(index)`
- `moveItemUp(index)`
- `moveItem(fromIndex, toIndex)`
- `initEnteredInfo()`

<a id="b-imtcontrols-ReorderableListDelegate"></a>
#### `ReorderableListDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/ReorderableListDelegate.qml](imtcontrols/Views/ReorderableListDelegate.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `roleIdValue: var`
- `list: Item`
- `reorderManager: var`
- `dragTarget: Item`
- `zTarget: Item`
- `dragArea: alias`
- `contentContainer: alias`
- `contentComp: Component`
- `isEntered: bool`
- `hasButtons: bool`
- `defaultBorderColor: string`
- `selectionBorderColor: string`
- `idRole: string`
- `sdlVarPrefix: string`

**Сигналы**

- `isEnteredUpdate()`
- `pressed()`
- `released()`

<a id="b-imtcontrols-ScrollIndicator"></a>
#### `ScrollIndicator`

Индикатор.

- **Файл:** [Views/ScrollIndicator.qml](imtcontrols/Views/ScrollIndicator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `targetItem: var`
- `vertical: bool`
- `minSize: real`
- `secondSize: real`
- `indicatorRadius: int`
- `backgroundColor: string`
- `indicatorColor: string`
- `highlightColor: string`
- `canFade: bool`
- `isMoving: bool`
- `pauseDuration: int`
- `indicatorMargin: int`
- `notUsed: bool`
- `visibleState: bool`
- `targetContentHeight: int`
- `targetContentWidth: int`

**Функции**

- `setVisibleFunc()`

<a id="b-imtcontrols-SimpleTableDelegate"></a>
#### `SimpleTableDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/SimpleTableDelegate.qml](imtcontrols/Views/SimpleTableDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtcontrols-Spinner"></a>
#### `Spinner`

Визуальный компонент.

- **Файл:** [Views/Spinner.qml](imtcontrols/Views/Spinner.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `itemHeight: int`
- `currentValue: int`
- `model: alias`

**Функции**

- `setValue(value)`
- `gotoIndex(inIndex)`
- `next()`
- `prev()`

<a id="b-imtcontrols-SplitView"></a>
#### `SplitView`

Визуальное представление.

- **Файл:** [Views/SplitView.qml](imtcontrols/Views/SplitView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `orientation: int`
- `compl: bool`
- `ready: bool`
- `hasAnimation: bool`
- `animationDuration: int`
- `parentWidth: real`
- `parentHeight: real`
- `prevWidth: real`
- `prevHeight: real`
- `sizeModel: TreeItemModel`

**Функции**

- `fillSizeModelDefault(count_)`
- `chechModel()`
- `correctSize()`
- `setElementsSizeFromModel()`
- `onModelDataChanged(topLeft, bottomRight, keyRoles)`

<a id="b-imtcontrols-Splitter"></a>
#### `Splitter`

Визуальный компонент.

- **Файл:** [Views/Splitter.qml](imtcontrols/Views/Splitter.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `type: int`
- `isPressed: bool`
- `leftLimit: int`
- `rightLimit: int`
- `leftLimitMargin: real`
- `rightLimitMargin: real`
- `isClickMode: bool`

**Сигналы**

- `clicked()`

<a id="b-imtcontrols-StackView"></a>
#### `StackView`

Визуальное представление.

- **Файл:** [Views/StackView.qml](imtcontrols/Views/StackView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `currentIndex: int`
- `count: alias`
- `pagesModel: ListModel`
- `adaptSizeToCurrentPage: bool`
- `defaultCurrentIndex: int`

**Сигналы**

- `pageAdded(int index, var item)`
- `pageRemoved(int index, var item)`
- `currentPageChanged(var item)`

**Функции**

- `addPage(comp)`
- `removePage(index)`
- `clear()`
- `setCurrentIndex(index)`
- `currentPage()`
- `next()`
- `previous()`
- `getComponent(index)`

<a id="b-imtcontrols-StackViewHeader"></a>
#### `StackViewHeader`

Визуальный компонент.

- **Файл:** [Views/StackViewHeader.qml](imtcontrols/Views/StackViewHeader.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `separation: string`
- `initialItemTitleVisible: bool`

**Сигналы**

- `closeClicked()`
- `headerItemClicked(int index, string headerId)`

**Функции**

- `addHeader(id, name)`
- `getIndexById(id)`
- `popHeader()`
- `removeHeader(id)`
- `setHeaderName(id, name)`
- `getHeaderName(id)`

<a id="b-imtcontrols-StackedBarChart"></a>
#### `StackedBarChart`

Диаграмма.

- **Файл:** [Views/StackedBarChart.qml](imtcontrols/Views/StackedBarChart.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `bars: var`
- `axisColor: color`
- `gridColor: color`
- `textColor: color`
- `barWidth: real`
- `showValues: bool`
- `showLegend: bool`
- `legendHorizontal: bool`
- `ySteps: int`
- `xLabel: string`
- `yLabel: string`
- `legendItems: var`
- `segmentsRects: var`
- `legendClickable: bool`

**Сигналы**

- `legendClicked(string id, string label, string color, int value)`

**Функции**

- `buildLegend()`

<a id="b-imtcontrols-SubMenuPanelButtonDecorator"></a>
#### `SubMenuPanelButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Views/SubMenuPanelButtonDecorator.qml](imtcontrols/Views/SubMenuPanelButtonDecorator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `imageSource: string`
- `visibleMarker: bool`
- `highlighted: bool`
- `selected: bool`
- `text: string`

**Сигналы**

- `accepted(string text)`
- `clicked()`

<a id="b-imtcontrols-SwipeView"></a>
#### `SwipeView`

Визуальное представление.

- **Файл:** [Views/SwipeView.qml](imtcontrols/Views/SwipeView.qml)
- **Базовый тип:** [`Flickable`](https://doc.qt.io/qt-6/qml-qtquick-flickable.html)

**Свойства**

- `orientation: int`
- `count: int`
- `currentIndex: int`
- `limit: int`

**Функции**

- `correctPosition()`
- `setCoordAnimFunc(from_, to_)`

<a id="b-imtcontrols-SwitchCustom"></a>
#### `SwitchCustom`

Вспомогательный компонент.

- **Файл:** [Views/SwitchCustom.qml](imtcontrols/Views/SwitchCustom.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: string`
- `backgroundWidth: int`
- `backgroundHeight: int`
- `backgroundColor: string`
- `controlColor: string`
- `backgroundColorChecked: string`
- `controlColorChecked: string`
- `backgroundOpacity: real`
- `checked: bool`
- `isLeftText: bool`
- `fontPixelSize: int`
- `fontBold: bool`
- `mainMargin: int`
- `readOnly: bool`
- `tooltipText: alias`
- `tooltipItem: alias`
- `controlWidth: real` — только чтение
- `controlRecX: real`

**Функции**

- `setChecked(checkedVal, animate)`

<a id="b-imtcontrols-Table"></a>
#### `Table`

Таблица.

- **Файл:** [Views/Table.qml](imtcontrols/Views/Table.qml)
- **Базовый тип:** [`TableBase`](#b-imtcontrols-TableBase)
- **Наследники:** [`TableTreeView`](#b-imtcontrols-TableTreeView)

<a id="b-imtcontrols-TableBase"></a>
#### `TableBase`

Базовый компонент (для наследования).

- **Файл:** [Views/TableBase.qml](imtcontrols/Views/TableBase.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`Table`](#b-imtcontrols-Table)

**Свойства**

- `selectedIndex: int`
- `itemHeight: int`
- `headerHeight: int`
- `headerMinHeight: int`
- `headerGap: int`
- `hasFilter: bool`
- `hasSort: bool`
- `checkable: bool`
- `selectable: bool`
- `isMultiCheckable: bool`
- `canSelectAll: bool`
- `scrollbarVisible: bool`
- `showHeaders: bool`
- `enableAlternating: bool`
- `alternatingColor: color`
- `alternatingCellColor: color`
- `alternatingOpacity: real`
- `hoverEnabled: bool`
- `separatorVisible: alias`
- `sortIndicatorIcon: string`
- `tableViewParams: TableViewParams`
- `headers: var`
- `defaultHeadersModel: var`
- `tableDecorator: TreeItemModel`
- `headerDecorator: TreeItemModel`
- `cellDecorator: TreeItemModel`
- `widthDecorator: TreeItemModel`
- `widthDecoratorDynamic: TreeItemModel`
- `scrollbarItem: alias`
- `scrollbarItemHoriz: alias`
- `rowDelegate: alias`
- `elements: alias`
- `elementsSpacing: alias`
- `elementsList: alias`
- `cacheBuffer: alias`
- `contentHeight: alias`
- `contentWidth: real`
- `originX: alias`
- `originY: alias`
- `contentX: alias`
- `contentY: alias`
- `elementsListWidth: alias`
- `elementsCount: alias`
- `reuseItems: alias`
- `headerDelegate: alias`
- `headerElementWidth: real`
- `headerElementHeight: alias`
- `backgroundElementsColor: alias`
- `backgroundHeadersColor: alias`
- `emptyDecorCell: bool`
- `columnCount: int`
- `borderColorHorizontal: string`
- `borderColorVertical: string`
- `horizontalBorderSize: int`
- `verticalBorderSize: int`
- `verticalBorderHeight: int`
- `visibleLeftBorderFirst: bool`
- `visibleRightBorderLast: bool`
- `visibleTopBorderFirst: bool`
- `visibleBottomBorderLast: bool`
- `canSetBorderParams: bool`
- `wrapMode: int`
- `elideMode: int`
- `isRightBorder: bool`
- `isAllItemChecked: bool`
- `readOnly: bool`
- `editableHeaderParams: bool`
- `canFitHeight: bool`
- `canMoveColumns: bool`
- `canSwapColumns: bool`
- `isFrameScrolling: bool`
- `minCellWidth: int`
- `currentHeaderId: string`
- `currentSortOrder: string`
- `nonSortableColumns: var`
- `isMultiSelect: bool`
- `selectionManager: IndexSelectionManager`
- `properties: TableProperties`
- `borderColorHorizontal_deleg: string`
- `borderColorVertical_deleg: string`
- `horizontalBorderSize_deleg: int`
- `verticalBorderSize_deleg: int`
- `visibleLeftBorderFirst_deleg: bool`
- `visibleRightBorderLast_deleg: bool`
- `visibleTopBorderFirst_deleg: bool`
- `visibleBottomBorderLast_deleg: bool`
- `canSetBorderParams_deleg: bool`
- `wrapMode_deleg: int`
- `elideMode_deleg: int`
- `isRightBorder_deleg: bool`
- `textMarginHor_deleg: int`
- `textMarginVer_deleg: int`
- `maxLengthText: string`
- `textMarginHor: int`
- `textMarginVer: int`
- `emptyDecor: bool`
- `emptyDecorHeader: bool`
- `columnContentComps: var`
- `registeredDrawCellFunctions: var`
- `compl: bool`
- `isFlickable: bool`
- `defaultColumnWidth: int`
- `cellDelegate: Component`
- `scrollbarRightMargin: int`

**Сигналы**

- `checkedItemsChanged()`
- `selectionChanged(var selection)`
- `clicked(int index)`
- `rightButtonMouseClicked(int mouseX, int mouseY)`
- `doubleClicked(string id, int index)`
- `setActiveFocusFromTable()`
- `headerClicked(string headerId)`
- `headerRightMouseClicked(string headerId)`
- `textFilterChanged(string id, int index, string text)`
- `filterClicked()`
- `saveWidth()`
- `sortingChanged(string headerId, string sortOrder)`
- `widthRecalc()`
- `heightRecalc()`
- `tableViewParamsAccepted()`
- `modelRefresh(int rowIndex, string modelRole)`

**Функции**

- `setSortingInfo(headerId, sortOrder)`
- `clearSortingInfo(beQuiet)`
- `setDecorators()`
- `getSelectedIndexes()`
- `select(index)`
- `resetSelection(beQuiet)`
- `checkItem(index)`
- `uncheckItem(index)`
- `itemIsChecked(index)`
- `uncheckAll()`
- `checkAll()`
- `isAllChecked()`
- `getCheckedItems()`
- `setIsEnabledItem(index, isEnabled)`
- `allEnabled()`
- `setVisibleItem(index, isVisible)`
- `allVisible()`
- `getHeaderIndex(headerId)`
- `getHeaderId(index)`
- `registerFunctionDrawCellDelegate(columnId, func)`
- `setColumnContentComponent(columnIndex, comp)`
- `setColumnContentById(headerId, comp)`
- `setCellHeight()`
- `setWidth()`
- `setBorderParams()`
- `setContentHeight(content_height)`
- `getContentHeight()`
- `resetViewParams()`
- `updateWidthFromViewParams()`
- `fitToWidth()`
- `fitToWidthOptional()`

<a id="b-imtcontrols-TableCellDateDelegate"></a>
#### `TableCellDateDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellDateDelegate.qml](imtcontrols/Views/TableCellDateDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `cellText: alias`

<a id="b-imtcontrols-TableCellDelegate"></a>
#### `TableCellDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellDelegate.qml](imtcontrols/Views/TableCellDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `pTableDelegateContainer: Item`
- `mainMouseArea: Item`
- `columnCount: int`
- `compl: bool`
- `complCompl: bool`
- `contentComp: alias`
- `textLeftIndent: real`
- `textRightIndent: real`
- `columnIndex: int`
- `rowIndex: int`
- `parent_temp: Item`
- `ready: bool`
- `cellId: string`
- `defaultContentComp: Component`

**Функции**

- `setCellWidth()`
- `getValue()`
- `setValue(value)`

<a id="b-imtcontrols-TableCellDelegateBase"></a>
#### `TableCellDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TableCellDelegateBase.qml](imtcontrols/Views/TableCellDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`ComboBoxCellContentComp`](#b-imtcontrols-ComboBoxCellContentComp), [`TableCellDateDelegate`](#b-imtcontrols-TableCellDateDelegate), [`TableCellDelegateCheckable`](#b-imtcontrols-TableCellDelegateCheckable), [`TableCellDelegateSimple`](#b-imtcontrols-TableCellDelegateSimple), [`TableCellIconTextDelegate`](#b-imtcontrols-TableCellIconTextDelegate), [`TableCellRowNumberDelegate`](#b-imtcontrols-TableCellRowNumberDelegate), [`TableCellTextDelegate`](#b-imtcontrols-TableCellTextDelegate), [`TableCellTextWithTooltipDelegate`](#b-imtcontrols-TableCellTextWithTooltipDelegate), [`TextInputCellContentComp`](#b-imtcontrols-TextInputCellContentComp), [`TextLinkCellDelegate`](#b-imtcontrols-TextLinkCellDelegate)

**Свойства**

- `rowDelegate: TableRowDelegateBase`
- `table: var`
- `columnCount: int`
- `rowCount: int`
- `textLeftIndent: real`
- `textRightIndent: real`
- `columnIndex: int`
- `rowIndex: int`
- `ready: bool`
- `emptyDecorCell: bool`
- `cellHeaderId: string`
- `sdlVarPrefix: string`
- `widthIsCalculated: bool`
- `compl: bool`
- `backgroundItem: Item`
- `leftBorderItem: Item`
- `rightBorderItem: Item`
- `topBorderItem: Item`
- `bottomBorderItem: Item`
- `ok: bool`

**Сигналы**

- `reused()`

**Функции**

- `refreshModelData(rowIndex_, modelRole_)`
- `getValue()`
- `setValue(value)`
- `setCellWidth()`

<a id="b-imtcontrols-TableCellDelegateCheckable"></a>
#### `TableCellDelegateCheckable`

Вспомогательный компонент.

- **Файл:** [Views/TableCellDelegateCheckable.qml](imtcontrols/Views/TableCellDelegateCheckable.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `tableItem: Item`

**Сигналы**

- `clicked()`

**Функции**

- `onRedraw()`

<a id="b-imtcontrols-TableCellDelegateSimple"></a>
#### `TableCellDelegateSimple` · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Views/TableCellDelegateSimple.qml](imtcontrols/Views/TableCellDelegateSimple.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

<a id="b-imtcontrols-TableCellIconTextDelegate"></a>
#### `TableCellIconTextDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellIconTextDelegate.qml](imtcontrols/Views/TableCellIconTextDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `icon: alias`
- `text: alias`

<a id="b-imtcontrols-TableCellRowNumberDelegate"></a>
#### `TableCellRowNumberDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellRowNumberDelegate.qml](imtcontrols/Views/TableCellRowNumberDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `text: alias`

**Сигналы**

- `clicked()`

<a id="b-imtcontrols-TableCellText"></a>
#### `TableCellText`

Визуальный компонент.

- **Файл:** [Views/TableCellText.qml](imtcontrols/Views/TableCellText.qml)
- **Базовый тип:** [`Text`](https://doc.qt.io/qt-6/qml-qtquick-text.html)

**Свойства**

- `rowDelegate: TableRowDelegateBase`
- `columnIndex: int`

<a id="b-imtcontrols-TableCellTextDelegate"></a>
#### `TableCellTextDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellTextDelegate.qml](imtcontrols/Views/TableCellTextDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `cellText: alias`

<a id="b-imtcontrols-TableCellTextWithTooltipDelegate"></a>
#### `TableCellTextWithTooltipDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableCellTextWithTooltipDelegate.qml](imtcontrols/Views/TableCellTextWithTooltipDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Сигналы**

- `clicked()`

**Функции**

- `onRedraw()`

<a id="b-imtcontrols-TableDecorator"></a>
#### `TableDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Views/TableDecorator.qml](imtcontrols/Views/TableDecorator.qml)
- **Базовый тип:** [`TreeItemModel`](#b-imtcontrols-TreeItemModel)

**Свойства**

- `compl: bool`

**Функции**

- `setDefault(model, count)`

<a id="b-imtcontrols-TableDelegate"></a>
#### `TableDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableDelegate.qml](imtcontrols/Views/TableDelegate.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `textTopMargin: int`
- `count: int`
- `minHeight: real`
- `selected: bool`
- `checkedState: int`
- `tableItem: Item`
- `bodyArray: var`
- `dataModel: var`
- `rowIndex: int`
- `headers: TreeItemModel`
- `cellDecorator: TreeItemModel`
- `widthDecorator: TreeItemModel`
- `widthDecoratorDynamic: TreeItemModel`
- `mouseArea: alias`
- `emptyDecorCell: bool`
- `readOnly: bool`
- `enabled: bool`
- `selectedOpacity: real`
- `hoverOpacity: real`
- `borderColorHorizontal: string`
- `borderColorVertical: string`
- `horizontalBorderSize: int`
- `verticalBorderSize: int`
- `visibleLeftBorderFirst: bool`
- `visibleRightBorderLast: bool`
- `visibleTopBorderFirst: bool`
- `visibleBottomBorderLast: bool`
- `canSetBorderParams: bool`
- `wrapMode: int`
- `elideMode: int`
- `isRightBorder: bool`
- `maxLengthText: string`
- `textMarginHor: int`
- `textLeftMargin: int`
- `textRightMargin: int`
- `textMarginVer: int`
- `compl: bool`
- `selectedColor: string`
- `cellDelegate: Component`

**Сигналы**

- `clicked()`
- `rightButtonMouseClicked(int mX, int mY)`
- `doubleClicked(int mX, int mY)`
- `widthRecalc()`

**Функции**

- `getItemData()`
- `getSelectedId()`
- `getSelectedName()`
- `setBorderParams()`
- `setHeightModelElememt(index_,height_)`
- `setCellHeightModelDefault()`
- `setCellHeight()`

<a id="b-imtcontrols-TableHeaderDelegate"></a>
#### `TableHeaderDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableHeaderDelegate.qml](imtcontrols/Views/TableHeaderDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `tableItem: TableBase`
- `columnCount: int`
- `columnIndex: int`
- `headerId: string`
- `headerName: string`
- `compl: bool`
- `complCompl: bool`
- `textIsCropped: bool`
- `rightMA: var`

**Функции**

- `createRightMA()`
- `setCellWidth()`
- `getPrevHeaderIndex()`
- `getNextHeaderIndex()`
- `getLastHeaderIndex()`

<a id="b-imtcontrols-TableHeaderParamComp"></a>
#### `TableHeaderParamComp`

Вспомогательный компонент.

- **Файл:** [Views/TableHeaderParamComp.qml](imtcontrols/Views/TableHeaderParamComp.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `tableViewParamsCopied: TableViewParams`
- `tableItem: Item`

<a id="b-imtcontrols-TableHeaders"></a>
#### `TableHeaders`

Вспомогательный компонент.

- **Файл:** [Views/TableHeaders.qml](imtcontrols/Views/TableHeaders.qml)
- **Базовый тип:** [`BaseModel`](#b-imtcontrols-BaseModel)

**Свойства**

- `headerInfoComp: Component`

**Функции**

- `addHeader(id, name, filterable, sortable)`
- `removeHeader(id)`
- `getHeaderIndex(id)`
- `setHeaderName(id, name)`

<a id="b-imtcontrols-TablePainterRowDelegateBase"></a>
#### `TablePainterRowDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TablePainterRowDelegateBase.qml](imtcontrols/Views/TablePainterRowDelegateBase.qml)
- **Базовый тип:** [`TableRowDelegateBase`](#b-imtcontrols-TableRowDelegateBase)

<a id="b-imtcontrols-TableProperties"></a>
#### `TableProperties`

Набор свойств оформления.

- **Файл:** [Views/TableProperties.qml](imtcontrols/Views/TableProperties.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `selectedItems: var`
- `invisibleItems: var`
- `disableItems: var`

**Сигналы**

- `visibleItemsChanged()`
- `stateItemsChanged()`
- `checkedItemsChanged()`

**Функции**

- `addInvisibleItem(index)`
- `clearInvisibleItems()`
- `clearDisabledItems()`
- `itemIsEnabled(index)`
- `setIsEnabledItem(index, isEnabled)`
- `setIsVisibleItem(index, isVisible)`
- `itemIsVisible(index)`
- `addCheckedItem(index)`
- `addSingleCheckedItem(index)`
- `addCheckedItems(indexes)`
- `removeCheckedItem(index)`
- `clearCheckedItems()`
- `itemIsChecked(index)`

<a id="b-imtcontrols-TableRowCanvasViewer"></a>
#### `TableRowCanvasViewer`

Визуальный компонент.

- **Файл:** [Views/TableRowCanvasViewer.qml](imtcontrols/Views/TableRowCanvasViewer.qml)
- **Базовый тип:** [`Canvas`](https://doc.qt.io/qt-6/qml-qtquick-canvas.html)

**Свойства**

- `rowDelegate: TableRowDelegateBase`

**Функции**

- `trimTextToFit(ctx, text, maxWidth)`
- `drawCell(ctx, x, y, cellWidth, cellHeight, columnIndex)`
- `drawText(ctx, x, y, cellWidth, text)`
- `getCellValue(key)`

<a id="b-imtcontrols-TableRowDelegate"></a>
#### `TableRowDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TableRowDelegate.qml](imtcontrols/Views/TableRowDelegate.qml)
- **Базовый тип:** [`TableRowDelegateBase`](#b-imtcontrols-TableRowDelegateBase)

**Свойства**

- `tableRowViewer: alias`

<a id="b-imtcontrols-TableRowDelegateBase"></a>
#### `TableRowDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TableRowDelegateBase.qml](imtcontrols/Views/TableRowDelegateBase.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`TablePainterRowDelegateBase`](#b-imtcontrols-TablePainterRowDelegateBase), [`TableRowDelegate`](#b-imtcontrols-TableRowDelegate)

**Свойства**

- `minHeight: real`
- `contentHeight: real`
- `textTopMargin: int`
- `columnCount: int`
- `selected: bool`
- `checkedState: int`
- `tableItem: TableBase`
- `dataModel: var`
- `rowIndex: int`
- `cellDecorator: TreeItemModel`
- `mouseArea: alias`
- `readOnly: bool`
- `enabled: bool`
- `selectedOpacity: real`
- `hoverOpacity: real`
- `borderColorHorizontal: string`
- `borderColorVertical: string`
- `horizontalBorderSize: int`
- `verticalBorderSize: int`
- `visibleLeftBorderFirst: bool`
- `visibleRightBorderLast: bool`
- `visibleTopBorderFirst: bool`
- `visibleBottomBorderLast: bool`
- `canSetBorderParams: bool`
- `wrapMode: int`
- `elideMode: int`
- `isRightBorder: bool`
- `maxLengthText: string`
- `cellColor: string`
- `textMarginHor: int`
- `textLeftMargin: int`
- `textRightMargin: int`
- `textMarginVer: int`
- `compl: bool`
- `selectedColor: string`
- `hoverColor: string`
- `isHovered: bool` — только чтение
- `cellDelegate: Component`
- `cellDelegateDefault: Component`

**Сигналы**

- `clicked()`
- `rightButtonMouseClicked(int mX, int mY)`
- `doubleClicked(int mX, int mY)`
- `widthRecalc()`
- `reused()`

**Функции**

- `rowBackgroundColor()`
- `onSelectionChanged(selection)`
- `onElementsChanged()`
- `getItemData()`
- `setBorderParams()`
- `setHeightModelElememt(index_,height_)`
- `setCellHeightModelDefault()`
- `setCellHeight()`
- `checkedItemsChanged()`
- `visibleItemsChanged()`
- `enabledItemsChanged()`

<a id="b-imtcontrols-TableRowViewer"></a>
#### `TableRowViewer`

Визуальный компонент.

- **Файл:** [Views/TableRowViewer.qml](imtcontrols/Views/TableRowViewer.qml)
- **Базовый тип:** [`Row`](https://doc.qt.io/qt-6/qml-qtquick-row.html)

**Свойства**

- `rowDelegate: TableRowDelegateBase`
- `compl: bool`
- `model: alias`
- `loadDefaultCellDelegate: bool`

<a id="b-imtcontrols-TableSelection"></a>
#### `TableSelection`

Вспомогательный компонент.

- **Файл:** [Views/TableSelection.qml](imtcontrols/Views/TableSelection.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `selectedIndexes: var`
- `lastSelectedIndex: int`
- `firstSelectedIndex: int`
- `focusedIndex: int`

**Сигналы**

- `selectionChanged()`

**Функции**

- `resetSelection()`
- `toggleSelect(index)`
- `singleSelect(index)`
- `rangeSelect(fromIndex, toIndex)`
- `isSelected(index)`

<a id="b-imtcontrols-TableTreeDecorator"></a>
#### `TableTreeDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Views/TableTreeDecorator.qml](imtcontrols/Views/TableTreeDecorator.qml)
- **Базовый тип:** [`TreeItemModel`](#b-imtcontrols-TreeItemModel)

**Свойства**

- `compl: bool`

**Функции**

- `setDefault(model, count)`

<a id="b-imtcontrols-TableTreeView"></a>
#### `TableTreeView`

Древовидное представление.

- **Файл:** [Views/TableTreeView.qml](imtcontrols/Views/TableTreeView.qml)
- **Базовый тип:** [`Table`](#b-imtcontrols-Table)

**Свойства**

- `model: TreeItemModel`
- `shift: int`
- `nameId: string`
- `delegateHeight: int`
- `delegateWidth: int`
- `delegateWidthFull: int`
- `maxLevel: int`
- `hasSelection: bool`

**Сигналы**

- `requestSignal(int index)`
- `openButtonClicked(int index)`
- `openBranch(int index)`
- `closeBranch(int index)`
- `forcedOpen(int index)`
- `inserted(int index)`

**Функции**

- `insertTree(index, model_)`
- `deleteBranch(index)`
- `setVisibleElements(visible, index)`
- `getVisibleCountInBranch(index)`
- `setContentWidth()`
- `openFunc(index)`
- `moveToElement(index)`
- `getIcon(type, isOpen)`
- `getData(key,index)`
- `getSelectedIndex()`
- `checkIsOpen(index)`
- `checkHasChildren(index)`
- `findParentIndex(index)`
- `findIndexById(id)`

<a id="b-imtcontrols-TableViewItemDelegateBase"></a>
#### `TableViewItemDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TableViewItemDelegateBase.qml](imtcontrols/Views/TableViewItemDelegateBase.qml)
- **Базовый тип:** [`FocusScope`](https://doc.qt.io/qt-6/qml-qtquick-focusscope.html)
- **Наследники:** [`TreeViewItemDelegateBase`](#b-imtcontrols-TreeViewItemDelegateBase)

**Свойства**

- `root: Item`
- `dataModel: var`
- `mouseArea: alias`
- `modelIndex: ModelIndex`
- `level: int`
- `itemData: var`
- `headerDelegate: alias`
- `headerItem: alias`
- `footerDelegate: alias`
- `footerItem: alias`
- `prefixRowDelegate: alias`
- `prefixRowItem: alias`
- `suffixRowDelegate: alias`
- `suffixRowItem: alias`
- `rowBodyDelegate: alias`
- `rowBodyItem: alias`
- `highlightDelegate: alias`
- `highlightItem: alias`
- `prefixRowLoader: alias`
- `selected: bool`
- `isActive: bool`
- `isVisible: bool`
- `isCheckable: bool`
- `checkState: int`

**Сигналы**

- `clicked()`

**Функции**

- `selectionChanged()`
- `getItemData()`
- `select()`

<a id="b-imtcontrols-TableViewParams"></a>
#### `TableViewParams`

Параметры фигуры.

- **Файл:** [Views/TableViewParams.qml](imtcontrols/Views/TableViewParams.qml)
- **Базовый тип:** [`TreeItemModel`](#b-imtcontrols-TreeItemModel)

**Свойства**

- `internal: QtObject`

**Функции**

- `getHeaderIds()`
- `getHeaderIndex(headerId)`
- `setHeaderVisible(headerId, visible)`
- `setHeaderOrder(headerId, order)`
- `setHeaderSize(headerId, size)`
- `getHeaderSize(headerId)`
- `getHeaderOrder(headerId)`
- `headerIsVisible(headerId)`
- `swapHeaders(index1, index2)`
- `headerIsExists(headerId)`
- `copyFromTreeModel(model)`
- `copyFrom(obj)`

<a id="b-imtcontrols-TextInputCellContentComp"></a>
#### `TextInputCellContentComp`

Вспомогательный компонент.

- **Файл:** [Views/TextInputCellContentComp.qml](imtcontrols/Views/TextInputCellContentComp.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Сигналы**

- `editingFinished(string text)`

<a id="b-imtcontrols-TextLinkCellDelegate"></a>
#### `TextLinkCellDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TextLinkCellDelegate.qml](imtcontrols/Views/TextLinkCellDelegate.qml)
- **Базовый тип:** [`TableCellDelegateBase`](#b-imtcontrols-TableCellDelegateBase)

**Свойства**

- `text: string`
- `imageSource: string`

**Сигналы**

- `linkActivated()`

<a id="b-imtcontrols-Title"></a>
#### `Title`

Визуальный компонент.

- **Файл:** [Views/Title.qml](imtcontrols/Views/Title.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `text: string`
- `icon: alias`
- `font: alias`
- `textMargin: int`

<a id="b-imtcontrols-TooltipArea"></a>
#### `TooltipArea`

Визуальный компонент.

- **Файл:** [Views/TooltipArea.qml](imtcontrols/Views/TooltipArea.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `text: alias`
- `mouseArea: MouseArea`
- `tooltip: alias`
- `containsMouse: bool`
- `interval: alias`

<a id="b-imtcontrols-TreeItemDelegate"></a>
#### `TreeItemDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Views/TreeItemDelegate.qml](imtcontrols/Views/TreeItemDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `itemHeight: int`
- `childModel: TreeItemModel`
- `rootItem: Item`
- `state: int`
- `itemId: string`
- `itemVisible: bool`
- `itemActive: bool`
- `itemData: var`
- `itemDelegate: alias`
- `childrenCount: int`
- `childRepeater: alias`

**Сигналы**

- `doubleClicked()`
- `clicked(var itemData, int index)`

<a id="b-imtcontrols-TreeViewColumn"></a>
#### `TreeViewColumn`

Вспомогательный компонент.

- **Файл:** [Views/TreeViewColumn.qml](imtcontrols/Views/TreeViewColumn.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `isTreeViewColumn: bool` — только чтение
- `columnId: string`
- `name: string`
- `width: int`
- `minWidth: int`
- `source: string`
- `type: string`
- `tree: bool`
- `editable: bool`
- `options: var`
- `horizontalAlignment: int`

<a id="b-imtcontrols-TreeViewCommandsDelegateBase"></a>
#### `TreeViewCommandsDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TreeViewCommandsDelegateBase.qml](imtcontrols/Views/TreeViewCommandsDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `treeViewModel: TreeItemModel`
- `selectedCount: int`

**Сигналы**

- `commandActivated(string commandId)`

**Функции**

- `insertNewItem(model)`
- `getSelectedItem(model)`
- `getSelectedItemIndex(model)`
- `commandHandle(commandId)`

<a id="b-imtcontrols-TreeViewGql"></a>
#### `TreeViewGql`

Визуальный компонент.

- **Файл:** [Views/TreeViewGql.qml](imtcontrols/Views/TreeViewGql.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `shift: int`
- `delegateWidth: int`
- `delegateHeight: int`
- `selectedIndex: int`
- `scrollSize: int`
- `nameId: string`
- `scrollBackgroundColor: string`
- `scrollIndicatorColor: string`
- `selectionColor: string`
- `hoverColor: string`
- `selectedTextColor: string`
- `textColor: string`
- `selectionRadius: real`
- `selectionOpacity: real`
- `hasAddDelegInfo: bool`
- `hasSelection: bool`
- `model: TreeItemModel`
- `additionalDelegateComp: Component`
- `delegate: alias`
- `reuseItems: alias`

**Сигналы**

- `requestSignal(int index)`
- `clicked(int index)`
- `doubleClicked(int index)`
- `rightButtonMouseClicked(int mX, int mY)`
- `openButtonClicked(int index)`
- `selectionChanged()`
- `openBranch(int index)`
- `closeBranch(int index)`
- `forcedOpen(int index)`
- `inserted(int index)`

**Функции**

- `getModelItemsCount(model_)`
- `copyModelItemData(index, externModel_, externIndex)`
- `insertTree(index, model_)`
- `deleteBranch(index)`
- `setVisibleElements(visible, index)`
- `getVisibleCountInBranch(index)`
- `setContentWidth()`
- `openFunc(index)`
- `moveToElement(index)`
- `getIcon(type, isOpen)`
- `getData(key,index)`
- `getSelectedIndex()`
- `checkIsOpen(index)`
- `checkHasChildren(index)`
- `findParentIndex(index)`
- `forceRefresh()`
- `findIndexById(id, nameId, startIndex)`

<a id="b-imtcontrols-TreeViewItemDelegateBase"></a>
#### `TreeViewItemDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Views/TreeViewItemDelegateBase.qml](imtcontrols/Views/TreeViewItemDelegateBase.qml)
- **Базовый тип:** [`TableViewItemDelegateBase`](#b-imtcontrols-TableViewItemDelegateBase)

**Свойства**

- `isOpened: bool`
- `hasChild: bool`
- `tristate: bool`
- `rootDelegate: var`
- `parentDelegate: var`
- `childrenDelegates: var`
- `childModelKey: string`

**Сигналы**

- `parentCheckStateChanged(var delegate)`
- `childrenCheckStateChanged(var delegate)`

<a id="b-imtcontrols-TreeViewSelection"></a>
#### `TreeViewSelection`

Вспомогательный компонент.

- **Файл:** [Views/TreeViewSelection.qml](imtcontrols/Views/TreeViewSelection.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `count: int`
- `items: var`

**Сигналы**

- `selectionChanged()`

**Функции**

- `resetSelection()`
- `singleSelect(item)`
- `isSelected(item)`
- `select(item)`
- `deselect(item)`
- `contains(item)`
- `updateSelection()`

<a id="b-imtcontrols-ValueLevelIndicator"></a>
#### `ValueLevelIndicator`

Индикатор.

- **Файл:** [Views/ValueLevelIndicator.qml](imtcontrols/Views/ValueLevelIndicator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `count: int`
- `barWidth: int`
- `spacing: int`
- `percent: int`
- `colorActive: string`
- `colorDefault: string`
- `constantHeight: bool`
- `continuous: bool`
- `constantColor: string`

**Функции**

- `roundPercent(percent_)`

### Popup/Internal/ — Popup/Internal

<a id="b-imtcontrols-FocusCoordinator"></a>
#### `FocusCoordinator` · **singleton** · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Popup/Internal/FocusCoordinator.qml](imtcontrols/Popup/Internal/FocusCoordinator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `push(rootItem, focusOwner)`
- `pop(rootItem, fallback)`
- `discard(rootItem)`
- `isAncestor(rootItem, item)`

<a id="b-imtcontrols-InputCoordinator"></a>
#### `InputCoordinator` · **singleton** · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Popup/Internal/InputCoordinator.qml](imtcontrols/Popup/Internal/InputCoordinator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `noAutoClose: int` — только чтение
- `closeOnPressOutside: int` — только чтение
- `closeOnPressOutsideParent: int` — только чтение
- `closeOnReleaseOutside: int` — только чтение
- `closeOnReleaseOutsideParent: int` — только чтение
- `closeOnEscape: int` — только чтение
- `defaultPolicy: int` — только чтение

**Функции**

- `shouldClosePress(popup, mouse, source)`
- `shouldCloseRelease(popup, mouse, source)`

<a id="b-imtcontrols-OverlayManager"></a>
#### `OverlayManager` · **singleton** · _не экспортируется (внутренний/по пути)_

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Popup/Internal/OverlayManager.qml](imtcontrols/Popup/Internal/OverlayManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `overlayFor(rootItem)`
- `attach(popup, rootItem)`
- `detach(popup)`
- `raise(popup)`

<a id="b-imtcontrols-PopupRegistry"></a>
#### `PopupRegistry` · **singleton** · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Popup/Internal/PopupRegistry.qml](imtcontrols/Popup/Internal/PopupRegistry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `popups: var`
- `generation: int`

**Сигналы**

- `windowGeometryChanged()`
- `localeChanged()`

**Функции**

- `register(popup)`
- `unregister(popup)`
- `prune()`
- `notifyWindowGeometryChanged()`
- `notifyLocaleChanged()`
- `topOpenPopup(window)`
- `hasModal(window)`

<a id="b-imtcontrols-PopupStackController"></a>
#### `PopupStackController` · **singleton** · _не экспортируется (внутренний/по пути)_

Контроллер: невизуальная логика и координация.

- **Файл:** [Popup/Internal/PopupStackController.qml](imtcontrols/Popup/Internal/PopupStackController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `attach(parent, child)`
- `detach(child)`
- `children(popup)`
- `closeDescendants(popup)`
- `closeAbove(popup)`
- `isDescendant(popup, candidate)`

<a id="b-imtcontrols-PopupTransition"></a>
#### `PopupTransition` · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Popup/Internal/PopupTransition.qml](imtcontrols/Popup/Internal/PopupTransition.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `enter: Component`
- `exit: Component`

<a id="b-imtcontrols-PositioningEngine"></a>
#### `PositioningEngine` · **singleton** · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Popup/Internal/PositioningEngine.qml](imtcontrols/Popup/Internal/PositioningEngine.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `position(popup)`

### Menu/Internal/ — Menu/Internal

<a id="b-imtcontrols-MenuItemDelegate"></a>
#### `MenuItemDelegate` · _не экспортируется (внутренний/по пути)_

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Menu/Internal/MenuItemDelegate.qml](imtcontrols/Menu/Internal/MenuItemDelegate.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `menuItem: var`
- `menu: var`
- `rowIndex: int`
- `current: bool`
- `hovered: bool`
- `isSeparator: bool`

### Base/web/ — Base/web

<a id="b-imtcontrols-RoundButton"></a>
#### `RoundButton` · _не экспортируется (внутренний/по пути)_

Кнопка.

- **Файл:** [Base/web/RoundButton.qml](imtcontrols/Base/web/RoundButton.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtcontrols-ScrollBar"></a>
#### `ScrollBar` · _не экспортируется (внутренний/по пути)_

Панель/строка.

- **Файл:** [Base/web/ScrollBar.qml](imtcontrols/Base/web/ScrollBar.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtcontrols-Switch"></a>
#### `Switch` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Base/web/Switch.qml](imtcontrols/Base/web/Switch.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtcontrols-ToolTip"></a>
#### `ToolTip` · _не экспортируется (внутренний/по пути)_

Визуальный компонент.

- **Файл:** [Base/web/ToolTip.qml](imtcontrols/Base/web/ToolTip.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtcontrols-Window"></a>
#### `Window` · _не экспортируется (внутренний/по пути)_

Окно.

- **Файл:** [Base/web/Window.qml](imtcontrols/Base/web/Window.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`MainWindow`](#b-imtcontrols-MainWindow)


---

<a id="imtstyle"></a>
## `imtstyle` — Стили и оформление

Модуль оформления, вынесенный из imtcontrols в отдельный модуль: базовые стили (StyleBase, StyleComponents) и наборы свойств шрифта/иконок (FontProperties, IconProperties). Оформление всегда живёт здесь, а не в подпапке Styles/ какого-либо модуля. Подключается строкой `import imtstyle 1.0`.

**Импорт:** `import imtstyle 1.0` · **Компонентов:** 4

### ./ — Компоненты модуля

<a id="b-imtstyle-FontProperties"></a>
#### `FontProperties`

Набор свойств оформления.

- **Файл:** [FontProperties.qml](imtstyle/FontProperties.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `family: string`
- `pixelSize: int`
- `bold: bool`
- `color: string`

<a id="b-imtstyle-IconProperties"></a>
#### `IconProperties`

Набор свойств оформления.

- **Файл:** [IconProperties.qml](imtstyle/IconProperties.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `source: string`
- `width: int`
- `height: int`

<a id="b-imtstyle-StyleBase"></a>
#### `StyleBase`

Базовый стиль (для наследования).

- **Файл:** [StyleBase.qml](imtstyle/StyleBase.qml)
- **Базовый тип:** [`StyleComponents`](#b-imtstyle-StyleComponents)

**Свойства**

- `styleId: string`
- `mobile: bool`
- `mainWidth: real`
- `mainHeight: real`
- `mainSize: real`
- `theme: string`
- `logoIconName: string`
- `language: string`
- `mainFontSource: string`
- `boldFontSource: string`
- `fontFamily: string`
- `fontFamilyBold: string`
- `mainFontLoader: alias`
- `boldFontLoader: alias`
- `startWindowWidth: int`
- `startWindowHeight: int`
- `fontSizeXHuge: int`
- `fontSizeHuge: int`
- `fontSizeXXXXLarge: int`
- `fontSizeXXXLarge: int`
- `fontSizeXXLarge: int`
- `fontSizeXLarge: int`
- `fontSizeLarge: int`
- `fontSizeNormal: int`
- `fontSizeSmall: int`
- `fontSizeXSmall: int`
- `fontSizeXXSmall: int`
- `fontSizeTiny: int`
- `buttonWidthLarge: int`
- `buttonWidthMedium: int`
- `buttonWidthSmall: int`
- `buttonWidthXSmall: int`
- `buttonWidthTiny: int`
- `buttonHeight: int`
- `iconSizeXLarge: int`
- `iconSizeLarge: int`
- `iconSizeMedium: int`
- `iconSizeSmall: int`
- `iconSizeXSmall: int`
- `itemSizeHuge: int`
- `itemSizeXLarge: int`
- `itemSizeLarge: int`
- `itemSizeMedium: int`
- `itemSizeSmall: int`
- `itemSizeXSmall: int`
- `paddingHuge: int`
- `paddingLarge: int`
- `paddingMedium: int`
- `paddingSmall: int`
- `sizeLargeMargin: int`
- `sizeMainMargin: int`
- `sizeSmallMargin: int`
- `fontSizeBXXXL: int`
- `fontSizeBXXL: int`
- `fontSizeBXL: int`
- `fontSizeXXXL: int`
- `fontSizeXXL: int`
- `fontSizeXL: int`
- `fontSizeL: int`
- `fontSizeM: int`
- `fontSizeS: int`
- `fontSizeXS: int`
- `fontSizeXXS: int`
- `fontSizeXXXS: int`
- `marginXXXL: int`
- `marginXXL: int`
- `marginXL: int`
- `marginL: int`
- `marginM: int`
- `marginS: int`
- `marginXS: int`
- `sizeHintXXXL: int`
- `sizeHintXXL: int`
- `sizeHintXL: int`
- `sizeHintL: int`
- `sizeHintM: int`
- `sizeHintS: int`
- `sizeHintXS: int`
- `sizeHintXXS: int`
- `sizeHintXXXS: int`
- `sizeHintBXS: int`
- `controlHeightL: int`
- `controlHeightM: int`
- `controlHeightS: int`
- `radiusXXL: int`
- `radiusXL: int`
- `radiusL: int`
- `radiusM: int`
- `radiusS: int`
- `radiusXS: int`
- `spacingXXXL: int`
- `spacingXXL: int`
- `spacingXL: int`
- `spacingL: int`
- `spacingM: int`
- `spacingS: int`
- `spacingXS: int`
- `spacingXXS: int`
- `iconSizeXXXL: int`
- `iconSizeXXL: int`
- `iconSizeXL: int`
- `iconSizeL: int`
- `iconSizeM: int`
- `iconSizeS: int`
- `iconSizeXS: int`
- `iconSizeXXS: int`
- `iconSizeXXXS: int`
- `buttonWidthXXXL: int`
- `buttonWidthXXL: int`
- `buttonWidthXL: int`
- `buttonWidthL: int`
- `buttonWidthM: int`
- `buttonWidthS: int`
- `buttonWidthXS: int`
- `buttonWidthXXS: int`
- `buttonWidthXXXS: int`
- `buttonHeightM: int`
- `buttonHeightS: int`
- `buttonHeightXS: int`
- `buttonHeightXXS: int`
- `buttonHeightXXXS: int`
- `buttonRadius: int`
- `buttonBorderWidth: int`
- `itemSizeXXL: int`
- `itemSizeXL: int`
- `itemSizeL: int`
- `itemSizeM: int`
- `itemSizeS: int`
- `itemSizeXS: int`
- `paddingXL: int`
- `paddingL: int`
- `paddingM: int`
- `paddingS: int`
- `menuRowHeight: int`
- `menuMinWidth: int`
- `tableRowHeight: int`
- `tableHeaderHeight: int`
- `gridCellSize: int`
- `treeBranchOffset: int`
- `mainWindowMargin: int`
- `pageMargin: int`
- `textFieldRadius: int`
- `comboBoxRadius: int`
- `menuPanelRadius: int`
- `sizePanelsHeight: int`
- `headerHeight: int`
- `menuPanelMinWidth: int`
- `opacityFaint: real`
- `opacityHigh: real`
- `opacityLow: real`
- `opacityOverlay: real`
- `selectedOpacity: real`
- `hoverOpacity: real`
- `highlightBackgroundOpacity: real`
- `enableAlternating: bool`
- `alternatingOpacity: real`
- `imagingToolsGradient0: string`
- `imagingToolsGradient1: string`
- `imagingToolsGradient2: string`
- `imagingToolsGradient3: string`
- `imagingToolsGradient4: string`
- `baseColor: string`
- `baseColorInverted: string`
- `mainColor: string`
- `firstColor: string`
- `secondColor: string`
- `firstColorHighlight: string`
- `colorMenuPanel: string`
- `errorColor: string`
- `errorTextColor: string`
- `imaginToolsAccentColor: string`
- `positiveAccentColor: string`
- `negativeAccentColor: string`
- `middleAccentColor: string`
- `textColor: string`
- `titleColor: string`
- `subtitleColor: string`
- `textSelectedColor: string`
- `inactiveTextColor: string`
- `placeHolderTextColor: string`
- `linkColor: string`
- `selectedLinkFromColor: string`
- `selectedLinkToColor: string`
- `popupErrorBackgroundColor: string`
- `popupWarningBackgroundColor: string`
- `popupSuccessBackgroundColor: string`
- `popupInfoBackgroundColor: string`
- `popupErrorAccentColor: string`
- `popupWarningAccentColor: string`
- `popupSuccessAccentColor: string`
- `popupInfoAccentColor: string`
- `popupCloseHoverColor: string`
- `popupCloseIdleColor: string`
- `popupIconChipOpacity: real`
- `borderColor: string`
- `borderColor2: string`
- `backgroundColor: string`
- `backgroundColor2: string`
- `menuPanelBackgroundColor: string`
- `dialogBackgroundColor: string`
- `overlayBackgroundColor: string`
- `tabPanelBackgroundColor: string`
- `highlightBackgroundColor: string`
- `highlightColor: string`
- `highlightDimmerColor: string`
- `secondaryHighlightColor: string`
- `selectedColor: string`
- `tabSelectedColor: string`
- `iconColorOnSelected: string`
- `buttonColor: string`
- `buttonBorderColor: string`
- `buttonHoverColor: string`
- `buttonPressedColor: string`
- `buttonAccentColor: string`
- `buttonInactiveColor: string`
- `buttonBorderFocusColor: string`
- `buttonTextColor: string`
- `buttonInactiveTextColor: string`
- `tableHeaderColor: string`
- `tableCellColor: string`
- `tableHeaderBorederColor: string`
- `tableCellBorderColor: string`
- `tableHeaderFontColor: string`
- `tableCellFontColor: string`
- `scrollBackgroundColor: string`
- `scrollIndicatorColor: string`
- `scrollHighlightColor: string`
- `shadowColor: string`
- `hover: string`
- `alternatingColor: color`
- `alternateBaseColor: string`
- `grayColor: string`
- `greenColor: string`
- `lightBlueColor: string`
- `maximumFlickVelocity: real`
- `enableHoverEffect: bool`
- `enableMenuPanelCollapse: bool`

**Функции**

- `getIconPath(iconName, state, mode, extention )`
- `getThemeColor(colorType, colorKey, themeType)`
- `setDecorators(sourceDecorators)`
- `parseStyleTheme(themeType)`

<a id="b-imtstyle-StyleComponents"></a>
#### `StyleComponents`

Набор компонентов стиля.

- **Файл:** [StyleComponents.qml](imtstyle/StyleComponents.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`Decorators`](#b-imtcontrols-Decorators), [`StyleBase`](#b-imtstyle-StyleBase)

**Свойства**

- `menuPanelButtonDecorator: Component`
- `subMenuPanelButtonDecorator: Component`
- `menuPanelDecorator: Component`
- `metaInfoPanelDecorator: Component`
- `filterPanelDecorator: Component`
- `bottomPanelDecorator: Component`
- `topButtonDecorator: Component`
- `topPanelDecorator: Component`
- `topCenterPanelDecorator: Component`
- `topLeftPanelDecorator: Component`
- `topRightPanelDecorator: Component`
- `tabPanelDecorator: Component`
- `buttonDecorator: Component`
- `firstSegmentDecorator: Component`
- `middleSegmentDecorator: Component`
- `lastSegmentDecorator: Component`
- `toolButtonDecorator: Component`
- `roundButtonDecorator: Component`
- `textFieldDecorator: Component`
- `textEditDecorator: Component`
- `comboBoxDecorator: Component`
- `checkBoxDecorator: Component`
- `switchDecorator: Component`
- `sliderDecorator: Component`
- `rangeSliderDecorator: Component`
- `radioButtonDecorator: Component`
- `textAreaDecorator: Component`
- `spinBoxDecorator: Component`
- `dialogDecorator: Component`
- `progressBarDecorator: Component`
- `busyIndicatorDecorator: Component`
- `tooltipDecorator: Component`
- `scrollBarDecorator: Component`
- `backGroundDecorator: Component`
- `messageDecorator: Component`
- `titleDecorator: Component`
- `inputTitleDecorator: Component`
- `frame: Component`
- `commonButtonDecorator: Component`
- `topPanelDialogDecorator: Component`
- `itemDelegateDecorator: Component`
- `popupDecorator: Component`
- `popupDecoratorGql: Component`
- `popupItemDelegateDecorator: Component`
- `detailedPopupItemDelegateDecorator: Component`
- `collectionTableDecorator: Component`
- `drawingContainerDecorator: Component`


---

<a id="imtqmlutils"></a>
## `imtqmlutils` — Утилиты (математика, валидаторы, модели, события)

Невизуальные утилиты, вынесенные из imtcontrols в отдельный модуль: геометрия и линейная алгебра, валидаторы ввода, генератор UUID, модельные помощники (сортировка/фильтрация, конечный автомат, построитель деревьев), общие функции, дизайн-схема и шина событий. Подключается строкой `import imtqmlutils 1.0`.

**Импорт:** `import imtqmlutils 1.0` · **Компонентов:** 17

### Core/ — Ядро

<a id="b-imtqmlutils-DesignScheme"></a>
#### `DesignScheme` · **singleton**

Схема.

- **Файл:** [Core/DesignScheme.qml](imtqmlutils/Core/DesignScheme.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `shapePointSize: real`
- `shapeSelectedPointSize: real`
- `boundingBoxLineWidth: int`
- `boundingBoxControlPointLineWidth: int`
- `boundingBoxControlPointRadius: int`
- `boundingBoxBorderColor: string`
- `selectionLineWidth: int`
- `selectionColor: string`

**Функции**

- `drawNode(ctx, point, radius)`
- `drawSelectedNode(ctx, point, radius)`
- `drawBoundingBoxBorder(ctx, cornerPointList, matrix)`
- `drawBoundingBoxRotationControl(ctx, cornerPointList, matrix)`
- `drawBoundingBoxControlPoint(ctx, point, matrix, color)`
- `drawEditPoint(ctx, point, color)`
- `drawPoint(ctx, point, radius , color, borderColor, lineWidth)`

<a id="b-imtqmlutils-EventManager"></a>
#### `EventManager` · **singleton**

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Core/EventManager.qml](imtqmlutils/Core/EventManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `events: var`

**Функции**

- `sendEvent(key, parameters)`
- `subscribeEvent(key, object, method)`
- `unSubscribeEvent(key, object, method)`
- `unSubscribeAllFromObject(object)`

<a id="b-imtqmlutils-Events"></a>
#### `Events` · **singleton**

Определения событий.

- **Файл:** [Core/Events.qml](imtqmlutils/Core/Events.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `events: var`

**Функции**

- `sendEvent(key, parameters)`
- `subscribeEvent(key, slot)`
- `unSubscribeAllFromSlot(slot)`
- `unSubscribeEvent(key, slot)`

<a id="b-imtqmlutils-Functions"></a>
#### `Functions` · **singleton**

Набор общих функций.

- **Файл:** [Core/Functions.qml](imtqmlutils/Core/Functions.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `formatDigitRev(digit)`
- `roundToPrecision(digit, precision)`
- `isFloatNumber(value)`
- `formatDigit(number_, decimalRoundupPrecision)`
- `dateToStr(date, format)`
- `formatPhoneNumber(phone, separator)`
- `stringToHex(str, delimiter)`
- `hexToString(hex, delimiter)`
- `hasElementInList(listArg, valueArg)`
- `lab2rgb(lab)`
- `rgb2lab(rArg,gArg, bArg)`
- `componentToHex(c)`
- `rgbToHex(r, g, b)`
- `labToRgbHex(argL, argA, argB)`
- `escapeSpecialChars(jsonString)`
- `getRadians(degreesArg)`
- `getAngle(xArg, yArg, center)`
- `darkenColor(hex, factor)`
- `fuzzyCompare(a, b, tolerance)`
- `fuzzyIsNull(a)`

### Math/ — Математика

<a id="b-imtqmlutils-AnalyticGeometry"></a>
#### `AnalyticGeometry` · **singleton**

Геометрические вычисления.

- **Файл:** [Math/AnalyticGeometry.qml](imtqmlutils/Math/AnalyticGeometry.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `scalarProduct2d(vector1, vector2)`
- `vectorLength2d(vectorPoint1, vectorPoint2)`
- `distanceBetweenPoints2d(point1, point2)`
- `distanceBetweenPointAndLineSection2d(point, linePoint1, linePoint2)`
- `intersectionLines2d(firstLinePoint1, firtsLinePoint2, secondLinePoint1, secondLinePoint2)`
- `intersectionBetweenLineSegments2d(firstSegmentPoint1, firstSegmentPoint2, secondSegmentPoint1, secondSegmentPoint2)`
- `hasIntersectionBetweenLineSegments2d(firstSegmentPoint1, firstSegmentPoint2, secondSegmentPoint1, secondSegmentPoint2)`
- `pointInPolygon(point, poligonPointList)`
- `nearestPointOnLine2d(point, linePoint1, linePoint2, isSection)`

<a id="b-imtqmlutils-LinearAlgebra"></a>
#### `LinearAlgebra` · **singleton**

Линейная алгебра.

- **Файл:** [Math/LinearAlgebra.qml](imtqmlutils/Math/LinearAlgebra.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `transformPoint2d(pointArg, matrixArg)`
- `rotatePoint(angle, center, pointArg)`
- `multiplyByMatrix3x3(matrixArg, externMatrix)`
- `multiplyByNumber(matrixArg, number)`
- `isIdentityMatrix(matrixArg)`
- `getInvertedMatrix3x3(matrixArg)`
- `getDeterminant3x3(matrixArg)`
- `getMinorMatrix3x3(matrixArg)`
- `getTransposedMatrix3x3(matrixArg)`
- `getDeterminant2x2(matrixArg)`

<a id="b-imtqmlutils-RelativePosition"></a>
#### `RelativePosition` · **singleton**

Вспомогательный компонент.

- **Файл:** [Math/RelativePosition.qml](imtqmlutils/Math/RelativePosition.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `left: int`
- `horizontalCenter: int`
- `right: int`
- `top: int`
- `verticalCenter: int`
- `bottom: int`

### Validators/ — Валидаторы

<a id="b-imtqmlutils-DoubleValidator"></a>
#### `DoubleValidator` · _не экспортируется (внутренний/по пути)_

Валидатор: проверка корректности ввода.

- **Файл:** [Validators/DoubleValidator.qml](imtqmlutils/Validators/DoubleValidator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `doubleRegExp: var`
- `regExp: var`

**Функции**

- `isValid(value)`

<a id="b-imtqmlutils-IntValidator"></a>
#### `IntValidator` · _не экспортируется (внутренний/по пути)_

Валидатор: проверка корректности ввода.

- **Файл:** [Validators/IntValidator.qml](imtqmlutils/Validators/IntValidator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `intRegExp: var`
- `regExp: var`

**Функции**

- `isValid(value)`

<a id="b-imtqmlutils-MacAddressValidator"></a>
#### `MacAddressValidator`

Валидатор: проверка корректности ввода.

- **Файл:** [Validators/MacAddressValidator.qml](imtqmlutils/Validators/MacAddressValidator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `macAddressRegExp: var`
- `regExp: var`

**Функции**

- `convert(text)`
- `isValid(macAddress)`

<a id="b-imtqmlutils-MailRegExpValidator"></a>
#### `MailRegExpValidator`

Валидатор: проверка корректности ввода.

- **Файл:** [Validators/MailRegExpValidator.qml](imtqmlutils/Validators/MailRegExpValidator.qml)
- **Базовый тип:** `RegularExpressionValidator`

### Models/ — Модели и данные

<a id="b-imtqmlutils-JSONListModel"></a>
#### `JSONListModel` · _не экспортируется (внутренний/по пути)_

Модель данных.

- **Файл:** [Models/JSONListModel.qml](imtqmlutils/Models/JSONListModel.qml)
- **Базовый тип:** `ListModel`
- **Наследники:** [`TreeItemModel`](#b-imtcontrols-TreeItemModel)

**Свойства**

- `source: string`
- `json: string`
- `inquiry: string`
- `target: string`
- `state: string`

**Функции**

- `makeJson()`
- `sendJson(method)`
- `updateJSONModel()`
- `parseJSONString(jsonString, jsonPathQuery)`

<a id="b-imtqmlutils-Process"></a>
#### `Process` · _не экспортируется (внутренний/по пути)_

Вспомогательный компонент.

- **Файл:** [Models/Process.qml](imtqmlutils/Models/Process.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

<a id="b-imtqmlutils-SortFilterProxyModel"></a>
#### `SortFilterProxyModel`

Модель данных.

- **Файл:** [Models/SortFilterProxyModel.qml](imtqmlutils/Models/SortFilterProxyModel.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `sourceModel: var`
- `filteredModel: var`
- `filterableFields: var`
- `textFilter: string`
- `sourceIndexes: var`

**Сигналы**

- `accepted()`

**Функции**

- `updateModel()`

<a id="b-imtqmlutils-StateMachine"></a>
#### `StateMachine`

Конечный автомат.

- **Файл:** [Models/StateMachine.qml](imtqmlutils/Models/StateMachine.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `stateModel: TreeItemModel`
- `availableModel: TreeItemModel`
- `states: var`

**Функции**

- `registerModel(model)`
- `addState(state, states)`
- `getAvailableModel(statusId)`
- `getStateIndex(statusId)`
- `getStateName(statusId)`

<a id="b-imtqmlutils-TreeModelBuilder"></a>
#### `TreeModelBuilder` · **singleton**

Вспомогательный компонент.

- **Файл:** [Models/TreeModelBuilder.qml](imtqmlutils/Models/TreeModelBuilder.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `internal: QtObject`

**Функции**

- `node(options)`
- `fromArray(items)`
- `fromListModel(listModel, mapItem, getChildren)`
- `fromListModelByFields(listModel, fields)`
- `fromTreeItemModel(treeItemModel, mapItem, getChildren)`
- `fromTreeItemModelByFields(treeItemModel, fields)`
- `fromBaseModel(baseModel, mapItem, getChildren)`
- `fromBaseModelByFields(baseModel, fields)`

<a id="b-imtqmlutils-UuidGenerator"></a>
#### `UuidGenerator` · **singleton**

Генератор.

- **Файл:** [Models/UuidGenerator.qml](imtqmlutils/Models/UuidGenerator.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Функции**

- `generateUUID()`


---

<a id="imtgraphics2d"></a>
## `imtgraphics2d` — 2D-графика на Canvas (фигуры, графики, матрицы)

2D-графическая подсистема на основе HTML5 Canvas, вынесенная из imtcontrols: контейнер сцены (GraphicsView, слои), фабрики и базовые классы фигур, набор фигур (полигоны, окружности, кривые Безье, сегменты, текст, изображения) с их параметрами, графики Graph2d/MultiGraph2d и матричная математика. Подключается строкой `import imtgraphics2d 1.0`.

**Импорт:** `import imtgraphics2d 1.0` · **Компонентов:** 35

### Core/ — Ядро

<a id="b-imtgraphics2d-BoundingBox"></a>
#### `BoundingBox`

Вспомогательный компонент.

- **Файл:** [Core/BoundingBox.qml](imtgraphics2d/Core/BoundingBox.qml)
- **Базовый тип:** [`GraphicsShapeBase`](#b-imtgraphics2d-GraphicsShapeBase)
- **Наследники:** [`BezierCurveShape`](#b-imtgraphics2d-BezierCurveShape), [`CircleShape`](#b-imtgraphics2d-CircleShape), [`CoordinateGridShape`](#b-imtgraphics2d-CoordinateGridShape), [`GradientPolyLineShape`](#b-imtgraphics2d-GradientPolyLineShape), [`ImageShape2d`](#b-imtgraphics2d-ImageShape2d), [`PolyLineShape`](#b-imtgraphics2d-PolyLineShape), [`PolygonShape`](#b-imtgraphics2d-PolygonShape), [`QuadraticCurveShape`](#b-imtgraphics2d-QuadraticCurveShape), [`RectangleShape`](#b-imtgraphics2d-RectangleShape), [`SegmentBaseShape`](#b-imtgraphics2d-SegmentBaseShape), [`SelectionShape`](#b-imtgraphics2d-SelectionShape), [`TextShape`](#b-imtgraphics2d-TextShape)

**Свойства**

- `editMode: bool`
- `controlPointColor: string`
- `controlPointBorderColor: string`
- `controlPointBorderWidth: int`
- `controlPointRadius: int`
- `controlPointsScreen: var`
- `pointsBackupScreen: var`
- `bbWidthBackupScreen: real`
- `bbHeightBackupScreen: real`
- `bbPointsBackupScreen: var`
- `bbPointsBackup: var`
- `shapeMatrixBackup: CanvasMatrix`
- `rotationTempMatrix: CanvasMatrix`
- `rotationTempMatrixScreen: CanvasMatrix`

**Функции**

- `drawBoundingBox(ctx)`
- `drawBoundingBoxBorder(ctx, cornerPointsObj, transformMatrix)`
- `drawBoundingBoxControlPoints(ctx, cornerPointsObj, transformMatrix)`
- `drawBoundingBoxRotationControl(ctx, cornerPointsObj, transformMatrix)`
- `getBoundingBoxCenter(cornerPointsObj)`
- `getLimitsObject(pointsArg)`
- `getBoundingBoxPoints()`
- `getBoundingBoxCornerPoints(isScreenPosition)`
- `getBoundingBoxMidPoints(isScreenPosition, cornerPointsObj)`
- `getBoundingBoxRotation()`
- `getBoundingBoxCoordinate()`
- `getBoundingBoxWidth(pointsBB)`
- `getBoundingBoxHeight(pointsBB)`
- `boundingBoxPointsToScreen(pointsObjLog)`
- `boundingBoxPointsTransform(pointsObjLog, matrixArg)`
- `editFunction(position)`
- `isInsideRotationArea(xArg, yArg, cornerPoints)`
- `isInside(xArg, yArg)`
- `isInsideBoundingBox(xArg, yArg)`
- `rotate(angle, center)`
- `leftMoving(scaleX, deltaXArg, deltaYArg, marginXArg)`
- `rightMoving(scaleX, deltaXArg, deltaYArg, marginXArg)`
- `topMoving(scaleY, deltaXArg, deltaYArg, marginYArg)`
- `bottomMoving(scaleY, deltaXArg, deltaYArg, marginYArg)`
- `topLeftMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg)`
- `topRightMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg)`
- `bottomLeftMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg)`
- `bottomRightMoving(scaleX, scaleY, deltaXArg, deltaYArg, marginXArg, marginYArg)`
- `transformPoints(scaleX, scaleY, translationX, translationY)`
- `getAngle(xArg, yArg, center, hasTransform)`
- `editPointsFunction(mouse)`
- `setBackup()`

<a id="b-imtgraphics2d-CanvasMatrix"></a>
#### `CanvasMatrix`

Матрица преобразования.

- **Файл:** [Core/CanvasMatrix.qml](imtgraphics2d/Core/CanvasMatrix.qml)
- **Базовый тип:** [`Matrix3x3`](#b-imtgraphics2d-Matrix3x3)

**Функции**

- `xScale()`
- `ySkew()`
- `xSkew()`
- `yScale()`
- `xTranslation()`
- `yTranslation()`
- `setXScale(xScaleArg)`
- `setYSkew(ySkewArg)`
- `setXSkew(xSkewArg)`
- `setYScale(yScaleArg)`
- `setXTranslation(xTranslationArg)`
- `setYTranslation(yTranslationArg)`
- `setContextTransform(ctx)`
- `transformContext(ctx)`
- `getRotationMatrix(matrixArg, angle, center)`
- `rotateContext(ctx, angle, center)`
- `translateContext(ctx, xArg, yArg)`
- `invertX()`
- `invertY()`
- `restore()`
- `saveInitialData()`
- `printMatrix()`

<a id="b-imtgraphics2d-GraphicsLayer"></a>
#### `GraphicsLayer`

Вспомогательный компонент.

- **Файл:** [Core/GraphicsLayer.qml](imtgraphics2d/Core/GraphicsLayer.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `viewItem: Item`
- `layerId: string`
- `shapeModel: var`
- `enabled: bool`
- `visible: bool`
- `canApplyViewTransform: bool`
- `isActive: bool`
- `viewMatrix: CanvasMatrix`
- `layerMatrix: CanvasMatrix`
- `tempMatrix: CanvasMatrix`
- `identityMatrix: CanvasMatrix`
- `viewMode: var`
- `clipRect: rect`
- `clipRectBackup: rect`

**Сигналы**

- `loadImageSignal(string source)`
- `layerChanded()`

**Функции**

- `addShape(shape)`
- `clear()`
- `removeShape(index)`
- `draw(ctx, viewMatrixArg)`
- `drawShape(ctx, shape)`
- `getScreenPosition(logPosition)`
- `getLogPosition(screenPosition)`
- `getLayerBoundingBox(isScreenPosition)`
- `getLimitsObject(pointsArg)`

<a id="b-imtgraphics2d-GraphicsShapeBase"></a>
#### `GraphicsShapeBase`

Базовый компонент (для наследования).

- **Файл:** [Core/GraphicsShapeBase.qml](imtgraphics2d/Core/GraphicsShapeBase.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `viewItem: Item`
- `layer: var`
- `hasCalibration: bool`
- `isTouched: bool`
- `isSelected: bool`
- `canSelect: bool`
- `isEditable: bool`
- `showNodes: bool`
- `showControlPoints: bool`
- `touchedNodeIndex: int`
- `highlightedNodeIndex: int`
- `editNodeIndex: int`
- `highlightedNodeCoordinate: var`
- `touchedNodeCoordinate: var`
- `viewMatrix: CanvasMatrix`
- `layerMatrix: CanvasMatrix`
- `shapeMatrix: CanvasMatrix`
- `tempMatrix: CanvasMatrix`
- `identityMatrix: CanvasMatrix`
- `viewMode: var`
- `points: var`
- `mouseIsPressed: bool`
- `mousePressedCoord: var`
- `isHidden: bool`
- `lineDashArray: var`

**Сигналы**

- `shapeChanged()`
- `shapeInfo(var info)`
- `pointInfo(var info)`
- `mouseClicked(var mouseEvent)`
- `mousePressed(var mouseEvent)`
- `mouseReleased(var mouseEvent)`
- `mouseDoubleClicked(var mouseEvent)`
- `mousePositionChanged(var mouseEvent)`
- `mousePositionShift(real deltaX, real deltaY)`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `getParams()`
- `setPoints(pointList)`
- `drawComplex(ctx, transformMatrixArg)`
- `drawControlPoints(ctx, transformMatrixArg)`
- `drawEditPoints(ctx)`
- `drawBoundingBox(ctx)`
- `getScreenPosition(logPosition)`
- `getLogPosition(screenPosition)`
- `getCalibratedPosition(position)`
- `getReverseCalibratedPosition(position)`
- `findNodeIndex(position)`
- `setCoordinateShift(deltaX, deltaY)`
- `getPointDescription(pointIndex)`
- `getShapeDescription()`
- `deletePoint(point)`
- `addPoint(point)`
- `getNearestPointIndex(point)`

<a id="b-imtgraphics2d-GraphicsShapeFactoryBase"></a>
#### `GraphicsShapeFactoryBase`

Базовый компонент (для наследования).

- **Файл:** [Core/GraphicsShapeFactoryBase.qml](imtgraphics2d/Core/GraphicsShapeFactoryBase.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `shapeItem: var`
- `typeName: string`
- `qmlShapeComp: Component`

**Функции**

- `createQmlShape(shapeItemArg, layerArg)`

<a id="b-imtgraphics2d-GraphicsShapeFactoryManager"></a>
#### `GraphicsShapeFactoryManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Core/GraphicsShapeFactoryManager.qml](imtgraphics2d/Core/GraphicsShapeFactoryManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `graphicsViewItem: Item`
- `sdlShapeModel: var`
- `textShapeComp: Component`
- `imageShapeComp: Component`
- `rectangleShapeComp: Component`
- `polygonShapeComp: Component`
- `polyLineShapeComp: Component`
- `segmentShapeComp: Component`
- `diskSegmentShapeComp: Component`
- `circleShapeComp: Component`
- `factoryList: var`
- `textShapeSdlComp: Component`
- `imageShapeSdlComp: Component`
- `rectangleShapeSdlComp: Component`
- `polygonShapeSdlComp: Component`
- `polyLineShapeSdlComp: Component`
- `lineShapeSdlComp: Component`
- `segmentShapeSdlComp: Component`
- `annulusShapeSdlComp: Component`
- `circleShapeSdlComp: Component`

**Сигналы**

- `sdlShapesCreated()`

**Функции**

- `createShapeList(layerArg)`
- `createShape(shapeItem, layerArg)`
- `addFactory(factorySdlComp, shapeComp)`
- `clearShapes()`

<a id="b-imtgraphics2d-GraphicsView"></a>
#### `GraphicsView`

Визуальное представление.

- **Файл:** [Core/GraphicsView.qml](imtgraphics2d/Core/GraphicsView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `shapeFactoryManger: GraphicsShapeFactoryManager`
- `sdlShapeModel: var`
- `selectedIndex: alias`
- `contentHeight: real`
- `contentY: real`
- `originY: real`
- `hideScrollbars: bool`
- `contentWidth: real`
- `contentX: real`
- `originX: real`
- `scaleStep: real`
- `minZoomLevel: real`
- `maxZoomLevel: real`
- `translateXPositiveLimit: var`
- `translateYPositiveLimit: var`
- `translateXNegativeLimit: var`
- `translateYNegativeLimit: var`
- `receiveMouseEvents: bool`
- `restrictZoom: bool`
- `restrictMove: bool`
- `restrictSelect: bool`
- `restrictMouseReacion: bool`
- `zoomOnlyInCenter: bool`
- `restrictMoveMargin: int`
- `fitToBorders: bool`
- `fitToViewMode: bool`
- `isSelectionMode: bool`
- `isEditMode: bool`
- `isPointsEditMode: bool`
- `isPointsAdditionMode: bool`
- `isPointsDeletionMode: bool`
- `hasHoverReaction: bool`
- `isMultiSelection: bool`
- `drawingAreaWidth: real`
- `drawingAreaHeight: real`
- `layerModel: var`
- `controlPanelComp: Component`
- `controlPanelItem: Item`
- `scaleCoeff: real`
- `deltaX: alias`
- `deltaY: alias`
- `viewMatrix: alias`
- `mouseArea: alias`
- `canvasAlias: alias`
- `hintShapeModel: var`
- `hintShapePointModel: var`
- `selectedShapeModel: var`
- `selectedShapePointModel: var`
- `hasLeftButtonMenu: bool`
- `hasRightButtonMenu: bool`
- `leftMenuCoordinates: var`
- `rightMenuCoordinates: var`
- `leftMenuIsOpen: bool`
- `rightMenuIsOpen: bool`
- `hasZoomInButton: bool`
- `hasZoomOutButton: bool`
- `hasFitToViewButton: bool`
- `hasResetViewButton: bool`
- `propagateWheelEvents: bool`
- `propagateMouseEvents: bool`
- `restrictDrawing: bool`
- `resizePauseDuration: int`
- `canvasAntialiasing: bool`
- `renderStrategy: int`
- `paintPauseDuration: int`

**Сигналы**

- `copySignal(int index)`
- `pasteSignal(int index)`
- `deleteSignal(int index)`
- `renameSignal(int index)`
- `revertSignal()`
- `modelDataChanged()`
- `cursorPosition(point position)`
- `hintShape(int shapeIndex)`
- `hintShapePoint(int shapeIndex,int pointIndex)`
- `paintTime(int time)`
- `mouseClicked(var mouse)`
- `mousePressed(var mouse)`
- `mouseReleased(var mouse)`
- `mouseDoubleClicked(var mouse)`
- `mousePositionChanged(var mouse)`
- `mouseEntered()`
- `mouseExited()`
- `painted()`
- `imageLoaded()`
- `sdlShapesCreated()`

**Функции**

- `drawSdlShapes()`
- `resize()`
- `fitToView()`
- `requestPaint()`
- `paintImmediately()`
- `createLayer(layerId)`
- `getLayer(layerId)`
- `getActiveLayer()`
- `getInactiveLayer()`
- `getBackgroundLayer()`
- `getToolsLayer()`
- `loadImage(source)`
- `designSchemeChanged(scheme)`
- `appSizeChanged(params)`
- `zoomIn()`
- `zoomOut()`
- `setZoom(scaleCoeff_, scaleX, scaleY)`
- `resetZoom()`
- `resetView(requestPaint_)`
- `findObject(mouseX, mouseY, selectionCodition)`
- `clearSelection(exeptShape)`
- `hoverReaction(position)`
- `fitToShapeList(shapeList, layer)`
- `fitToLayer(layer)`
- `fitToShape(shape)`
- `fitToActiveLayer()`
- `fitToInactivAndActiveLayer()`
- `boundingBoxPointsToScreen(pointsObjLog, shape)`

<a id="b-imtgraphics2d-Matrix3x3"></a>
#### `Matrix3x3`

Вспомогательный компонент.

- **Файл:** [Core/Matrix3x3.qml](imtgraphics2d/Core/Matrix3x3.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`CanvasMatrix`](#b-imtgraphics2d-CanvasMatrix)

**Свойства**

- `matrix: var`

**Функции**

- `isIdentityMatrix(matrixArg)`
- `multiplyByMatrix(matrixArg, externMatrix)`
- `transformPoint(pointArg, matrixArg)`
- `multiplyByNumber(matrixArg, number)`
- `getInvertedMatrix(matrixArg)`
- `getDeterminant(matrixArg)`
- `getMinorMatrix(matrixArg)`
- `getTransposedMatrix(matrixArg)`
- `copyFrom(externMatrix)`
- `reset()`

### Params/ — Редакторы параметров

<a id="b-imtgraphics2d-ArcParams"></a>
#### `ArcParams`

Параметры фигуры.

- **Файл:** [Params/ArcParams.qml](imtgraphics2d/Params/ArcParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `lineWidth: int`
- `color: string`
- `point: var`
- `radius: int`
- `startAngle: real`
- `endAngle: real`
- `anticlockwise: bool`

<a id="b-imtgraphics2d-BezierCurveParams"></a>
#### `BezierCurveParams`

Параметры фигуры.

- **Файл:** [Params/BezierCurveParams.qml](imtgraphics2d/Params/BezierCurveParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

<a id="b-imtgraphics2d-CircleParams"></a>
#### `CircleParams`

Параметры фигуры.

- **Файл:** [Params/CircleParams.qml](imtgraphics2d/Params/CircleParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `point: var`
- `radius: real`
- `color: string`
- `borderColor: string`
- `lineWidth: int`

<a id="b-imtgraphics2d-ImageParams"></a>
#### `ImageParams`

Параметры фигуры.

- **Файл:** [Params/ImageParams.qml](imtgraphics2d/Params/ImageParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `source: string`
- `point: var`
- `width: real`
- `height: real`

<a id="b-imtgraphics2d-LineParams"></a>
#### `LineParams`

Параметры фигуры.

- **Файл:** [Params/LineParams.qml](imtgraphics2d/Params/LineParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `points: var`
- `color: string`
- `lineWidth: int`

<a id="b-imtgraphics2d-PolygonParams"></a>
#### `PolygonParams`

Параметры фигуры.

- **Файл:** [Params/PolygonParams.qml](imtgraphics2d/Params/PolygonParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `points: var`
- `color: string`
- `borderColor: string`
- `lineWidth: int`

<a id="b-imtgraphics2d-QuadraticCurveParams"></a>
#### `QuadraticCurveParams`

Параметры фигуры.

- **Файл:** [Params/QuadraticCurveParams.qml](imtgraphics2d/Params/QuadraticCurveParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

<a id="b-imtgraphics2d-RectangleParams"></a>
#### `RectangleParams`

Параметры фигуры.

- **Файл:** [Params/RectangleParams.qml](imtgraphics2d/Params/RectangleParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `point: var`
- `radius: real`
- `color: string`
- `borderColor: string`
- `lineWidth: int`
- `width: int`
- `height: int`

<a id="b-imtgraphics2d-TextParams"></a>
#### `TextParams`

Параметры фигуры.

- **Файл:** [Params/TextParams.qml](imtgraphics2d/Params/TextParams.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `point: var`
- `text: string`
- `color: string`
- `fontSize: int`
- `bold: bool`
- `fontFamily: string`
- `lineWidth: int`

### Graphs/ — Графики

<a id="b-imtgraphics2d-Graph2d"></a>
#### `Graph2d`

Визуальный компонент.

- **Файл:** [Graphs/Graph2d.qml](imtgraphics2d/Graphs/Graph2d.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`MultiGraph2d`](#b-imtgraphics2d-MultiGraph2d)

**Свойства**

- `title: string`
- `legendX: string`
- `legendY: string`
- `lineColor: string`
- `gridStepMajorX: real`
- `gridStepMajorY: real`
- `gridStepMinorX: real`
- `gridStepMinorY: real`
- `labelFontSize: int`
- `xMinLabelValue: var`
- `xMaxLabelValue: var`
- `yMinLabelValue: var`
- `yMaxLabelValue: var`
- `linePoints: var`
- `pointCount: int`
- `hasData: bool`
- `canDraw: bool`
- `tooltipValues: var`
- `labelXValues: var`
- `labelYValues: var`
- `lableXOriginMargin: int`
- `legendMargin: int`
- `alwaysShowOrigin: bool`
- `hasTooltip: bool`
- `hasMinorGrid: bool`
- `isMultiGraph: bool`
- `rightLimit: var`
- `leftLimit: var`
- `coordinateShiftX: int`
- `coordinateShiftY: int`
- `xScale: real`
- `yScale: real`
- `graphicsViewAlias: alias`
- `gridShapeAlias: alias`
- `ready: bool`
- `fitToWidth: bool`
- `wasFitToWidth: bool`
- `legendFontSize: int`
- `setMinZoomByFitToView: bool`
- `restrictFitByResize: bool`
- `hasPointValidation: bool`
- `hasPointMovementControl: bool`
- `hasLeftButtonMenu: alias`
- `hasRightButtonMenu: alias`
- `restrictMove: alias`
- `restrictZoom: alias`
- `restrictSelect: alias`
- `hideScrollbars: alias`
- `coordinateGridLineThickness: alias`
- `xScaleBackup: real`
- `xScaleBackupWasSet: bool`

**Сигналы**

- `invalidPoint(int lineIndex, int pointIndex)`
- `editingFinished()`

**Функции**

- `validatorX(pointArg, nextPointArg, deltaArg, pointListArg, indexArg)`
- `validatorXBase(pointArg, nextPointArg, deltaArg, pointListArg, indexArg)`
- `validatorY(pointArg, nextPointArg, deltaArg, pointListArg, indexArg)`
- `validatorYBase(pointArg, nextPointArg, deltaArg, pointListArg, indexArg)`
- `getXCorrection(deltaArg, pointListArg, indexArg)`
- `getYCorrection(deltaArg, pointListArg, indexArg)`
- `getInvalidPoints(pointsArg)`
- `getInvalidPointsBase(pointsArg)`
- `getInvalidPointsBaseBothDirection(pointsArg)`
- `validatePoints()`
- `getScreenPosition(logPosition)`
- `requestPaint()`
- `fitToView()`
- `setEditMode(mode)`
- `createLine()`
- `getLabelX(xArg)`
- `getLabelY(yArg)`
- `getTooltipText(pointIndex, lineIndex)`
- `checkTooltips(pointIndex, lineIndex)`
- `reset()`
- `setLeftLimit(limitArg, requestPaintArg)`
- `setRightLimit(limitArg, requestPaintArg)`

<a id="b-imtgraphics2d-MultiGraph2d"></a>
#### `MultiGraph2d`

Вспомогательный компонент.

- **Файл:** [Graphs/MultiGraph2d.qml](imtgraphics2d/Graphs/MultiGraph2d.qml)
- **Базовый тип:** [`Graph2d`](#b-imtgraphics2d-Graph2d)

**Свойства**

- `tooltipValuesArray: var`

**Функции**

- `addLine(pointsArg, colorArg, showNodesArg, isEditableArg, lineDashArrayArg, tooltipValuesArg)`
- `getLine(index)`
- `getPoints(index)`
- `setPoints(points, index)`
- `clear()`
- `clearLines()`
- `clearLabels()`
- `getTooltipText(pointIndex, lineIndex)`
- `checkTooltips(pointIndex, lineIndex)`
- `getLineCount()`

### Shapes/ — Фигуры

<a id="b-imtgraphics2d-ArcShape"></a>
#### `ArcShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/ArcShape.qml](imtgraphics2d/Shapes/ArcShape.qml)
- **Базовый тип:** [`SegmentBaseShape`](#b-imtgraphics2d-SegmentBaseShape)

**Свойства**

- `radius: real`
- `lineWidth: int`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `isInsideRadius(dist)`
- `getBoundingBoxPoints()`

<a id="b-imtgraphics2d-BezierCurveShape"></a>
#### `BezierCurveShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/BezierCurveShape.qml](imtgraphics2d/Shapes/BezierCurveShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

<a id="b-imtgraphics2d-CircleShape"></a>
#### `CircleShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/CircleShape.qml](imtgraphics2d/Shapes/CircleShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `center: var`
- `radius: real`
- `color: string`
- `borderColor: string`
- `borderWidth: int`
- `opacity: real`
- `brashAlpha: real`
- `penAlpha: real`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `drawSelection(ctx, transformMatrixArg, centerScreen, radiusScreen)`
- `drawNodes(ctx, transformMatrixArg)`
- `getBoundingBoxCoordinate()`
- `getBoundingBoxWidth()`
- `getBoundingBoxHeight()`
- `isInside(xArg, yArg)`
- `getBoundingBoxPoints()`
- `setCoordinateShift(deltaX, deltaY)`
- `drawControlPoints(ctx, transformMatrixArg)`
- `getFillStyle(ctx, transformMatrixArg)`

<a id="b-imtgraphics2d-ComplexShape"></a>
#### `ComplexShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/ComplexShape.qml](imtgraphics2d/Shapes/ComplexShape.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Функции**

- `draw(ctx)`
- `getParams()`

<a id="b-imtgraphics2d-CoordinateGridShape"></a>
#### `CoordinateGridShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/CoordinateGridShape.qml](imtgraphics2d/Shapes/CoordinateGridShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `labelYWidth: int`
- `labelXHeight: int`
- `gridStepMajorX: real`
- `gridStepMinorX: real`
- `gridStepMajorY: real`
- `gridStepMinorY: real`
- `thinningZoomLevel: real`
- `fontSize: int`
- `fontColor: string`
- `backgroundColor: string`
- `majorGridColor: string`
- `minorGridColor: string`
- `majorGridOpacity: real`
- `axesColor: string`
- `gridLineThickness: real`
- `labelX: string`
- `labelY: string`
- `axesOrigin: var`
- `isFixedOrigin: bool`
- `cutAxesEnds: bool`
- `legendMargin: int`
- `canDrawText: bool`
- `showGridLines: bool`
- `showGridFrame: bool`
- `hasMinorGrid: bool`
- `labelPrecision: int`
- `lableXOriginMargin: real`
- `xMinLabelValue: var`
- `xMaxLabelValue: var`
- `yMinLabelValue: var`
- `yMaxLabelValue: var`
- `labelMatrix: CanvasMatrix`
- `xScale: real`
- `yScale: real`

**Функции**

- `getLogicalMajorLineX(lineX)`
- `getLogicalMajorLineY(lineY)`
- `getLabelX(xArg)`
- `getLabelY(yArg)`
- `getLabelXBase(xArg)`
- `getLabelYBase(yArg)`
- `draw(ctx, transformMatrixArg)`
- `thinningCheck(scaleCoeff, index, stepArg)`

<a id="b-imtgraphics2d-DiskSegmentShape"></a>
#### `DiskSegmentShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/DiskSegmentShape.qml](imtgraphics2d/Shapes/DiskSegmentShape.qml)
- **Базовый тип:** [`SegmentBaseShape`](#b-imtgraphics2d-SegmentBaseShape)

**Свойства**

- `innerRadius: real`
- `outerRadius: real`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `isInsideRadius(dist)`
- `getBoundingBoxPoints()`
- `drawControlPoints(ctx, transformMatrixArg)`

<a id="b-imtgraphics2d-GradientPolyLineShape"></a>
#### `GradientPolyLineShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/GradientPolyLineShape.qml](imtgraphics2d/Shapes/GradientPolyLineShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `points: var`
- `pointSize: real`
- `lineWidth: real`

**Функции**

- `draw(ctx, layerMatrix)`
- `drawPoint(ctx, x, y, aColor, fillCircle)`

<a id="b-imtgraphics2d-ImageShape2d"></a>
#### `ImageShape2d`

Вспомогательный компонент.

- **Файл:** [Shapes/ImageShape2d.qml](imtgraphics2d/Shapes/ImageShape2d.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `width: real`
- `height: real`
- `imageSource: string`
- `bitmapShape: var`
- `connections: Connections`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `getBoundingBoxCoordinate()`
- `getBoundingBoxWidth()`
- `getBoundingBoxHeight()`
- `isInside(xArg, yArg)`
- `getBoundingBoxPoints()`

<a id="b-imtgraphics2d-PolyLineShape"></a>
#### `PolyLineShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/PolyLineShape.qml](imtgraphics2d/Shapes/PolyLineShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `color: string`
- `selectionColor: string`
- `colorList: var`
- `lineWidth: int`
- `shapePointSize: real`
- `shapeSelectedPointSize: real`
- `showLine: bool`
- `isMultiColor: bool`
- `shapePointBorderColor: string`
- `hasHoverReaction: bool`
- `penAlpha: real`
- `invalidPointIndexArr: var`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `drawBase(ctx, transformMatrixArg)`
- `drawNodes(ctx, transformMatrixArg)`
- `drawEditPoints(ctx)`
- `getPointDescription(pointIndex)`
- `isInside(xArg, yArg)`

<a id="b-imtgraphics2d-PolygonShape"></a>
#### `PolygonShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/PolygonShape.qml](imtgraphics2d/Shapes/PolygonShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `color: string`
- `borderColor: string`
- `brashAlpha: real`
- `penAlpha: real`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `isInside(xArg, yArg)`
- `addPoint(point)`

<a id="b-imtgraphics2d-QuadraticCurveShape"></a>
#### `QuadraticCurveShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/QuadraticCurveShape.qml](imtgraphics2d/Shapes/QuadraticCurveShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

<a id="b-imtgraphics2d-RectangleShape"></a>
#### `RectangleShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/RectangleShape.qml](imtgraphics2d/Shapes/RectangleShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `width: real`
- `height: real`
- `radius: real`
- `color: string`
- `borderColor: string`
- `brashAlpha: real`
- `penAlpha: real`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `drawSelection(ctx, transformMatrixArg, xArg, yArg, widthArg, heightArg, radiusArg)`
- `drawNodes(ctx, transformMatrixArg)`
- `getBoundingBoxCoordinate()`
- `getBoundingBoxWidth()`
- `getBoundingBoxHeight()`
- `isInside(xArg, yArg)`
- `getBoundingBoxPoints()`

<a id="b-imtgraphics2d-SegmentBaseShape"></a>
#### `SegmentBaseShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/SegmentBaseShape.qml](imtgraphics2d/Shapes/SegmentBaseShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)
- **Наследники:** [`ArcShape`](#b-imtgraphics2d-ArcShape), [`DiskSegmentShape`](#b-imtgraphics2d-DiskSegmentShape), [`SegmentShape`](#b-imtgraphics2d-SegmentShape)

**Свойства**

- `center: var`
- `startAngle: real`
- `endAngle: real`
- `color: string`
- `borderColor: string`
- `borderWidth: int`
- `opacity: real`
- `brashAlpha: real`
- `penAlpha: real`
- `precision: real`
- `anticlockwise: bool` — только чтение
- `equalAngles: bool`

**Функции**

- `isInside(xArg, yArg)`
- `isInsideRadius(dist)`
- `transformAngle(angle)`
- `isInsideAngles(angle)`
- `getAngle(xArg, yArg, center, hasTransform)`
- `setCoordinateShift(deltaX, deltaY)`
- `getFillStyle(ctx, transformMatrixArg)`

<a id="b-imtgraphics2d-SegmentShape"></a>
#### `SegmentShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/SegmentShape.qml](imtgraphics2d/Shapes/SegmentShape.qml)
- **Базовый тип:** [`SegmentBaseShape`](#b-imtgraphics2d-SegmentBaseShape)

**Свойства**

- `radius: real`

**Функции**

- `draw(ctx, transformMatrixArg)`
- `isInsideRadius(dist)`
- `getBoundingBoxPoints()`
- `drawControlPoints(ctx, transformMatrixArg)`

<a id="b-imtgraphics2d-SelectionShape"></a>
#### `SelectionShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/SelectionShape.qml](imtgraphics2d/Shapes/SelectionShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `source: QtObject`

**Функции**

- `draw(ctx, layerMatrix)`
- `drawSelection(ctx)`
- `drawBorderPoints(ctx)`
- `getPoint(params_)`

<a id="b-imtgraphics2d-TextShape"></a>
#### `TextShape`

Графическая фигура (рисуется на Canvas).

- **Файл:** [Shapes/TextShape.qml](imtgraphics2d/Shapes/TextShape.qml)
- **Базовый тип:** [`BoundingBox`](#b-imtgraphics2d-BoundingBox)

**Свойства**

- `fontFamily: string`
- `fontSize: int`
- `fontBold: bool`
- `fillColor: string`
- `strokeColor: string`
- `text: string`
- `canZoomText: bool`
- `scaleCoeff: real`

**Функции**

- `draw(ctx, layerMatrix)`


---

<a id="imtgui"></a>
## `imtgui` — Каркас приложения и представлений

Каркас прикладного интерфейса поверх imtcontrols: оболочка приложения и управление страницами, панели (верхние/нижние/меню), редакторы параметров (Params), настройки (Settings) и обширный набор представлений-элементов (ElementView) для форм и таблиц.

**Импорт:** `import imtgui 1.0` · **Компонентов:** 110

### Application/ — Приложение

<a id="b-imtgui-AlertMessage"></a>
#### `AlertMessage`

Визуальный компонент.

- **Файл:** [Application/AlertMessage.qml](imtgui/Application/AlertMessage.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `margin: int`
- `message: string`
- `colorText: string`
- `iconSource: string`

<a id="b-imtgui-ApplicationEvents"></a>
#### `ApplicationEvents`

Определения событий.

- **Файл:** [Application/ApplicationEvents.qml](imtgui/Application/ApplicationEvents.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Сигналы**

- `localizationChanged(string langId)`
- `designSchemeChanged(string scheme)`
- `appWidthChanged(int width)`
- `appHeightChanged(int height)`

<a id="b-imtgui-ApplicationInfoProvider"></a>
#### `ApplicationInfoProvider` · **singleton**

Провайдер данных.

- **Файл:** [Application/ApplicationInfoProvider.qml](imtgui/Application/ApplicationInfoProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `serverApplicationInfo: ApplicationInfo`
- `webSocketPort: int`
- `userMode: string`
- `superuserStatus: string`
- `superuserMessage: string`
- `loading: bool` — только чтение
- `request: GqlSdlRequestSender`

**Сигналы**

- `updated()`
- `userModeReceived(string userMode)`
- `superuserExistResult(string status, string error)`

**Функции**

- `updateModel()`
- `getHeaders()`

<a id="b-imtgui-ApplicationMain"></a>
#### `ApplicationMain`

Визуальный компонент.

- **Файл:** [Application/ApplicationMain.qml](imtgui/Application/ApplicationMain.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `decorators: Decorators`
- `localSettings: alias`
- `thumbDecMenuPanelRadius: alias`
- `serverReady: bool`
- `authorizationServerConnected: bool`
- `useWebSocketSubscription: bool`
- `firstModelsIsInit: bool`
- `serverConnected: bool`
- `subscriptionManager: alias`
- `loadPageByClick: alias`
- `canRecoveryPassword: alias`
- `authConnectionState: alias`
- `pageAboutProvider: alias`
- `webSocketPort: int`
- `decoratorsQt: DecoratorsQt`
- `thumbnailDecoratorGui: alias`
- `designProvider: DesignSchemaProvider`
- `languageProvider: LanguageProvider`
- `messagePageComp: Component`
- `timer: Timer`

**Сигналы**

- `saveSettings(string json)`
- `settingsSaved()`
- `settingsSaveFailed()`

**Функции**

- `showMessagePage(message, loadingVisible)`
- `setDecorators()`
- `getServerUrl()`
- `reconnect()`
- `getHeaders()`
- `getWebSocketUrl(serverUrl)`
- `updateAllModels()`
- `onSimpleUserManagement()`
- `enterLoggedOutUi()`
- `firstModelsInit(force)`
- `connectToWebSocketServer()`
- `routeUserManagementUi()`

<a id="b-imtgui-ClientSettingsController"></a>
#### `ClientSettingsController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Application/ClientSettingsController.qml](imtgui/Application/ClientSettingsController.qml)
- **Базовый тип:** [`ParamsSetController`](#b-imtgui-ParamsSetController)

**Свойства**

- `json: string`
- `serverConnectionParam: ServerConnectionParam`

**Сигналы**

- `saveSettings()`
- `urlChanged()`

**Функции**

- `saveParam()`
- `updateConnectionParam()`
- `getServerUrl()`

<a id="b-imtgui-Configurator"></a>
#### `Configurator`

Визуальный компонент.

- **Файл:** [Application/Configurator.qml](imtgui/Application/Configurator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `localSettings: string`
- `paramsSet: ParamsSet`
- `decorators: Decorators`

**Сигналы**

- `saveSettings(string json)`
- `settingsSaved()`
- `settingsSaveFailed()`

<a id="b-imtgui-DatabaseBackupController"></a>
#### `DatabaseBackupController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Application/DatabaseBackupController.qml](imtgui/Application/DatabaseBackupController.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

<a id="b-imtgui-DesignSchemaProvider"></a>
#### `DesignSchemaProvider`

Провайдер данных.

- **Файл:** [Application/DesignSchemaProvider.qml](imtgui/Application/DesignSchemaProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `typeId: string`
- `currentDesignId: string`
- `selectionParam: SelectionParam`

**Функции**

- `setDesignSchema(designId)`

<a id="b-imtgui-LanguageProvider"></a>
#### `LanguageProvider`

Провайдер данных.

- **Файл:** [Application/LanguageProvider.qml](imtgui/Application/LanguageProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `typeId: string`
- `currentLanguage: string`
- `selectionParam: SelectionParam`

**Функции**

- `setLanguage(langId, beQuiet)`

<a id="b-imtgui-PagesManager"></a>
#### `PagesManager`

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [Application/PagesManager.qml](imtgui/Application/PagesManager.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `pageModel: TreeItemModel`
- `activeItem: Item`
- `activePageIndex: int`
- `loadByClick: bool`
- `modelState: alias`

**Функции**

- `updateModel()`
- `clearModels()`
- `loadPageContent(index)`

<a id="b-imtgui-PagesProvider"></a>
#### `PagesProvider`

Провайдер данных.

- **Файл:** [Application/PagesProvider.qml](imtgui/Application/PagesProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `pagesModel: TreeItemModel`
- `modelState: string`
- `pagesGqlModel: GqlModel`

**Функции**

- `updateModel()`
- `clearModel()`

<a id="b-imtgui-ThumbnailDecorator"></a>
#### `ThumbnailDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Application/ThumbnailDecorator.qml](imtgui/Application/ThumbnailDecorator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `drawingContainer: alias`
- `stackView: alias`
- `topPanel: alias`
- `mainMargin: int`
- `pageMargin: int`
- `mainRadius: int`
- `dialogManager: Item`
- `menuPanelRadius: alias`
- `loadPageByClick: alias`
- `canRecoveryPassword: bool`
- `settingsController: SettingsController`
- `authorizationPageComp: Component`
- `superuserPasswordPageComp: Component`
- `loadingPageComp: Component`
- `errorDialog: Component`

**Функции**

- `updateModels()`
- `onLogout()`
- `clearModels()`
- `fillPreferenceParamsSet()`
- `showPreferencePage()`
- `showPage(pageComp)`
- `startLoading()`
- `stopLoading()`

### View/ — Представления

<a id="b-imtgui-ButtonElementView"></a>
#### `ButtonElementView`

Представление элемента формы.

- **Файл:** [View/ButtonElementView.qml](imtgui/View/ButtonElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)
- **Наследники:** [`FilePickerElementView`](#b-imtgui-FilePickerElementView)

**Свойства**

- `text: string`
- `button: Button`
- `buttonEnabled: bool`

**Сигналы**

- `clicked()`

<a id="b-imtgui-CheckBoxElementView"></a>
#### `CheckBoxElementView`

Представление элемента формы.

- **Файл:** [View/CheckBoxElementView.qml](imtgui/View/CheckBoxElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `checkState: int`
- `text: string`

**Сигналы**

- `clicked()`

<a id="b-imtgui-ClearableComboBoxElementView"></a>
#### `ClearableComboBoxElementView`

Представление элемента формы.

- **Файл:** [View/ClearableComboBoxElementView.qml](imtgui/View/ClearableComboBoxElementView.qml)
- **Базовый тип:** [`ComboBoxElementView`](#b-imtgui-ComboBoxElementView)

<a id="b-imtgui-ComboBoxAddressElementView"></a>
#### `ComboBoxAddressElementView`

Представление элемента формы.

- **Файл:** [View/ComboBoxAddressElementView.qml](imtgui/View/ComboBoxAddressElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `isSelectionRequired: bool`
- `errorText: string`
- `itemHeight: int`
- `shownItemsCount: int`
- `commandId: string`
- `nameId: string`
- `fields: var`
- `textFilteringInfoIds: var`
- `currentIndex: int`
- `currentText: string`
- `filterText: string`
- `cbRef: SearchAddressComp`

**Сигналы**

- `finished(string addressId, string address)`

**Функции**

- `setupComboBox(cbRef)`

<a id="b-imtgui-ComboBoxElementView"></a>
#### `ComboBoxElementView`

Представление элемента формы.

- **Файл:** [View/ComboBoxElementView.qml](imtgui/View/ComboBoxElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)
- **Наследники:** [`ClearableComboBoxElementView`](#b-imtgui-ClearableComboBoxElementView), [`FilterableComboBoxElementView`](#b-imtgui-FilterableComboBoxElementView)

**Свойства**

- `isSelectionRequired: bool`
- `errorText: string`
- `model: var`
- `changeable: bool`
- `shownItemsCount: int`
- `currentIndex: int`
- `delegate: var`
- `nameId: string`
- `cbRef: ComboBox`

**Сигналы**

- `finished(string itemId, int index)`

**Функции**

- `setupComboBox(cbRef)`

<a id="b-imtgui-ComboBoxGqlElementView"></a>
#### `ComboBoxGqlElementView`

Представление элемента формы.

- **Файл:** [View/ComboBoxGqlElementView.qml](imtgui/View/ComboBoxGqlElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `isSelectionRequired: bool`
- `errorText: string`
- `itemHeight: int`
- `shownItemsCount: int`
- `placeHolderText: string`
- `commandId: string`
- `nameId: string`
- `fields: var`
- `textFilteringInfoIds: var`
- `currentIndex: int`
- `currentText: string`
- `filterText: string`
- `cbRef: SearchComp`

**Сигналы**

- `finished(string itemId, int index)`

**Функции**

- `setupComboBox(cbRef)`

<a id="b-imtgui-ComboBoxGqlLegacyElementView"></a>
#### `ComboBoxGqlLegacyElementView`

Представление элемента формы.

- **Файл:** [View/ComboBoxGqlLegacyElementView.qml](imtgui/View/ComboBoxGqlLegacyElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `isSelectionRequired: bool`
- `errorText: string`
- `itemHeight: int`
- `elementHeight: int`
- `placeHolderText: string`
- `commandId: string`
- `valueName: string`
- `currentIndex: int`
- `currentText: string`
- `canClose: bool`
- `isAddressSearch: bool`
- `hasSearchProperty: bool`
- `canReturnModel: bool`
- `gettedParam: string`
- `filterId: string`
- `propertyId: string`
- `cbRef: SearchCompLegacy`

**Сигналы**

- `finished(string itemId)`

**Функции**

- `setupComboBox(cbRef)`

<a id="b-imtgui-CommandsView"></a>
#### `CommandsView`

Визуальное представление.

- **Файл:** [View/CommandsView.qml](imtgui/View/CommandsView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `contentWidth: alias`
- `maximumWidth: int`
- `commandsModel: GuiElementModel`
- `allElements: var`
- `priorityElements: var`
- `negativeAccentCommandIds: var`
- `positiveAccentCommandIds: var`
- `positiveAccentColor: string`
- `negativeAccentColor: string`
- `commandsCount: alias`

**Сигналы**

- `commandActivated(string commandId, var params)`
- `updateWidth()`

**Функции**

- `clear()`
- `calcVisibleCommandsCount()`
- `hasHiddenCommands()`
- `checkWidth()`

<a id="b-imtgui-DateTimePickerElementView"></a>
#### `DateTimePickerElementView`

Представление элемента формы.

- **Файл:** [View/DateTimePickerElementView.qml](imtgui/View/DateTimePickerElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `readOnly: bool`
- `timeEdit: bool`
- `datePicker: DateInput`

**Сигналы**

- `editingFinished()`

**Функции**

- `setDateAsString(dateStr)`
- `getDateAsString()`
- `setDate(date)`

<a id="b-imtgui-DoubleInputElementView"></a>
#### `DoubleInputElementView`

Представление элемента формы.

- **Файл:** [View/DoubleInputElementView.qml](imtgui/View/DoubleInputElementView.qml)
- **Базовый тип:** [`TextInputElementView`](#b-imtgui-TextInputElementView)

**Свойства**

- `topValue: alias`
- `bottomValue: alias`
- `decimalPlaces: alias`
- `value: double`
- `popupError: bool`

<a id="b-imtgui-ElementView"></a>
#### `ElementView`

Представление элемента формы.

- **Файл:** [View/ElementView.qml](imtgui/View/ElementView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`ButtonElementView`](#b-imtgui-ButtonElementView), [`CheckBoxElementView`](#b-imtgui-CheckBoxElementView), [`ComboBoxAddressElementView`](#b-imtgui-ComboBoxAddressElementView), [`ComboBoxElementView`](#b-imtgui-ComboBoxElementView), [`ComboBoxGqlElementView`](#b-imtgui-ComboBoxGqlElementView), [`ComboBoxGqlLegacyElementView`](#b-imtgui-ComboBoxGqlLegacyElementView), [`DateTimePickerElementView`](#b-imtgui-DateTimePickerElementView), [`SpinBoxElementView`](#b-imtgui-SpinBoxElementView), [`SwitchElementView`](#b-imtgui-SwitchElementView), [`TableElementView`](#b-imtgui-TableElementView), [`TextElementView`](#b-imtgui-TextElementView), [`TextInputElementView`](#b-imtgui-TextInputElementView), [`TreeViewElementView`](#b-imtgui-TreeViewElementView)

**Свойства**

- `contentSpacing: alias`
- `titleFontSize: int`
- `name: alias`
- `description: alias`
- `controlComp: alias`
- `controlItem: alias`
- `topComp: alias`
- `topItem: alias`
- `bottomComp: alias`
- `bottomItem: alias`
- `topRoundedCorners: bool`
- `bottomRoundedCorners: bool`
- `contentWidth: alias`
- `nameWidth: alias`
- `controlWidth: int`
- `contentMargin: int`
- `nameMargin: int`
- `contentHeight: int`
- `controlHeight: int`

<a id="b-imtgui-FilePickerElementView"></a>
#### `FilePickerElementView`

Представление элемента формы.

- **Файл:** [View/FilePickerElementView.qml](imtgui/View/FilePickerElementView.qml)
- **Базовый тип:** [`ButtonElementView`](#b-imtgui-ButtonElementView)

**Свойства**

- `pathType: int`
- `path: string`
- `fileDialog: alias`
- `folderDialog: alias`

**Сигналы**

- `accepted()`

<a id="b-imtgui-FilterableComboBoxElementView"></a>
#### `FilterableComboBoxElementView`

Представление элемента формы.

- **Файл:** [View/FilterableComboBoxElementView.qml](imtgui/View/FilterableComboBoxElementView.qml)
- **Базовый тип:** [`ComboBoxElementView`](#b-imtgui-ComboBoxElementView)

**Свойства**

- `filteringFields: var`
- `descriptionFieldId: string`
- `sourceModel: var`

<a id="b-imtgui-GroupElementView"></a>
#### `GroupElementView`

Представление элемента формы.

- **Файл:** [View/GroupElementView.qml](imtgui/View/GroupElementView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
- **Наследники:** [`ServerConnectionParamElementView`](#b-imtgui-ServerConnectionParamElementView)

**Свойства**

- `expandedHeight: int` — только чтение
- `opened: bool`
- `contentHeight: int`

**Функции**

- `scheduleRecalc()`
- `relayout()`
- `calcHeight()`
- `update()`

<a id="b-imtgui-GroupHeaderView"></a>
#### `GroupHeaderView`

Визуальное представление.

- **Файл:** [View/GroupHeaderView.qml](imtgui/View/GroupHeaderView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `opened: bool`
- `groupView: GroupElementView`
- `title: alias`
- `titleText: alias`
- `topPadding: int`
- `leftPadding: int`
- `rightPadding: int`
- `bottomPadding: int`
- `controlItem: Item`
- `controlComp: Component`

<a id="b-imtgui-IntegerInputElementView"></a>
#### `IntegerInputElementView`

Представление элемента формы.

- **Файл:** [View/IntegerInputElementView.qml](imtgui/View/IntegerInputElementView.qml)
- **Базовый тип:** [`TextInputElementView`](#b-imtgui-TextInputElementView)

**Свойства**

- `topValue: alias`
- `bottomValue: alias`
- `value: int`

<a id="b-imtgui-LogView"></a>
#### `LogView`

Визуальное представление.

- **Файл:** [View/LogView.qml](imtgui/View/LogView.qml)
- **Базовый тип:** [`ViewBase`](#b-imtgui-ViewBase)

**Функции**

- `updateGui()`

<a id="b-imtgui-MacAddressElementView"></a>
#### `MacAddressElementView`

Представление элемента формы.

- **Файл:** [View/MacAddressElementView.qml](imtgui/View/MacAddressElementView.qml)
- **Базовый тип:** [`TextInputElementView`](#b-imtgui-TextInputElementView)

**Свойства**

- `prevText: string`
- `block: bool`
- `macAddressValidator: MacAddressValidator`

<a id="b-imtgui-MultiPageView"></a>
#### `MultiPageView`

Визуальное представление.

- **Файл:** [View/MultiPageView.qml](imtgui/View/MultiPageView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `pagesModel: ListModel`
- `currentIndex: int`
- `mainMargin: int`
- `panelWidth: int`

**Сигналы**

- `pageLoaded(int index, var pageItem, string pageId)`

**Функции**

- `clear()`
- `addPage(pageId, pageName, pageComp, icon)`
- `addSubPage(parentId, pageId, pageName, pageComp)`
- `setPageBadge(pageId, badgeText)`
- `removePage(pageId)`
- `getPageById(pageId)`
- `getPageByIndex(index)`
- `getIndexById(pageId)`
- `hasSubPages(pageId)`
- `toggleExpanded(pageId)`
- `isExpanded(pageId)`
- `getFirstSubPageIndex(parentId)`
- `getLastSubPageIndex(parentId)`
- `rememberSubPageSelection(index)`
- `ensurePageLoaded(index)`
- `activatePage(index)`

<a id="b-imtgui-ObjectVisualStatusProvider"></a>
#### `ObjectVisualStatusProvider`

Провайдер данных.

- **Файл:** [View/ObjectVisualStatusProvider.qml](imtgui/View/ObjectVisualStatusProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Сигналы**

- `visualStatusReceived(string objectId, string icon, string text, string description)`
- `visualStatusReceiveFailed(string objectId, string errorMessage)`

**Функции**

- `getVisualStatus(id, typeId)`

<a id="b-imtgui-PageContainer"></a>
#### `PageContainer`

Контейнер.

- **Файл:** [View/PageContainer.qml](imtgui/View/PageContainer.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `pageId: string`
- `pageName: string`
- `startItemSource: string`
- `startItemSourceComp: Component`

**Функции**

- `createStartItemComp()`
- `startItemIsCreated()`

<a id="b-imtgui-Pagination"></a>
#### `Pagination`

Визуальный компонент.

- **Файл:** [View/Pagination.qml](imtgui/View/Pagination.qml)
- **Базовый тип:** [`Row`](https://doc.qt.io/qt-6/qml-qtquick-row.html)

**Свойства**

- `pagesSize: int`
- `currentValue: int`
- `currentIndex: int`
- `color: color`
- `countElements: int`
- `countAllElements: int`
- `pageCount: alias`
- `perPageTitleVisible: bool`

**Функции**

- `refreshBtn()`

<a id="b-imtgui-PopupContainer"></a>
#### `PopupContainer`

Контейнер.

- **Файл:** [View/PopupContainer.qml](imtgui/View/PopupContainer.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `autoCloseInterval: int`
- `maxPopupCount: int`

**Функции**

- `addMessage(type, text, autoClose, id)`
- `addCustomMessage(id, customComponent, properties)`
- `replaceMessage(id, type, text, autoClose)`
- `removeMessage(index)`
- `removeMessageById(id)`
- `findMessage(id)`
- `clear()`

<a id="b-imtgui-PopupManager"></a>
#### `PopupManager` · **singleton**

Менеджер: управление состоянием/жизненным циклом.

- **Файл:** [View/PopupManager.qml](imtgui/View/PopupManager.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `popupContainer: PopupContainer`

**Функции**

- `registerPopupContainer(popup)`
- `addErrorMessage(text, autoClose, id)`
- `addWarningMessage(text, autoClose, id)`
- `addSuccessMessage(text, autoClose, id)`
- `addInfoMessage(text, autoClose, id)`
- `addCustomMessage(id, customComponent, properties)`
- `replaceErrorMessage(id, text, autoClose)`
- `replaceWarningMessage(id, text, autoClose)`
- `replaceSuccessMessage(id, text, autoClose)`
- `replaceInfoMessage(id, text, autoClose)`
- `closeMessage(id)`
- `getIndexByMessageId(id)`
- `messageIsOpened(id)`
- `clear()`

<a id="b-imtgui-PopupView"></a>
#### `PopupView`

Визуальное представление.

- **Файл:** [View/PopupView.qml](imtgui/View/PopupView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`TimeFilterParamView`](#b-imtgui-TimeFilterParamView), [`TimeInputDialog`](#b-imtcontrols-TimeInputDialog)

**Свойства**

- `root: Item`
- `uuid: string`
- `model: TreeItemModel`
- `hiddenBackground: bool`
- `noMouseArea: bool`
- `forceFocus: bool`
- `escapeEnabled: bool`

**Функции**

- `onBackgroundClicked()`
- `onAppSizeChanged(parameters)`

<a id="b-imtgui-SearchAddressComp"></a>
#### `SearchAddressComp`

Вспомогательный компонент.

- **Файл:** [View/SearchAddressComp.qml](imtgui/View/SearchAddressComp.qml)
- **Базовый тип:** [`SearchComp`](#b-imtgui-SearchComp)

**Свойства**

- `parentIds: string`
- `previousText: string`

**Функции**

- `setCustomFilters(inputParams)`
- `setAddressCurrentText(index, appendCurrText)`
- `textChangeFunc(filterText)`
- `setExternalAddress(address, parentIds)`
- `arraySize(str)`
- `removeLastElement(str)`
- `keepNElements(str, elementCount)`

<a id="b-imtgui-SearchComp"></a>
#### `SearchComp`

Вспомогательный компонент.

- **Файл:** [View/SearchComp.qml](imtgui/View/SearchComp.qml)
- **Базовый тип:** [`ComboBoxGql`](imtguigql/Inputs/ComboBoxGql.qml)
- **Наследники:** [`SearchAddressComp`](#b-imtgui-SearchAddressComp)

**Функции**

- `reset()`

<a id="b-imtgui-SearchCompLegacy"></a>
#### `SearchCompLegacy`

Визуальный компонент.

- **Файл:** [View/SearchCompLegacy.qml](imtgui/View/SearchCompLegacy.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `elementHeight: int`
- `fontColor: string`
- `textSize: int`
- `commandId: string`
- `gettedParam: string`
- `filterId: string`
- `propertyId: string`
- `hasSearchProperty: bool`
- `isAddressSearch: bool`
- `canSearchAnyStr: bool`
- `canClose: alias`
- `valueName: string`
- `retValName: string`
- `externalSearchParam: string`
- `gettedParamsModel: TreeItemModel`
- `filterIdsModel: TreeItemModel`
- `propertiesModel: TreeItemModel`
- `additionalFilterModel: TreeItemModel`
- `returnModel: TreeItemModel`
- `canReturnModel: bool`
- `parentIds: string`
- `excludeFilterPart: string`
- `selectedText: string`
- `additionalFilter: string`
- `isTextIncrease: bool`
- `previousText: string`
- `searchDelegate: alias`
- `textField: alias`
- `currentText: alias`
- `placeHolderText: alias`
- `itemHeight: alias`
- `textFieldWidth: alias`
- `textFieldTextSize: alias`
- `arrowIconSource: alias`
- `arrowIconRotation: alias`
- `openST: alias`

**Сигналы**

- `accepted(string retVal)`
- `searchPartialAddress(string addressStr)`
- `returnModelSignal()`
- `clearSignal()`
- `closeSignal()`

**Функции**

- `clearSearchFunc()`
- `clearSearchParams()`
- `clearSearchParamsExeptModels()`
- `setGettedParamsModel(param)`
- `setFilterIdsModel(id)`
- `setPropertiesModel(id_,value_)`
- `setAdditionalFilterParams()`
- `setAdditionalFilterParamsForPopup(model_)`
- `setCurrentTextAddressFunc(modelll, index_, addStr_)`
- `setCurrentTextFunc(modelll, index_)`
- `textChangeFunc(filterText)`
- `closeComboFunc()`
- `closeComboAddressFunc()`
- `removeLastAddressElement()`
- `clearModels()`
- `setSearchModels(gettedParamModel_, filterIdsModel_, propertiesModel_)`
- `copyToGettedParamsModel(gettedParamModel_)`
- `copyToFilterIdsModel(filterIdsModel_)`
- `copyToPropertiesModel(propertiesModel_)`
- `removeComma(str)`
- `removeLastElement(str)`
- `removeNElements(str, N)`
- `keepNElements(str, N)`
- `arraySize(str)`
- `getLastArrayElemet(str)`
- `newArrayCount(str1, str2)`
- `comaCount(text)`
- `setAddressFromOutside(address, parentIds)`

<a id="b-imtgui-SearchPage"></a>
#### `SearchPage`

Визуальный компонент.

- **Файл:** [View/SearchPage.qml](imtgui/View/SearchPage.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `currentText: string`
- `pageSize: int`
- `categories: var`
- `activeTabItems: var`
- `activeTabIndex: int`
- `activeTabTotal: int`
- `loadingMore: bool`
- `pendingRequestKey: string`
- `tabDataArrays: var`

**Функции**

- `makeRequestKey(text, rid, off)`
- `updateSearch(text)`
- `selectTab(tabIndex)`
- `loadMore()`

<a id="b-imtgui-SearchResultsView"></a>
#### `SearchResultsView`

Визуальное представление.

- **Файл:** [View/SearchResultsView.qml](imtgui/View/SearchResultsView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `searchResults: SearchResults`
- `categories: var`
- `activeItems: var`
- `activeTabIndex: int`
- `activeTotal: int`
- `loadingMore: bool`
- `contentWidth: int`
- `searchText: string`

**Сигналы**

- `itemDoubleClicked(int tabIndex, int itemIndex)`
- `selectionChanged(var selection)`
- `tabSelected(int tabIndex)`
- `requestLoadMore()`

**Функции**

- `checkWidth()`
- `updateGui()`
- `startLoading()`
- `stopLoading()`

<a id="b-imtgui-Separator"></a>
#### `Separator`

Визуальный компонент.

- **Файл:** [View/Separator.qml](imtgui/View/Separator.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

<a id="b-imtgui-ServerConnectionParamElementView"></a>
#### `ServerConnectionParamElementView`

Представление элемента формы.

- **Файл:** [View/ServerConnectionParamElementView.qml](imtgui/View/ServerConnectionParamElementView.qml)
- **Базовый тип:** [`GroupElementView`](#b-imtgui-GroupElementView)

**Свойства**

- `host: string`
- `httpPort: int`
- `wsPort: int`
- `isSecure: bool`
- `readOnly: bool`
- `controlWidth: int`
- `hostInput: alias`
- `httpPortInput: alias`
- `wsPortInput: alias`
- `isSecureSwitch: alias`

**Сигналы**

- `paramsChanged()`

<a id="b-imtgui-ServerNoConnectionView"></a>
#### `ServerNoConnectionView`

Визуальное представление.

- **Файл:** [View/ServerNoConnectionView.qml](imtgui/View/ServerNoConnectionView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `text: alias`
- `loadingVisible: alias`

<a id="b-imtgui-SimpleCommandsDecorator"></a>
#### `SimpleCommandsDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [View/SimpleCommandsDecorator.qml](imtgui/View/SimpleCommandsDecorator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commandModel: TreeItemModel`
- `buttonSize: int`

**Сигналы**

- `commandActivated(string commandId)`
- `commandAdded(int index, string commandId)`

**Функции**

- `setCommandVisible(commandId, visible)`
- `setCommandIsEnabled(commandId, isEnabled)`
- `isCommandVisible(commandId)`
- `isCommandIsEnabled(commandId)`
- `getCommandIndex(commandId)`

<a id="b-imtgui-SpinBoxElementView"></a>
#### `SpinBoxElementView`

Представление элемента формы.

- **Файл:** [View/SpinBoxElementView.qml](imtgui/View/SpinBoxElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `spinBox: SpinBox`
- `readOnly: bool`
- `startValue: real`
- `from: real`
- `to: real`
- `stepSize: real`
- `value: real`

<a id="b-imtgui-StickerView"></a>
#### `StickerView`

Визуальное представление.

- **Файл:** [View/StickerView.qml](imtgui/View/StickerView.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `text: alias`
- `textColor: alias`
- `font: alias`

<a id="b-imtgui-SwitchElementView"></a>
#### `SwitchElementView`

Представление элемента формы.

- **Файл:** [View/SwitchElementView.qml](imtgui/View/SwitchElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `checked: bool`
- `switchRef: SwitchCustom`
- `readOnly: bool`

**Функции**

- `setChecked(checked)`

<a id="b-imtgui-TabView"></a>
#### `TabView`

Визуальное представление.

- **Файл:** [View/TabView.qml](imtgui/View/TabView.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `tabModel: ListModel`
- `currentIndex: alias`
- `closable: alias`
- `mainMargin: int`
- `tabVisible: bool`
- `contentColor: color`
- `tabPanelColor: color`
- `tabDelegateDecorator: alias`

**Сигналы**

- `tabLoaded(int index, string tabId, var tabItem)`
- `tabClicked(var mouse, var tabItem, int index)`
- `startTabContentLoading(string tabId)`
- `stopTabContentLoading(string tabId)`

**Функции**

- `clear()`
- `addTab(tabId, tabName, tabComp, icon, description, waitName, pinned)`
- `removeTab(tabId)`
- `getTabById(tabId)`
- `setTabDescription(tabId, description)`
- `setTabIcon(tabId, icon)`
- `setTabName(tabId, name)`
- `getTabName(tabId)`
- `getTabByIndex(index)`
- `getIndexById(tabId)`
- `getTabIdByIndex(index)`
- `onCloseTab(index)`

<a id="b-imtgui-TableElementView"></a>
#### `TableElementView`

Представление элемента формы.

- **Файл:** [View/TableElementView.qml](imtgui/View/TableElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `table: Table`

<a id="b-imtgui-TextElementView"></a>
#### `TextElementView`

Представление элемента формы.

- **Файл:** [View/TextElementView.qml](imtgui/View/TextElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `text: string`
- `textRef: Text`

<a id="b-imtgui-TextInputElementView"></a>
#### `TextInputElementView`

Представление элемента формы.

- **Файл:** [View/TextInputElementView.qml](imtgui/View/TextInputElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)
- **Наследники:** [`DoubleInputElementView`](#b-imtgui-DoubleInputElementView), [`IntegerInputElementView`](#b-imtgui-IntegerInputElementView), [`MacAddressElementView`](#b-imtgui-MacAddressElementView)

**Свойства**

- `text: string`
- `textInputFocus: bool`
- `textInputMask: var`
- `textInputValidator: var`
- `readOnly: bool`
- `acceptableInput: bool`
- `maximumLength: int`
- `placeHolderText: string`
- `borderColor: string`
- `echoMode: int`
- `errorText: string`
- `showErrorWhenInvalid: bool`
- `textSize: int`
- `placeHolderTextSize: int`

**Сигналы**

- `accepted()`
- `cancelled()`
- `textEdited()`
- `editingFinished()`

**Функции**

- `ensureVisible(pos)`

<a id="b-imtgui-TimeFilterParamView"></a>
#### `TimeFilterParamView`

Просмотр параметра.

- **Файл:** [View/TimeFilterParamView.qml](imtgui/View/TimeFilterParamView.qml)
- **Базовый тип:** [`PopupView`](#b-imtgui-PopupView)

**Свойства**

- `timeFilter: TimeFilter`
- `listView: alias`
- `buttonDecorator: Component`
- `fontSize: int`
- `canTimeRangeEdit: bool`

**Сигналы**

- `timeRangeChanged(string begin, string end)`
- `timeUnitChanged(string mode, string unit)`
- `cancelled()`

**Функции**

- `updateGui()`
- `setItemName(index, name)`
- `setItemVisible(index, visible)`
- `setItemUnit(index, unit)`
- `setItemMode(index, mode)`
- `addItem(unit, name, mode, index)`
- `fromFormat(date)`
- `formatDate(date)`

<a id="b-imtgui-TreeViewElementView"></a>
#### `TreeViewElementView`

Представление элемента формы.

- **Файл:** [View/TreeViewElementView.qml](imtgui/View/TreeViewElementView.qml)
- **Базовый тип:** [`ElementView`](#b-imtgui-ElementView)

**Свойства**

- `treeView: var`
- `maxTreeHeight: int`

<a id="b-imtgui-ViewBase"></a>
#### `ViewBase`

Базовый компонент (для наследования).

- **Файл:** [View/ViewBase.qml](imtgui/View/ViewBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`LogView`](#b-imtgui-LogView)

**Свойства**

- `viewTypeId: string`
- `viewId: string`
- `model: var`
- `commandsControllerComp: Component`
- `commandsController: CommandsController`
- `commandsRequested: bool`
- `commandsDelegateComp: Component`
- `commandsDelegate: ViewCommandsDelegateBase`
- `commandsView: alias`
- `commandsViewComp: alias`
- `readOnly: bool`
- `contentColor: string`
- `commandsPanelVisible: bool`
- `commandsSeparatorVisible: bool`
- `commandsPanelHeight: int`
- `viewContentY: real`
- `viewContentRightMargin: real`
- `internal__: QtObject`

**Сигналы**

- `commandsModelChanged(var commandsModel)`
- `commandActivated(string commandId)`
- `modelDataChanged(var view, var model)`
- `guiUpdated(var view, var model)`
- `guiVisibleChanged(var view, bool visible)`

**Функции**

- `setAlertPanel(alertPanelComp)`
- `requestCommands()`
- `setReadOnly(readOnly)`
- `updateGui()`
- `updateModel()`
- `doUpdateModel()`
- `doUpdateGui()`
- `setBlockingUpdateModel(value)`
- `setBlockingUpdateGui(value)`
- `guiIsBlocked()`

<a id="b-imtgui-ViewCommandsDelegateBase"></a>
#### `ViewCommandsDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [View/ViewCommandsDelegateBase.qml](imtgui/View/ViewCommandsDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `view: Item`

**Сигналы**

- `commandActivated(string commandId)`

**Функции**

- `commandHandle(commandId, params)`

### Panels/ — Панели

<a id="b-imtgui-BottomPanel"></a>
#### `BottomPanel`

Панель.

- **Файл:** [Panels/BottomPanel.qml](imtgui/Panels/BottomPanel.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

<a id="b-imtgui-ButtonPanel"></a>
#### `ButtonPanel`

Панель.

- **Файл:** [Panels/ButtonPanel.qml](imtgui/Panels/ButtonPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `delegateWidth: int`
- `delegateHeight: int`
- `verticalMenuWidth: int`
- `visibleCount: int`
- `mainMargin: int`
- `horizCount: int`
- `horizontalSpacing: int`
- `verticalSpacing: int`
- `hasShadow: bool`
- `openST: bool`
- `hasActiveState: bool`
- `activeId: string`
- `startActiveIndex: int`
- `centered: bool`
- `openButtonText: string`
- `openButtonImageSource: string`
- `openButtonWidth: int`
- `openButtonHeight: int`
- `openDuration: int`
- `widthArr: var`
- `widthArrVer: var`
- `shadowColor: string`
- `baseColor: string`
- `buttonModel: TreeItemModel`
- `horizontalModel: TreeItemModel`
- `verticalModel: TreeItemModel`
- `proxiModel: TreeItemModel`
- `rightOrderModel: TreeItemModel`
- `vertMenuItem: Item`
- `buttonDelegate: Component`
- `buttonDelegateVert: Component`
- `menuAlignRight: bool`
- `canChangeOrder: bool`
- `verticalMenuHasMargins: bool`
- `compl: bool`
- `ready: bool`
- `horizontalWidth: real`

**Сигналы**

- `clicked(string buttonId)`

**Функции**

- `updateModel()`
- `clearModel()`
- `setReady()`
- `setOpenButtonVisible()`
- `menuPositionCorrection()`
- `assignModel()`
- `setVertMenuWidth()`
- `getMaxString()`
- `getMenuPoint()`
- `setModels()`
- `setActive(buttonId)`
- `setModelsWithActive(index)`
- `checkActiveInVertical()`
- `setRightOrder()`

<a id="b-imtgui-CommandsDecorator"></a>
#### `CommandsDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/CommandsDecorator.qml](imtgui/Panels/CommandsDecorator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commandId: string`
- `clearEventCommandId: string`
- `setCommandsVisibleEventCommandId: string`
- `updateCommandsGuiEventCommandId: string`
- `loadImages: int`
- `buttonModel: TreeItemModel`

**Функции**

- `getCommandIdRequest(callback)`
- `counterImage()`
- `setCommandsModel(parameters)`
- `clearModel(parameters)`
- `setVisible(visible)`

<a id="b-imtgui-CommandsPanel"></a>
#### `CommandsPanel`

Панель.

- **Файл:** [Panels/CommandsPanel.qml](imtgui/Panels/CommandsPanel.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `commandsModel: GuiElementContainer`

**Сигналы**

- `commandsReady()`
- `commandActivated(string commandId, var params)`

**Функции**

- `setNegativeAccentCommandIds(commandIds)`
- `setPositiveAccentCommandIds(commandIds)`
- `updateGui()`
- `clearModel()`
- `checkCommandsWidth()`
- `checkButtonVisible()`
- `computeMaxWidth(role, left, center, right, full)`

<a id="b-imtgui-EnableableFilterDelegate"></a>
#### `EnableableFilterDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Panels/EnableableFilterDelegate.qml](imtgui/Panels/EnableableFilterDelegate.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `filterId: string`
- `name: string`
- `isActive: bool`
- `readOnly: bool`
- `filterMenu: FilterMenu`
- `collectionFilter: CollectionFilter`
- `defaultFieldFilter: FieldFilter`
- `filterValue: string`
- `allowEmptyFilterValue: bool`

**Сигналы**

- `clearFilter(bool beQuiet)`

<a id="b-imtgui-FieldFilterDelegate"></a>
#### `FieldFilterDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Panels/FieldFilterDelegate.qml](imtgui/Panels/FieldFilterDelegate.qml)
- **Базовый тип:** [`OptionsFilterDelegate`](#b-imtgui-OptionsFilterDelegate)

**Свойства**

- `defaultFieldFilter: FieldFilter`

**Сигналы**

- `filterDependencyChanged(string filterId, var filterValue)`

**Функции**

- `setFieldFilterForOption(optionId, fieldFilter)`

<a id="b-imtgui-FilterDelegateBase"></a>
#### `FilterDelegateBase`

Базовый делегат — основа для конкретных делегатов.

- **Файл:** [Panels/FilterDelegateBase.qml](imtgui/Panels/FilterDelegateBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`OptionsFilterDelegate`](#b-imtgui-OptionsFilterDelegate), [`TimeFilterDelegate`](#b-imtgui-TimeFilterDelegate)

**Свойства**

- `filterId: string`
- `name: string`
- `mainButtonText: string`
- `isActive: bool`
- `readOnly: bool`
- `filterMenu: FilterMenu`
- `mainButton: alias`
- `clearButton: alias`

**Сигналы**

- `clearFilter(bool beQuiet)`
- `openFilter()`

<a id="b-imtgui-FilterMenu"></a>
#### `FilterMenu`

Меню.

- **Файл:** [Panels/FilterMenu.qml](imtgui/Panels/FilterMenu.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `canResetFilters: bool`
- `complexFilter: CollectionFilter`
- `documentFilter: DocCollectionFilter`
- `enabledFilters: var`
- `registeredFilters: var`
- `filterDependencies: var`
- `filtersModel: ListModel`

**Сигналы**

- `close()`
- `clear()`
- `filterChanged()`
- `clearAllFilters(bool beQuiet)`

**Функции**

- `hasActiveFilter()`
- `setFilterIsEnabled(filterId, enabled)`
- `registerFieldFilterDelegate(filterId, filterDelegateComp)`
- `removeFieldFilterDelegate(filterId)`
- `getFilterDelegate(filterId)`
- `setFilterDependency(filterId, dependsOnFilterId)`
- `hasDependsOn(filterId, dependsOnFilterId)`

<a id="b-imtgui-FilterPanelDecorator"></a>
#### `FilterPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/FilterPanelDecorator.qml](imtgui/Panels/FilterPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `contentWidth: alias`
- `complexFilter: CollectionFilter`
- `documentFilter: DocCollectionFilter`
- `canResetFilters: bool`
- `hasDateFilter: bool`
- `hasDocumentFilter: bool`
- `searchTextInputWidth: alias`
- `clearAllButtonWidth: alias`

<a id="b-imtgui-MenuPanel"></a>
#### `MenuPanel`

Панель.

- **Файл:** [Panels/MenuPanel.qml](imtgui/Panels/MenuPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `textColor: string`
- `fontName: string`
- `activePageId: string`
- `activePageName: string`
- `activeIcon: string`
- `firstElementImageSources: string`
- `activePageIndex: int`
- `model: TreeItemModel`
- `topPagesModel: TreeItemModel`
- `bottomPagesModel: TreeItemModel`
- `spacing: int`
- `buttonWidth: int`
- `buttonHeight: int`
- `centered: bool`
- `collapsed: bool`
- `menuDefaultWidth: real`
- `delegate: Component`

**Сигналы**

- `activePageChanged()`

**Функции**

- `onMenuModelRequest(ok)`
- `updateVisualStatus(data)`
- `clearModels()`
- `setCollapsed(stateArg)`
- `updateGui()`
- `setActivePage(pageId)`

<a id="b-imtgui-MenuPanelButton"></a>
#### `MenuPanelButton`

Кнопка.

- **Файл:** [Panels/MenuPanelButton.qml](imtgui/Panels/MenuPanelButton.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `text: string`
- `iconSource: string`
- `textColor: string`
- `fontName: string`
- `enabled: bool`
- `selected: bool`
- `highlighted: bool`
- `imageDecrease: real`
- `imageSelectedCoeff: real`
- `fontSize: real`
- `contentWidth: int`
- `contentHeight: int`
- `decoratorSource: string`
- `subPagesCount: alias`
- `menuPanelRef: Item`
- `mouseArea: alias`

**Сигналы**

- `clicked()`

<a id="b-imtgui-MenuPanelButtonDecorator"></a>
#### `MenuPanelButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/MenuPanelButtonDecorator.qml](imtgui/Panels/MenuPanelButtonDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `textIsCropped: bool`

**Сигналы**

- `accepted(string text)`
- `clicked()`

<a id="b-imtgui-OptionsFilterDelegate"></a>
#### `OptionsFilterDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Panels/OptionsFilterDelegate.qml](imtgui/Panels/OptionsFilterDelegate.qml)
- **Базовый тип:** [`FilterDelegateBase`](#b-imtgui-FilterDelegateBase)
- **Наследники:** [`FieldFilterDelegate`](#b-imtgui-FieldFilterDelegate)

**Свойства**

- `selectionParam: SelectionParam`
- `visibleItemCount: int`
- `collectionFilter: CollectionFilter`

**Сигналы**

- `optionSelectionChanged(var optionIds, var optionIndexes, bool beQuiet)`

**Функции**

- `setSelectedId(optionId, beQuiet)`
- `setSelectedIndex(index, beQuiet)`
- `updateMainText()`
- `setOptionsList(optionsList)`
- `addOption(option)`
- `createAndAddOption(id, name, description, enabled)`
- `removeOption(optionId)`
- `removeOptionByIndex(optionIndex)`
- `isValidModel()`
- `getOptionId(optionIndex)`
- `getOptionIndex(optionId)`

<a id="b-imtgui-SubMenuPanelButtonDecorator"></a>
#### `SubMenuPanelButtonDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/SubMenuPanelButtonDecorator.qml](imtgui/Panels/SubMenuPanelButtonDecorator.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `title: string`
- `imageSource: string`
- `visibleMarker: bool`
- `highlighted: bool`
- `selected: bool`
- `text: string`

**Сигналы**

- `accepted(string text)`
- `clicked()`

<a id="b-imtgui-TabDelegate"></a>
#### `TabDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Panels/TabDelegate.qml](imtgui/Panels/TabDelegate.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `selected: bool`
- `firstElement: bool`
- `lastElement: bool`
- `pinned: bool`
- `autoWidth: bool`
- `isCloseEnable: bool`
- `text: string`
- `description: string`
- `icon: string`
- `index: int`
- `selectedIndex: int`
- `maxWidth: int`
- `minWidth: int`
- `listView: ListView`
- `tabPanel: Item`
- `mouseArea: alias`

**Сигналы**

- `closeSignal()`
- `startContentLoading()`
- `stopContentLoading()`

<a id="b-imtgui-TabPanel"></a>
#### `TabPanel`

Панель.

- **Файл:** [Panels/TabPanel.qml](imtgui/Panels/TabPanel.qml)
- **Базовый тип:** [`Rectangle`](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)

**Свойства**

- `selectedIndex: int`
- `count: int`
- `spacing: alias`
- `isCloseEnable: bool`
- `maxWidth: int`
- `model: alias`
- `tabDelegate: alias`
- `tabsList: alias`
- `tabDelegateDecorator: Component`

**Сигналы**

- `closeItem(int index)`
- `tabClicked(var mouse, var tabItem, int index)`
- `rightClicked()`
- `leftClicked()`
- `startTabContentLoading(string tabId)`
- `stopTabContentLoading(string tabId)`

**Функции**

- `viewTabInListView(index)`
- `appSizeChanged()`

<a id="b-imtgui-TabPanelDecorator"></a>
#### `TabPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/TabPanelDecorator.qml](imtgui/Panels/TabPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `textIsCropped: bool`

<a id="b-imtgui-TimeFilterDelegate"></a>
#### `TimeFilterDelegate`

Делегат: шаблон отрисовки элемента списка/таблицы.

- **Файл:** [Panels/TimeFilterDelegate.qml](imtgui/Panels/TimeFilterDelegate.qml)
- **Базовый тип:** [`FilterDelegateBase`](#b-imtgui-FilterDelegateBase)

**Свойства**

- `canTimeRangeEdit: bool`
- `showFilterDetails: bool`
- `timeFilter: TimeFilter`

**Сигналы**

- `accepted()`
- `cleared()`

**Функции**

- `setTimeUnit(mode, unit, beQuiet)`
- `setTimeRange(begin, end, beQuiet)`

<a id="b-imtgui-TopCenterPanel"></a>
#### `TopCenterPanel`

Панель.

- **Файл:** [Panels/TopCenterPanel.qml](imtgui/Panels/TopCenterPanel.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

<a id="b-imtgui-TopCenterPanelDecorator"></a>
#### `TopCenterPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/TopCenterPanelDecorator.qml](imtgui/Panels/TopCenterPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `canSearch: bool`
- `contentWidth: int`

**Функции**

- `checkWidth()`
- `setVisible(visible)`

<a id="b-imtgui-TopLeftPanel"></a>
#### `TopLeftPanel`

Панель.

- **Файл:** [Panels/TopLeftPanel.qml](imtgui/Panels/TopLeftPanel.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `topPanel: Item`

<a id="b-imtgui-TopLeftPanelDecorator"></a>
#### `TopLeftPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/TopLeftPanelDecorator.qml](imtgui/Panels/TopLeftPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `maxWidth: int`
- `menuWidth: int`

**Функции**

- `onMenuWidthChanged(widthArg)`

<a id="b-imtgui-TopPanel"></a>
#### `TopPanel`

Панель.

- **Файл:** [Panels/TopPanel.qml](imtgui/Panels/TopPanel.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

<a id="b-imtgui-TopPanelDecorator"></a>
#### `TopPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/TopPanelDecorator.qml](imtgui/Panels/TopPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Свойства**

- `centerPanel: Item`
- `topPanel: Item`

<a id="b-imtgui-TopRightPanel"></a>
#### `TopRightPanel`

Панель.

- **Файл:** [Panels/TopRightPanel.qml](imtgui/Panels/TopRightPanel.qml)
- **Базовый тип:** [`ControlBase`](#b-imtcontrols-ControlBase)

**Свойства**

- `topPanel: Item`

<a id="b-imtgui-TopRightPanelDecorator"></a>
#### `TopRightPanelDecorator`

Декоратор: расширяет/настраивает базовый компонент.

- **Файл:** [Panels/TopRightPanelDecorator.qml](imtgui/Panels/TopRightPanelDecorator.qml)
- **Базовый тип:** [`DecoratorBase`](#b-imtcontrols-DecoratorBase)

**Функции**

- `onLogin()`
- `onLogout()`

### Params/ — Редакторы параметров

<a id="b-imtgui-ComposedParamsGui"></a>
#### `ComposedParamsGui`

Вспомогательный компонент.

- **Файл:** [Params/ComposedParamsGui.qml](imtgui/Params/ComposedParamsGui.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `paramsSet: ParamsSet`
- `settingsController: var`
- `spacing: int`

<a id="b-imtgui-ConstrainedDoubleParamEditor"></a>
#### `ConstrainedDoubleParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/ConstrainedDoubleParamEditor.qml](imtgui/Params/ConstrainedDoubleParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `valueParam: ConstrainedDoubleParam`
- `bottomValue: real`
- `topValue: real`

<a id="b-imtgui-DatabaseAccessSettingsEditor"></a>
#### `DatabaseAccessSettingsEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/DatabaseAccessSettingsEditor.qml](imtgui/Params/DatabaseAccessSettingsEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `databaseParams: DatabaseAccessSettings`

<a id="b-imtgui-DoubleParamEditor"></a>
#### `DoubleParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/DoubleParamEditor.qml](imtgui/Params/DoubleParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `valueParam: DoubleParam`

<a id="b-imtgui-EnableableParamEditor"></a>
#### `EnableableParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/EnableableParamEditor.qml](imtgui/Params/EnableableParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `enableableParam: EnableableParam`

<a id="b-imtgui-FileNameParamEditor"></a>
#### `FileNameParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/FileNameParamEditor.qml](imtgui/Params/FileNameParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `fileNameParam: FileNameParam`

<a id="b-imtgui-IntegerParamEditor"></a>
#### `IntegerParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/IntegerParamEditor.qml](imtgui/Params/IntegerParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `valueParam: IntegerParam`

<a id="b-imtgui-ParamController"></a>
#### `ParamController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Params/ParamController.qml](imtgui/Params/ParamController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)
- **Наследники:** [`ParamsSetController`](#b-imtgui-ParamsSetController)

**Свойства**

- `paramFactoryComp: Component`
- `paramModel: var`

**Сигналы**

- `paramModelCreated()`

**Функции**

- `createParamFromJson(json, beQuiet)`
- `getParamComp()`
- `getParam()`
- `saveParam()`

<a id="b-imtgui-ParamEditorBase"></a>
#### `ParamEditorBase`

Базовый компонент (для наследования).

- **Файл:** [Params/ParamEditorBase.qml](imtgui/Params/ParamEditorBase.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)
- **Наследники:** [`ComposedParamsGui`](#b-imtgui-ComposedParamsGui), [`ConstrainedDoubleParamEditor`](#b-imtgui-ConstrainedDoubleParamEditor), [`DatabaseAccessSettingsEditor`](#b-imtgui-DatabaseAccessSettingsEditor), [`DoubleParamEditor`](#b-imtgui-DoubleParamEditor), [`EnableableParamEditor`](#b-imtgui-EnableableParamEditor), [`FileNameParamEditor`](#b-imtgui-FileNameParamEditor), [`IntegerParamEditor`](#b-imtgui-IntegerParamEditor), [`SchedulerParamEditor`](#b-imtgui-SchedulerParamEditor), [`SelectionParamEditor`](#b-imtgui-SelectionParamEditor), [`ServerConnectionParamEditor`](#b-imtgui-ServerConnectionParamEditor), [`ServerLogProvider`](#b-imtgui-ServerLogProvider), [`TextParamEditor`](#b-imtgui-TextParamEditor), [`UrlParamEditor`](#b-imtgui-UrlParamEditor)

**Свойства**

- `paramId: string`
- `typeId: string`
- `name: string`
- `description: string`
- `readOnly: bool`
- `editorModel: var`
- `editorModelComp: alias`
- `sourceComp: alias`
- `sourceItem: alias`
- `paramController: ParamController`

**Сигналы**

- `editorModelDataChanged(string paramId, string key)`

<a id="b-imtgui-ParamsController"></a>
#### `ParamsController` · _не экспортируется (внутренний/по пути)_

Контроллер: невизуальная логика и координация.

- **Файл:** [Params/ParamsController.qml](imtgui/Params/ParamsController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `registeredParams: var`
- `registeredParamEditors: var`
- `textParamComp: Component`
- `idParamComp: Component`
- `urlParamComp: Component`
- `databaseAccessSettingsParamComp: Component`
- `selectionParamComp: Component`
- `schedulerParamComp: Component`
- `backupSettingsParamComp: Component`
- `paramsSetComp: Component`
- `fileNameParamComp: Component`
- `textParamEditorComp: Component`
- `urlParamEditorComp: Component`
- `databaseAccessSettingsEditorComp: Component`
- `paramsSetEditorComp: Component`
- `fileNameParamEditorComp: Component`
- `schedulerParamEditorComp: Component`
- `selectionParamEditorComp: Component`

**Функции**

- `registerBaseParams()`
- `joinParamToParamsSet(paramsSet)`
- `registerParam(typeId, paramComp, paramEditorComp)`
- `createParam(typeId, json)`
- `getParamComp(typeId)`
- `getParamEditorComp(typeId)`
- `createParamEditor(typeId)`

<a id="b-imtgui-ParamsSetController"></a>
#### `ParamsSetController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Params/ParamsSetController.qml](imtgui/Params/ParamsSetController.qml)
- **Базовый тип:** [`ParamController`](#b-imtgui-ParamController)
- **Наследники:** [`ClientSettingsController`](#b-imtgui-ClientSettingsController), [`PageAboutProvider`](#b-imtgui-PageAboutProvider)

**Свойства**

- `paramsSet: ParamsSet`
- `parameterComp: Component`

**Функции**

- `addParam(typeId, id, name, description, parameterData)`
- `clearParamsSet()`
- `getParameterById(id)`

<a id="b-imtgui-PasswordParamEditor"></a>
#### `PasswordParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/PasswordParamEditor.qml](imtgui/Params/PasswordParamEditor.qml)
- **Базовый тип:** [`TextParamEditor`](#b-imtgui-TextParamEditor)

<a id="b-imtgui-SchedulerParamEditor"></a>
#### `SchedulerParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/SchedulerParamEditor.qml](imtgui/Params/SchedulerParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `schedulerParam: SchedulerParam`

<a id="b-imtgui-SelectionParamEditor"></a>
#### `SelectionParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/SelectionParamEditor.qml](imtgui/Params/SelectionParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `selectionParam: SelectionParam`

<a id="b-imtgui-ServerConnectionParamEditor"></a>
#### `ServerConnectionParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/ServerConnectionParamEditor.qml](imtgui/Params/ServerConnectionParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `serverConnectionParam: ServerConnectionParam`

<a id="b-imtgui-TextParamEditor"></a>
#### `TextParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/TextParamEditor.qml](imtgui/Params/TextParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)
- **Наследники:** [`PasswordParamEditor`](#b-imtgui-PasswordParamEditor), [`TextParamView`](#b-imtgui-TextParamView)

**Свойства**

- `textParam: TextParam`

<a id="b-imtgui-TextParamView"></a>
#### `TextParamView`

Просмотр параметра.

- **Файл:** [Params/TextParamView.qml](imtgui/Params/TextParamView.qml)
- **Базовый тип:** [`TextParamEditor`](#b-imtgui-TextParamEditor)

<a id="b-imtgui-TimeParamEditor"></a>
#### `TimeParamEditor` · _не экспортируется (внутренний/по пути)_

Редактор: форма создания/изменения значения.

- **Файл:** [Params/TimeParamEditor.qml](imtgui/Params/TimeParamEditor.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `value: string`

<a id="b-imtgui-UrlParamEditor"></a>
#### `UrlParamEditor`

Редактор: форма создания/изменения значения.

- **Файл:** [Params/UrlParamEditor.qml](imtgui/Params/UrlParamEditor.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

**Свойства**

- `urlParam: UrlParam`

### Settings/ — Настройки

<a id="b-imtgui-PageAboutProvider"></a>
#### `PageAboutProvider`

Провайдер данных.

- **Файл:** [Settings/PageAboutProvider.qml](imtgui/Settings/PageAboutProvider.qml)
- **Базовый тип:** [`ParamsSetController`](#b-imtgui-ParamsSetController)

**Свойства**

- `serverLogProvider: Component`
- `serverVersion: string`
- `serverVersionText: TextParam`

**Функции**

- `createRepresentation()`

<a id="b-imtgui-Preference"></a>
#### `Preference`

Визуальный компонент.

- **Файл:** [Settings/Preference.qml](imtgui/Settings/Preference.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `paramsSet: ParamsSet`
- `settingsController: SettingsController`

**Сигналы**

- `editorModelDataChanged(string paramId, string key)`

<a id="b-imtgui-PreferenceButton"></a>
#### `PreferenceButton`

Кнопка.

- **Файл:** [Settings/PreferenceButton.qml](imtgui/Settings/PreferenceButton.qml)
- **Базовый тип:** [`Item`](https://doc.qt.io/qt-6/qml-qtquick-item.html)

**Свойства**

- `iconSource: alias`

<a id="b-imtgui-PreferenceDialog"></a>
#### `PreferenceDialog`

Диалог: модальное окно для ввода/подтверждения.

- **Файл:** [Settings/PreferenceDialog.qml](imtgui/Settings/PreferenceDialog.qml)
- **Базовый тип:** [`Dialog`](#b-imtcontrols-Dialog)

**Свойства**

- `paramsSet: ParamsSet`
- `settingsController: SettingsController`
- `rootWidth: int`
- `paramIdsChanges: var`

**Функции**

- `updateButtons()`

<a id="b-imtgui-ServerLogProvider"></a>
#### `ServerLogProvider`

Провайдер данных.

- **Файл:** [Settings/ServerLogProvider.qml](imtgui/Settings/ServerLogProvider.qml)
- **Базовый тип:** [`ParamEditorBase`](#b-imtgui-ParamEditorBase)

<a id="b-imtgui-SettingsController"></a>
#### `SettingsController`

Контроллер: невизуальная логика и координация.

- **Файл:** [Settings/SettingsController.qml](imtgui/Settings/SettingsController.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `paramsSet: ParamsSet`
- `paramsSetController: ParamsSetController`
- `supportedParamEditors: var`
- `registeredControllers: var`
- `textParamEditorComp: Component`
- `textViewParamEditorComp: Component`
- `urlParamEditorComp: Component`
- `databaseAccessSettingsEditorComp: Component`
- `fileNameParamEditorComp: Component`
- `schedulerParamEditorComp: Component`
- `selectionParamEditorComp: Component`
- `paramsSetEditorComp: Component`
- `passwordParamEditorComp: Component`
- `integerParamEditorComp: Component`
- `doubleParamEditorComp: Component`
- `constrainedDoubleParamEditorComp: Component`
- `enableableParamEditorComp: Component`
- `serverConnectionParamEditorComp: Component`
- `paramsSetInfoComp: Component`

**Функции**

- `addParamsSet(id, name, paramsSet)`
- `registerParamEditor(typeId, paramEditorComp)`
- `getParamEditor(typeId)`
- `registerParamsSetController(id, name, controller)`
- `createRepresentation()`
- `saveParam(paramId)`

<a id="b-imtgui-SettingsObserver"></a>
#### `SettingsObserver`

Вспомогательный компонент.

- **Файл:** [Settings/SettingsObserver.qml](imtgui/Settings/SettingsObserver.qml)
- **Базовый тип:** [`TreeItemModelObserver`](#b-imtcontrols-TreeItemModelObserver)

**Свойства**

- `designProvider: DesignSchemaProvider`
- `languageProvider: LanguageProvider`
- `settingsProvider: SettingsProvider`

**Сигналы**

- `urlChanged(string newUrl)`

**Функции**

- `onDesignSchemaChanged(newVal)`
- `onServerUrlChanged(newVal)`
- `onLanguageChanged(newVal)`
- `onDatabaseAccessSettingsChanged()`

<a id="b-imtgui-SettingsProvider"></a>
#### `SettingsProvider`

Провайдер данных.

- **Файл:** [Settings/SettingsProvider.qml](imtgui/Settings/SettingsProvider.qml)
- **Базовый тип:** [`QtObject`](https://doc.qt.io/qt-6/qml-qtqml-qtobject.html)

**Свойства**

- `serverModel: TreeItemModel`
- `localModel: TreeItemModel`
- `applicationInfoProvider: var`
- `aboutApplicationProvider: PageAboutProvider`
- `private_: QtObject`
- `modelsCompleted: bool`

**Сигналы**

- `serverSettingsSaved()`
- `localSettingsSaved()`

**Функции**

- `onLocalModelDataChanged()`
- `clearModel()`
- `getRepresentationModel()`
- `updateModel()`
- `saveLocalModel()`
- `saveServerModel()`
- `setDesignSchema(schema)`
- `setLanguage(language)`
- `setServerUrl(serverUrl)`
- `setWebSocketServerUrl(webSocketServerUrl)`
- `cacheServerModel()`
- `rewriteModel(fromModel, toModel)`
- `getValue(parameterId)`
- `findValue(model, parameterId)`
- `getHeaders()`

