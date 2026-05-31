# QML Modul-Referenz

Vollständige Referenz aller QML-Module im ImtCore SDK.

---

## 1. imtcontrols — Basis-UI-Framework

**Import:** `import imtcontrols 1.0`
**Pfad:** `Qml/imtcontrols/`

Das Fundament aller UI-Komponenten. Enthält ca. 160 Komponenten in 7 Kategorien.

### 1.1 Base — Grundlagen

Singletons und Basistypen, die von allen anderen Komponenten genutzt werden.

| Komponente | Typ | Beschreibung |
|-----------|-----|-------------|
| `Events` | Singleton | Globales Event-System für komponentenübergreifende Kommunikation |
| `EventManager` | Singleton | Event-Registrierung und -Versand |
| `Enums` | Singleton | Gemeinsame Enumerationen und Konstanten |
| `Functions` | Singleton | Utility-Funktionen (Formatierung, Konvertierung) |
| `AnalyticGeometry` | Singleton | Geometrie-Berechnungen (Punkte, Linien, Flächen) |
| `LinearAlgebra` | Singleton | Matrix- und Vektor-Operationen |
| `RelativePosition` | Singleton | Relative Positionsberechnungen |
| `DesignScheme` | Singleton | Design-Tokens: Farben, Schriften, Abstände |
| `BaseClass` | Typ | Grundlagen-Basistyp für alle Komponenten |
| `BaseModel` | Typ | Basis für Datenmodelle |
| `ControlBase` | Typ | Basis für interaktive Steuerelemente |
| `DecoratorBase` | Typ | Basis für Dekoratoren (Decorator-Pattern) |
| `Decorators` | Typ | Standard-Dekoratoren-Sammlung |
| `DecoratorsQt` | Typ | Qt-spezifische Dekoratoren |
| `StyleBase` | Typ | Basis für Stil-Definitionen |
| `StyleComponents` | Typ | Vordefinierte Stil-Komponenten |
| `MainWindow` | Typ | Hauptfenster-Wrapper |
| `WrapItem` | Typ | Item-Wrapper für Layout-Zwecke |
| `IconProperties` | Typ | Icon-Konfiguration |
| `FontProperties` | Typ | Schrift-Konfiguration |
| `StateManager` | Typ | State-Machine-basiertes Zustandsmanagement |
| `Icon` | JS | Icon-Konstanten und -Pfade |
| `ButtonType` | JS | Button-Typ-Konstanten |

**Web-spezifisch** (`Base/web/`):

| Komponente | Beschreibung |
|-----------|-------------|
| `ScrollBar` | Web-kompatibler Scrollbalken |
| `ToolTip` | Web-kompatibler Tooltip |
| `Switch` | Web-kompatibler Schalter |
| `Window` | Web-kompatibler Fenster-Wrapper |
| `RoundButton` | Web-kompatibler runder Button |

### 1.2 Buttons — Schaltflächen

| Komponente | Beschreibung |
|-----------|-------------|
| `Button` | Standard-Button mit Text und/oder Icon |
| `TextButton` | Button ohne Rahmen, nur Text |
| `ToolButton` | Kompakter Button für Toolbars, primär Icon-basiert |
| `MenuButton` | Button mit Dropdown-Menü |
| `RadioButton` | Radio-Button für Einzelauswahl |
| `RadioBox` | Gruppierung von Radio-Buttons |
| `SegmentedButton` | Segmentierte Auswahl (Toggle-Gruppe) |
| `ButtonDecorator` | Dekorator für Button-Anpassung |
| `IconButtonDecorator` | Dekorator für Icon-Buttons |
| `TextButtonDecorator` | Dekorator für Text-Buttons |
| `ToolButtonDecorator` | Dekorator für Tool-Buttons |
| `RadioButtonDecorator` | Dekorator für Radio-Buttons |
| `TopButtonDecorator` | Dekorator für Top-Panel-Buttons |
| `TopButtonDecoratorNew` | Neuer Dekorator für Top-Panel-Buttons |
| `ExternalSegmentDecorator` | Dekorator für externe Segmente |

