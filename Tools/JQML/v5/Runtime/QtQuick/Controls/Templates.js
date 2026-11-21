// QtQuick.Templates of Qt 6.12: the controls without a style (JQ.types['T.<Name>'], `import QtQuick.Templates as T`)
const { Control } = require('./Control')
const { Label } = require('./Label')
const buttons = require('./AbstractButton')

module.exports = Object.assign({ Control, Label }, buttons)
