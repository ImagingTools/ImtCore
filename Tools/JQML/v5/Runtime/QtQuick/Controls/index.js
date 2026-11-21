// The controls of QtQuick.Controls (the Basic style) and of QtQuick.Templates the runtime has: name -> class. A document sees
// them only through an import of the module (Runtime/Qt/modules.js: CONTROLS, TEMPLATES)
const Templates = require('./Templates')
const Basic = require('./Basic')

const BASIC_NAMES = ['Control', 'Label', 'AbstractButton', 'Button', 'ToolButton', 'RoundButton', 'CheckBox', 'RadioButton', 'Switch',
    'DelayButton', 'TabButton', 'ItemDelegate', 'CheckDelegate', 'RadioDelegate', 'SwitchDelegate', 'ScrollBar', 'ScrollIndicator', 'Slider', 'RangeSlider', 'ProgressBar', 'BusyIndicator', 'Dial', 'PageIndicator', 'SpinBox', 'Pane', 'Frame', 'GroupBox', 'Page', 'ToolBar', 'ToolSeparator', 'MenuSeparator', 'Container', 'TabBar', 'SwipeView']
const TEMPLATE_NAMES = ['Control', 'Label', 'AbstractButton', 'Button', 'ToolButton', 'RoundButton', 'CheckBox', 'RadioButton', 'Switch',
    'DelayButton', 'TabButton', 'ItemDelegate', 'CheckDelegate', 'RadioDelegate', 'SwitchDelegate', 'ScrollBar', 'ScrollIndicator', 'Slider', 'RangeSlider', 'ProgressBar', 'BusyIndicator', 'Dial', 'PageIndicator', 'SpinBox', 'Pane', 'Frame', 'GroupBox', 'Page', 'ToolBar', 'ToolSeparator', 'MenuSeparator', 'Container', 'TabBar', 'SwipeView']

// JQ.types entries: Basic by name, Templates as 'T.<Name>'
function types(){
    let out = {}
    for(let name of BASIC_NAMES) out[name] = Basic[name]
    for(let name of TEMPLATE_NAMES) out['T.' + name] = Templates[name]
    return out
}

module.exports = { types, BASIC_NAMES, TEMPLATE_NAMES }