### 1.3 Inputs — Eingabefelder

| Komponente | Beschreibung |
|-----------|-------------|
| `CustomTextField` | Text-Eingabefeld mit Validierung |
| `TextFieldWithTitle` | Text-Eingabe mit Überschrift |
| `TextInputWithLabel` | Text-Eingabe mit Label |
| `TextArea` | Mehrzeiliges Text-Eingabefeld |
| `TextEditCustom` | Erweiterter Text-Editor |
| `CustomTextEdit` | Benutzerdefinierter Text-Editor |
| `SearchTextInput` | Suchfeld mit Such-Icon und Filter-Logik |
| `ComboBox` | Dropdown-Auswahl |
| `FilterableComboBox` | Dropdown mit Textfilter |
| `CheckBox` | Kontrollkästchen |
| `CheckBoxMenu` | CheckBox mit Menü-Funktionalität |
| `SpinBox` | Numerisches Eingabefeld mit +/- Buttons |
| `SpinBoxComp` | Erweiterte SpinBox-Komponente |
| `Slider` | Schieberegler |
| `RangeSlider` | Bereichs-Schieberegler (Min/Max) |
| `DatePicker` | Datum-Auswahl |
| `TimePicker` | Zeit-Auswahl |
| `DateTimePicker` | Datum- und Zeit-Auswahl |
| `DateInput` | Datum-Eingabefeld |
| `TimeInput` | Zeit-Eingabefeld |
| `DateTimeInput` | Datum-Zeit-Eingabefeld |
| `TimeIntervalPicker` | Zeitintervall-Auswahl |
| `TimeInputDialog` | Dialog für Zeit-Eingabe |
| `Calendar` | Kalender-Widget |
| `CalendarView` | Kalender-Ansicht |
| `CalendarFunctions` | Kalender-Hilfsfunktionen |
| `CalendarCellDelegateBase` | Basis für Kalender-Zellen |
| `CalendarCellTextDelegate` | Text-Delegate für Kalender-Zellen |
| `BarControl` | Balken-Steuerung |
| `PathPicker` | Dateipfad-Auswahl |
| `InputActions` | Aktionen für Eingabefelder |
| `TextField` | Basis-Textfeld |

### 1.4 Views — Anzeige-Komponenten

