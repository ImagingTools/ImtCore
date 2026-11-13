const { defineType } = require('../../Core/type')
const { LinearLayout } = require('./LinearLayout')

class RowLayout extends LinearLayout {
    $horizontal(){ return true }
}
defineType(RowLayout)

module.exports = { RowLayout }
