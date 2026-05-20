# Справочник QML-модулей

Полный справочник всех QML-модулей в SDK ImtCore.

---

## 1. imtcontrols — Базовый UI-фреймворк

**Импорт:** `import imtcontrols 1.0`
**Путь:** `Qml/imtcontrols/`

Основа всех UI-компонентов. Содержит около 160 компонентов в 7 категориях.

### 1.1 Base — Основы

Синглтоны и базовые типы, используемые всеми остальными компонентами.

| Компонент | Тип | Описание |
|-----------|-----|----------|
| `Events` | Синглтон | Глобальная система событий для межкомпонентного взаимодействия |
| `EventManager` | Синглтон | Регистрация и диспетчеризация событий |
| `Enums` | Синглтон | Общие перечисления и константы |
| `Functions` | Синглтон | Утилитарные функции (форматирование, преобразование) |
| `AnalyticGeometry` | Синглтон | Геометрические вычисления (точки, линии, поверхности) |
| `LinearAlgebra` | Синглтон | Операции с матрицами и векторами |
| `RelativePosition` | Синглтон | Вычисления относительного положения |
| `DesignScheme` | Синглтон | Дизайн-токены: цвета, шрифты, отступы |
| `BaseClass` | Тип | Фундаментальный базовый тип для всех компонентов |
| `BaseModel` | Тип | Базовый класс для моделей данных |
| `ControlBase` | Тип | Базовый класс для интерактивных элементов управления |
| `DecoratorBase` | Тип | Базовый класс для декораторов (паттерн «Декоратор») |
| `Decorators` | Тип | Стандартная коллекция декораторов |
| `DecoratorsQt` | Тип | Qt-специфичные декораторы |
| `StyleBase` | Тип | Базовый класс для определений стилей |
| `StyleComponents` | Тип | Предопределённые компоненты стилей |
| `MainWindow` | Тип | Обёртка главного окна |
| `WrapItem` | Тип | Обёртка элемента для целей компоновки |
| `IconProperties` | Тип | Конфигурация иконок |
| `FontProperties` | Тип | Конфигурация шрифтов |
| `StateManager` | Тип | Управление состоянием на основе конечного автомата |
| `Icon` | JS | Константы и пути иконок |
| `ButtonType` | JS | Константы типов кнопок |

**Веб-специфичные** (`Base/web/`):

| Компонент | Описание |
|-----------|----------|
| `ScrollBar` | Веб-совместимая полоса прокрутки |
| `ToolTip` | Веб-совместимая всплывающая подсказка |
| `Switch` | Веб-совместимый переключатель |
| `Window` | Веб-совместимая обёртка окна |
| `RoundButton` | Веб-совместимая круглая кнопка |

### 1.2 Buttons — Кнопки

| Компонент | Описание |
|-----------|----------|
| `Button` | Стандартная кнопка с текстом и/или иконкой |
| `TextButton` | Кнопка без рамки, только текст |
| `ToolButton` | Компактная кнопка для панелей инструментов, преимущественно с иконкой |
| `MenuButton` | Кнопка с выпадающим меню |
| `RadioButton` | Радиокнопка для единичного выбора |
| `RadioBox` | Группировка радиокнопок |
| `SegmentedButton` | Сегментированный выбор (группа переключателей) |
| `ButtonDecorator` | Декоратор для настройки кнопок |
| `IconButtonDecorator` | Декоратор для кнопок с иконками |
| `TextButtonDecorator` | Декоратор для текстовых кнопок |
| `ToolButtonDecorator` | Декоратор для кнопок панели инструментов |
| `RadioButtonDecorator` | Декоратор для радиокнопок |
| `TopButtonDecorator` | Декоратор для кнопок верхней панели |
| `TopButtonDecoratorNew` | Новый декоратор для кнопок верхней панели |
| `ExternalSegmentDecorator` | Декоратор для внешних сегментов |

### 1.3 Inputs — Поля ввода