| Komponente | Beschreibung |
|-----------|-------------|
| **Tabellen** | |
| `Table` | Hauptkomponente für Tabellendarstellung |
| `TableBase` | Basis für Tabellen |
| `BasicTableView` | Einfache Tabellen-Ansicht |
| `TableDecorator` | Tabellen-Dekorator |
| `TableHeaders` | Tabellen-Kopfzeilen |
| `TableHeaderDelegate` | Delegate für einzelne Kopfzelle |
| `TableHeaderParamComp` | Parameter für Kopfzeilen |
| `TableDelegate` | Standard-Tabellen-Delegate |
| `TableRowDelegate` | Zeilen-Delegate |
| `TableRowDelegateBase` | Basis für Zeilen-Delegates |
| `TableRowViewer` | Zeilen-Anzeige |
| `TableRowCanvasViewer` | Canvas-basierte Zeilen-Anzeige |
| `TablePainterRowDelegateBase` | Basis für Canvas-Zeilen-Delegates |
| `TableCellDelegate` | Zellen-Delegate |
| `TableCellDelegateBase` | Basis für Zellen-Delegates |
| `TableCellDelegateSimple` | Einfacher Zellen-Delegate |
| `TableCellDelegateCheckable` | Zellen-Delegate mit CheckBox |
| `TableCellTextDelegate` | Text-Zellen-Delegate |
| `TableCellText` | Text in Tabellenzelle |
| `TableCellTextWithTooltipDelegate` | Text-Zelle mit Tooltip |
| `TableCellDateDelegate` | Datum-Zellen-Delegate |
| `TableCellIconTextDelegate` | Icon+Text-Zelle |
| `TableCellRowNumberDelegate` | Zeilennummer-Delegate |
| `TextLinkCellDelegate` | Klickbarer Link in Tabelle |
| `SimpleTableDelegate` | Vereinfachter Tabellen-Delegate |
| `ComboBoxCellContentComp` | ComboBox in Tabellenzelle |
| `TextInputCellContentComp` | TextInput in Tabellenzelle |
| `TableSelection` | Tabellen-Auswahl-Verwaltung |
| `TableProperties` | Tabellen-Eigenschaften |
| `TableViewParams` | Parameter für TableView |
| `TableViewItemDelegateBase` | Basis für TableView-Delegates |
| `TableTreeView` | Tabelle mit Baum-Struktur |
| `TableTreeDecorator` | Dekorator für Tree-Tabellen |
| **Bäume** | |
| `BasicTreeView` | Einfache Baum-Ansicht |
| `TreeViewGql` | Baum-Ansicht mit GraphQL-Daten |
| `TreeItemDelegate` | Delegate für Baum-Einträge |
| `TreeViewItemDelegateBase` | Basis für Baum-Delegates |
| `TreeViewSelection` | Auswahl in Baum-Ansicht |
| `TreeViewColumn` | Spalte in Baum-Ansicht |
| `TreeViewCommandsDelegateBase` | Basis für Befehls-Delegates |
| **Navigation** | |
| `StackView` | Stapel-basierte Seitennavigation |
| `StackViewHeader` | Breadcrumb-Header für StackView |
| `SwipeView` | Swipe-basierte Seitennavigation |
| `SplitView` | Geteilte Ansicht |
| `Splitter` | Trennlinie für SplitView |
| `Gallery` | Galerie-Ansicht |
| **Indikatoren** | |
| `ProgressBar` | Fortschrittsbalken |
| `ProgressRoundIndicator` | Runder Fortschritts-Indikator |
| `BusyIndicator` | Lade-Indikator |
| `Loading` | Lade-Anzeige |
| `Spinner` | Spinner-Animation |
| `ValueLevelIndicator` | Wert-Pegel-Anzeige |
| `IntervalBar` | Intervall-Balken |
| **Diagramme** | |
| `BarChart` | Balkendiagramm |
| `BarChartSimple` | Vereinfachtes Balkendiagramm |
| `StackedBarChart` | Gestapeltes Balkendiagramm |
| `Piechart` | Kreisdiagramm |
| `Diagram` | Generisches Diagramm |
| `DiagramIndicator` | Diagramm-Indikator |
| **Sonstiges** | |
| `BaseText` | Basis-Text-Komponente |
| `Title` | Überschrift |
| `ItemDelegate` | Element-Delegate |
| `ItemDelegateDecorator` | Element-Delegate-Dekorator |
| `TooltipArea` | Tooltip-Bereich |
| `CustomTooltip` | Benutzerdefinierter Tooltip |
| `CustomScrollbar` | Benutzerdefinierter Scrollbalken |
| `ScrollIndicator` | Scroll-Indikator |
| `SwitchCustom` | Benutzerdefinierter Schalter |
| `DrawingContainer` | Zeichen-Container |
| `ExpandingContainer` | Erweiterbarer Container |
| `CollapsibleRectangle` | Zusammenklappbares Rechteck |
| `ChangeItemSize` | Größenänderungs-Komponente |
| `PaintedCorner` | Gemalte Ecke |
| `InputView` | Eingabe-Ansicht |
| `Arrowhead` | Pfeilspitze |
| `PopupDetailedDelegateDecorator` | Dekorator für Detail-Popups |
| `SubMenuPanelButtonDecorator` | Dekorator für Untermenü-Panel-Buttons |
| `MultiPageView` | Mehrseitige Ansicht |

### 1.5 Dialogs — Dialogfenster

