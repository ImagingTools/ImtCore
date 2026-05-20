# QML Module Reference

Complete reference of all QML modules in the ImtCore SDK.

---

## 1. imtcontrols — Base UI Framework

**Import:** `import imtcontrols 1.0`
**Path:** `Qml/imtcontrols/`

The foundation of all UI components. Contains approximately 160 components in 7 categories.

### 1.1 Base — Fundamentals

Singletons and base types used by all other components.

| Component | Type | Description |
|-----------|------|-------------|
| `Events` | Singleton | Global event system for cross-component communication |
| `EventManager` | Singleton | Event registration and dispatch |
| `Enums` | Singleton | Shared enumerations and constants |
| `Functions` | Singleton | Utility functions (formatting, conversion) |
| `AnalyticGeometry` | Singleton | Geometry calculations (points, lines, surfaces) |
| `LinearAlgebra` | Singleton | Matrix and vector operations |
| `RelativePosition` | Singleton | Relative position calculations |
| `DesignScheme` | Singleton | Design tokens: colors, fonts, spacing |
| `BaseClass` | Type | Fundamental base type for all components |
| `BaseModel` | Type | Base for data models |
| `ControlBase` | Type | Base for interactive controls |
| `DecoratorBase` | Type | Base for decorators (decorator pattern) |
| `Decorators` | Type | Standard decorator collection |
| `DecoratorsQt` | Type | Qt-specific decorators |
| `StyleBase` | Type | Base for style definitions |
| `StyleComponents` | Type | Predefined style components |
| `MainWindow` | Type | Main window wrapper |
| `WrapItem` | Type | Item wrapper for layout purposes |
| `IconProperties` | Type | Icon configuration |
| `FontProperties` | Type | Font configuration |
| `StateManager` | Type | State-machine-based state management |
| `Icon` | JS | Icon constants and paths |
| `ButtonType` | JS | Button type constants |

**Web-specific** (`Base/web/`):

| Component | Description |
|-----------|-------------|
| `ScrollBar` | Web-compatible scrollbar |
| `ToolTip` | Web-compatible tooltip |
| `Switch` | Web-compatible switch |
| `Window` | Web-compatible window wrapper |
| `RoundButton` | Web-compatible round button |

### 1.2 Buttons

| Component | Description |
|-----------|-------------|
| `Button` | Standard button with text and/or icon |
| `TextButton` | Borderless button, text only |
| `ToolButton` | Compact button for toolbars, primarily icon-based |
| `MenuButton` | Button with dropdown menu |
| `RadioButton` | Radio button for single selection |
| `RadioBox` | Radio button grouping |
| `SegmentedButton` | Segmented selection (toggle group) |
| `ButtonDecorator` | Decorator for button customization |
| `IconButtonDecorator` | Decorator for icon buttons |
| `TextButtonDecorator` | Decorator for text buttons |
| `ToolButtonDecorator` | Decorator for tool buttons |
| `RadioButtonDecorator` | Decorator for radio buttons |
| `TopButtonDecorator` | Decorator for top panel buttons |
| `TopButtonDecoratorNew` | New decorator for top panel buttons |
| `ExternalSegmentDecorator` | Decorator for external segments |

### 1.3 Inputs — Input Fields

| Component | Description |
|-----------|-------------|
| `CustomTextField` | Text input field with validation |
| `TextFieldWithTitle` | Text input with title |
| `TextInputWithLabel` | Text input with label |
| `TextArea` | Multi-line text input field |
| `TextEditCustom` | Extended text editor |
| `CustomTextEdit` | Custom text editor |
| `SearchTextInput` | Search field with search icon and filter logic |
| `ComboBox` | Dropdown selection |
| `FilterableComboBox` | Dropdown with text filter |
| `CheckBox` | Checkbox |
| `CheckBoxMenu` | Checkbox with menu functionality |
| `SpinBox` | Numeric input field with +/- buttons |
| `SpinBoxComp` | Extended SpinBox component |
| `Slider` | Slider |
| `RangeSlider` | Range slider (min/max) |
| `DatePicker` | Date picker |
| `TimePicker` | Time picker |
| `DateTimePicker` | Date and time picker |
| `DateInput` | Date input field |
| `TimeInput` | Time input field |
| `DateTimeInput` | Date-time input field |
| `TimeIntervalPicker` | Time interval picker |
| `TimeInputDialog` | Dialog for time input |
| `Calendar` | Calendar widget |
| `CalendarView` | Calendar view |
| `CalendarFunctions` | Calendar helper functions |
| `CalendarCellDelegateBase` | Base for calendar cells |
| `CalendarCellTextDelegate` | Text delegate for calendar cells |
| `BarControl` | Bar control |
| `PathPicker` | File path picker |
| `InputActions` | Actions for input fields |
| `TextField` | Base text field |