| Компонент | Описание |
|-----------|----------|
| `CustomTextField` | Текстовое поле ввода с валидацией |
| `TextFieldWithTitle` | Текстовое поле ввода с заголовком |
| `TextInputWithLabel` | Текстовое поле ввода с меткой |
| `TextArea` | Многострочное текстовое поле ввода |
| `TextEditCustom` | Расширенный текстовый редактор |
| `CustomTextEdit` | Пользовательский текстовый редактор |
| `SearchTextInput` | Поле поиска с иконкой и логикой фильтрации |
| `ComboBox` | Выпадающий список |
| `FilterableComboBox` | Выпадающий список с текстовым фильтром |
| `CheckBox` | Флажок |
| `CheckBoxMenu` | Флажок с функциональностью меню |
| `SpinBox` | Числовое поле ввода с кнопками +/- |
| `SpinBoxComp` | Расширенный компонент SpinBox |
| `Slider` | Ползунок |
| `RangeSlider` | Ползунок диапазона (мин./макс.) |
| `DatePicker` | Выбор даты |
| `TimePicker` | Выбор времени |
| `DateTimePicker` | Выбор даты и времени |
| `DateInput` | Поле ввода даты |
| `TimeInput` | Поле ввода времени |
| `DateTimeInput` | Поле ввода даты и времени |
| `TimeIntervalPicker` | Выбор временного интервала |
| `TimeInputDialog` | Диалог ввода времени |
| `Calendar` | Виджет календаря |
| `CalendarView` | Представление календаря |
| `CalendarFunctions` | Вспомогательные функции календаря |
| `CalendarCellDelegateBase` | Базовый класс для ячеек календаря |
| `CalendarCellTextDelegate` | Текстовый делегат для ячеек календаря |
| `BarControl` | Элемент управления в виде полосы |
| `PathPicker` | Выбор пути к файлу |
| `InputActions` | Действия для полей ввода |
| `TextField` | Базовое текстовое поле |

### 1.4 Views — Компоненты отображения

| Компонент | Описание |
|-----------|----------|
| **Таблицы** | |
| `Table` | Основной компонент для отображения таблиц |
| `TableBase` | Базовый класс для таблиц |
| `BasicTableView` | Простое представление таблицы |
| `TableDecorator` | Декоратор таблицы |
| `TableHeaders` | Заголовки таблицы |
| `TableHeaderDelegate` | Делегат для отдельной ячейки заголовка |
| `TableHeaderParamComp` | Параметры заголовков |
| `TableDelegate` | Стандартный делегат таблицы |
| `TableRowDelegate` | Делегат строки |
| `TableRowDelegateBase` | Базовый класс для делегатов строк |
| `TableRowViewer` | Средство просмотра строк |
| `TableRowCanvasViewer` | Средство просмотра строк на основе Canvas |
| `TablePainterRowDelegateBase` | Базовый класс для Canvas-делегатов строк |
| `TableCellDelegate` | Делегат ячейки |
| `TableCellDelegateBase` | Базовый класс для делегатов ячеек |
| `TableCellDelegateSimple` | Простой делегат ячейки |
| `TableCellDelegateCheckable` | Делегат ячейки с флажком |
| `TableCellTextDelegate` | Текстовый делегат ячейки |
| `TableCellText` | Текст в ячейке таблицы |
| `TableCellTextWithTooltipDelegate` | Текстовая ячейка с всплывающей подсказкой |
| `TableCellDateDelegate` | Делегат ячейки даты |
| `TableCellIconTextDelegate` | Ячейка с иконкой и текстом |
| `TableCellRowNumberDelegate` | Делегат номера строки |
| `TextLinkCellDelegate` | Кликабельная ссылка в таблице |
| `SimpleTableDelegate` | Упрощённый делегат таблицы |
| `ComboBoxCellContentComp` | ComboBox в ячейке таблицы |
| `TextInputCellContentComp` | TextInput в ячейке таблицы |
| `TableSelection` | Управление выделением таблицы |
| `TableProperties` | Свойства таблицы |
| `TableViewParams` | Параметры TableView |
| `TableViewItemDelegateBase` | Базовый класс для делегатов TableView |
| `TableTreeView` | Таблица с древовидной структурой |
| `TableTreeDecorator` | Декоратор для древовидных таблиц |
| **Деревья** | |
| `BasicTreeView` | Простое древовидное представление |
| `TreeViewGql` | Древовидное представление с данными GraphQL |
| `TreeItemDelegate` | Делегат для элементов дерева |
| `TreeViewItemDelegateBase` | Базовый класс для делегатов дерева |
| `TreeViewSelection` | Выделение в древовидном представлении |
| `TreeViewColumn` | Столбец в древовидном представлении |
| `TreeViewCommandsDelegateBase` | Базовый класс для делегатов команд |
| **Навигация** | |
| `StackView` | Стековая навигация по страницам |
| `StackViewHeader` | Заголовок-хлебные крошки для StackView |
| `SwipeView` | Навигация по страницам смахиванием |
| `SplitView` | Разделённое представление |
| `Splitter` | Разделительная линия для SplitView |
| `Gallery` | Представление галереи |
| **Индикаторы** | |
| `ProgressBar` | Индикатор прогресса |
| `ProgressRoundIndicator` | Круглый индикатор прогресса |
| `BusyIndicator` | Индикатор загрузки |
| `Loading` | Отображение загрузки |
| `Spinner` | Анимация вращения |
| `ValueLevelIndicator` | Индикатор уровня значения |
| `IntervalBar` | Полоса интервала |
| **Диаграммы** | |
| `BarChart` | Столбчатая диаграмма |
| `BarChartSimple` | Упрощённая столбчатая диаграмма |
| `StackedBarChart` | Составная столбчатая диаграмма |
| `Piechart` | Круговая диаграмма |
| `Diagram` | Универсальная диаграмма |
| `DiagramIndicator` | Индикатор диаграммы |
| **Прочее** | |
| `BaseText` | Базовый текстовый компонент |
| `Title` | Заголовок |
| `ItemDelegate` | Делегат элемента |
| `ItemDelegateDecorator` | Декоратор делегата элемента |
| `TooltipArea` | Область всплывающей подсказки |
| `CustomTooltip` | Пользовательская всплывающая подсказка |
| `CustomScrollbar` | Пользовательская полоса прокрутки |
| `ScrollIndicator` | Индикатор прокрутки |
| `SwitchCustom` | Пользовательский переключатель |
| `DrawingContainer` | Контейнер для рисования |
| `ExpandingContainer` | Расширяемый контейнер |
| `CollapsibleRectangle` | Сворачиваемый прямоугольник |
| `ChangeItemSize` | Компонент изменения размера |
| `PaintedCorner` | Отрисованный угол |
| `InputView` | Представление ввода |
| `Arrowhead` | Наконечник стрелки |
| `PopupDetailedDelegateDecorator` | Декоратор для детализированных всплывающих окон |
| `SubMenuPanelButtonDecorator` | Декоратор для кнопок панели подменю |
| `MultiPageView` | Многостраничное представление |