| Komponente | Beschreibung |
|-----------|-------------|
| `Dialog` | Standard-Dialog |
| `DialogNew` | Neuer Dialog-Stil |
| `DialogDecorator` | Dialog-Dekorator |
| `DialogButtons` | Standard-Dialog-Buttons (OK, Abbrechen) |
| `DialogManagerView` | Zentrale Dialog-Verwaltung |
| `MessageDialog` | Nachricht-Dialog (Info/Warnung/Fehler) |
| `MessageDialogBody` | Inhalt eines Nachricht-Dialogs |
| `MessagePopup` | Popup-Nachricht |
| `InputDialog` | Eingabe-Dialog |
| `InputDialogBody` | Inhalt eines Eingabe-Dialogs |
| `ErrorDialog` | Fehler-Dialog |
| `CriticalErrorDialog` | Kritischer Fehler-Dialog |
| `PopupDecorator` | Popup-Dekorator |
| `PopupDecoratorGql` | Popup-Dekorator mit GraphQL |
| `PopupItemDelegateDecorator` | Popup-Item-Delegate-Dekorator |
| `PopupMenuDialog` | Popup-Menü als Dialog |
| `TopPanelDialog` | Dialog im Top-Panel |
| `FileImportDialog` | Datei-Import-Dialog |
| `FileExportDialog` | Datei-Export-Dialog |
| `ModalDialogManager` | Manager für modale Dialoge |
| `ProgressDialogManager` | Manager für Fortschritts-Dialoge |

### 1.6 Models — Datenmodelle

| Komponente | Beschreibung |
|-----------|-------------|
| `JSONListModel` | Modell aus JSON-Daten |
| `GqlModel` | Modell für GraphQL-Daten |
| `TreeItemModel` | Hierarchisches Baum-Modell |
| `TreeItemModelConverter` | Konverter für Baum-Modelle |
| `TreeItemModelObserver` | Observer für Baum-Modell-Änderungen |
| `TreeModelBuilder` | Builder für Baum-Modelle (Feld- und Callback-basiert) |
| `SortFilterProxyModel` | Sort/Filter-Proxy für Modelle |
| `SortController` | Controller für Sortierung |
| `DataModelController` | Controller für Daten-Modelle |
| `DataModelProvider` | Provider für Daten-Modelle |
| `CommandsController` | Controller für Befehle/Aktionen |
| `NavigationController` | Controller für Navigation |
| `NavigableItem` | Navigierbares Element |
| `PopupMenuModel` | Modell für Popup-Menüs |
| `OptionManager` | Verwaltung von Optionen |
| `OptionsListAdapter` | Adapter für Options-Listen |
| `OptionsListManager` | Manager für Options-Listen |
| `TableProperties` | Tabellen-Eigenschaften-Modell |
| `ModelIndex` | Index in einem Modell |
| `StateMachine` | Zustandsmaschine |
| `Process` | Prozess-Abstraktion |
| `UuidGenerator` | UUID-Generator |
| `IntValidator` | Integer-Validierung |
| `DoubleValidator` | Double-Validierung |
| `MacAddressValidator` | MAC-Adress-Validierung |
| `MailRegExpValidator` | E-Mail-Validierung |

### 1.7 Actions — Aktionen & Interaktionen

| Komponente | Beschreibung |
|-----------|-------------|
| `Action` | Abstrakte Aktion (Befehl) |
| `ControlArea` | Interaktionsbereich |
| `PopupMenuDelegate` | Delegate für Popup-Menü-Einträge |
| `DetailedPopupMenuDelegate` | Detaillierter Popup-Menü-Delegate |
| `FilterableComboBoxDelegate` | Delegate für filterbare ComboBox |
| `MovingItem` | Verschiebbares Element |
| `ResizeItem` | Größenveränderbares Element |
| `TableSelectionManager` | Manager für Tabellen-Auswahl |
| `IdSelectionManager` | ID-basierter Auswahl-Manager |
| `IndexSelectionManager` | Index-basierter Auswahl-Manager |
| `LocalizationEvent` | Lokalisierungs-Event |