### 1.4 Views — Display Components

| Component | Description |
|-----------|-------------|
| **Tables** | |
| `Table` | Main component for table display |
| `TableBase` | Base for tables |
| `BasicTableView` | Simple table view |
| `TableDecorator` | Table decorator |
| `TableHeaders` | Table headers |
| `TableHeaderDelegate` | Delegate for individual header cell |
| `TableHeaderParamComp` | Parameters for headers |
| `TableDelegate` | Standard table delegate |
| `TableRowDelegate` | Row delegate |
| `TableRowDelegateBase` | Base for row delegates |
| `TableRowViewer` | Row viewer |
| `TableRowCanvasViewer` | Canvas-based row viewer |
| `TablePainterRowDelegateBase` | Base for canvas row delegates |
| `TableCellDelegate` | Cell delegate |
| `TableCellDelegateBase` | Base for cell delegates |
| `TableCellDelegateSimple` | Simple cell delegate |
| `TableCellDelegateCheckable` | Cell delegate with checkbox |
| `TableCellTextDelegate` | Text cell delegate |
| `TableCellText` | Text in table cell |
| `TableCellTextWithTooltipDelegate` | Text cell with tooltip |
| `TableCellDateDelegate` | Date cell delegate |
| `TableCellIconTextDelegate` | Icon+text cell |
| `TableCellRowNumberDelegate` | Row number delegate |
| `TextLinkCellDelegate` | Clickable link in table |
| `SimpleTableDelegate` | Simplified table delegate |
| `ComboBoxCellContentComp` | ComboBox in table cell |
| `TextInputCellContentComp` | TextInput in table cell |
| `TableSelection` | Table selection management |
| `TableProperties` | Table properties |
| `TableViewParams` | Parameters for TableView |
| `TableViewItemDelegateBase` | Base for TableView delegates |
| `TableTreeView` | Table with tree structure |
| `TableTreeDecorator` | Decorator for tree tables |
| **Trees** | |
| `BasicTreeView` | Simple tree view |
| `TreeViewGql` | Tree view with GraphQL data |
| `TreeItemDelegate` | Delegate for tree items |
| `TreeViewItemDelegateBase` | Base for tree delegates |
| `TreeViewSelection` | Selection in tree view |
| `TreeViewColumn` | Column in tree view |
| `TreeViewCommandsDelegateBase` | Base for command delegates |
| **Navigation** | |
| `StackView` | Stack-based page navigation |
| `StackViewHeader` | Breadcrumb header for StackView |
| `SwipeView` | Swipe-based page navigation |
| `SplitView` | Split view |
| `Splitter` | Divider for SplitView |
| `Gallery` | Gallery view |
| **Indicators** | |
| `ProgressBar` | Progress bar |
| `ProgressRoundIndicator` | Round progress indicator |
| `BusyIndicator` | Busy indicator |
| `Loading` | Loading display |
| `Spinner` | Spinner animation |
| `ValueLevelIndicator` | Value level display |
| `IntervalBar` | Interval bar |
| **Charts** | |
| `BarChart` | Bar chart |
| `BarChartSimple` | Simplified bar chart |
| `StackedBarChart` | Stacked bar chart |
| `Piechart` | Pie chart |
| `Diagram` | Generic diagram |
| `DiagramIndicator` | Diagram indicator |
| **Miscellaneous** | |
| `BaseText` | Base text component |
| `Title` | Title/heading |
| `ItemDelegate` | Item delegate |
| `ItemDelegateDecorator` | Item delegate decorator |
| `TooltipArea` | Tooltip area |
| `CustomTooltip` | Custom tooltip |
| `CustomScrollbar` | Custom scrollbar |
| `ScrollIndicator` | Scroll indicator |
| `SwitchCustom` | Custom switch |
| `DrawingContainer` | Drawing container |
| `ExpandingContainer` | Expandable container |
| `CollapsibleRectangle` | Collapsible rectangle |
| `ChangeItemSize` | Resize component |
| `PaintedCorner` | Painted corner |
| `InputView` | Input view |
| `Arrowhead` | Arrowhead |
| `PopupDetailedDelegateDecorator` | Decorator for detail popups |
| `SubMenuPanelButtonDecorator` | Decorator for submenu panel buttons |
| `MultiPageView` | Multi-page view |