### 1.5 Dialogs — Диалоговые окна

| Компонент | Описание |
|-----------|----------|
| `Dialog` | Стандартный диалог |
| `DialogNew` | Новый стиль диалога |
| `DialogDecorator` | Декоратор диалога |
| `DialogButtons` | Стандартные кнопки диалога (ОК, Отмена) |
| `DialogManagerView` | Централизованное управление диалогами |
| `MessageDialog` | Диалог сообщений (информация/предупреждение/ошибка) |
| `MessageDialogBody` | Содержимое диалога сообщений |
| `MessagePopup` | Всплывающее сообщение |
| `InputDialog` | Диалог ввода |
| `InputDialogBody` | Содержимое диалога ввода |
| `ErrorDialog` | Диалог ошибки |
| `CriticalErrorDialog` | Диалог критической ошибки |
| `PopupDecorator` | Декоратор всплывающего окна |
| `PopupDecoratorGql` | Декоратор всплывающего окна с GraphQL |
| `PopupItemDelegateDecorator` | Декоратор делегата элемента всплывающего окна |
| `PopupMenuDialog` | Всплывающее меню в виде диалога |
| `TopPanelDialog` | Диалог в верхней панели |
| `FileImportDialog` | Диалог импорта файлов |
| `FileExportDialog` | Диалог экспорта файлов |
| `ModalDialogManager` | Менеджер модальных диалогов |
| `ProgressDialogManager` | Менеджер диалогов прогресса |

### 1.6 Models — Модели данных