### 1.8 GraphicsView — 2D-Grafik

Canvas-basierte 2D-Zeichnung mit Transformations-Matrix.

| Komponente | Beschreibung |
|-----------|-------------|
| `GraphicsView` | Haupt-Container für 2D-Grafiken |
| `GraphicsLayer` | Zeichen-Ebene |
| `GraphicsShapeBase` | Basis für alle Formen |
| `GraphicsShapeFactoryBase` | Basis für Shape-Factories |
| `GraphicsShapeFactoryManager` | Manager für Shape-Factories |
| `Graph2d` | 2D-Graph |
| `MultiGraph2d` | Mehrfach-2D-Graph |
| `CanvasMatrix` | Canvas-Transformationsmatrix |
| `Matrix3x3` | 3x3-Matrix |
| **Formen** | |
| `RectangleShape` | Rechteck |
| `CircleShape` | Kreis |
| `PolygonShape` | Polygon |
| `PolyLineShape` | Polylinie |
| `GradientPolyLineShape` | Polylinie mit Gradient |
| `SegmentShape` | Segment |
| `SegmentBaseShape` | Segment-Basis |
| `DiskSegmentShape` | Disk-Segment |
| `ArcShape` | Bogen |
| `BezierCurveShape` | Bézier-Kurve |
| `QuadraticCurveShape` | Quadratische Kurve |
| `ComplexShape` | Komplexe Form |
| `TextShape` | Text-Form |
| `ImageShape2d` | Bild in 2D |
| `SelectionShape` | Auswahl-Form |
| `CoordinateGridShape` | Koordinatengitter |
| `BoundingBox` | Begrenzungsrahmen |
| **Parameter** | |
| `LineParams` | Linien-Parameter |
| `RectangleParams` | Rechteck-Parameter |
| `CircleParams` | Kreis-Parameter |
| `PolygonParams` | Polygon-Parameter |
| `ArcParams` | Bogen-Parameter |
| `BezierCurveParams` | Bézier-Kurven-Parameter |
| `QuadraticCurveParams` | Quadratische Kurven-Parameter |
| `TextParams` | Text-Parameter |
| `ImageParams` | Bild-Parameter |

---

## 2. imtgui — Applikations-Framework

**Import:** `import imtgui 1.0`
**Pfad:** `Qml/imtgui/`

Baut auf `imtcontrols` auf und bietet das komplette Applikations-Framework.

### 2.1 Application — App-Lebenszyklus

| Komponente | Beschreibung |
|-----------|-------------|
| `ApplicationMain` | Haupteinstiegspunkt der Applikation |
| `Configurator` | App-Konfiguration |
| `ApplicationInfoProvider` | App-Informationen (Version, Name) |
| `ApplicationEvents` | App-weite Events |
| `PagesManager` | Seiten-Verwaltung und -Navigation |
| `PagesProvider` | Seiten-Definition und -Bereitstellung |
| `LanguageProvider` | Mehrsprachigkeit/Lokalisierung |
| `DesignSchemaProvider` | Design-Schema-Verwaltung |
| `DatabaseBackupController` | Datenbank-Backup-Steuerung |
| `AlertMessage` | App-weite Benachrichtigungen |
| `ThumbnailDecorator` | Vorschaubild-Dekorator |
| `ClientSettingsController` | Client-Einstellungen |

### 2.2 Panels — Layout-Bereiche

