// The rich text of a TextEdit (QQuickTextEdit / QQuickTextControl of Qt 6.12 with a QTextDocument): setting it as HTML,
// reading it back (text, getText, getFormattedText), the edits of the methods and of the keys. A document is changed in
// place; the TextEdit keeps a copy for each undo step.
const F = require('./Formats')
const { TextDocument, PARAGRAPH_SEPARATOR, LINE_SEPARATOR, OBJECT_REPLACEMENT } = require('./Document')
const { TextCursor } = require('./Cursor')
const { setHtml } = require('./HtmlImporter')
const { HtmlExporter, EXPORT_FRAGMENT, strongDirection } = require('./HtmlExporter')
const { fragmentOf, fragmentFromHtml, insertFragment } = require('./Fragment')

// Qt::mightBeRichText (qtextdocument.cpp): the first tag of the first line is an element Qt knows
const HTML_ELEMENTS = new Set(['a', 'address', 'b', 'big', 'blockquote', 'body', 'br', 'caption', 'center', 'cite', 'code', 'dd', 'dfn',
    'div', 'dl', 'dt', 'em', 'font', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'head', 'hr', 'html', 'i', 'img', 'kbd', 'li', 'link', 'meta',
    'nobr', 'ol', 'p', 'pre', 'qt', 's', 'samp', 'script', 'small', 'span', 'strong', 'style', 'sub', 'sup', 'table', 'tbody',
    'td', 'tfoot', 'th', 'thead', 'title', 'tr', 'tt', 'u', 'ul', 'var'])

function mightBeRichText(text){
    text = String(text)
    let n = text.length, start = 0
    while(start < n && /\s/.test(text[start])) start++
    if(text.substr(start, 5) === '<?xml'){
        while(start < n){
            if(text[start] === '?' && start + 2 < n && text[start + 1] === '>'){ start += 2; break }
            start++
        }
        while(start < n && /\s/.test(text[start])) start++
    }
    if(text.substr(start, 5).toLowerCase() === '<!doc') return true
    let open = start
    while(open < n && text[open] !== '<' && text[open] !== '\n'){
        if(text[open] === '&' && text.substr(open + 1, 3) === 'lt;') return true
        open++
    }
    if(open >= n || text[open] !== '<') return false
    let close = text.indexOf('>', open)
    if(close < 0) return false
    let tag = ''
    for(let i = open + 1; i < close; i++){
        let c = text[i]
        let space = /\s/.test(c)
        if(/[\p{L}\p{Nd}]/u.test(c)) tag += c.toLowerCase()
        else if(tag && space) break
        else if(tag && c === '/' && i + 1 === close) break
        else if(!space && (tag || c !== '!')) return false
    }
    return HTML_ELEMENTS.has(tag)
}

function createDocument(){
    return new TextDocument()
}

// QTextDocument::setHtml through QQuickTextControlPrivate::setContent (an empty text clears the document)
function setDocumentHtml(doc, html){
    // QTextDocument::setHtml: no undo step, the undo history cleared
    let undo = doc.undoEnabled
    doc.undoEnabled = false
    if(html === '') doc.clear()
    else setHtml(doc, html)
    doc.undoEnabled = undo
    doc.clearUndo()
    updateContentDirection(doc)
}

// QTextDocument::setPlainText: the text in one block for each line (\n, \r\n, \r, U+2029), no undo step
function setPlainText(doc, text){
    let undo = doc.undoEnabled
    doc.undoEnabled = false
    doc.clear()
    if(text.length) new TextCursor(doc, 0).insertText(text, F.EMPTY)
    doc.undoEnabled = undo
    doc.clearUndo()
    updateContentDirection(doc)
}

// QTextCursor::deleteChar / deletePreviousChar at position: one step the next one joins; the cursor after it
function deleteChar(doc, position, previous){
    let cursor = new TextCursor(doc, position)
    cursor.deleteChar(previous)
    updateContentDirection(doc)
    return cursor.position
}

// QQuickTextEdit::q_textChanged: the direction of the first block with a strong character sets the text direction of
// the document's text option (left to right, the direction of the input method, when there is none)
function updateContentDirection(doc){
    let direction = 0
    for(let b = 0; b < doc.blocks.length; b++){
        let d = strongDirection(doc.blockText(b))
        if(d !== -1){
            direction = d
            break
        }
    }
    doc.defaultTextDirection = direction
}

// QTextDocument::toHtml with the default font of the document
function toHtml(doc, fontFormat){
    return new HtmlExporter(doc, fontFormat).toHtml()
}