| Компонент | Описание |
|-----------|----------|
| `JSONListModel` | Модель из данных JSON |
| `GqlModel` | Модель для данных GraphQL |
| `TreeItemModel` | Иерархическая древовидная модель |
| `TreeItemModelConverter` | Конвертер для древовидных моделей |
| `TreeItemModelObserver` | Наблюдатель за изменениями древовидной модели |
| `TreeModelBuilder` | Построитель древовидных моделей (на основе полей и обратных вызовов) |
| `SortFilterProxyModel` | Прокси сортировки/фильтрации для моделей |
| `SortController` | Контроллер сортировки |
| `DataModelController` | Контроллер моделей данных |
| `DataModelProvider` | Провайдер моделей данных |
| `CommandsController` | Контроллер команд/действий |
| `NavigationController` | Контроллер навигации |
| `NavigableItem` | Навигируемый элемент |
| `PopupMenuModel` | Модель для всплывающих меню |
| `OptionManager` | Управление опциями |
| `OptionsListAdapter` | Адаптер для списков опций |
| `OptionsListManager` | Менеджер списков опций |
| `TableProperties` | Модель свойств таблицы |
| `ModelIndex` | Индекс в модели |
| `StateMachine` | Конечный автомат |
| `Process` | Абстракция процесса |
| `UuidGenerator` | Генератор UUID |
| `IntValidator` | Валидация целых чисел |
| `DoubleValidator` | Валидация чисел с плавающей точкой |
| `MacAddressValidator` | Валидация MAC-адресов |
| `MailRegExpValidator` | Валидация электронной почты |

### 1.7 Actions — Действия и взаимодействия

| Компонент | Описание |
|-----------|----------|
| `Action` | Абстрактное действие (команда) |
| `ControlArea` | Область взаимодействия |
| `PopupMenuDelegate` | Делегат для пунктов всплывающего меню |
| `DetailedPopupMenuDelegate` | Детализированный делегат всплывающего меню |
| `FilterableComboBoxDelegate` | Делегат для фильтруемого ComboBox |
| `MovingItem` | Перемещаемый элемент |
| `ResizeItem` | Элемент с изменяемым размером |
| `TableSelectionManager` | Менеджер выделения таблицы |
| `IdSelectionManager` | Менеджер выделения на основе ID |
| `IndexSelectionManager` | Менеджер выделения на основе индекса |
| `LocalizationEvent` | Событие локализации |

### 1.8 GraphicsView — 2D-графика

2D-рисование на основе Canvas с матрицей трансформации.

| Компонент | Описание |
|-----------|----------|
| `GraphicsView` | Основной контейнер для 2D-графики |
| `GraphicsLayer` | Слой рисования |
| `GraphicsShapeBase` | Базовый класс для всех фигур |
| `GraphicsShapeFactoryBase` | Базовый класс для фабрик фигур |
| `GraphicsShapeFactoryManager` | Менеджер фабрик фигур |
| `Graph2d` | 2D-график |
| `MultiGraph2d` | Множественный 2D-график |
| `CanvasMatrix` | Матрица трансформации Canvas |
| `Matrix3x3` | Матрица 3x3 |
| **Фигуры** | |
| `RectangleShape` | Прямоугольник |
| `CircleShape` | Круг |
| `PolygonShape` | Полигон |
| `PolyLineShape` | Полилиния |
| `GradientPolyLineShape` | Полилиния с градиентом |
| `SegmentShape` | Сегмент |
| `SegmentBaseShape` | Базовый сегмент |
| `DiskSegmentShape` | Дисковый сегмент |
| `ArcShape` | Дуга |
| `BezierCurveShape` | Кривая Безье |
| `QuadraticCurveShape` | Квадратичная кривая |
| `ComplexShape` | Сложная фигура |
| `TextShape` | Текстовая фигура |
| `ImageShape2d` | Изображение в 2D |
| `SelectionShape` | Фигура выделения |
| `CoordinateGridShape` | Координатная сетка |
| `BoundingBox` | Ограничивающий прямоугольник |
| **Параметры** | |
| `LineParams` | Параметры линии |
| `RectangleParams` | Параметры прямоугольника |
| `CircleParams` | Параметры круга |
| `PolygonParams` | Параметры полигона |
| `ArcParams` | Параметры дуги |
| `BezierCurveParams` | Параметры кривой Безье |
| `QuadraticCurveParams` | Параметры квадратичной кривой |
| `TextParams` | Параметры текста |
| `ImageParams` | Параметры изображения |

---