| Komponente | Beschreibung |
|-----------|-------------|
| `TopPanel` | Oberer Panel-Bereich (Header) |
| `TopLeftPanel` | Linker Teil des Top-Panels |
| `TopLeftPanelDecorator` | Dekorator für linken Top-Panel |
| `TopCenterPanel` | Mittlerer Teil des Top-Panels |
| `TopCenterPanelDecorator` | Dekorator für mittleren Top-Panel |
| `TopRightPanel` | Rechter Teil des Top-Panels |
| `TopRightPanelDecorator` | Dekorator für rechten Top-Panel |
| `TopPanelDecorator` | Allgemeiner Top-Panel-Dekorator |
| `MenuPanel` | Seitenleisten-Menü |
| `MenuPanelButton` | Button im Menü-Panel |
| `MenuPanelButtonDecorator` | Dekorator für Menü-Panel-Buttons |
| `SubMenuPanelButtonDecorator` | Dekorator für Untermenü-Buttons |
| `TabPanel` | Tab-basiertes Panel |
| `TabPanelDecorator` | Dekorator für Tab-Panel |
| `TabDelegate` | Delegate für Tabs |
| `CommandsPanel` | Befehls-/Toolbar-Panel |
| `CommandsDecorator` | Dekorator für Befehls-Panel |
| `BottomPanel` | Unterer Panel-Bereich (Footer) |
| `ButtonPanel` | Panel mit Buttons |
| `FilterPanelDecorator` | Filter-Panel-Dekorator |
| `FilterMenu` | Filter-Menü |
| `FilterDelegateBase` | Basis für Filter-Delegates |
| `FieldFilterDelegate` | Feld-basierter Filter |
| `EnableableFilterDelegate` | Aktivierbarer Filter |
| `OptionsFilterDelegate` | Options-basierter Filter |
| `TimeFilterDelegate` | Zeit-basierter Filter |

### 2.3 View — Ansichts-Komponenten

| Komponente | Beschreibung |
|-----------|-------------|
| `ViewBase` | Basis für alle Views |
| `ElementView` | Generischer Element-View |
| `PageContainer` | Seiten-Container |
| `TabView` | Tab-basierte Ansicht |
| `MultiPageView` | Mehrseitige Ansicht |
| `PopupView` | Popup-Ansicht |
| `PopupContainer` | Popup-Container |
| `PopupManager` | Popup-Verwaltung |
| `LogView` | Log-Ansicht |
| `SearchPage` | Such-Seite |
| `SearchComp` | Such-Komponente |
| `SearchCompLegacy` | Legacy-Such-Komponente |
| `SearchResultsView` | Suchergebnis-Ansicht |
| `SearchAddressComp` | Adress-Suche |
| `CommandsView` | Befehls-Ansicht |
| `ViewCommandsDelegateBase` | Basis für View-Befehls-Delegates |
| `SimpleCommandsDecorator` | Einfacher Befehls-Dekorator |
| `GroupHeaderView` | Gruppen-Kopfzeile |
| `GroupElementView` | Gruppen-Element |
| `Separator` | Trennlinie |
| `StickerView` | Sticker-Ansicht |
| `Pagination` | Seitennavigation |
| `ServerNoConnectionView` | Keine-Verbindung-Ansicht |
| `ObjectVisualStatusProvider` | Visueller Status-Provider |
| **Element-Views** | |
| `TextElementView` | Text-Anzeige |
| `TextInputElementView` | Text-Eingabe-View |
| `CheckBoxElementView` | CheckBox-View |
| `ComboBoxElementView` | ComboBox-View |
| `ComboBoxGqlElementView` | GraphQL-ComboBox-View |
| `ComboBoxGqlLegacyElementView` | Legacy-GraphQL-ComboBox |
| `ClearableComboBoxElementView` | Löschbare ComboBox |
| `ComboBoxAddressElementView` | Adress-ComboBox |
| `FilterableComboBoxElementView` | Filterbare ComboBox-View |
| `SpinBoxElementView` | SpinBox-View |
| `SwitchElementView` | Switch-View |
| `ButtonElementView` | Button-View |
| `TableElementView` | Tabellen-View |
| `TreeViewElementView` | Baum-View |
| `DateTimePickerElementView` | Datum-Zeit-Picker-View |
| `DoubleInputElementView` | Double-Eingabe-View |
| `IntegerInputElementView` | Integer-Eingabe-View |
| `MacAddressElementView` | MAC-Adress-View |
| `FilePickerElementView` | Datei-Picker-View |
| `TimeFilterParamView` | Zeit-Filter-View |
| `ServerConnectionParamElementView` | Server-Verbindungs-View |

