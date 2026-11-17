# Чего не хватает движку JQML v4 для приложений (IotPlatform, Lisa, ProLife, Rosa)

Анализ от 17.11.2026, ветка `jqml-v4-work5` (после PR #1131). Цель: найти то, что приложения используют, а движок не умеет или
делает не так, как Qt, и расставить приоритеты.

## Как проверялось

1. **Статический разбор исходников**: 4148 файлов `.qml` и `.js` (ImtCore/Qml, IotPlatform, Lisa, ProLife) и QML-документы Rosa из
   её собранного бандла (исходников Rosa нет). Подсчитаны используемые типы, импорты, `Qt.*`, методы Canvas и методы моделей.
2. **Запуск**: каждое приложение (IotPlatform, Lisa, ProLife) собрано в debug-режиме и запущено без браузера; затем **каждый** его
   QML-файл (2339 / 1866 / 2048 документов) создан движком отдельно, в мягком режиме. Движок сам записывает неизвестные типы
   (`engine.unknownTypes`) и присваивания, которые он не понял (`engine.ignoredProperties`).
3. **Сверка API**: методы `CTreeItemModel` (C++) с методами `TreeItemModel` движка; перечень типов Qt Quick, Quick Effects, Maps с
   тем, что движок создаёт.
4. **Измерения**: `test/bench-pages.mjs` (открытие реальных страниц IotPlatform).

Ограничения: документ создаётся один, без данных сервера и без окружения родителя, поэтому ошибки вида «model is not defined»
в этом анализе — шум и в отчёт не вошли. Для Rosa только статический разбор. Типы SDL (`…Input`, `…Payload`) берутся из
сгенерированных папок и не считались.

## Что точно не хватает (приложения это используют)

### Ломает функцию

| Что | Сколько раз | Где | Что происходит |
|---|---|---|---|
| `UuidGenerator.generateUUID()` и тип `UuidGenerator {}` | 3 + 1 | `imtauthgui/AuthorizationPage.qml:704`, `GqlBasedTenantManagementApiClient.qml:1757`, `GqlBasedUserAdministrationApiClient.qml:320`, `imtcolgui/CollectionStructureCommandsDelegateBase.qml:372` | в движке нет. Проверено: значение остаётся пустым, ошибки в консоли нет. Новая роль, пользователь, организация получают пустой `m_id` |
| `FileIO` | 13 | `FileExportDialog`, `FileImportDialog`, `CollectionViewCommandsDelegateBase` (ImtCore, Rosa) | экспорт и импорт файлов |
| `FolderDialog`, `Qt.labs.platform` `FileDialog` (`QLP.FileDialog`) | 4 + 3 | ImtCore, ProLife, Rosa | выбор папки и файла (обычный `FileDialog` есть) |
| `Instantiator` (`onObjectAdded`, `onObjectRemoved`) | 4 | ImtCore, Rosa | объекты не создаются |
| методы `TreeItemModel`: `setUpdateEnabled` 6, `swapItems` 6, `isEqualWithModel` 5, `beginChanges`/`endChanges` 8, `getParent` 2, `insertNewItemWithParameters` 2 | 29 | `CollectionStructureBase`, `TableBase`, `BaseClass`, `ViewBase`, `BasicTableView` | вызов падает. `ViewBase.doUpdateModel` проверяет `beginChanges !== undefined` и молча пропускает группировку изменений |
| Canvas: `arcTo` 6, `createConicGradient` 2 (Rosa), `getImageData` 1 (Rosa) | 9 | ImtCore, ProLife, Rosa | рисунок неполный |
| `Qt.md5` 2 (IotPlatform), `Qt.Key_Asterisk` 2 | 4 | | |

### Делает не так, как Qt (в логе только «ignored»)

Число — сколько присваиваний встретилось при создании всех документов приложения (IotPlatform / Lisa / ProLife).

| Что | Сколько | Последствие |
|---|---|---|
| `Image.sourceSize.width/height` | 4656 / 2918 / 3526 | размер декодирования не учитывается: резкость SVG-иконок, память больших картинок (проверить) |
| `MouseArea.onCanceled` | 1383 / 894 / 1053 | нет сигнала потери нажатия (скролл забирает мышь у кнопки): кнопка может остаться «нажатой» |
| `NumberAnimation.easing.type` | 459 / 367 / 410 | кривые не применяются, всё идёт линейно (`items.js`, комментарий у `Animation`) |
| `Text.onLinkActivated` | 89 / 74 / 81 | ссылки в RichText не нажимаются |
| `MouseArea.onPressAndHold` | 50 / 40 / 41 | долгое нажатие не распознаётся |
| `ListView/Flickable.onMovementStarted/Ended`, `onFlickStarted` | ≈7 | |
| `Drag.*`, `MouseArea.drag.target`, `DropArea` | 1–3 | перетаскивание между элементами |
| корневой `Window` (`IotPlatformWeb.qml`, `MainWindow.qml`, `ModalDialogManager.qml:264`) | 3 | тип неизвестен, `title`, `flags`, `onClosing` не работают |
| `Item.defaultFieldFilter.m_fieldId` / `m_filterValueType` | 15–23 | значения группового свойства теряются (`TenantCollectionView.qml:478`, `UserActionCollectionView.qml:39`); **проверить на живом приложении**, возможно особенность проверки |

### Только мобильные и настольные, в вебе вряд ли нужны

`WebView`, `CameraPermission`, `CaptureSession`, `VideoOutput`, `TextMetrics` (по 1, мобильный IotPlatform), `Process` (2),
`Qt.PermissionStatus` (4), типы C++ приложений (`PumaConnectionChecker`, `BiometricController`, `PAYMENTS.*`, контроллеры).
`Qt.vector3d` (78) — только `imt3dgui`, 3D движок не поддерживает.

### Типы Qt Quick, которых нет, но приложения их не используют

`Grid`, `PathView` и все `Path*`, `Shape`, `TableView`, `TreeView`, `TapHandler`, `HoverHandler`, `PinchHandler`, `SwipeView`,
`Popup`, `ToolTip` и остальные Quick Controls. Правило проекта: свои контролы, не Quick Controls, поэтому это не пробел.

## Качество и скорость

- **Открытие страниц** (`test/bench-pages.mjs`, реальные контролы IotPlatform): первый проход 1512 мс, следующие 1250 мс. Таблица 559 мс,
  дерево 115 мс, список 76 мс. 152 тыс. вызовов скриптов, 20 мкс каждый; почти всё время уходит в пересчёт биндингов.
- **Выражения без `with`**: 99,9 % выражений, но обработчики и функции (тела с операторами) идут через `with` и не оптимизируются V8.
- **Память**: вкладка измерения Rosa — около 17 700 элементов. Утечки закрытых страниц исправлены (`6f3127c0a7`, `d37df02164`);
  остаётся около 12 МБ на закрытую вкладку (держатель пустых заготовок не найден).
- **Canvas**: сглаживание и края не совпадают с Qt пиксель в пиксель (отложено по решению владельца).
- **Текст**: измерение ширины строк с округлением как у Qt сделано (`ELIDE_SLACK`), но метрики шрифтов браузера и Qt различаются.
- **Отладка QML**: точки остановки работают в `?qmlsource=1` и debug-сборке (`9a591b73a9`); в обычной сборке по умолчанию выключено из-за цены.

## Тесты и инфраструктура

- 476 сравнений с настоящим Qt (`test/desktop`): 3 не проходят (`Changed_002_use_site_object_default`, `Loader_010_property_object_released`,
  `Import_01`), `Canvas_01_base64` нестабилен.
- Node-тесты: `incremental.mjs` и `run.mjs` не стартуют (нужна папка `qml`, её нет в репозитории); `lisa-runtime.mjs` — 2 проверки падают.
- Страницы приложений проверяются только вручную, `bench-pages` и `page-leak` берут контролы IotPlatform. Предложение: сделать сканер
  из этого анализа обычным тестом: «в приложении не появилось новых неизвестных типов и непонятых присваиваний».
- Lisa: после правки роли Undo/Redo не обновляются — сервер не присылает `OnUndoRedoChanged` на любой версии движка (не движок).

## Предлагаемый порядок

1. `UuidGenerator`, `FileIO`, `FolderDialog` и `Qt.labs.platform` `FileDialog`, `Instantiator` — без них не работают создание сущностей и
   файловый обмен.
2. Методы `TreeItemModel` из таблицы (`setUpdateEnabled`, `swapItems`, `isEqualWithModel`, `beginChanges`/`endChanges`, `getParent`,
   `insertNewItemWithParameters`).
3. `MouseArea.onCanceled`, `Image.sourceSize`, `easing.type` — самые частые «молчаливые» расхождения.
4. `Text.onLinkActivated`, `onPressAndHold`, `Canvas.arcTo`, `createConicGradient`, `getImageData`, `Qt.md5`.
5. Корневой `Window`, `Drag`/`DropArea`, `defaultFieldFilter` (сначала проверить).
6. Скорость обработчиков и функций, остаток памяти закрытых вкладок.