## 2. imtgui — Фреймворк приложений

**Импорт:** `import imtgui 1.0`
**Путь:** `Qml/imtgui/`

Построен поверх `imtcontrols` и предоставляет полный фреймворк приложений.

### 2.1 Application — Жизненный цикл приложения

| Компонент | Описание |
|-----------|----------|
| `ApplicationMain` | Главная точка входа приложения |
| `Configurator` | Конфигурация приложения |
| `ApplicationInfoProvider` | Информация о приложении (версия, название) |
| `ApplicationEvents` | События уровня приложения |
| `PagesManager` | Управление страницами и навигация |
| `PagesProvider` | Определение и предоставление страниц |
| `LanguageProvider` | Многоязычность/локализация |
| `DesignSchemaProvider` | Управление схемой дизайна |
| `DatabaseBackupController` | Управление резервным копированием базы данных |
| `AlertMessage` | Уведомления уровня приложения |
| `ThumbnailDecorator` | Декоратор миниатюр |
| `ClientSettingsController` | Настройки клиента |

### 2.2 Panels — Области компоновки

| Компонент | Описание |
|-----------|----------|
| `TopPanel` | Верхняя область панели (шапка) |
| `TopLeftPanel` | Левая часть верхней панели |
| `TopLeftPanelDecorator` | Декоратор для левой верхней панели |
| `TopCenterPanel` | Центральная часть верхней панели |
| `TopCenterPanelDecorator` | Декоратор для центральной верхней панели |
| `TopRightPanel` | Правая часть верхней панели |
| `TopRightPanelDecorator` | Декоратор для правой верхней панели |
| `TopPanelDecorator` | Общий декоратор верхней панели |
| `MenuPanel` | Боковое меню |
| `MenuPanelButton` | Кнопка в панели меню |
| `MenuPanelButtonDecorator` | Декоратор для кнопок панели меню |
| `SubMenuPanelButtonDecorator` | Декоратор для кнопок подменю |
| `TabPanel` | Панель на основе вкладок |
| `TabPanelDecorator` | Декоратор для панели вкладок |
| `TabDelegate` | Делегат для вкладок |
| `CommandsPanel` | Панель команд/инструментов |
| `CommandsDecorator` | Декоратор для панели команд |
| `BottomPanel` | Нижняя область панели (подвал) |
| `ButtonPanel` | Панель с кнопками |
| `FilterPanelDecorator` | Декоратор панели фильтров |
| `FilterMenu` | Меню фильтров |
| `FilterDelegateBase` | Базовый класс для делегатов фильтров |
| `FieldFilterDelegate` | Фильтр на основе полей |
| `EnableableFilterDelegate` | Активируемый фильтр |
| `OptionsFilterDelegate` | Фильтр на основе опций |
| `TimeFilterDelegate` | Фильтр на основе времени |

### 2.3 View — Компоненты представления

| Компонент | Описание |
|-----------|----------|
| `ViewBase` | Базовый класс для всех представлений |
| `ElementView` | Универсальное представление элемента |
| `PageContainer` | Контейнер страницы |
| `TabView` | Представление на основе вкладок |
| `MultiPageView` | Многостраничное представление |
| `PopupView` | Всплывающее представление |
| `PopupContainer` | Контейнер всплывающего окна |
| `PopupManager` | Управление всплывающими окнами |
| `LogView` | Представление журнала |
| `SearchPage` | Страница поиска |
| `SearchComp` | Компонент поиска |
| `SearchCompLegacy` | Устаревший компонент поиска |
| `SearchResultsView` | Представление результатов поиска |
| `SearchAddressComp` | Поиск по адресу |
| `CommandsView` | Представление команд |
| `ViewCommandsDelegateBase` | Базовый класс для делегатов команд представления |
| `SimpleCommandsDecorator` | Простой декоратор команд |
| `GroupHeaderView` | Заголовок группы |
| `GroupElementView` | Элемент группы |
| `Separator` | Разделительная линия |
| `StickerView` | Представление стикера |
| `Pagination` | Постраничная навигация |
| `ServerNoConnectionView` | Представление «Нет соединения» |
| `ObjectVisualStatusProvider` | Провайдер визуального статуса |
| **Представления элементов** | |
| `TextElementView` | Отображение текста |
| `TextInputElementView` | Представление ввода текста |
| `CheckBoxElementView` | Представление флажка |
| `ComboBoxElementView` | Представление ComboBox |
| `ComboBoxGqlElementView` | Представление GraphQL ComboBox |
| `ComboBoxGqlLegacyElementView` | Устаревший GraphQL ComboBox |
| `ClearableComboBoxElementView` | Очищаемый ComboBox |
| `ComboBoxAddressElementView` | ComboBox для адресов |
| `FilterableComboBoxElementView` | Представление фильтруемого ComboBox |
| `SpinBoxElementView` | Представление SpinBox |
| `SwitchElementView` | Представление переключателя |
| `ButtonElementView` | Представление кнопки |
| `TableElementView` | Представление таблицы |
| `TreeViewElementView` | Представление дерева |
| `DateTimePickerElementView` | Представление выбора даты и времени |
| `DoubleInputElementView` | Представление ввода Double |
| `IntegerInputElementView` | Представление ввода Integer |
| `MacAddressElementView` | Представление MAC-адреса |
| `FilePickerElementView` | Представление выбора файла |
| `TimeFilterParamView` | Представление фильтра по времени |
| `ServerConnectionParamElementView` | Представление подключения к серверу |