### 2.4 Params — Parameter-Editoren

| Komponente | Beschreibung |
|-----------|-------------|
| `ParamEditorBase` | Basis für Parameter-Editoren |
| `ParamController` | Controller für einzelnen Parameter |
| `ParamsController` | Controller für Parameter-Gruppen |
| `ParamsSetController` | Controller für Parameter-Sets |
| `ComposedParamsGui` | Zusammengesetzte Parameter-GUI |
| `TextParamEditor` | Text-Parameter-Editor |
| `TextParamView` | Text-Parameter-Ansicht (readonly) |
| `PasswordParamEditor` | Passwort-Parameter-Editor |
| `IntegerParamEditor` | Integer-Parameter-Editor |
| `DoubleParamEditor` | Double-Parameter-Editor |
| `ConstrainedDoubleParamEditor` | Beschränkter Double-Editor |
| `SelectionParamEditor` | Auswahl-Parameter-Editor |
| `EnableableParamEditor` | Aktivierbarer Parameter-Editor |
| `UrlParamEditor` | URL-Parameter-Editor |
| `FileNameParamEditor` | Dateiname-Parameter-Editor |
| `TimeParamEditor` | Zeit-Parameter-Editor |
| `SchedulerParamEditor` | Scheduler-Parameter-Editor |
| `ServerConnectionParamEditor` | Server-Verbindungs-Editor |
| `DatabaseAccessSettingsEditor` | Datenbank-Zugangs-Editor |
| `PatTokenParamEditor` | PAT-Token-Editor |
| `PatTokenListParam` | PAT-Token-Listen-Parameter |

### 2.5 Settings — Einstellungen

| Komponente | Beschreibung |
|-----------|-------------|
| `SettingsController` | Einstellungen-Controller |
| `SettingsObserver` | Observer für Einstellungen-Änderungen |
| `SettingsProvider` | Einstellungen-Provider |
| `PreferenceDialog` | Einstellungen-Dialog |
| `Preference` | Einzelne Einstellung |
| `PreferenceButton` | Button für Einstellung |
| `PageAboutProvider` | Über-Seite-Provider |
| `ServerLogProvider` | Server-Log-Provider |
| `PatTokenSettingsController` | PAT-Token-Einstellungen |

---

## 3. imtguigql — GraphQL-Integration

**Import:** `import imtguigql 1.0`
**Pfad:** `Qml/imtguigql/`

GraphQL-basierte Datenanbindung und Server-Kommunikation.

