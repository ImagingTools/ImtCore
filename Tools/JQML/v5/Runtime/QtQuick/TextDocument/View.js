// The layout of a rich text as DOM (what QQuickTextNodeEngine draws for QQuickTextEdit): the backgrounds of blocks and
// of formats, the selection, each piece of text at its place in its own font and color, images, list markers, rules.
// The view is rebuilt when the layout, the selection or the colors change.
const { toCss } = require('../../Core/color')
const { familyCss } = require('../TextLayout')
const F = require('./Formats')
const { displayText } = require('./Layout')

function fontCss(style, font){
    style.fontFamily = familyCss(font.family)
    style.fontSize = font.pixelSize + 'px'
    style.fontWeight = String(font.weight)
    style.fontStyle = font.italic ? 'italic' : 'normal'
    let decoration = [font.underline ? 'underline' : '', font.overline ? 'overline' : '', font.strikeout ? 'line-through' : ''].filter(Boolean).join(' ')
    style.textDecoration = decoration || 'none'
    style.letterSpacing = font.letterSpacing ? font.letterSpacing + 'px' : ''
    style.wordSpacing = font.wordSpacing ? font.wordSpacing + 'px' : ''
    style.fontVariant = font.capitalization === 3 ? 'small-caps' : ''
}

const box = (parent, left, top, width, height, background) => {
    let el = document.createElement('div')
    el.style.cssText = 'position:absolute;left:' + left + 'px;top:' + top + 'px;width:' + Math.max(0, width) + 'px;height:' + Math.max(0, height) + 'px;'
    if(background) el.style.background = background
    parent.appendChild(el)
    return el
}

