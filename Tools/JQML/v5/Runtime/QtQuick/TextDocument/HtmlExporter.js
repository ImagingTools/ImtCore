// QTextHtmlExporter (qtextdocument.cpp of Qt 6.12): a document as HTML, the way QTextDocument::toHtml and
// QTextDocumentFragment::toHtml write it
const F = require('./Formats')
const { BEGINNING_OF_FRAME, END_OF_FRAME, OBJECT_REPLACEMENT } = require('./Document')

const EXPORT_ENTIRE_DOCUMENT = 0, EXPORT_FRAGMENT = 1

// QString::number(double): %g with 6 significant digits
function qtNumber(v){
    if(!Number.isFinite(v)) return String(v)
    if(Number.isInteger(v) && Math.abs(v) < 1e6) return String(v)
    let s = v.toPrecision(6)
    if(s.includes('e')){
        let [mantissa, exponent] = s.split('e')
        if(mantissa.includes('.')) mantissa = mantissa.replace(/0+$/, '').replace(/\.$/, '')
        let e = Number(exponent)
        return mantissa + 'e' + (e < 0 ? '-' : '+') + String(Math.abs(e)).padStart(2, '0')
    }
    if(s.includes('.')) s = s.replace(/0+$/, '').replace(/\.$/, '')
    return s
}

// QString::toHtmlEscaped
function htmlEscaped(text){
    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;')
}

// colorValue: the name of an opaque color, rgba() of a translucent one, transparent
function colorValue(color){
    let hex = color.length === 9 ? color : color
    let alpha = hex.length === 9 ? parseInt(hex.slice(1, 3), 16) : 255
    let rgb = hex.length === 9 ? '#' + hex.slice(3) : hex
    if(alpha === 255) return rgb
    if(alpha === 0) return 'transparent'
    let alphaValue = (alpha / 255).toFixed(6).replace(/0+$/, '').replace(/\.$/, '')
    return 'rgba(' + parseInt(rgb.slice(1, 3), 16) + ',' + parseInt(rgb.slice(3, 5), 16) + ',' + parseInt(rgb.slice(5, 7), 16) + ',' + alphaValue + ')'
}

// formatDifference: the properties of to that from does not have the same
function formatDifference(from, to){
    let diff = {}
    for(let k in to) if(!F.same(to[k], from[k])) diff[k] = to[k]
    return diff
}

const RTL = /[\p{Script=Hebrew}\p{Script=Arabic}\p{Script=Syriac}\p{Script=Thaana}\p{Script=Nko}\p{Script=Samaritan}\p{Script=Mandaic}]/u

// the direction of the first strong character (QChar::direction DirL, DirR, DirAL): 0 left to right, 1 right to left,
// -1 none
function strongDirection(text){
    for(let c of text){
        if(RTL.test(c)) return 1
        if(/[\p{L}\p{Nl}]/u.test(c)) return 0
    }
    return -1
}

class HtmlExporter {
    // defaultFontFormat: the char format of the document's default font
    constructor(doc, defaultFontFormat){
        this.doc = doc
        this.defaultCharFormat = defaultFontFormat || F.EMPTY
        this.fragmentMarkers = false
        this.closingTags = []
        this.html = ''
    }