| Komponente | Beschreibung |
|-----------|-------------|
| **Requests** | |
| `GqlRequest` | Basis GraphQL-Request |
| `GqlRequestSender` | GraphQL-Request-Sender |
| `GqlSdlRequestSender` | SDL-basierter Request-Sender |
| `GqlRequestDocumentDataController` | Request für Document-Daten |
| `XmlHttpRequestProxy` | XHR-Proxy für Web |
| **Document-Controller** | |
| `GqlDocumentDataController` | Document-Daten-Controller |
| `GqlBasedCollectionDataController` | Collection-Daten-Controller |
| `GqlBasedCollectionDocumentService` | Collection-Document-Service |
| `GqlBasedDataModelController` | Daten-Modell-Controller |
| `GqlBasedDataModelProvider` | Daten-Modell-Provider |
| `GqlBasedCommandsController` | Befehls-Controller |
| `GqlBasedOptionManager` | Options-Manager |
| `GqlBasedObjectVisualStatusProvider` | Visueller Status-Provider |
| **Subscriptions** | |
| `SubscriptionClient` | WebSocket-Subscription-Client |
| `SubscriptionManager` | Subscription-Verwaltung |
| `WebSocketConnectionChecker` | WebSocket-Verbindungsprüfung |
| `WebSocketPortProvider` | WebSocket-Port-Provider |
| **Collection-Views** | |
| `CollectionRepresentation` | Collection-Darstellung |
| `RemoteCollectionView` | Remote-Collection-Ansicht |
| `RemoteCollectionViewDialog` | Remote-Collection-Dialog |
| `SelectableCollectionEditor` | Auswählbare Collection-Editor |
| `GqlCollectionDocManagerPageView` | Collection-Doc-Manager-Seite |
| `GqlSingleDocCollectionPageView` | Einzeldokument-Collection-Seite |
| **ComboBoxen** | |
| `ComboBoxGql` | GraphQL-basierte ComboBox |
| `ComboBoxGqlLegacy` | Legacy GraphQL-ComboBox |
| `ComboBoxGqlSimple` | Einfache GraphQL-ComboBox |
| `ComboBoxGqlSimpleElementView` | View für einfache GraphQL-ComboBox |
| **Sonstiges** | |
| `FilterableSelectGqlDataProvider` | Filterbarer Select-Provider |
| `SdlObjectProvider` | SDL-Object-Provider |
| `MetaInfoProvider` | Metadaten-Provider |
| `ClientStyle` | Client-Stil-Definitionen |
| `TableViewParamController` | TableView-Parameter-Controller |
| `PopupMenuDialogGql` | GraphQL-Popup-Menü |
| `PopupMenuDialogGqlLegacy` | Legacy GraphQL-Popup-Menü |
| `RemoteFileController` | Remote-Datei-Controller |

---

## 4. Domain-spezifische Module

### 4.1 imtauthgui — Authentifizierung

**Import:** `import imtauthgui 1.0`

Login, Registrierung, Benutzer- und Tenant-Verwaltung.

Wichtige Komponenten:
- Login-/Registrierungs-Formulare
- Benutzer-Profil-Editor
- Tenant-Editor (Rollen, Gruppen, Berechtigungen)
- User-Panel

### 4.2 imtcolgui — Collections

**Import:** `import imtcolgui 1.0`

Collection-basierte CRUD-Oberflächen.

### 4.3 imtdocgui — Dokumente

**Import:** `import imtdocgui 1.0`

Dokumenten-Management, Editor-Oberflächen.

### 4.4 imtchatgui — Chat

**Import:** `import imtchatgui 1.0`

Chat-Oberfläche und Echtzeit-Messaging.

### 4.5 imtdeskgui — Ticketing

**Import:** `import imtdeskgui 1.0`

Ticket-/Desk-System-Oberfläche.

### 4.6 imtlicgui — Lizenzen

**Import:** `import imtlicgui 1.0`

Lizenz-Management und -Anzeige.

### 4.7 imt3dgui — 3D-Visualisierung

**Import:** `import imt3dgui 1.0`

3D-Szenen-Darstellung, kompatibel mit JQML (Three.js im Web).

Wichtige Komponenten:
- `View3D` — 3D-Szenen-Container
- `Model` — 3D-Modell (Mesh)
- `PerspectiveCamera` / `OrthographicCamera`
- `DirectionalLight` / `PointLight` / `SpotLight`
- `PrincipledMaterial` / `DefaultMaterial`
- `Node3D` — 3D-Szenen-Knoten

### 4.8 imtgeogui — Geo-Visualisierung

**Import:** `import imtgeogui 1.0`

Karten und Geo-Daten-Darstellung.

---

## 5. Web-Modul

**Pfad:** `Qml/web/`

Web-spezifische Komponenten, die nur im JQML-Web-Build verwendet werden.

---

## Siehe auch

- **[QML Documentation Index](QML_Documentation_Index.md)** — Übersicht
- **[Getting Started](QML_Getting_Started.md)** — Einstieg
- **[JQML Guide](QML_JQML_Guide.md)** — Web-Kompilierung
- **[Komponenten-Tutorials](QML_Component_Tutorials.md)** — Praxis-Beispiele