### 1.5 Dialogs — Dialog Windows

| Component | Description |
|-----------|-------------|
| `Dialog` | Standard dialog |
| `DialogNew` | New dialog style |
| `DialogDecorator` | Dialog decorator |
| `DialogButtons` | Standard dialog buttons (OK, Cancel) |
| `DialogManagerView` | Central dialog management |
| `MessageDialog` | Message dialog (info/warning/error) |
| `MessageDialogBody` | Content of a message dialog |
| `MessagePopup` | Popup message |
| `InputDialog` | Input dialog |
| `InputDialogBody` | Content of an input dialog |
| `ErrorDialog` | Error dialog |
| `CriticalErrorDialog` | Critical error dialog |
| `PopupDecorator` | Popup decorator |
| `PopupDecoratorGql` | Popup decorator with GraphQL |
| `PopupItemDelegateDecorator` | Popup item delegate decorator |
| `PopupMenuDialog` | Popup menu as dialog |
| `TopPanelDialog` | Dialog in top panel |
| `FileImportDialog` | File import dialog |
| `FileExportDialog` | File export dialog |
| `ModalDialogManager` | Manager for modal dialogs |
| `ProgressDialogManager` | Manager for progress dialogs |

### 1.6 Models — Data Models

| Component | Description |
|-----------|-------------|
| `JSONListModel` | Model from JSON data |
| `GqlModel` | Model for GraphQL data |
| `TreeItemModel` | Hierarchical tree model |
| `TreeItemModelConverter` | Converter for tree models |
| `TreeItemModelObserver` | Observer for tree model changes |
| `TreeModelBuilder` | Builder for tree models (field- and callback-based) |
| `SortFilterProxyModel` | Sort/filter proxy for models |
| `SortController` | Controller for sorting |
| `DataModelController` | Controller for data models |
| `DataModelProvider` | Provider for data models |
| `CommandsController` | Controller for commands/actions |
| `NavigationController` | Controller for navigation |
| `NavigableItem` | Navigable item |
| `PopupMenuModel` | Model for popup menus |
| `OptionManager` | Option management |
| `OptionsListAdapter` | Adapter for option lists |
| `OptionsListManager` | Manager for option lists |
| `TableProperties` | Table properties model |
| `ModelIndex` | Index in a model |
| `StateMachine` | State machine |
| `Process` | Process abstraction |
| `UuidGenerator` | UUID generator |
| `IntValidator` | Integer validation |
| `DoubleValidator` | Double validation |
| `MacAddressValidator` | MAC address validation |
| `MailRegExpValidator` | Email validation |

### 1.7 Actions — Actions & Interactions

| Component | Description |
|-----------|-------------|
| `Action` | Abstract action (command) |
| `ControlArea` | Interaction area |
| `PopupMenuDelegate` | Delegate for popup menu items |
| `DetailedPopupMenuDelegate` | Detailed popup menu delegate |
| `FilterableComboBoxDelegate` | Delegate for filterable ComboBox |
| `MovingItem` | Movable item |
| `ResizeItem` | Resizable item |
| `TableSelectionManager` | Manager for table selection |
| `IdSelectionManager` | ID-based selection manager |
| `IndexSelectionManager` | Index-based selection manager |
| `LocalizationEvent` | Localization event |

### 1.8 GraphicsView — 2D Graphics

Canvas-based 2D drawing with transformation matrix.