    toHtml(mode = EXPORT_ENTIRE_DOCUMENT){
        this.html = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">\n'
            + '<html><head><meta name="qrichtext" content="1" />'
        this.fragmentMarkers = mode === EXPORT_FRAGMENT
        this.html += '<meta charset="utf-8" />'
        if(this.doc.title) this.html += '<title>' + this.doc.title + '</title>'
        this.html += '<style type="text/css">\n'
        this.html += 'p, li { white-space: pre-wrap; }\n'
        this.html += 'hr { height: 1px; border-width: 0; }\n'
        this.html += 'li.unchecked::marker { content: "\\2610"; }\n'
        this.html += 'li.checked::marker { content: "\\2612"; }\n'
        this.html += '</style>'
        this.html += '</head><body'
        if(mode === EXPORT_ENTIRE_DOCUMENT){
            let d = this.defaultCharFormat
            this.html += ' style="'
            this.emitFontFamily(d.families || [])
            if('pointSize' in d) this.html += ' font-size:' + qtNumber(d.pointSize) + 'pt;'
            else if('pixelSize' in d) this.html += ' font-size:' + Math.trunc(d.pixelSize) + 'px;'
            this.html += ' font-weight:' + F.fontWeight(d) + ';'
            this.html += ' font-style:' + (F.fontItalic(d) ? 'italic' : 'normal') + ';'
            let percent = d.letterSpacingType === 'percent'
            if('letterSpacing' in d && (!percent || d.letterSpacing !== 0)){
                let value = d.letterSpacing
                if(percent) value = value / 100 - 1
                this.html += ' letter-spacing:' + qtNumber(value) + (percent ? 'em;' : 'px;')
            }
            if('wordSpacing' in d && d.wordSpacing !== 0) this.html += ' word-spacing:' + qtNumber(d.wordSpacing) + 'px;'
            let decoration = ' text-decoration:'
            let any = false
            if(('fontUnderline' in d || 'underlineStyle' in d) && F.fontUnderline(d)){ decoration += ' underline'; any = true }
            if('overline' in d && d.overline){ decoration += ' overline'; any = true }
            if('strikeOut' in d && d.strikeOut){ decoration += ' line-through'; any = true }
            if(any) this.html += decoration + ';'
            this.html += '"'
            this.emitBackgroundAttribute(this.doc.rootFrameFormat)
        } else {
            this.defaultCharFormat = F.EMPTY
        }
        this.html += '>'
        this.emitFrame(0, this.doc.blocks.length)
        this.html += '</body></html>'
        return this.html
    }

    emitAttribute(attribute, value){
        this.html += ' ' + attribute + '="' + htmlEscaped(value) + '"'
    }

    // the properties of the format that differ from the default char format; whether any was written
    emitCharFormatStyle(format){
        let d = this.defaultCharFormat
        let emitted = false
        let families = format.families || []
        if(families.length && !F.same(families, d.families || [])){
            this.emitFontFamily(families)
            emitted = true
        }
        if('pointSize' in format && format.pointSize !== (d.pointSize || 0)){
            this.html += ' font-size:' + qtNumber(format.pointSize) + 'pt;'
            emitted = true
        } else if('sizeAdjustment' in format){
            let names = ['small', 'medium', 'large', 'x-large', 'xx-large']
            let idx = format.sizeAdjustment + 1
            if(idx >= 0 && idx <= 4){
                this.html += ' font-size:' + names[idx] + ';'
                emitted = true
            }
        } else if('pixelSize' in format && format.pixelSize !== d.pixelSize){
            this.html += ' font-size:' + Math.trunc(format.pixelSize) + 'px;'
            emitted = true
        }
        if('weight' in format && F.fontWeight(format) !== F.fontWeight(d)){
            this.html += ' font-weight:' + F.fontWeight(format) + ';'
            emitted = true
        }
        if('italic' in format && F.fontItalic(format) !== F.fontItalic(d)){
            this.html += ' font-style:' + (F.fontItalic(format) ? 'italic' : 'normal') + ';'
            emitted = true
        }
        let decoration = ' text-decoration:'
        let text = ''
        let hasDecoration = false, any = false
        if(('fontUnderline' in format || 'underlineStyle' in format) && F.fontUnderline(format) !== F.fontUnderline(d)){
            hasDecoration = true
            if(F.fontUnderline(format)){ text += ' underline'; any = true }
        }
        if('overline' in format && F.fontOverline(format) !== F.fontOverline(d)){
            hasDecoration = true
            if(F.fontOverline(format)){ text += ' overline'; any = true }
        }
        if('strikeOut' in format && F.fontStrikeOut(format) !== F.fontStrikeOut(d)){
            hasDecoration = true
            if(F.fontStrikeOut(format)){ text += ' line-through'; any = true }
        }
        if(hasDecoration){
            this.html += decoration + text + (any ? '' : 'none') + ';'
            if('underlineColor' in format) this.html += ' text-decoration-color:' + colorValue(format.underlineColor) + ';'
            emitted = true
        }
        if(format.foreground && format.foreground !== d.foreground){
            this.html += ' color:' + colorValue(format.foreground) + ';'
            emitted = true
        }
        if(format.background && format.background !== d.background){
            this.html += ' background-color:' + colorValue(format.background) + ';'
            emitted = true
        }
        let valign = F.verticalAlignment(format)
        if(valign !== F.verticalAlignment(d) && valign !== F.ALIGN_NORMAL){
            this.html += ' vertical-align:' + (valign === F.ALIGN_SUBSCRIPT ? 'sub' : valign === F.ALIGN_SUPERSCRIPT ? 'super'
                : valign === F.ALIGN_MIDDLE ? 'middle' : valign === F.ALIGN_TOP ? 'top' : valign === F.ALIGN_BOTTOM ? 'bottom' : '') + ';'
            emitted = true
        }
        let caps = F.fontCapitalization(format)
        if(caps !== 0){
            if(caps === 1) this.html += ' text-transform:uppercase;'
            else if(caps === 2) this.html += ' text-transform:lowercase;'
            else if(caps === 3) this.html += ' font-variant:small-caps;'
            emitted = true
        }
        if(F.fontWordSpacing(format) !== 0){
            this.html += ' word-spacing:' + qtNumber(format.wordSpacing) + 'px;'
            emitted = true
        }
        return emitted
    }