### 2.4 Params — Редакторы параметров

| Компонент | Описание |
|-----------|----------|
| `ParamEditorBase` | Базовый класс для редакторов параметров |
| `ParamController` | Контроллер для отдельного параметра |
| `ParamsController` | Контроллер для групп параметров |
| `ParamsSetController` | Контроллер для наборов параметров |
| `ComposedParamsGui` | Составной GUI параметров |
| `TextParamEditor` | Редактор текстового параметра |
| `TextParamView` | Представление текстового параметра (только чтение) |
| `PasswordParamEditor` | Редактор параметра пароля |
| `IntegerParamEditor` | Редактор параметра Integer |
| `DoubleParamEditor` | Редактор параметра Double |
| `ConstrainedDoubleParamEditor` | Редактор Double с ограничениями |
| `SelectionParamEditor` | Редактор параметра выбора |
| `EnableableParamEditor` | Активируемый редактор параметра |
| `UrlParamEditor` | Редактор параметра URL |
| `FileNameParamEditor` | Редактор параметра имени файла |
| `TimeParamEditor` | Редактор параметра времени |
| `SchedulerParamEditor` | Редактор параметра планировщика |
| `ServerConnectionParamEditor` | Редактор подключения к серверу |
| `DatabaseAccessSettingsEditor` | Редактор настроек доступа к базе данных |
| `PatTokenParamEditor` | Редактор PAT-токена |
| `PatTokenListParam` | Параметр списка PAT-токенов |

### 2.5 Settings — Настройки

| Компонент | Описание |
|-----------|----------|
| `SettingsController` | Контроллер настроек |
| `SettingsObserver` | Наблюдатель за изменениями настроек |
| `SettingsProvider` | Провайдер настроек |
| `PreferenceDialog` | Диалог настроек |
| `Preference` | Отдельная настройка |
| `PreferenceButton` | Кнопка для настройки |
| `PageAboutProvider` | Провайдер страницы «О программе» |
| `ServerLogProvider` | Провайдер журнала сервера |
| `PatTokenSettingsController` | Настройки PAT-токена |

---

## 3. imtguigql — Интеграция с GraphQL

**Импорт:** `import imtguigql 1.0`
**Путь:** `Qml/imtguigql/`

Привязка данных и серверное взаимодействие на основе GraphQL.