// QQuickTextEdit::getFormattedText: QTextDocumentFragment(cursor).toHtml()
function formattedText(doc, start, end){
    let fragment = fragmentOf(doc, start, end)
    return new HtmlExporter(fragment).toHtml(EXPORT_FRAGMENT)
}

// QQuickTextEdit::insert of a rich text: QTextCursor::insertHtml at position; the position after it
function insertHtml(doc, position, html){
    let cursor = new TextCursor(doc, position)
    insertFragment(cursor, fragmentFromHtml(html))
    updateContentDirection(doc)
    return cursor.position
}

// QTextCursor::insertText at position (plain text in a rich document: QTextCursor::insertText with the char format at
// the cursor, or the format given)
function insertText(doc, position, text, format){
    let cursor = new TextCursor(doc, position)
    cursor.insertText(text, format)
    updateContentDirection(doc)
    return cursor.position
}

// the selection from anchor to position replaced by text typed (QQuickTextControl: cursor.insertText(e->text()) over
// the selection); the cursor after it
function replaceText(doc, anchor, position, text, format){
    let cursor = new TextCursor(doc, anchor)
    cursor.setPosition(position, true)
    if(format === undefined && anchor !== position){
        // the format at the cursor, as it had it before the selection was removed
        let probe = new TextCursor(doc, position)
        format = F.without(probe.charFormat(), 'objectType')
    }
    if(text.length) cursor.insertText(text, format)
    else cursor.removeSelectedText()
    updateContentDirection(doc)
    return cursor.position
}

// QQuickTextEdit::remove; the range removed indeed ([start, end) widened over a table), null for cells cleared
function removeRange(doc, start, end){
    if(start === end) return { start, end }
    let cursor = new TextCursor(doc, start)
    cursor.setPosition(end, true)
    cursor.removeSelectedText()
    updateContentDirection(doc)
    return cursor.removed
}

// QQuickTextEdit::append: a new block at the end (unless the document is empty), the text in it as rich or plain text
// (QTextCursor::beginEditBlock of a cursor at the start: undoing it puts the cursor at the start)
function append(doc, text, rich){
    doc.beginEditBlock(0)
    let cursor = new TextCursor(doc, doc.length())
    if(!doc.isEmpty()) cursor.insertBlock()
    if(rich) insertFragment(cursor, fragmentFromHtml(text))
    else cursor.insertText(text)
    doc.endEditBlock()
    updateContentDirection(doc)
}

// QQuickTextControl::keyPressEvent, Return: QTextCursor::insertBlock at the cursor; the cursor after it
function insertBlockAt(doc, anchor, position){
    let cursor = new TextCursor(doc, anchor)
    cursor.setPosition(position, true)
    let blockFormat = doc.blockFormat(doc.blockIndexAt(position))
    let probe = new TextCursor(doc, position)
    let charFormat = F.without(probe.charFormat(), 'objectType')
    cursor.insertBlock(blockFormat, charFormat)
    return cursor.position
}

// Backspace at the start of a block (QQuickTextControlPrivate::keyPressEvent): a list item leaves its list, an indented
// block loses one indent; null when it is neither (the previous character is deleted)
function backspaceAtBlockStart(doc, position){
    let b = doc.blockIndexAt(position)
    if(doc.blockStart(b) !== position) return null
    let format = doc.blockFormat(b)
    if(format.list){
        // QTextList::remove: the block leaves the list, the list's indent is added to its own
        let indent = (format.indent || 0) + (format.list.format.indent || 0)
        doc.setBlockFormat(b, F.with_(F.without(format, 'list'), { indent }))
        return true
    }
    if((format.indent || 0) > 0){
        doc.setBlockFormat(b, F.with_(format, { indent: format.indent - 1 }))
        return true
    }
    return null
}

// QTextCursor::selectedText of a selection from anchor to position
function selectedText(doc, anchor, position){
    let cursor = new TextCursor(doc, anchor)
    cursor.setPosition(position, true)
    return cursor.selectedText()
}

// the link of the character at the position, '' when it has none
function anchorAt(doc, position){
    if(position < 0 || position >= doc.length()) return ''
    let format = doc.charFormatAt(position)
    return format.isAnchor && format.anchorHref ? format.anchorHref : ''
}

module.exports = {
    mightBeRichText, createDocument, setDocumentHtml, setPlainText, deleteChar, toHtml, formattedText, insertHtml, insertText, replaceText, removeRange,
    append, insertBlockAt, backspaceAtBlockStart, anchorAt, selectedText, updateContentDirection,
    PARAGRAPH_SEPARATOR, LINE_SEPARATOR, OBJECT_REPLACEMENT,
}