| Component | Description |
|-----------|-------------|
| `GraphicsView` | Main container for 2D graphics |
| `GraphicsLayer` | Drawing layer |
| `GraphicsShapeBase` | Base for all shapes |
| `GraphicsShapeFactoryBase` | Base for shape factories |
| `GraphicsShapeFactoryManager` | Manager for shape factories |
| `Graph2d` | 2D graph |
| `MultiGraph2d` | Multi 2D graph |
| `CanvasMatrix` | Canvas transformation matrix |
| `Matrix3x3` | 3x3 matrix |
| **Shapes** | |
| `RectangleShape` | Rectangle |
| `CircleShape` | Circle |
| `PolygonShape` | Polygon |
| `PolyLineShape` | Polyline |
| `GradientPolyLineShape` | Polyline with gradient |
| `SegmentShape` | Segment |
| `SegmentBaseShape` | Segment base |
| `DiskSegmentShape` | Disk segment |
| `ArcShape` | Arc |
| `BezierCurveShape` | Bézier curve |
| `QuadraticCurveShape` | Quadratic curve |
| `ComplexShape` | Complex shape |
| `TextShape` | Text shape |
| `ImageShape2d` | Image in 2D |
| `SelectionShape` | Selection shape |
| `CoordinateGridShape` | Coordinate grid |
| `BoundingBox` | Bounding box |
| **Parameters** | |
| `LineParams` | Line parameters |
| `RectangleParams` | Rectangle parameters |
| `CircleParams` | Circle parameters |
| `PolygonParams` | Polygon parameters |
| `ArcParams` | Arc parameters |
| `BezierCurveParams` | Bézier curve parameters |
| `QuadraticCurveParams` | Quadratic curve parameters |
| `TextParams` | Text parameters |
| `ImageParams` | Image parameters |

---

## 2. imtgui — Application Framework

**Import:** `import imtgui 1.0`
**Path:** `Qml/imtgui/`

Built on top of `imtcontrols` and provides the complete application framework.

### 2.1 Application — App Lifecycle

| Component | Description |
|-----------|-------------|
| `ApplicationMain` | Main entry point of the application |
| `Configurator` | App configuration |
| `ApplicationInfoProvider` | App information (version, name) |
| `ApplicationEvents` | App-wide events |
| `PagesManager` | Page management and navigation |
| `PagesProvider` | Page definition and provisioning |
| `LanguageProvider` | Multi-language/localization |
| `DesignSchemaProvider` | Design schema management |
| `DatabaseBackupController` | Database backup control |
| `AlertMessage` | App-wide notifications |
| `ThumbnailDecorator` | Thumbnail decorator |
| `ClientSettingsController` | Client settings |

### 2.2 Panels — Layout Areas

| Component | Description |
|-----------|-------------|
| `TopPanel` | Top panel area (header) |
| `TopLeftPanel` | Left part of top panel |
| `TopLeftPanelDecorator` | Decorator for left top panel |
| `TopCenterPanel` | Center part of top panel |
| `TopCenterPanelDecorator` | Decorator for center top panel |
| `TopRightPanel` | Right part of top panel |
| `TopRightPanelDecorator` | Decorator for right top panel |
| `TopPanelDecorator` | General top panel decorator |
| `MenuPanel` | Sidebar menu |
| `MenuPanelButton` | Button in menu panel |
| `MenuPanelButtonDecorator` | Decorator for menu panel buttons |
| `SubMenuPanelButtonDecorator` | Decorator for submenu buttons |
| `TabPanel` | Tab-based panel |
| `TabPanelDecorator` | Decorator for tab panel |
| `TabDelegate` | Delegate for tabs |
| `CommandsPanel` | Command/toolbar panel |
| `CommandsDecorator` | Decorator for command panel |
| `BottomPanel` | Bottom panel area (footer) |
| `ButtonPanel` | Panel with buttons |
| `FilterPanelDecorator` | Filter panel decorator |
| `FilterMenu` | Filter menu |
| `FilterDelegateBase` | Base for filter delegates |
| `FieldFilterDelegate` | Field-based filter |
| `EnableableFilterDelegate` | Enableable filter |
| `OptionsFilterDelegate` | Options-based filter |
| `TimeFilterDelegate` | Time-based filter |

### 2.3 View — View Components

