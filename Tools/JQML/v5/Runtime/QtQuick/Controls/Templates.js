// QtQuick.Templates of Qt 6.12: the controls without a style (JQ.types['T.<Name>'], `import QtQuick.Templates as T`)
const { Control } = require('./Control')
const { Label } = require('./Label')
const buttons = require('./AbstractButton')
const { ScrollBar, ScrollIndicator } = require('./ScrollBar')
const range = require('./Range')

module.exports = Object.assign({ Control, Label, ScrollBar, ScrollIndicator }, buttons, range)
