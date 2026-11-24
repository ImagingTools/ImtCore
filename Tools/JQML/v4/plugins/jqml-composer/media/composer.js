// The page of the schema composer. It shows the view of the schema that the extension sends (the objects of the
// schema, of its bases and the insides of the schemas it uses) and asks the extension to change the schema:
// { type: 'op', op, uid, ... }. The extension answers with the new state, so what is drawn is always what is in the file.
//
// The objects that belong to a base schema or to the inside of an instance are drawn and listed, but they are
// read-only (editable: false). An instance of a schema, and the root of a schema that extends another, take only the
// exported properties of that schema (view.accepts) and the properties of the placement.

const vscode = acquireVsCodeApi()

const STACKS = { Column: 'v', ColumnLayout: 'v', Row: 'h', RowLayout: 'h' }
const NON_VISUAL = new Set([
    'Timer', 'ListModel', 'ListElement', 'Repeater', 'Shortcut', 'FontLoader', 'Gradient', 'GradientStop', 'TestCase', 'TreeItemModel',
    'GqlModel', 'Settings', 'WebSocket', 'Connections', 'Binding', 'Component', 'QtObject', 'RegularExpressionValidator', 'WheelHandler',
    'DragHandler', 'Behavior', 'State', 'Transition', 'NumberAnimation', 'ColorAnimation', 'PropertyAnimation', 'SequentialAnimation',
    'ParallelAnimation', 'PauseAnimation',
])
// What a type gets when it is dropped on the canvas.
const DEFAULTS = {
    Rectangle: { width: '100', height: '60', color: '"#cccccc"' },
    Text: { text: '"Text"' },
    Image: { width: '100', height: '100' },
    MouseArea: { width: '100', height: '40' },
    Item: { width: '100', height: '60' },
    TextInput: { width: '100', height: '24', text: '"Text"' },
    TextEdit: { width: '100', height: '60', text: '"Text"' },
    Column: { width: '100', height: '100' },
    Row: { width: '100', height: '40' },
}
const PLACEMENT = ['x', 'y', 'z', 'width', 'height', 'visible', 'opacity', 'rotation', 'scale', 'clip', 'enabled']

const state = {
    view: null,
    exports: [],
    extends: '',
    imports: [],
    types: { common: [], builtin: [], project: [], schemas: [] },
    error: null,
    problems: [],
    selected: '',
    zoom: 1,
    search: '',
    members: {},
    failure: '',
}
const saved = vscode.getState() || {}
if (typeof saved.zoom === 'number') state.zoom = saved.zoom
let inspectorSignature = ''

const $ = (id) => document.getElementById(id)

function el(tag, className, text) {
    const node = document.createElement(tag)
    if (className) node.className = className
    if (text !== undefined) node.textContent = text
    return node
}

function post(message) {
    vscode.postMessage(message)
}

function isPlacement(name) {
    return PLACEMENT.includes(name) || name.startsWith('anchors.') || name.startsWith('Layout.')
}

// ----- the tree of the view -----

function find(uid, node, parent) {
    if (node === null || node === undefined) return null
    if (node.uid === uid) return { node, parent: parent || null }
    for (const child of node.children) {
        const found = find(uid, child, node)
        if (found !== null) return found
    }
    return null
}

function nodeByUid(uid) {
    const found = find(uid, state.view, null)
    return found === null ? null : found.node
}

function parentOf(uid) {
    const found = find(uid, state.view, null)
    return found === null ? null : found.parent
}

function propOf(node, name) {
    const found = node.props.find((prop) => prop.name === name)
    return found === undefined ? null : found.value
}

function ownOf(node, name) {
    const found = (node.own || []).find((prop) => prop.name === name)
    return found === undefined ? null : found.value
}

function hasPrefix(node, prefix) {
    return node.props.some((prop) => prop.name.startsWith(prefix))
}

function numberOf(value) {
    if (value === null) return null
    return /^-?\d+(\.\d+)?$/.test(value.trim()) ? Number(value) : null
}

function unquote(value) {
    if (value === null) return ''
    const text = value.trim()
    if (text.length >= 2 && (text[0] === '"' || text[0] === "'") && text[text.length - 1] === text[0]) return text.slice(1, -1)
    return text
}

function isVisual(type) {
    return !NON_VISUAL.has(type)
}

function editable() {
    return state.error === null && state.view !== null
}

// A node that objects can be put into: an object of this schema that is not an instance of another schema.
function accepts(node) {
    return node !== null && node.editable === true && !node.instanceOf
}

function isClosed(node) {
    return Boolean(node.instanceOf) || Boolean(node.derived)
}

// ----- the geometry of the blocks -----