// options: { layout, layouter, selectionStart, selectionEnd, color, selectionColor, selectedTextColor, width (of the
// text area), resolveUrl(name) }
function render(container, options){
    let { layout, layouter } = options
    let doc = options.doc
    let selStart = options.selectionStart, selEnd = options.selectionEnd
    let hasSelection = selStart < selEnd
    let selectionCss = toCss(options.selectionColor)
    let selectedTextCss = toCss(options.selectedTextColor)
    let colorCss = toCss(options.color)
    let fullWidth = Math.max(layout.width, options.width || 0)
    // the drawing of a block is kept while its layout (the same object) and the colors stay, and no selection is in it
    let colorKey = [colorCss, selectionCss, selectedTextCss, fullWidth, layout.margin].join(',')
    let kept = container.$jq2ColorKey === colorKey && container.$jq2Blocks ? container.$jq2Blocks : new Map()
    let keep = new Map()
    let children = []
    let tablesLayer = document.createElement('div')
    tablesLayer.style.cssText = 'position:absolute;left:0;top:0;'
    children.push(tablesLayer)
    // tables: their background, border and the cells' backgrounds and borders (QTextDocumentLayoutPrivate::drawFrame)
    let inner = { outset: 'inset', inset: 'outset', groove: 'ridge', ridge: 'groove' }
    for(let table of layout.tables || []){
        let el = box(tablesLayer, table.x, table.y, table.width, table.height, table.background ? toCss(table.background) : '')
        el.style.boxSizing = 'border-box'
        if(table.border > 0) el.style.border = table.border + 'px ' + (table.borderStyle === 'none' ? 'solid' : table.borderStyle) + ' ' + toCss(table.borderColor)
        for(let c of table.cells){
            let cell = box(tablesLayer, c.x, c.y, c.width, c.height, c.background ? toCss(c.background) : '')
            cell.style.boxSizing = 'border-box'
            let f = c.cell.format
            if(c.border > 0) cell.style.border = c.border + 'px ' + (inner[table.borderStyle] || table.borderStyle || 'solid') + ' ' + toCss(table.borderColor)
            for(let side of ['top', 'right', 'bottom', 'left']){
                let key = side.charAt(0).toUpperCase() + side.slice(1)
                if((side + 'Border') in f){
                    cell.style['border' + key + 'Width'] = f[side + 'Border'] + 'px'
                    cell.style['border' + key + 'Style'] = f[side + 'BorderStyle'] || 'solid'
                    cell.style['border' + key + 'Color'] = toCss(f[side + 'BorderBrush'] || table.borderColor)
                }
            }
        }
    }
    for(let block of layout.blocks){
        let selected = hasSelection && selStart <= block.start + block.length && selEnd >= block.start
        let group = selected ? null : kept.get(block)
        if(!group){
            group = document.createElement('div')
            group.style.cssText = 'position:absolute;left:0;top:0;'
            drawBlock(group, block)
        }
        if(!selected) keep.set(block, group)
        children.push(group)
    }
    container.replaceChildren(...children)
    container.$jq2Blocks = keep
    container.$jq2ColorKey = colorKey

    function drawBlock(container, block){
        let format = block.format
        // the background of the block over the width of the text
        if(format.background && block.lines.length){
            let top = block.lines[0].top
            let bottom = block.lines[block.lines.length - 1].top + block.lines[block.lines.length - 1].lineHeight
            box(container, block.left - (format.leftMargin || 0) < 0 ? 0 : layout.margin, top, fullWidth - 2 * layout.margin, bottom - top, toCss(format.background))
        }
        if(block.hr){
            let line = block.lines[0]
            let width = fullWidth - 2 * layout.margin
            let length = format.hrWidth
            if(length && length.type === 'fixed') width = length.value
            else if(length && length.type === 'percent') width = width * length.value / 100
            let el = box(container, layout.margin, line.top + line.lineHeight / 2, width, 1, format.background ? toCss(format.background) : '#a0a0a0')
            el.className = 'jq2-rich-hr'
            return
        }
        if(block.marker){
            let m = block.marker
            let color = m.format.foreground ? toCss(m.format.foreground) : colorCss
            if(m.check){
                // the box of a task, a cross of two lines in it when checked (QTextDocumentLayoutPrivate::drawListItem)
                let adj = m.check.adj
                let outer = box(container, m.x - adj, m.y - adj, m.width + 2 * adj, m.height + 2 * adj)
                outer.className = 'jq2-rich-check'
                outer.dataset.checked = m.check.checked ? '1' : '0'
                outer.style.border = '1px solid ' + color
                outer.style.boxSizing = 'border-box'
                if(m.check.checked){
                    let ns = 'http://www.w3.org/2000/svg'
                    let svg = document.createElementNS(ns, 'svg')
                    svg.setAttribute('width', String(m.width))
                    svg.setAttribute('height', String(m.height))
                    svg.style.cssText = 'position:absolute;overflow:visible;left:' + (adj - 1) + 'px;top:' + (adj - 1) + 'px;'
                    for(let [x1, y1, x2, y2] of [[0, 0, m.width, m.height], [m.width, 0, 0, m.height]]){
                        let l = document.createElementNS(ns, 'line')
                        l.setAttribute('x1', x1)
                        l.setAttribute('y1', y1)
                        l.setAttribute('x2', x2)
                        l.setAttribute('y2', y2)
                        l.setAttribute('stroke', color)
                        l.setAttribute('stroke-width', '2')
                        svg.appendChild(l)
                    }
                    outer.appendChild(svg)
                }
            }
            // a bullet (none in a task's box), or the item's number
            if(m.shape){
                if(!m.check){
                    let el = box(container, m.x, m.y, m.width, m.height)
                    if(m.shape === 'disc'){ el.style.borderRadius = '50%'; el.style.background = color }
                    else if(m.shape === 'circle'){ el.style.borderRadius = '50%'; el.style.border = '1px solid ' + color; el.style.boxSizing = 'border-box' }
                    else el.style.background = color
                }
            } else {
                let el = document.createElement('span')
                el.textContent = m.text
                el.style.cssText = 'position:absolute;white-space:pre;line-height:normal;left:' + m.x + 'px;top:' + m.y + 'px;'
                fontCss(el.style, m.font)
                el.style.textDecoration = 'none'
                el.style.color = color
                container.appendChild(el)
            }
        }
        for(let li = 0; li < block.lines.length; li++){
            let line = block.lines[li]
            // the selection of the line: its characters, to the end of the line when it goes on past it
            if(hasSelection && selStart < Math.max(line.end, line.start + 1) && selEnd > line.start){
                let from = Math.max(selStart, line.start), to = Math.min(selEnd, line.end)
                let x1 = from <= line.start ? line.x : options.cursorToX(line, from)
                let x2 = options.cursorToX(line, to)
                let past = selEnd > line.end
                if(past) x2 += layouter.width(' ', layouter.font(doc.blockCharFormat(block.index)))
                if(x2 > x1) box(container, x1, line.top, x2 - x1, line.lineHeight, selectionCss)
            }
            for(let item of line.items){
                if(item.kind === 'break') continue
                let format = item.format
                let font = item.font
                let m = layouter.metrics(font)
                let shift = font.valign === F.ALIGN_SUPERSCRIPT ? -m.ascent * 0.5 : font.valign === F.ALIGN_SUBSCRIPT ? m.descent * 0.6 : 0
                if(format.background && item.kind !== 'object'){
                    box(container, item.x, line.top, item.width, line.lineHeight, toCss(format.background))
                }
                if(item.kind === 'object'){
                    let img = document.createElement('img')
                    let src = options.resolveUrl ? options.resolveUrl(format.imageName || '') : (format.imageName || '')
                    img.src = src
                    img.alt = format.imageAlt || ''
                    img.draggable = false
                    img.style.cssText = 'position:absolute;left:' + item.x + 'px;top:' + (line.top + line.ascent - item.height) + 'px;width:' + item.width + 'px;height:' + item.height + 'px;'
                    // the natural size of the image, once it is there (at once for one the browser has already)
                    if(options.onImageLoad){
                        if(img.complete && img.naturalWidth) queueMicrotask(() => options.onImageLoad(format.imageName || '', img))
                        else img.addEventListener('load', () => options.onImageLoad(format.imageName || '', img), { once: true })
                    }
                    container.appendChild(img)
                    if(hasSelection && item.start >= selStart && item.end <= selEnd) box(container, item.x, line.top, item.width, line.lineHeight, selectionCss).style.opacity = '0.5'
                    continue
                }
                if(item.kind === 'tab' || !item.text.trim() && !font.underline && !font.strikeout && !font.overline) continue
                let baseColor = format.foreground ? toCss(format.foreground) : colorCss
                // the selected part in the selected text color
                let pieces = [{ start: item.start, end: item.end, selected: false }]
                if(hasSelection && selStart < item.end && selEnd > item.start){
                    pieces = []
                    let a = Math.max(selStart, item.start), b = Math.min(selEnd, item.end)
                    if(a > item.start) pieces.push({ start: item.start, end: a, selected: false })
                    pieces.push({ start: a, end: b, selected: true })
                    if(b < item.end) pieces.push({ start: b, end: item.end, selected: false })
                }
                for(let piece of pieces){
                    let text = item.text.slice(piece.start - item.start, piece.end - item.start)
                    if(!text) continue
                    let x = piece.start === item.start ? item.x : options.cursorToX(line, piece.start)
                    let span = document.createElement('span')
                    span.textContent = displayText(text, font)
                    span.style.cssText = 'position:absolute;white-space:pre;line-height:normal;left:' + x + 'px;top:' + (line.top + line.ascent - m.ascent + shift) + 'px;'
                    fontCss(span.style, font)
                    span.style.color = piece.selected ? selectedTextCss : baseColor
                    if(format.underlineColor) span.style.textDecorationColor = toCss(format.underlineColor)
                    if(format.isAnchor && format.anchorHref){
                        span.dataset.href = format.anchorHref
                        span.className = 'jq2-rich-link'
                    }
                    if(format.toolTip) span.title = format.toolTip
                    container.appendChild(span)
                }
            }
        }
    }
}

module.exports = { render }