| Компонент | Описание |
|-----------|----------|
| **Запросы** | |
| `GqlRequest` | Базовый запрос GraphQL |
| `GqlRequestSender` | Отправитель запросов GraphQL |
| `GqlSdlRequestSender` | Отправитель запросов на основе SDL |
| `GqlRequestDocumentDataController` | Запрос данных документа |
| `XmlHttpRequestProxy` | XHR-прокси для веба |
| **Контроллеры документов** | |
| `GqlDocumentDataController` | Контроллер данных документа |
| `GqlBasedCollectionDataController` | Контроллер данных коллекции |
| `GqlBasedCollectionDocumentService` | Сервис документов коллекции |
| `GqlBasedDataModelController` | Контроллер модели данных |
| `GqlBasedDataModelProvider` | Провайдер модели данных |
| `GqlBasedCommandsController` | Контроллер команд |
| `GqlBasedOptionManager` | Менеджер опций |
| `GqlBasedObjectVisualStatusProvider` | Провайдер визуального статуса |
| **Подписки** | |
| `SubscriptionClient` | Клиент подписок WebSocket |
| `SubscriptionManager` | Управление подписками |
| `WebSocketConnectionChecker` | Проверка соединения WebSocket |
| `WebSocketPortProvider` | Провайдер порта WebSocket |
| **Представления коллекций** | |
| `CollectionRepresentation` | Представление коллекции |
| `RemoteCollectionView` | Представление удалённой коллекции |
| `RemoteCollectionViewDialog` | Диалог удалённой коллекции |
| `SelectableCollectionEditor` | Редактор выбираемой коллекции |
| `GqlCollectionDocManagerPageView` | Страница менеджера документов коллекции |
| `GqlSingleDocCollectionPageView` | Страница коллекции одного документа |
| **ComboBox** | |
| `ComboBoxGql` | ComboBox на основе GraphQL |
| `ComboBoxGqlLegacy` | Устаревший GraphQL ComboBox |
| `ComboBoxGqlSimple` | Простой GraphQL ComboBox |
| `ComboBoxGqlSimpleElementView` | Представление простого GraphQL ComboBox |
| **Прочее** | |
| `FilterableSelectGqlDataProvider` | Фильтруемый провайдер выбора |
| `SdlObjectProvider` | Провайдер SDL-объектов |
| `MetaInfoProvider` | Провайдер метаданных |
| `ClientStyle` | Определения клиентского стиля |
| `TableViewParamController` | Контроллер параметров TableView |
| `PopupMenuDialogGql` | Всплывающее меню GraphQL |
| `PopupMenuDialogGqlLegacy` | Устаревшее всплывающее меню GraphQL |
| `RemoteFileController` | Контроллер удалённых файлов |

---

## 4. Предметно-ориентированные модули

### 4.1 imtauthgui — Аутентификация

**Импорт:** `import imtauthgui 1.0`

Вход в систему, регистрация, управление пользователями и тенантами.

Основные компоненты:
- Формы входа/регистрации
- Редактор профиля пользователя
- Редактор тенанта (роли, группы, разрешения)
- Панель пользователя

### 4.2 imtcolgui — Коллекции

**Импорт:** `import imtcolgui 1.0`

CRUD-интерфейсы на основе коллекций.

### 4.3 imtdocgui — Документы

**Импорт:** `import imtdocgui 1.0`

Управление документами, интерфейсы редактора.

### 4.4 imtchatgui — Чат

**Импорт:** `import imtchatgui 1.0`

Интерфейс чата и обмен сообщениями в реальном времени.

### 4.5 imtdeskgui — Тикеты

**Импорт:** `import imtdeskgui 1.0`

Интерфейс системы тикетов/заявок.

### 4.6 imtlicgui — Лицензирование

**Импорт:** `import imtlicgui 1.0`

Управление лицензиями и их отображение.

### 4.7 imt3dgui — 3D-визуализация

**Импорт:** `import imt3dgui 1.0`

Отображение 3D-сцен, совместимо с JQML (Three.js в вебе).

Основные компоненты:
- `View3D` — Контейнер 3D-сцены
- `Model` — 3D-модель (меш)
- `PerspectiveCamera` / `OrthographicCamera`
- `DirectionalLight` / `PointLight` / `SpotLight`
- `PrincipledMaterial` / `DefaultMaterial`
- `Node3D` — Узел 3D-сцены

### 4.8 imtgeogui — Геовизуализация

**Импорт:** `import imtgeogui 1.0`

Карты и визуализация геоданных.

---

## 5. Веб-модуль

**Путь:** `Qml/web/`

Веб-специфичные компоненты, используемые только в веб-сборке JQML.

---

## Смотрите также

- **[QML Documentation Index](QML_Documentation_Index.md)** — Обзор
- **[Getting Started](QML_Getting_Started.md)** — Начало работы
- **[JQML Guide](QML_JQML_Guide.md)** — Веб-компиляция
- **[Component Tutorials](QML_Component_Tutorials.md)** — Практические примеры
