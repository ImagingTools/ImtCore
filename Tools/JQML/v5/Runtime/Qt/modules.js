// Types of the Qt modules for Qt.createComponent(moduleUri, typeName): what `import <module>` makes visible, with the
// modules it imports, checked against Qt 6.8 and 6.12. An uncreatable type is a
// QML type that has no instances (abstract, attached, an enum holder).

const QML = ['QtObject', 'Component', 'Timer', 'Connections', 'Binding']
const MODELS = ['ListModel', 'ListElement', 'Instantiator', 'ItemSelectionModel', 'DelegateChooser', 'DelegateChoice', 'DelegateModel']
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
// QtQuick.Controls.Basic (the style QtQuick.Controls imports) and QtQuick.Templates (the controls without a style): the
// types the runtime has, the names of the Qt 6.12 qmldir / plugins.qmltypes. A Templates class is JQ.types['T.<Name>']
const TABLE_CONTROLS = ['HorizontalHeaderView', 'VerticalHeaderView', 'TableViewDelegate', 'TreeViewDelegate', 'SelectionRectangle']
// the style's types (JQ.types.<Name>) and those of Templates (JQ.types['T.<Name>']); tests/unit/Controls.test.js checks
// that the runtime has each of them
const BASIC_CONTROLS = ['Control', 'Label', 'AbstractButton', 'Button', 'ToolButton', 'RoundButton', 'CheckBox', 'RadioButton', 'Switch',
    'DelayButton', 'TabButton', 'ItemDelegate', 'CheckDelegate', 'RadioDelegate', 'SwitchDelegate', 'ScrollBar', 'ScrollIndicator', 'Slider', 'RangeSlider', 'ProgressBar', 'BusyIndicator', 'Dial', 'PageIndicator', 'SpinBox']
const CONTROLS = [...TABLE_CONTROLS, ...BASIC_CONTROLS]
const TEMPLATES = ['HeaderViewDelegate', ...BASIC_CONTROLS]
const LAYOUTS = ['RowLayout', 'ColumnLayout', 'GridLayout', 'StackLayout', 'LayoutItemProxy']

// transitive: the modules its qmldir imports (`import X auto`, `default import X auto`). QQmlImports looks up a name in
// the modules a document imports, the last one first, then in the modules they import, then in the document's directory
// (checked in Qt 6.12: `import ModA; import QtQuick.Controls` takes the Button of ModA, as QtQuick.Controls has none of
// its own: its controls are those of QtQuick.Controls.Basic)
const MODULES = {
    'QtQml': { types: QML, uncreatable: [], transitive: ['QtQml.Models'] },
    'QtQml.Models': { types: MODELS, uncreatable: [] },
    'QtQuick': { types: [...QML, ...MODELS, ...QUICK], uncreatable: QUICK_UNCREATABLE, transitive: ['QtQml'] },
    'QtQuick.Window': { types: ['Window'], uncreatable: [], transitive: ['QtQuick'] },
    'QtQuick.Layouts': { types: LAYOUTS, uncreatable: ['Layout'] },
    'Qt.labs.qmlmodels': { types: LABS_MODELS, uncreatable: [] },
    'QtQuick.Controls': { types: [], uncreatable: [], transitive: ['QtQuick.Controls.Basic'] },
    'QtQuick.Controls.Basic': { types: CONTROLS, uncreatable: [] },
    'QtQuick.Templates': { types: TEMPLATES, uncreatable: [], prefix: 'T.' },
    'QtQuick.Dialogs': { types: ['FileDialog', 'FolderDialog'], uncreatable: [] },
    'QtWebSockets': { types: ['WebSocket'], uncreatable: [] },
    'Qt5Compat.GraphicalEffects': { types: ['DropShadow'], uncreatable: [] },
    'QtCore': { types: ['Settings'], uncreatable: [] },
    'Qt.labs.settings': { types: ['Settings'], uncreatable: [] },
    'QtTest': { types: ['TestCase', 'SignalSpy'], uncreatable: [] },
}

// The types of the runtime a document sees only through the import of a module that has them, in the order of its imports
// (QQmlImports): the ones Qt added to QtQuick, QtQml.Models and Qt.labs.qmlmodels for tables, and the controls, whose
// names a project of its own may use for a type (imtcontrols has a SortFilterProxyModel, a Button). The older types stay
// visible without an import, as v5 always had them.
const IMPORTED_TYPES = new Set(['ItemSelectionModel', 'TableView', 'TreeView', ...LABS_MODELS, ...CONTROLS, ...TEMPLATES])

// { types: Set, uncreatable: Set, transitive: [module names], key(name): its name in JQ.types } of a Qt module, or null
function qtModule(name){
    let module = MODULES[name]
    if(!module) return null
    if(!module.typeSet) module.typeSet = new Set(module.types)
    if(!module.uncreatableSet) module.uncreatableSet = new Set(module.uncreatable)
    let prefix = module.prefix || ''
    return { types: module.typeSet, uncreatable: module.uncreatableSet, transitive: module.transitive || [], key: (type) => prefix + type }
}

// the modules a Qt module imports, and those they import, in the order Qt searches them
function transitiveModules(name, seen = new Set()){
    let out = []
    for(let next of (MODULES[name] && MODULES[name].transitive) || []){
        if(seen.has(next)) continue
        seen.add(next)
        out.push(next, ...transitiveModules(next, seen))
    }
    return out
}

// the key in JQ.types of a type of a module, its own or one of the modules it imports (Qt.createComponent), or null
function moduleTypeKey(name, type){
    for(let module of [name, ...transitiveModules(name)]){
        let m = qtModule(module)
        if(m && m.types.has(type)) return m.key(type)
    }
    return null
}

module.exports = { qtModule, transitiveModules, moduleTypeKey, IMPORTED_TYPES, CONTROLS, TEMPLATES }