| Component | Description |
|-----------|-------------|
| `ViewBase` | Base for all views |
| `ElementView` | Generic element view |
| `PageContainer` | Page container |
| `TabView` | Tab-based view |
| `MultiPageView` | Multi-page view |
| `PopupView` | Popup view |
| `PopupContainer` | Popup container |
| `PopupManager` | Popup management |
| `LogView` | Log view |
| `SearchPage` | Search page |
| `SearchComp` | Search component |
| `SearchCompLegacy` | Legacy search component |
| `SearchResultsView` | Search results view |
| `SearchAddressComp` | Address search |
| `CommandsView` | Commands view |
| `ViewCommandsDelegateBase` | Base for view command delegates |
| `SimpleCommandsDecorator` | Simple commands decorator |
| `GroupHeaderView` | Group header |
| `GroupElementView` | Group element |
| `Separator` | Separator line |
| `StickerView` | Sticker view |
| `Pagination` | Page navigation |
| `ServerNoConnectionView` | No-connection view |
| `ObjectVisualStatusProvider` | Visual status provider |
| **Element Views** | |
| `TextElementView` | Text display |
| `TextInputElementView` | Text input view |
| `CheckBoxElementView` | Checkbox view |
| `ComboBoxElementView` | ComboBox view |
| `ComboBoxGqlElementView` | GraphQL ComboBox view |
| `ComboBoxGqlLegacyElementView` | Legacy GraphQL ComboBox |
| `ClearableComboBoxElementView` | Clearable ComboBox |
| `ComboBoxAddressElementView` | Address ComboBox |
| `FilterableComboBoxElementView` | Filterable ComboBox view |
| `SpinBoxElementView` | SpinBox view |
| `SwitchElementView` | Switch view |
| `ButtonElementView` | Button view |
| `TableElementView` | Table view |
| `TreeViewElementView` | Tree view |
| `DateTimePickerElementView` | Date-time picker view |
| `DoubleInputElementView` | Double input view |
| `IntegerInputElementView` | Integer input view |
| `MacAddressElementView` | MAC address view |
| `FilePickerElementView` | File picker view |
| `TimeFilterParamView` | Time filter view |
| `ServerConnectionParamElementView` | Server connection view |

### 2.4 Params — Parameter Editors

| Component | Description |
|-----------|-------------|
| `ParamEditorBase` | Base for parameter editors |
| `ParamController` | Controller for individual parameter |
| `ParamsController` | Controller for parameter groups |
| `ParamsSetController` | Controller for parameter sets |
| `ComposedParamsGui` | Composed parameter GUI |
| `TextParamEditor` | Text parameter editor |
| `TextParamView` | Text parameter view (read-only) |
| `PasswordParamEditor` | Password parameter editor |
| `IntegerParamEditor` | Integer parameter editor |
| `DoubleParamEditor` | Double parameter editor |
| `ConstrainedDoubleParamEditor` | Constrained double editor |
| `SelectionParamEditor` | Selection parameter editor |
| `EnableableParamEditor` | Enableable parameter editor |
| `UrlParamEditor` | URL parameter editor |
| `FileNameParamEditor` | Filename parameter editor |
| `TimeParamEditor` | Time parameter editor |
| `SchedulerParamEditor` | Scheduler parameter editor |
| `ServerConnectionParamEditor` | Server connection editor |
| `DatabaseAccessSettingsEditor` | Database access editor |
| `PatTokenParamEditor` | PAT token editor |
| `PatTokenListParam` | PAT token list parameter |

### 2.5 Settings

| Component | Description |
|-----------|-------------|
| `SettingsController` | Settings controller |
| `SettingsObserver` | Observer for settings changes |
| `SettingsProvider` | Settings provider |
| `PreferenceDialog` | Settings dialog |
| `Preference` | Individual setting |
| `PreferenceButton` | Button for setting |
| `PageAboutProvider` | About page provider |
| `ServerLogProvider` | Server log provider |
| `PatTokenSettingsController` | PAT token settings |

---

## 3. imtguigql — GraphQL Integration

**Import:** `import imtguigql 1.0`
**Path:** `Qml/imtguigql/`

GraphQL-based data binding and server communication.

