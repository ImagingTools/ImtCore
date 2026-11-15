const { defineType } = require('../../Core/type')
const { LinearLayout } = require('./LinearLayout')

class ColumnLayout extends LinearLayout {
    $horizontal(){ return false }
}
defineType(ColumnLayout)

module.exports = { ColumnLayout }
