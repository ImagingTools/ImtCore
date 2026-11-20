// Types of the Qt modules for Qt.createComponent(moduleUri, typeName): what `import <module>` makes visible, with the
// modules it imports (QtQuick imports QtQml and QtQml.Models), checked against Qt 6.8. An uncreatable type is a
// QML type that has no instances (abstract, attached, an enum holder).

const QML = ['QtObject', 'Component', 'Timer', 'Connections', 'Binding']
const MODELS = ['ListModel', 'ListElement', 'Instantiator', 'ItemSelectionModel', 'DelegateChooser', 'DelegateChoice']
const LABS_MODELS = ['TableModel', 'TableModelColumn', 'TreeModel', 'DelegateChooser', 'DelegateChoice']
const QUICK = [
    'Item', 'FocusScope', 'Rectangle', 'Gradient', 'GradientStop', 'Text', 'TextInput', 'TextEdit', 'Image', 'Canvas',
    'MouseArea', 'DropArea', 'Flickable', 'Column', 'Row', 'Flow', 'Repeater', 'Loader', 'ListView', 'GridView', 'TableView', 'TreeView', 'Shortcut',
    'IntValidator', 'DoubleValidator', 'RegularExpressionValidator', 'FontLoader',
    'State', 'PropertyChanges', 'StateChangeScript', 'Transition', 'StateGroup',
    'PauseAnimation', 'PropertyAnimation', 'NumberAnimation', 'ColorAnimation', 'RotationAnimation', 'SmoothedAnimation',
    'XAnimator', 'YAnimator', 'ScaleAnimator', 'RotationAnimator', 'OpacityAnimator', 'PropertyAction', 'ScriptAction',
    'SequentialAnimation', 'ParallelAnimation', 'Behavior', 'Window',
]
const QUICK_UNCREATABLE = ['Keys', 'KeyNavigation', 'Animation', 'Easing', 'StandardKey']
// QtQuick.Controls: the views of tables only (the project has controls of its own)
const CONTROLS = ['HorizontalHeaderView', 'VerticalHeaderView', 'HeaderViewDelegate', 'TableViewDelegate', 'TreeViewDelegate']
const LAYOUTS = ['RowLayout', 'ColumnLayout', 'GridLayout', 'StackLayout', 'LayoutItemProxy']

const MODULES = {
    'QtQml': { types: [...QML, ...MODELS], uncreatable: [] },
    'QtQml.Models': { types: MODELS, uncreatable: [] },
    'QtQuick': { types: [...QML, ...MODELS, ...QUICK], uncreatable: QUICK_UNCREATABLE },
    'QtQuick.Window': { types: ['Window'], uncreatable: [] },
    'QtQuick.Layouts': { types: LAYOUTS, uncreatable: ['Layout'] },
    'Qt.labs.qmlmodels': { types: LABS_MODELS, uncreatable: [] },
    'QtQuick.Controls': { types: CONTROLS, uncreatable: [] },
    'QtQuick.Controls.Basic': { types: CONTROLS, uncreatable: [] },
    'QtQuick.Dialogs': { types: ['FileDialog', 'FolderDialog'], uncreatable: [] },
    'QtWebSockets': { types: ['WebSocket'], uncreatable: [] },
    'Qt5Compat.GraphicalEffects': { types: ['DropShadow'], uncreatable: [] },
    'QtCore': { types: ['Settings'], uncreatable: [] },
    'Qt.labs.settings': { types: ['Settings'], uncreatable: [] },
    'QtTest': { types: ['TestCase', 'SignalSpy'], uncreatable: [] },
}

// The types of the runtime a document sees only through the import of a module that has them, in the order of its imports
// (QQmlImports): the ones Qt added to QtQuick, QtQml.Models and Qt.labs.qmlmodels for tables, whose names a project of
// its own may use for a type (imtcontrols has a SortFilterProxyModel). The older types stay visible without an import,
// as v5 always had them.
const IMPORTED_TYPES = new Set(['ItemSelectionModel', 'TableView', 'TreeView', ...LABS_MODELS, ...CONTROLS])

// { types: Set, uncreatable: Set } of a Qt module, or null
function qtModule(name){
    let module = MODULES[name]
    if(!module) return null
    if(!module.typeSet) module.typeSet = new Set(module.types)
    if(!module.uncreatableSet) module.uncreatableSet = new Set(module.uncreatable)
    return { types: module.typeSet, uncreatable: module.uncreatableSet }
}

module.exports = { qtModule, IMPORTED_TYPES }
