const engine = require('../../Core/engine')
const { QtObject } = require('../../Core/QtObject')
const { defineType } = require('../../Core/type')

// ---- ListModel: rows are plain objects; views subscribe through $views ----------------

// default value of a role for an element that has no value for it (ListElement::getProperty)
function roleDefault(value){
    if(typeof value === 'string') return ''
    if(typeof value === 'number') return 0
    if(typeof value === 'boolean') return false
    return undefined
}

const missing = (value) => value === undefined || value === null

// Without dynamicRoles a role is created by the first value seen for it and every element has it,
// an undefined/null member creates no role (QQmlListModel, ListModel::setOrCreateProperty)
class ListModel extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$rows = []
        this.$views = null
        this.$roles = new Map()
    }

    // initial rows from ListElement children, written by the compiler
    $init(rows){
        this.$rows = []
        for(let row of rows) this.$rows.push(this.$newRow(row))
        this.$write(ListModel.COUNT, this.$rows.length)
    }

    $addRole(name, value){
        let def = roleDefault(value)
        this.$roles.set(name, def)
        for(let row of this.$rows) if(!(name in row)) row[name] = def
    }

    $newRow(value){
        if(this.$v[ListModel.DYNAMICROLES]) return Object.assign({}, value)
        let row = {}
        for(let key in value){
            let v = value[key]
            if(missing(v)){
                let what = v === null ? 'null' : 'undefined'
                console.warn(key + ' is ' + what + '. Adding an object with a ' + what + ' member does not create a role for it.')
                continue
            }
            if(!this.$roles.has(key)) this.$addRole(key, v)
            row[key] = v
        }
        for(let [role, def] of this.$roles) if(!(role in row)) row[role] = def
        return row
    }

    // the object of an element is live: a binding reading one of its roles follows that role, and writing a role
    // goes through the model (QQmlListModel::get, ModelObject)
    get(index){
        let row = this.$rows[index]
        if(!row) return undefined
        let element = elements.get(row)
        if(!element) elements.set(row, element = new Proxy(row, elementHandler(this)))
        return element
    }

    // roles: the roles a 'change' touched (dataChanged), all of them when not given
    $notify(kind, index, count, roles = null){
        this.$write(ListModel.COUNT, this.$rows.length)
        if(kind === 'change'){
            for(let i = index; i < index + count; i++) if(this.$rows[i] !== writingElement) notifyCells(this.$rows[i], roles)
        }
        if(this.$views){
            for(let view of this.$views.slice()) view.$modelChanged(kind, index, count, roles)
        }
    }

    append(value){
        let rows = Array.isArray(value) ? value : [value]
        let index = this.$rows.length
        for(let row of rows) this.$rows.push(this.$newRow(row))
        this.$notify('insert', index, rows.length)
    }

    insert(index, value){
        let rows = Array.isArray(value) ? value : [value]
        this.$rows.splice(index, 0, ...rows.map(row => this.$newRow(row)))
        this.$notify('insert', index, rows.length)
    }

    remove(index, count = 1){
        this.$rows.splice(index, count)
        this.$notify('remove', index, count)
    }

    clear(){
        let count = this.$rows.length
        this.$rows = []
        if(count) this.$notify('remove', 0, count)
    }

    set(index, value){
        if(index === this.$rows.length){
            this.append(value)
            return
        }
        let row = this.$rows[index]
        if(this.$v[ListModel.DYNAMICROLES]) Object.assign(row, value)
        else for(let key in value){
            let v = value[key]
            if(missing(v)){
                if(this.$roles.has(key)) row[key] = this.$roles.get(key)
                continue
            }
            if(!this.$roles.has(key)) this.$addRole(key, v)
            row[key] = v
        }
        this.$notify('change', index, 1, Object.keys(value))
    }

    setProperty(index, role, value){
        if(!this.$v[ListModel.DYNAMICROLES]){
            // the element keeps its value
            if(missing(value)){
                console.warn("Can't create role for unsupported data type")
                return
            }
            if(!this.$roles.has(role)) this.$addRole(role, value)
        }
        this.$rows[index][role] = value
        this.$notify('change', index, 1, [role])
    }

    move(from, to, count = 1){
        let moved = this.$rows.splice(from, count)
        this.$rows.splice(to, 0, ...moved)
        this.$notify('reset', 0, this.$rows.length)
    }

    $addView(view){
        if(!this.$views) this.$views = []
        this.$views.push(view)
    }

    $removeView(view){
        if(!this.$views) return
        let index = this.$views.indexOf(view)
        if(index >= 0) this.$views.splice(index, 1)
    }
}

defineType(ListModel, {
    props: { count: 'int', dynamicRoles: 'bool' },
})

const elements = new WeakMap()      // row -> the live object get() returns
const elementCells = new WeakMap()  // row -> Map(role -> RoleCell)
let writingElement = null

function elementHandler(model){
    return {
        get(row, key){
            if(typeof key === 'string' && key in row){
                let cells = elementCells.get(row)
                if(!cells) elementCells.set(row, cells = new Map())
                let cell = cells.get(key)
                if(!cell) cells.set(key, cell = new RoleCell())
                engine.track(cell, RoleCell.SIGNAL_changed)
            }
            return row[key]
        },
        set(row, key, value){
            let index = model.$rows.indexOf(row)
            if(index < 0 || typeof key !== 'string'){
                row[key] = value
                return true
            }
            // the views see the change, the bindings that read this object do not (JQMLHost)
            writingElement = row
            try {
                model.setProperty(index, key, value)
            } finally {
                writingElement = null
            }
            return true
        },
    }
}

function notifyCells(row, roles){
    let cells = row && elementCells.get(row)
    if(!cells) return
    for(let [role, cell] of Array.from(cells)){
        if(!roles || roles.indexOf(role) >= 0) engine.emitArgs(cell, RoleCell.SIGNAL_changed, [])
    }
}

class RoleCell extends QtObject {}

defineType(RoleCell, { signals: ['changed'] })

module.exports = { ListModel, RoleCell }