// The box of a node inside its parent, from the literal numbers of its properties; what is computed by a binding or by
// a layout is only approximated, and such a box is not given to the mouse (`fixed`).
function layoutOf(node, parentBox, stack, isRoot) {
    const box = { x: 0, y: 0, w: 0, h: 0, fixed: false }
    const fill = propOf(node, 'anchors.fill') !== null
    const width = numberOf(propOf(node, 'width'))
    const height = numberOf(propOf(node, 'height'))
    const anchored = hasPrefix(node, 'anchors.')
    if (fill && parentBox !== null) {
        box.w = parentBox.w
        box.h = parentBox.h
        box.fixed = true
        return box
    }
    box.w = width !== null ? width : defaultSize(node, 'w', parentBox, isRoot)
    box.h = height !== null ? height : defaultSize(node, 'h', parentBox, isRoot)
    box.x = numberOf(propOf(node, 'x')) || 0
    box.y = numberOf(propOf(node, 'y')) || 0
    if (propOf(node, 'width') !== null && width === null) box.fixed = true
    if (propOf(node, 'height') !== null && height === null) box.fixed = true
    if (anchored || stack) box.fixed = true
    if (parentBox !== null) {
        const margin = (side) => numberOf(propOf(node, 'anchors.' + side + 'Margin')) || numberOf(propOf(node, 'anchors.margins')) || 0
        if (propOf(node, 'anchors.centerIn') !== null) {
            box.x = (parentBox.w - box.w) / 2
            box.y = (parentBox.h - box.h) / 2
        } else {
            if (propOf(node, 'anchors.horizontalCenter') !== null) box.x = (parentBox.w - box.w) / 2
            if (propOf(node, 'anchors.verticalCenter') !== null) box.y = (parentBox.h - box.h) / 2
            if (propOf(node, 'anchors.right') !== null && propOf(node, 'anchors.left') === null) box.x = parentBox.w - box.w - margin('right')
            if (propOf(node, 'anchors.bottom') !== null && propOf(node, 'anchors.top') === null) box.y = parentBox.h - box.h - margin('bottom')
            if (propOf(node, 'anchors.left') !== null) box.x = margin('left')
            if (propOf(node, 'anchors.top') !== null) box.y = margin('top')
        }
    }
    return box
}

function defaultSize(node, axis, parentBox, isRoot) {
    if (node.type === 'Text' || node.type === 'TextInput' || node.type === 'TextEdit') {
        const text = unquote(propOf(node, 'text'))
        const size = numberOf(propOf(node, 'font.pixelSize')) || 14
        return axis === 'w' ? Math.max(8, Math.round(text.length * size * 0.55)) : Math.round(size * 1.3)
    }
    if (isRoot) return axis === 'w' ? 640 : 480
    return axis === 'w' ? 100 : 40
}

// Places the children of a node: a Column or a Row puts them one after another.
function placeChildren(node, box) {
    const stack = STACKS[node.type]
    const spacing = numberOf(propOf(node, 'spacing')) || 0
    let cursor = 0
    const placed = []
    for (const child of node.children) {
        if (!isVisual(child.type)) continue
        const childBox = layoutOf(child, box, !!stack, false)
        if (stack === 'v') {
            childBox.x = numberOf(propOf(child, 'x')) || 0
            childBox.y = cursor
            cursor += childBox.h + spacing
        } else if (stack === 'h') {
            childBox.y = numberOf(propOf(child, 'y')) || 0
            childBox.x = cursor
            cursor += childBox.w + spacing
        }
        placed.push({ node: child, box: childBox })
    }
    return placed
}

// ----- the canvas -----