    emitTextLength(attribute, length){
        if(!length || length.type === 'variable') return
        this.html += ' ' + attribute + '="' + qtNumber(length.value) + (length.type === 'percent' ? '%"' : '"')
    }

    emitAlignment(align){
        if(align & F.ALIGN_LEFT) return
        if(align & F.ALIGN_RIGHT) this.html += ' align="right"'
        else if(align & F.ALIGN_HCENTER) this.html += ' align="center"'
        else if(align & F.ALIGN_JUSTIFY) this.html += ' align="justify"'
    }

    emitFontFamily(families){
        this.html += ' font-family:'
        let first = true
        for(let family of families){
            let quote = family.includes("'") ? '&quot;' : "'"
            if(!first) this.html += ','
            first = false
            this.html += quote + htmlEscaped(family) + quote
        }
        this.html += ';'
    }

    emitMargins(top, bottom, left, right){
        this.html += ' margin-top:' + top + 'px;'
        this.html += ' margin-bottom:' + bottom + 'px;'
        this.html += ' margin-left:' + left + 'px;'
        this.html += ' margin-right:' + right + 'px;'
    }

    emitPageBreakPolicy(policy){
        if(policy & 0x1) this.html += ' page-break-before:always;'
        if(policy & 0x2) this.html += ' page-break-after:always;'
    }

    emitBackgroundAttribute(format){
        if('backgroundImageUrl' in format) this.emitAttribute('background', format.backgroundImageUrl)
        else if(format.background) this.emitAttribute('bgcolor', colorValue(format.background))
    }