| Component | Description |
|-----------|-------------|
| **Requests** | |
| `GqlRequest` | Base GraphQL request |
| `GqlRequestSender` | GraphQL request sender |
| `GqlSdlRequestSender` | SDL-based request sender |
| `GqlRequestDocumentDataController` | Request for document data |
| `XmlHttpRequestProxy` | XHR proxy for web |
| **Document Controllers** | |
| `GqlDocumentDataController` | Document data controller |
| `GqlBasedCollectionDataController` | Collection data controller |
| `GqlBasedCollectionDocumentService` | Collection document service |
| `GqlBasedDataModelController` | Data model controller |
| `GqlBasedDataModelProvider` | Data model provider |
| `GqlBasedCommandsController` | Commands controller |
| `GqlBasedOptionManager` | Options manager |
| `GqlBasedObjectVisualStatusProvider` | Visual status provider |
| **Subscriptions** | |
| `SubscriptionClient` | WebSocket subscription client |
| `SubscriptionManager` | Subscription management |
| `WebSocketConnectionChecker` | WebSocket connection checker |
| `WebSocketPortProvider` | WebSocket port provider |
| **Collection Views** | |
| `CollectionRepresentation` | Collection representation |
| `RemoteCollectionView` | Remote collection view |
| `RemoteCollectionViewDialog` | Remote collection dialog |
| `SelectableCollectionEditor` | Selectable collection editor |
| `GqlCollectionDocManagerPageView` | Collection doc manager page |
| `GqlSingleDocCollectionPageView` | Single document collection page |
| **ComboBoxes** | |
| `ComboBoxGql` | GraphQL-based ComboBox |
| `ComboBoxGqlLegacy` | Legacy GraphQL ComboBox |
| `ComboBoxGqlSimple` | Simple GraphQL ComboBox |
| `ComboBoxGqlSimpleElementView` | View for simple GraphQL ComboBox |
| **Miscellaneous** | |
| `FilterableSelectGqlDataProvider` | Filterable select provider |
| `SdlObjectProvider` | SDL object provider |
| `MetaInfoProvider` | Metadata provider |
| `ClientStyle` | Client style definitions |
| `TableViewParamController` | TableView parameter controller |
| `PopupMenuDialogGql` | GraphQL popup menu |
| `PopupMenuDialogGqlLegacy` | Legacy GraphQL popup menu |
| `RemoteFileController` | Remote file controller |

---

## 4. Domain-Specific Modules

### 4.1 imtauthgui — Authentication

**Import:** `import imtauthgui 1.0`

Login, registration, user and tenant management.

Key components:
- Login/registration forms
- User profile editor
- Tenant editor (roles, groups, permissions)
- User panel

### 4.2 imtcolgui — Collections

**Import:** `import imtcolgui 1.0`

Collection-based CRUD interfaces.

### 4.3 imtdocgui — Documents

**Import:** `import imtdocgui 1.0`

Document management, editor interfaces.

### 4.4 imtchatgui — Chat

**Import:** `import imtchatgui 1.0`

Chat interface and real-time messaging.

### 4.5 imtdeskgui — Ticketing

**Import:** `import imtdeskgui 1.0`

Ticket/desk system interface.

### 4.6 imtlicgui — Licensing

**Import:** `import imtlicgui 1.0`

License management and display.

### 4.7 imt3dgui — 3D Visualization

**Import:** `import imt3dgui 1.0`

3D scene rendering, compatible with JQML (Three.js on web).

Key components:
- `View3D` — 3D scene container
- `Model` — 3D model (mesh)
- `PerspectiveCamera` / `OrthographicCamera`
- `DirectionalLight` / `PointLight` / `SpotLight`
- `PrincipledMaterial` / `DefaultMaterial`
- `Node3D` — 3D scene node

### 4.8 imtgeogui — Geo Visualization

**Import:** `import imtgeogui 1.0`

Maps and geodata visualization.

---

## 5. Web Module

**Path:** `Qml/web/`

Web-specific components used only in the JQML web build.

---

## See Also

- **[QML Documentation Index](QML_Documentation_Index.md)** — Overview
- **[Getting Started](QML_Getting_Started.md)** — Getting started
- **[JQML Guide](QML_JQML_Guide.md)** — Web compilation
- **[Component Tutorials](QML_Component_Tutorials.md)** — Practical examples