function colorOf(node) {
    const value = unquote(propOf(node, 'color'))
    return /^(#[0-9a-fA-F]{3,8}|[a-zA-Z]+)$/.test(value) ? value : ''
}

function renderBlock(node, box, parentEl) {
    const block = el('div', 'block')
    block.dataset.uid = node.uid
    if (!node.editable) block.classList.add('inherited')
    block.style.left = box.x + 'px'
    block.style.top = box.y + 'px'
    block.style.width = box.w + 'px'
    block.style.height = box.h + 'px'
    if (node.type === 'Rectangle') {
        const color = colorOf(node)
        block.style.background = color !== '' ? color : '#00000010'
        const radius = numberOf(propOf(node, 'radius'))
        if (radius !== null) block.style.borderRadius = radius + 'px'
        const borderWidth = numberOf(propOf(node, 'border.width'))
        if (borderWidth) block.style.border = borderWidth + 'px solid ' + (unquote(propOf(node, 'border.color')) || '#000')
    } else if (node.type === 'Item' || node.type === 'MouseArea') {
        block.style.background = node.type === 'MouseArea' ? '#4a90e220' : 'transparent'
    } else if (node.type === 'Image') {
        block.style.background = '#8884'
    }
    if (unquote(propOf(node, 'visible')) === 'false') block.classList.add('hidden-item')
    const opacity = numberOf(propOf(node, 'opacity'))
    if (opacity !== null) block.style.opacity = String(opacity)
    if (unquote(propOf(node, 'clip')) === 'true') block.style.overflow = 'hidden'
    if (node.type === 'Text' || node.type === 'TextInput' || node.type === 'TextEdit') {
        const text = el('div', 'text', unquote(propOf(node, 'text')))
        const textColor = unquote(propOf(node, 'color'))
        if (/^(#[0-9a-fA-F]{3,8}|[a-zA-Z]+)$/.test(textColor)) text.style.color = textColor
        const size = numberOf(propOf(node, 'font.pixelSize'))
        if (size !== null) text.style.fontSize = size + 'px'
        if (unquote(propOf(node, 'font.bold')) === 'true') text.style.fontWeight = '700'
        block.appendChild(text)
    }
    block.appendChild(el('div', 'label', nodeTitle(node)))
    parentEl.appendChild(block)
    for (const child of placeChildren(node, box)) renderBlock(child.node, child.box, block)
    return block
}

function nodeTitle(node) {
    if (node.instanceOf) return (node.id !== '' ? node.id + ' : ' : '') + node.instanceOf
    return node.type + (node.id !== '' ? ' #' + node.id : '')
}

function renderCanvas() {
    const canvas = $('canvas')
    const holder = $('canvas-holder')
    canvas.textContent = ''
    $('empty').hidden = state.view !== null
    holder.hidden = state.view === null
    if (state.view === null) return
    const rootBox = layoutOf(state.view, null, false, true)
    rootBox.x = 0
    rootBox.y = 0
    canvas.style.width = rootBox.w + 'px'
    canvas.style.height = rootBox.h + 'px'
    canvas.style.transform = 'scale(' + state.zoom + ')'
    holder.style.width = rootBox.w * state.zoom + 'px'
    holder.style.height = rootBox.h * state.zoom + 'px'
    renderBlock(state.view, rootBox, canvas)
    markSelection()
}

function blockOf(uid) {
    return $('canvas').querySelector('[data-uid="' + CSS.escape(uid) + '"]')
}

function markSelection() {
    for (const old of $('canvas').querySelectorAll('.block.selected')) old.classList.remove('selected')
    for (const old of $('canvas').querySelectorAll('.handle')) old.remove()
    const node = nodeByUid(state.selected)
    const block = node === null ? null : blockOf(state.selected)
    if (block === null) return
    block.classList.add('selected')
    if (movableNode(node) && !hasPrefix(node, 'anchors.') && numberOrAbsent(node, 'width') && numberOrAbsent(node, 'height')) {
        for (const kind of ['e', 's', 'se']) {
            const handle = el('div', 'handle ' + kind)
            handle.addEventListener('pointerdown', (event) => startResize(event, node, block, kind))
            block.appendChild(handle)
        }
    }
}

function numberOrAbsent(node, name) {
    const value = propOf(node, name)
    return value === null || numberOf(value) !== null
}

// ----- the mouse on the canvas -----

// An object that the mouse may place: one of this schema, in an object that does not lay its children out, with its
// place made of plain numbers. The root is the canvas itself.
function movableNode(node) {
    if (!editable() || !node.editable || state.view === null || node.uid === state.view.uid) return false
    const parent = parentOf(node.uid)
    if (parent === null || STACKS[parent.type] !== undefined || hasPrefix(node, 'anchors.')) return false
    return numberOrAbsent(node, 'x') && numberOrAbsent(node, 'y')
}

function startMove(event, node, block) {
    const startX = event.clientX
    const startY = event.clientY
    const originX = block.offsetLeft
    const originY = block.offsetTop
    let moved = false
    const onMove = (move) => {
        const dx = (move.clientX - startX) / state.zoom
        const dy = (move.clientY - startY) / state.zoom
        if (!moved && Math.abs(dx) + Math.abs(dy) < 3) return
        moved = true
        block.style.left = Math.round(originX + dx) + 'px'
        block.style.top = Math.round(originY + dy) + 'px'
    }
    const onUp = () => {
        window.removeEventListener('pointermove', onMove)
        window.removeEventListener('pointerup', onUp)
        if (!moved) return
        post({ type: 'op', op: 'setMany', uid: node.uid, props: { x: String(block.offsetLeft), y: String(block.offsetTop) } })
    }
    window.addEventListener('pointermove', onMove)
    window.addEventListener('pointerup', onUp)
}

function startResize(event, node, block, kind) {
    event.stopPropagation()
    event.preventDefault()
    const startX = event.clientX
    const startY = event.clientY
    const originW = block.offsetWidth
    const originH = block.offsetHeight
    let moved = false
    const onMove = (move) => {
        moved = true
        if (kind !== 's') block.style.width = Math.max(1, Math.round(originW + (move.clientX - startX) / state.zoom)) + 'px'
        if (kind !== 'e') block.style.height = Math.max(1, Math.round(originH + (move.clientY - startY) / state.zoom)) + 'px'
    }
    const onUp = () => {
        window.removeEventListener('pointermove', onMove)
        window.removeEventListener('pointerup', onUp)
        if (!moved) return
        const props = {}
        if (kind !== 's') props.width = String(block.offsetWidth)
        if (kind !== 'e') props.height = String(block.offsetHeight)
        post({ type: 'op', op: 'setMany', uid: node.uid, props })
    }
    window.addEventListener('pointermove', onMove)
    window.addEventListener('pointerup', onUp)
}

// The nearest object, from the block under the mouse up, that objects can be dropped into.
function dropParent(target) {
    let block = target.closest('.block')
    while (block !== null) {
        const node = nodeByUid(block.dataset.uid)
        if (accepts(node)) return { node, block }
        block = block.parentElement === null ? null : block.parentElement.closest('.block')
    }
    return null
}

function clearDropTargets() {
    for (const old of $('canvas').querySelectorAll('.drop-target')) old.classList.remove('drop-target')
}

function bindCanvas() {
    const canvas = $('canvas')
    canvas.addEventListener('pointerdown', (event) => {
        if (event.button !== 0 || event.target.classList.contains('handle')) return
        const block = event.target.closest('.block')
        if (block === null) return
        event.stopPropagation()
        const node = nodeByUid(block.dataset.uid)
        select(block.dataset.uid)
        if (node !== null && movableNode(node)) startMove(event, node, blockOf(node.uid))
    })
    $('canvas-scroll').addEventListener('pointerdown', (event) => {
        if (event.target.closest('.block') === null && event.target.closest('#canvas') === null && state.view !== null) select(state.view.uid)
    })
    canvas.addEventListener('dragover', (event) => {
        if (!editable()) return
        event.preventDefault()
        clearDropTargets()
        const found = dropParent(event.target)
        if (found !== null) found.block.classList.add('drop-target')
    })
    canvas.addEventListener('dragleave', (event) => {
        if (event.target === canvas) clearDropTargets()
    })
    canvas.addEventListener('drop', (event) => {
        event.preventDefault()
        clearDropTargets()
        const spec = specOf(event.dataTransfer)
        const found = dropParent(event.target)
        if (spec === null || found === null || !editable()) return
        const rect = found.block.getBoundingClientRect()
        const x = Math.max(0, Math.round((event.clientX - rect.left) / state.zoom))
        const y = Math.max(0, Math.round((event.clientY - rect.top) / state.zoom))
        addObject(spec, found.node, { x, y })
    })
}

// ----- the operations -----

function select(uid) {
    state.selected = uid
    markSelection()
    renderTree()
    renderInspector(true)
    syncToolbar()
}

function specOf(transfer) {
    const text = transfer.getData('text/jqml-spec')
    if (!text) return null
    try {
        return JSON.parse(text)
    } catch (error) {
        return null
    }
}

function addObject(spec, parent, position, index) {
    const props = {}
    const visual = spec.source !== undefined || isVisual(spec.type)
    if (visual && position !== undefined && STACKS[parent.type] === undefined) {
        props.x = String(position.x)
        props.y = String(position.y)
    }
    if (spec.type !== undefined) Object.assign(props, DEFAULTS[spec.type] || {})
    post({ type: 'op', op: 'add', uid: parent.uid, spec, props, index })
}

function deleteSelected() {
    const node = nodeByUid(state.selected)
    if (!editable() || node === null || !node.editable || node.uid === state.view.uid) return
    post({ type: 'op', op: 'delete', uid: node.uid })
}

// The objects of the parent that this schema owns, in order: the ones of a base come before them and are not moved.
function ownChildren(parent) {
    return parent.children.filter((child) => child.editable)
}

function siblingMove(delta) {
    const node = nodeByUid(state.selected)
    const parent = node === null ? null : parentOf(node.uid)
    if (!editable() || node === null || !node.editable || parent === null || !accepts(parent)) return
    const own = ownChildren(parent)
    const index = own.indexOf(own.find((child) => child.uid === node.uid))
    const target = index + delta
    if (target < 0 || target >= own.length) return
    post({ type: 'op', op: 'move', uid: node.uid, parent: parent.uid, index: delta > 0 ? target + 1 : target })
}

function syncToolbar() {
    const node = nodeByUid(state.selected)
    const ok = editable() && node !== null && node.editable
    const parent = ok ? parentOf(node.uid) : null
    const own = parent !== null && accepts(parent) ? ownChildren(parent) : []
    const index = own.findIndex((child) => node !== null && child.uid === node.uid)
    $('delete').disabled = !(ok && parent !== null)
    $('up').disabled = !(ok && index > 0)
    $('down').disabled = !(ok && index >= 0 && index < own.length - 1)
    $('generate').disabled = state.error !== null
}

// ----- the palette -----

function paletteItem(label, note, spec) {
    const item = el('div', 'palette-item', label)
    if (note) item.appendChild(el('small', '', note))
    item.draggable = true
    item.addEventListener('dragstart', (event) => {
        event.dataTransfer.setData('text/jqml-spec', JSON.stringify(spec))
        event.dataTransfer.effectAllowed = 'copy'
    })
    item.addEventListener('dblclick', () => {
        const node = nodeByUid(state.selected)
        const parent = node !== null && accepts(node) ? node : state.view
        if (editable() && accepts(parent)) addObject(spec, parent, undefined)
    })
    return item
}

function renderPalette() {
    const host = $('palette')
    host.textContent = ''
    const needle = state.search.trim().toLowerCase()
    const match = (name) => needle === '' || name.toLowerCase().includes(needle)
    const groups = [
        ['Schemas', state.types.schemas.filter((item) => match(item.name)).map((item) => [item.name, 'schema', { source: item.ref }])],
        ['Common', state.types.common.filter(match).map((name) => [name, '', { type: name }])],
        ['Project', state.types.project.filter((item) => match(item.name)).map((item) => [item.name, item.file, { type: item.name }])],
        ['All types', state.types.builtin.filter(match).map((name) => [name, '', { type: name }])],
    ]
    for (const [title, items] of groups) {
        if (items.length === 0) continue
        host.appendChild(el('div', 'palette-group', title))
        for (const [name, note, spec] of items) host.appendChild(paletteItem(name, note, spec))
    }
}

// ----- the tree -----

function clearDropMarks() {
    for (const old of $('tree').querySelectorAll('.drop-into, .drop-before, .drop-after')) old.classList.remove('drop-into', 'drop-before', 'drop-after')
}

// Where on a row a drop goes: before it, after it, or into the object.
function dropZone(event, row, node) {
    if (node.uid === state.view.uid || !node.editable) return 'into'
    const rect = row.getBoundingClientRect()
    const offset = (event.clientY - rect.top) / rect.height
    if (offset < 0.25) return 'before'
    if (offset > 0.75) return 'after'
    return 'into'
}

function treeRow(node, depth) {
    const row = el('div', 'tree-row')
    row.style.paddingLeft = 6 + depth * 14 + 'px'
    if (node.uid === state.selected) row.classList.add('selected')
    if (!node.editable) row.classList.add('inherited')
    if (node.instanceOf) {
        row.appendChild(el('span', 'type', node.id !== '' ? node.id : node.instanceOf))
        row.appendChild(el('span', 'id', ': ' + node.instanceOf))
    } else {
        row.appendChild(el('span', 'type', node.type))
        if (node.id !== '') row.appendChild(el('span', 'id', '#' + node.id))
    }
    if (node.derived) row.appendChild(el('span', 'id', 'extends ' + node.derived))
    row.dataset.uid = node.uid
    row.addEventListener('click', () => select(node.uid))
    row.draggable = node.editable && node.uid !== state.view.uid
    row.addEventListener('dragstart', (event) => {
        event.dataTransfer.setData('text/jqml-uid', node.uid)
        event.dataTransfer.effectAllowed = 'move'
    })
    row.addEventListener('dragover', (event) => {
        if (!editable() || !node.editable) return
        event.preventDefault()
        clearDropMarks()
        const zone = dropZone(event, row, node)
        row.classList.add(zone === 'before' ? 'drop-before' : zone === 'after' ? 'drop-after' : 'drop-into')
    })
    row.addEventListener('dragleave', () => clearDropMarks())
    row.addEventListener('drop', (event) => {
        event.preventDefault()
        clearDropMarks()
        if (!editable() || !node.editable) return
        const zone = dropZone(event, row, node)
        let parent = node
        let index
        if (zone !== 'into') {
            parent = parentOf(node.uid)
            const own = parent === null ? [] : ownChildren(parent)
            index = own.findIndex((child) => child.uid === node.uid) + (zone === 'after' ? 1 : 0)
        }
        if (!accepts(parent)) return
        const spec = specOf(event.dataTransfer)
        const moving = event.dataTransfer.getData('text/jqml-uid')
        if (spec !== null) addObject(spec, parent, undefined, index)
        else if (moving) post({ type: 'op', op: 'move', uid: moving, parent: parent.uid, index })
    })
    return row
}

function renderTreeNode(host, node, depth) {
    host.appendChild(treeRow(node, depth))
    for (const child of node.children) renderTreeNode(host, child, depth + 1)
}

function renderTree() {
    const host = $('tree')
    host.textContent = ''
    if (state.view !== null) renderTreeNode(host, state.view, 0)
}

// ----- the inspector -----

function isHandler(name) {
    const last = name.split('.').pop()
    return last.length > 2 && last.startsWith('on') && /^_*[A-Z]/.test(last.slice(2))
}

function commitValue(node, name, value, before) {
    if (!editable() || !node.editable) return
    if ((before === null ? '' : before) === value.trim()) return
    post({ type: 'op', op: 'set', uid: node.uid, name, value })
}

function exportOf(node, property) {
    return state.exports.find((item) => !item.inherited && item.uid === node.uid && item.property === property) || null
}

function fieldFor(node, name, value, typeName, options) {
    const row = el('div', 'field')
    const label = el('label', '', name)
    row.appendChild(label)
    const holder = el('div', 'value')
    row.title = name + (typeName ? ': ' + typeName : '')
    const type = (typeName || '').toLowerCase()
    const readOnly = !node.editable
    const current = value === null ? '' : value
    let input
    if (isHandler(name)) {
        input = el('textarea')
        input.value = current
        input.rows = 2
        input.addEventListener('change', () => commitValue(node, name, input.value, value))
    } else if (type === 'bool' || type === 'boolean') {
        input = el('select')
        for (const option of ['', 'true', 'false']) {
            const item = el('option', '', option === '' ? (options && options.placeholder ? options.placeholder : '(binding)') : option)
            item.value = option
            input.appendChild(item)
        }
        input.value = ['true', 'false'].includes(current.trim()) ? current.trim() : ''
        input.addEventListener('change', () => commitValue(node, name, input.value, value))
    } else {
        input = el('input')
        input.type = 'text'
        input.value = current
        input.spellcheck = false
        if (options && options.placeholder) input.placeholder = options.placeholder
        input.addEventListener('change', () => commitValue(node, name, input.value, value))
        input.addEventListener('keydown', (event) => {
            if (event.key === 'Enter') input.blur()
        })
        if (type === 'color') {
            const picker = el('input')
            picker.type = 'color'
            const literal = unquote(current)
            picker.value = /^#[0-9a-fA-F]{6}$/.test(literal) ? literal : '#000000'
            picker.disabled = readOnly
            picker.addEventListener('change', () => commitValue(node, name, '"' + picker.value + '"', value))
            holder.appendChild(picker)
        }
    }
    input.disabled = readOnly
    holder.appendChild(input)
    row.appendChild(holder)
    const side = el('div', 'side')
    if (options && options.removable && !readOnly) {
        const remove = el('button', 'remove', 'x')
        remove.type = 'button'
        remove.title = 'Remove the property'
        remove.addEventListener('click', () => commitValue(node, name, '', value))
        side.appendChild(remove)
    }
    row.appendChild(side)
    if (options && options.exportable && !readOnly) {
        const exported = exportOf(node, name)
        const toggle = el('button', 'export' + (exported ? ' on' : ''), 'E')
        toggle.type = 'button'
        toggle.title = exported ? 'Exported as ' + exported.name + ' (click to stop exporting)' : 'Export this property'
        toggle.addEventListener('click', () => {
            if (exported) post({ type: 'op', op: 'removeExport', uid: node.uid, name: exported.name })
            else post({ type: 'op', op: 'addExport', uid: node.uid, property: name, typeName })
        })
        row.classList.add('has-export')
        row.appendChild(toggle)
    }
    return row
}

function membersKey(node) {
    return node.instanceOf || node.derived ? node.source : node.type
}

function membersOf(node) {
    const key = membersKey(node)
    const cached = state.members[key]
    if (cached !== undefined) return cached
    state.members[key] = []
    if (node.instanceOf || node.derived) post({ type: 'members', typeName: key, ref: node.source })
    else post({ type: 'members', typeName: key })
    return state.members[key]
}

function datalistFor(members) {
    let list = document.getElementById('member-names')
    if (list === null) {
        list = el('datalist')
        list.id = 'member-names'
        document.body.appendChild(list)
    }
    list.textContent = ''
    for (const member of members) {
        const option = el('option')
        option.value = member.name
        option.label = member.typeName
        list.appendChild(option)
    }
}

// What a property gets when it is added by name: a value that is valid for its type.
function initialValue(name, typeName) {
    if (isHandler(name)) return '{ }'
    const type = typeName.toLowerCase()
    if (type === 'bool' || type === 'boolean') return 'true'
    if (type === 'int' || type === 'real' || type === 'double') return '0'
    if (type === 'string' || type === 'url') return '""'
    if (type === 'color') return '"#ffffff"'
    return '0'
}

function adderFor(node, members, placeholder, onlyPlacement) {
    const form = el('div', 'adder')
    const input = el('input')
    input.type = 'text'
    input.placeholder = placeholder
    input.setAttribute('list', 'member-names')
    input.spellcheck = false
    datalistFor(onlyPlacement ? members.filter((member) => isPlacement(member.name)) : members)
    const add = el('button', '', 'Add')
    add.type = 'button'
    const submit = () => {
        const name = input.value.trim()
        if (name === '' || !editable()) return
        if (onlyPlacement && !isPlacement(name)) {
            state.failure = name + ' is not exported by the schema and is not a placement property: only exported properties can be set'
            renderBanner()
            return
        }
        const typeName = (members.find((member) => member.name === name) || {}).typeName || ''
        post({ type: 'op', op: 'set', uid: node.uid, name, value: initialValue(name, typeName) })
        input.value = ''
    }
    add.addEventListener('click', submit)
    input.addEventListener('keydown', (event) => {
        if (event.key === 'Enter') submit()
    })
    form.appendChild(input)
    form.appendChild(add)
    return form
}

function idField(node) {
    const row = el('div', 'field')
    row.appendChild(el('label', '', 'id'))
    const holder = el('div', 'value')
    const input = el('input')
    input.type = 'text'
    input.value = node.id
    input.spellcheck = false
    input.disabled = !node.editable
    input.addEventListener('change', () => {
        if (editable()) post({ type: 'op', op: 'setId', uid: node.uid, id: input.value.trim() })
    })
    holder.appendChild(input)
    row.appendChild(holder)
    row.appendChild(el('div', 'side'))
    return row
}

function renderExportsSection(host, node) {
    host.appendChild(el('div', 'group', 'Exports of this schema'))
    if (state.exports.length === 0) host.appendChild(el('div', 'hint', 'Nothing is exported. Press E next to a property to export it: a schema that inherits this one, or uses it as a component, can set only the exported properties.'))
    for (const item of state.exports) {
        const row = el('div', 'export-row' + (item.inherited ? ' inherited' : ''))
        const name = el('input')
        name.type = 'text'
        name.value = item.name
        name.disabled = item.inherited || !editable()
        name.addEventListener('change', () => {
            if (name.value.trim() !== item.name) post({ type: 'op', op: 'renameExport', uid: state.view.uid, name: item.name, to: name.value.trim() })
        })
        row.appendChild(name)
        const target = nodeByUidOrigin(item.uid)
        row.appendChild(el('span', 'target', (target !== null && target.id !== '' ? target.id : item.inherited ? 'base' : '?') + '.' + item.property))
        if (!item.inherited && editable()) {
            const remove = el('button', 'remove', 'x')
            remove.type = 'button'
            remove.title = 'Stop exporting'
            remove.addEventListener('click', () => post({ type: 'op', op: 'removeExport', uid: state.view.uid, name: item.name }))
            row.appendChild(remove)
        }
        host.appendChild(row)
    }
}

// The node of the view that an own export points at (the objects of this schema keep their own uid).
function nodeByUidOrigin(uid) {
    return nodeByUid(uid)
}

function renderInspector(force) {
    const host = $('inspector')
    const node = nodeByUid(state.selected)
    const signature = JSON.stringify([state.selected, node === null ? null : [node.type, node.id, node.own, node.props, node.accepts, node.editable], state.exports, state.error === null, node === null ? 0 : (state.members[membersKey(node)] || []).length])
    if (!force && signature === inspectorSignature && host.contains(document.activeElement)) return
    inspectorSignature = signature
    host.textContent = ''
    if (node === null) {
        host.appendChild(el('div', 'hint', 'Select an object.'))
        return
    }
    const head = el('h3', '', node.instanceOf ? node.instanceOf : node.type)
    if (node.instanceOf) head.appendChild(el('small', '', ' (schema)'))
    if (node.derived) head.appendChild(el('small', '', ' extends ' + node.derived))
    host.appendChild(head)
    if (!node.editable) host.appendChild(el('div', 'hint', 'This object belongs to a base schema or to the inside of a schema that is used as a component. It is changed in that schema.'))
    host.appendChild(idField(node))
    const members = membersOf(node)
    const typeOf = new Map(members.map((member) => [member.name, member.typeName]))
    if (isClosed(node) && node.editable) {
        // the schema is closed: only its exports and the placement can be set
        host.appendChild(el('div', 'group', 'Exported properties'))
        const accepted = node.accepts || []
        if (accepted.length === 0) host.appendChild(el('div', 'hint', 'The schema exports nothing.'))
        for (const item of accepted) host.appendChild(fieldFor(node, item.name, ownOf(node, item.name), item.type, { placeholder: 'default', removable: ownOf(node, item.name) !== null }))
        const names = new Set(accepted.map((item) => item.name))
        const placement = (node.own || []).filter((prop) => !names.has(prop.name))
        host.appendChild(el('div', 'group', 'Placement'))
        for (const prop of placement) host.appendChild(fieldFor(node, prop.name, prop.value, typeOf.get(prop.name) || '', { removable: true }))
        host.appendChild(adderFor(node, members, 'x, y, width, anchors.fill ...', true))
    } else {
        const list = node.editable ? node.own || [] : node.props
        const plain = list.filter((prop) => !isHandler(prop.name))
        const handlers = list.filter((prop) => isHandler(prop.name))
        if (plain.length > 0) host.appendChild(el('div', 'group', 'Properties'))
        for (const prop of plain) host.appendChild(fieldFor(node, prop.name, prop.value, typeOf.get(prop.name) || '', { removable: true, exportable: !node.derived }))
        if (handlers.length > 0) host.appendChild(el('div', 'group', 'Handlers'))
        for (const prop of handlers) host.appendChild(fieldFor(node, prop.name, prop.value, '', { removable: true }))
        if (node.editable) {
            host.appendChild(el('div', 'group', 'Add'))
            host.appendChild(adderFor(node, members, 'property or onSignal', false))
            host.appendChild(el('div', 'group', 'Export'))
            host.appendChild(exportAdder(node, members))
        }
    }
    renderExportsSection(host, node)
}

// Exports a property that is not set yet (it keeps the default of its type).
function exportAdder(node, members) {
    const form = el('div', 'adder')
    const input = el('input')
    input.type = 'text'
    input.placeholder = 'property to export'
    input.setAttribute('list', 'member-names')
    input.spellcheck = false
    const add = el('button', '', 'Export')
    add.type = 'button'
    const submit = () => {
        const name = input.value.trim()
        if (name === '' || !editable() || node.derived) return
        post({ type: 'op', op: 'addExport', uid: node.uid, property: name, typeName: (members.find((member) => member.name === name) || {}).typeName || '' })
        input.value = ''
    }
    add.addEventListener('click', submit)
    input.addEventListener('keydown', (event) => {
        if (event.key === 'Enter') submit()
    })
    form.appendChild(input)
    form.appendChild(add)
    return form
}

// ----- the page -----

function renderBanner() {
    const banner = $('banner')
    if (state.failure !== '') {
        banner.hidden = false
        banner.className = 'banner failure'
        banner.textContent = state.failure
    } else if (state.error !== null) {
        banner.hidden = false
        banner.className = 'banner failure'
        banner.textContent = 'The schema cannot be shown: ' + state.error + ' The composer is read-only until it is fixed in the text.'
    } else if (state.problems.length > 0) {
        banner.hidden = false
        banner.className = 'banner'
        banner.textContent = state.problems[0] + (state.problems.length > 1 ? ' (and ' + (state.problems.length - 1) + ' more problems)' : '')
        banner.title = state.problems.join('\n')
    } else {
        banner.hidden = true
    }
}

function renderHeader() {
    const select = $('extends')
    select.textContent = ''
    const none = el('option', '', '(none)')
    none.value = ''
    select.appendChild(none)
    const known = new Set()
    for (const item of state.types.schemas) {
        const option = el('option', '', item.name)
        option.value = item.ref
        select.appendChild(option)
        known.add(item.ref)
    }
    if (state.extends !== '' && !known.has(state.extends)) {
        const option = el('option', '', state.extends + ' (missing)')
        option.value = state.extends
        select.appendChild(option)
    }
    select.value = state.extends
    select.disabled = !editable()
    if (document.activeElement !== $('imports')) $('imports').value = state.imports.join(', ')
    $('imports').disabled = !editable()
}

function render() {
    renderBanner()
    renderHeader()
    renderPalette()
    renderTree()
    renderCanvas()
    renderInspector(false)
    syncToolbar()
}

function onState(message) {
    state.view = message.view
    state.exports = message.exports
    state.extends = message.extends
    state.imports = message.imports
    state.types = message.types
    state.error = message.error
    state.problems = message.problems || []
    state.failure = ''
    if (message.select !== null && message.select !== undefined) state.selected = message.select
    if (state.view !== null && nodeByUid(state.selected) === null) state.selected = state.view.uid
    render()
}

window.addEventListener('message', (event) => {
    const message = event.data
    if (!message) return
    if (message.type === 'state') onState(message)
    else if (message.type === 'members') {
        state.members[message.typeName] = message.members
        renderInspector(true)
    } else if (message.type === 'failure') {
        state.failure = message.message
        renderBanner()
    }
})

window.addEventListener('keydown', (event) => {
    const typing = event.target.closest && event.target.closest('input, textarea, select') !== null
    if (typing) return
    if (event.key === 'Delete' || event.key === 'Backspace') {
        event.preventDefault()
        deleteSelected()
    }
})

function init() {
    bindCanvas()
    $('zoom').value = String(state.zoom)
    $('zoom').addEventListener('change', () => {
        state.zoom = Number($('zoom').value)
        vscode.setState({ zoom: state.zoom })
        renderCanvas()
    })
    $('type-search').addEventListener('input', () => {
        state.search = $('type-search').value
        renderPalette()
    })
    $('delete').addEventListener('click', deleteSelected)
    $('up').addEventListener('click', () => siblingMove(-1))
    $('down').addEventListener('click', () => siblingMove(1))
    $('extends').addEventListener('change', () => post({ type: 'op', op: 'setExtends', ref: $('extends').value }))
    $('imports').addEventListener('change', () => post({ type: 'op', op: 'setImports', imports: $('imports').value.split(',').map((item) => item.trim()).filter(Boolean) }))
    $('generate').addEventListener('click', () => post({ type: 'generate' }))
    post({ type: 'ready' })
}

init()