    emitFragment(fragment){
        let format = fragment.format
        let closeAnchor = false
        if(format.isAnchor){
            let names = format.anchorNames || []
            if(names.length) this.html += '<a name="' + htmlEscaped(names[0]) + '"></a>'
            let href = format.anchorHref || ''
            let tooltip = format.toolTip || ''
            if(href || tooltip){
                this.html += '<a'
                if(href) this.html += ' href="' + htmlEscaped(href) + '"'
                if(tooltip) this.html += ' title="' + htmlEscaped(tooltip) + '"'
                this.html += '>'
                closeAnchor = true
            }
        }
        let txt = fragment.text
        let isObject = txt.includes(OBJECT_REPLACEMENT)
        let isImage = isObject && format.objectType === 'image'
        let spanToolTip = !closeAnchor && !isImage ? (format.toolTip || '') : ''
        // the style written apart, then the span with it (the HTML so far is not cut back)
        let outer = this.html
        this.html = ''
        let emitted = false
        if(!isImage) emitted = this.emitCharFormatStyle(format)
        let style = this.html
        this.html = outer
        let spanEmitted = emitted
        if(emitted) this.html += '<span style="' + style + '"'
        if(spanToolTip){
            if(!spanEmitted){
                this.html += '<span'
                spanEmitted = true
            }
            this.html += ' title="' + htmlEscaped(spanToolTip) + '"'
        }
        if(spanEmitted) this.html += '>'
        if(isObject){
            for(let i = 0; isImage && i < txt.length; i++){
                this.html += '<img'
                let maxWidthCss = ''
                if(format.imageMaxWidth){
                    let length = format.imageMaxWidth
                    maxWidthCss += 'max-width:'
                    if(length.type === 'percent') maxWidthCss += qtNumber(length.value) + '%;'
                    else if(length.type === 'fixed') maxWidthCss += qtNumber(length.value) + 'px;'
                }
                if('imageName' in format) this.emitAttribute('src', format.imageName)
                if('imageAlt' in format) this.emitAttribute('alt', format.imageAlt)
                if('imageTitle' in format) this.emitAttribute('title', format.imageTitle)
                if('imageWidth' in format) this.emitAttribute('width', qtNumber(format.imageWidth))
                if('imageHeight' in format) this.emitAttribute('height', qtNumber(format.imageHeight))
                let valign = F.verticalAlignment(format)
                if(valign === F.ALIGN_MIDDLE) this.html += ' style="vertical-align: middle;' + maxWidthCss + '"'
                else if(valign === F.ALIGN_TOP) this.html += ' style="vertical-align: top;' + maxWidthCss + '"'
                else if(maxWidthCss) this.html += ' style="' + maxWidthCss + '"'
                this.html += ' />'
            }
        } else {
            this.html += htmlEscaped(txt).replace(/\n/g, '<br />').replace(/\u2028/g, '<br />')
        }
        if(spanEmitted) this.html += '</span>'
        if(closeAnchor) this.html += '</a>'
    }

    // QTextBlock::textDirection: the layout direction of the format, else the text direction of the document's text option,
    // else the first strong character of the block
    textDirection(index){
        let format = this.doc.blockFormat(index)
        if(format.layoutDirection === 0 || format.layoutDirection === 1) return format.layoutDirection
        if(this.doc.defaultTextDirection === 0 || this.doc.defaultTextDirection === 1) return this.doc.defaultTextDirection
        let direction = strongDirection(this.doc.blockText(index))
        return direction === 1 ? 1 : 0
    }

    emitBlockAttributes(index){
        let format = this.doc.blockFormat(index)
        let align = format.alignment || 0
        if(!(align & 0xf)) align |= F.ALIGN_LEFT
        this.emitAlignment(align)
        if(this.textDirection(index) === 1) this.html += " dir='rtl'"
        this.html += ' style="'
        let emptyBlock = this.doc.blockLength(index) === 1
        if(emptyBlock) this.html += '-qt-paragraph-type:empty;'
        this.emitMargins(qtNumber(format.topMargin || 0), qtNumber(format.bottomMargin || 0), qtNumber(format.leftMargin || 0), qtNumber(format.rightMargin || 0))
        this.html += ' -qt-block-indent:' + (format.indent || 0) + ';'
        this.html += ' text-indent:' + qtNumber(format.textIndent || 0) + 'px;'
        let userState = this.doc.blocks[index].userState
        if(userState !== -1) this.html += ' -qt-user-state:' + userState + ';'
        let lineHeightType = format.lineHeightType || 0
        if(lineHeightType !== 0){
            this.html += ' line-height:' + qtNumber(format.lineHeight || 0)
            switch(lineHeightType){
                case 1: this.html += '%;'; break
                case 2: this.html += '; -qt-line-height-type: fixed;'; break
                case 3: this.html += 'px;'; break
                case 4: this.html += '; -qt-line-height-type: line-distance;'; break
                default: this.html += ';'
            }
        }
        this.emitPageBreakPolicy(format.pageBreakPolicy || 0)
        let diff = {}
        if(emptyBlock) diff = formatDifference(this.defaultCharFormat, this.doc.blockCharFormat(index))
        delete diff.background
        if(format.background) diff.background = format.background
        if(Object.keys(diff).length) this.emitCharFormatStyle(diff)
        this.html += '"'
    }

    emitBlock(index){
        let doc = this.doc
        if(doc.blockLength(index) === 1){
            let p = doc.blockStart(index)
            if(p > 0) p--
            let ch = doc.characterAt(p)
            if(ch === BEGINNING_OF_FRAME || ch === END_OF_FRAME) return
        }
        this.html += '\n'
        let oldDefaultCharFormat = this.defaultCharFormat
        let list = doc.listOf(index)
        let items = list ? doc.listItems(list) : null
        let itemNumber = list ? items.indexOf(index) : -1
        if(list){
            if(itemNumber === 0){
                let format = list.format
                let style = format.style
                let ordered = false
                switch(style){
                    case F.LIST_DISC: this.html += '<ul'; break
                    case F.LIST_CIRCLE: this.html += '<ul type="circle"'; break
                    case F.LIST_SQUARE: this.html += '<ul type="square"'; break
                    case F.LIST_DECIMAL: this.html += '<ol'; ordered = true; break
                    case F.LIST_LOWER_ALPHA: this.html += '<ol type="a"'; ordered = true; break
                    case F.LIST_UPPER_ALPHA: this.html += '<ol type="A"'; ordered = true; break
                    case F.LIST_LOWER_ROMAN: this.html += '<ol type="i"'; ordered = true; break
                    case F.LIST_UPPER_ROMAN: this.html += '<ol type="I"'; ordered = true; break
                    default: this.html += '<ul'
                }
                if(ordered && 'start' in format && format.start !== 1) this.html += ' start="' + format.start + '"'
                let styleString = 'margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px;'
                if('indent' in format) styleString += ' -qt-list-indent: ' + format.indent + ';'
                if('numberPrefix' in format){
                    let prefix = format.numberPrefix.replace(/"/g, '\\22').replace(/'/g, '\\27')
                    styleString += " -qt-list-number-prefix: '" + prefix + "';"
                }
                if('numberSuffix' in format && format.numberSuffix !== '.'){
                    let suffix = format.numberSuffix.replace(/"/g, '\\22').replace(/'/g, '\\27')
                    styleString += " -qt-list-number-suffix: '" + suffix + "';"
                }
                this.html += ' style="' + styleString + '">\n'
            }
            this.html += '<li'
            let blockFmt = formatDifference(this.defaultCharFormat, doc.blockCharFormat(index))
            if(Object.keys(blockFmt).length){
                this.html += ' style="'
                this.emitCharFormatStyle(blockFmt)
                this.html += '"'
                this.defaultCharFormat = F.merge(this.defaultCharFormat, doc.blockCharFormat(index))
            }
            let marker = doc.blockFormat(index).marker
            if(marker === 2) this.html += ' class="checked"'
            else if(marker === 1) this.html += ' class="unchecked"'
        }
        let blockFormat = doc.blockFormat(index)
        if('hrWidth' in blockFormat){
            this.html += '<hr'
            if(blockFormat.hrWidth.type !== 'variable') this.emitTextLength('width', blockFormat.hrWidth)
            this.html += ' '
            if(blockFormat.background) this.html += 'style="background-color:' + colorValue(blockFormat.background) + ';"'
            this.html += '/>'
            return
        }
        let pre = !!blockFormat.nonBreakableLines
        let heading = blockFormat.headingLevel || 0
        if(pre){
            if(list) this.html += '>'
            this.html += '<pre'
        } else if(!list){
            this.html += heading > 0 && heading <= 6 ? '<h' + heading : '<p'
        }
        this.emitBlockAttributes(index)
        this.html += '>'
        let fragments = doc.blockFragments(index)
        if(!fragments.length) this.html += '<br />'
        if(this.fragmentMarkers && fragments.length && index === 0) this.html += '<!--StartFragment-->'
        for(let fragment of fragments) this.emitFragment(fragment)
        if(this.fragmentMarkers && index === doc.blocks.length - 1) this.html += '<!--EndFragment-->'
        let closeTags = ''
        if(pre) this.html += '</pre>'
        else if(list) closeTags += '</li>'
        else this.html += heading > 0 && heading <= 6 ? '</h' + heading + '>' : '</p>'
        if(list){
            let last = itemNumber === items.length - 1
            if(last){
                let style = list.format.style
                let ordered = style === F.LIST_DECIMAL || style === F.LIST_LOWER_ALPHA || style === F.LIST_UPPER_ALPHA
                    || style === F.LIST_LOWER_ROMAN || style === F.LIST_UPPER_ROMAN
                closeTags += ordered ? '</ol>' : '</ul>'
            }
            let nextList = index + 1 < doc.blocks.length ? doc.listOf(index + 1) : null
            if(nextList && doc.itemNumber(nextList, index + 1) === 0 && (nextList.format.indent || 0) > (list.format.indent || 0)){
                let lastTag = ''
                if(this.closingTags.length && last) lastTag = this.closingTags.pop()
                lastTag = closeTags + lastTag
                this.closingTags.push(lastTag)
            } else if(last){
                this.html += closeTags
                if(this.closingTags.length) this.html += this.closingTags.pop()
            } else this.html += closeTags
        }
        this.defaultCharFormat = oldDefaultCharFormat
    }

    // the blocks [from, to) of a frame and the tables in it (QTextFrame::iterator); a cell with one empty block is empty
    emitFrame(from, to, isRoot = true, ancestors = []){
        let doc = this.doc
        if(!this.tableList) this.tableList = doc.tables().map(entry => Object.assign(entry, {
            firstBlock: doc.blockIndexAt(entry.start + 1),
            lastBlock: doc.blockIndexAt(entry.end),
        }))
        // a table starts at a block when its first U+FDD0 ends the block before
        let tableAt = (i) => this.tableList.find(t => t.firstBlock === i && t.start === doc.blockStart(i) - 1 && !ancestors.includes(t))
        if(!isRoot && to - from === 1 && doc.blockLength(from) === 1 && !tableAt(from)) return
        for(let i = from; i < to; i++){
            let entry = tableAt(i)
            if(entry){
                this.emitTable(entry, ancestors.concat([entry]))
                i = entry.lastBlock
                continue
            }
            this.emitBlock(i)
        }
    }

    // QTextHtmlExporter::emitTable
    emitTable(entry, ancestors = [entry]){
        let doc = this.doc
        let format = entry.table.format
        this.html += '\n<table'
        if('border' in format) this.emitAttribute('border', qtNumber(format.border))
        this.emitFrameStyle(format, 'table')
        let align = format.alignment || 0
        if(!(align & 0xf)) align |= F.ALIGN_LEFT
        this.emitAlignment(align)
        this.emitTextLength('width', format.width)
        if('cellSpacing' in format) this.emitAttribute('cellspacing', qtNumber(format.cellSpacing))
        if('cellPadding' in format) this.emitAttribute('cellpadding', qtNumber(format.cellPadding))
        this.emitBackgroundAttribute(format)
        this.html += '>'
        let grid = doc.tableGrid(entry)
        let rows = grid.rows, columns = grid.columns
        let widths = format.columnWidthConstraints && format.columnWidthConstraints.length === columns ? format.columnWidthConstraints : new Array(columns).fill({ type: 'variable', value: 0 })
        let widthEmitted = new Array(columns).fill(false)
        let headerRowCount = Math.min(format.headerRowCount || 0, rows)
        if(headerRowCount > 0) this.html += '<thead>'
        for(let row = 0; row < rows; row++){
            this.html += '\n<tr>'
            for(let col = 0; col < columns; col++){
                let cell = grid.cellAt(row, col)
                if(!cell || cell.row !== row || cell.column !== col) continue
                this.html += '\n<td'
                if(!widthEmitted[col] && cell.columnSpan === 1){
                    this.emitTextLength('width', widths[col])
                    widthEmitted[col] = true
                }
                if(cell.columnSpan > 1) this.emitAttribute('colspan', String(cell.columnSpan))
                if(cell.rowSpan > 1) this.emitAttribute('rowspan', String(cell.rowSpan))
                let cf = cell.format
                this.emitBackgroundAttribute(cf)
                if(cf.toolTip) this.emitAttribute('title', cf.toolTip)
                let oldDefault = this.defaultCharFormat
                let style = ''
                let valign = F.verticalAlignment(cf)
                if(valign >= F.ALIGN_MIDDLE && valign <= F.ALIGN_BOTTOM){
                    style += ' vertical-align:' + (valign === F.ALIGN_MIDDLE ? 'middle' : valign === F.ALIGN_TOP ? 'top' : 'bottom') + ';'
                    this.defaultCharFormat = F.merge(this.defaultCharFormat, { verticalAlignment: valign })
                }
                for(let side of ['left', 'right', 'top', 'bottom']){
                    let key = side + 'Padding'
                    if(key in cf) style += ' padding-' + side + ':' + qtNumber(cf[key]) + ';'
                }
                for(let side of ['top', 'right', 'bottom', 'left']) if((side + 'Border') in cf) style += ' border-' + side + ':' + qtNumber(cf[side + 'Border']) + 'px;'
                for(let side of ['top', 'right', 'bottom', 'left']) if((side + 'BorderBrush') in cf) style += ' border-' + side + '-color:' + cf[side + 'BorderBrush'].slice(-7) + ';'
                for(let side of ['top', 'right', 'bottom', 'left']) if((side + 'BorderStyle') in cf) style += ' border-' + side + '-style:' + cf[side + 'BorderStyle'] + ';'
                if(style) this.html += ' style="' + style + '"'
                this.html += '>'
                this.emitFrame(doc.blockIndexAt(cell.first), doc.blockIndexAt(cell.last) + 1, false, ancestors)
                this.html += '</td>'
                this.defaultCharFormat = oldDefault
            }
            this.html += '</tr>'
            if(headerRowCount > 0 && row === headerRowCount - 1) this.html += '</thead>'
        }
        this.html += '</table>'
    }

    // QTextHtmlExporter::emitFrameStyle
    emitFrameStyle(format, frameType){
        let mark = this.html.length
        this.html += ' style="'
        let start = this.html.length
        if(frameType === 'frame') this.html += '-qt-table-type: frame;'
        else if(frameType === 'root') this.html += '-qt-table-type: root;'
        if(format.position === 'left') this.html += ' float: left;'
        else if(format.position === 'right') this.html += ' float: right;'
        this.emitPageBreakPolicy(format.pageBreakPolicy || 0)
        if(format.borderBrush && format.borderBrush !== '#808080') this.html += ' border-color:' + colorValue(format.borderBrush) + ';'
        if(format.borderStyle && format.borderStyle !== 'outset') this.html += ' border-style:' + format.borderStyle + ';'
        if('margin' in format || 'leftMargin' in format || 'rightMargin' in format || 'topMargin' in format || 'bottomMargin' in format){
            let m = (k) => qtNumber(k in format ? format[k] : (format.margin || 0))
            this.emitMargins(m('topMargin'), m('bottomMargin'), m('leftMargin'), m('rightMargin'))
        }
        if(format.borderCollapse) this.html += ' border-collapse:collapse;'
        if(this.html.length === start) this.html = this.html.slice(0, mark)
        else this.html += '"'
    }
}

module.exports = { HtmlExporter, EXPORT_ENTIRE_DOCUMENT, EXPORT_FRAGMENT, qtNumber, htmlEscaped, colorValue, strongDirection }
