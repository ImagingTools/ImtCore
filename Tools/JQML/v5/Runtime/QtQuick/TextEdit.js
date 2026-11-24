const engine = require('../Core/engine')
const QtEnums = require('../Qt/enums')
const { toCss } = require('../Core/color')
const { defineType, makeReadonly } = require('../Core/type')
const { QRectF } = require('../Core/ValueTypes')
const { Item, bits } = require('./Item')
const { TextField, common, relayout } = require('./TextField')
const { measure } = require('./TextLayout')
const { ACCEPTED } = require('./KeyDelivery')
const RT = require('./TextDocument/RichText')
const L = require('./TextDocument/Layout')
const F = require('./TextDocument/Formats')
const { TextDocument } = require('./TextDocument/Document')
const { TextCursor } = require('./TextDocument/Cursor')
const { fragmentOf, fragmentFromHtml, insertFragment } = require('./TextDocument/Fragment')
const M = require('./TextDocument/Markdown')

const STYLE = bits.STYLE
const TEXT = 64

const PLAIN_TEXT = 0, RICH_TEXT = 1, AUTO_TEXT = 2, MARKDOWN_TEXT = 3

const DEFAULT_FONT = { pixelSize: 12, pointSize: 9, sizeUnit: 'pt', family: 'Segoe UI', weight: 400, italic: false, underline: false, strikeout: false, capitalization: 0, letterSpacing: 0, wordSpacing: 0 }

// QQuickTextUtil::alignedY
function alignedY(textHeight, availableHeight, alignment){
    if(alignment & 0x40) return availableHeight - textHeight
    if(alignment & 0x80) return (availableHeight - textHeight) / 2
    return 0
}

// QTextDocumentFragment::toPlainText: separators as line feeds, no-break spaces as spaces
const plainOf = (raw) => raw.replace(/[\ufdd0\ufdd1\u2029\u2028]/g, '\n').replace(/\u00a0/g, ' ')

// A TextEdit keeps its text as a QTextDocument (QQuickTextEdit): a plain text is a document without formats whose text
// is its plain text (QTextDocument::toPlainText), a rich text (RichText, or AutoText that looks like HTML) one whose
// text is its HTML (toHtml), a Markdown text (MarkdownText) one whose text is its Markdown (toMarkdown). $rich: the
// text is rich or Markdown (the document's formats are drawn, its raw text is the content); $markdown: Markdown. The edits of the methods and of the keys are the document's, as QTextCursor makes them, and
// so is the undo history (QTextDocument::undo / redo). A plain text is shown by the native control; a rich text is laid
// out (TextDocument/Layout.js) and drawn (TextDocument/View.js) over the control, which then only takes the keys and the
// input methods.
class TextEdit extends TextField {
    constructor(parent = null){
        super(parent, true)
    }

    // drawn by the layout of the document: a rich text, or a plain one a TextSelection gave formats to (QTextDocument draws
    // the formats it has whatever text() gives)
    $drawsRich(){
        return this.$rich || !!this.$styled
    }

    // QQuickTextEdit::cursorSelection: the TextSelection of the TextEdit's cursor
    $selection(){
        if(!this.$cursorSelection){
            const { TextSelection } = require('./TextSelection')
            this.$cursorSelection = new TextSelection(this, true)
        }
        return this.$cursorSelection
    }

    // QQuickTextEdit::textDocument
    $textDocument(){
        if(!this.$quickDocument){
            const { TextDocument: QuickTextDocument } = require('./TextDocumentType')
            this.$quickDocument = new QuickTextDocument(this)
        }
        return this.$quickDocument
    }

    // a file loaded by textDocument.source (QQuickTextDocumentPrivate::load and onDocumentStatusChanged): HTML or Markdown
    // as such unless textFormat is PlainText, the cursor at the start; rich when the format allows it
    $loadDocument(text, format, base){
        let C = TextEdit
        let textFormat = this.$v[C.TEXTFORMAT]
        let plainOnly = textFormat === PLAIN_TEXT
        // onDocumentStatusChanged: rich (Markdown) when the format is RichText (MarkdownText) or AutoText; a document of
        // the other one keeps its formats, its text is the plain text
        let rich = format === 'html' && (textFormat === RICH_TEXT || textFormat === AUTO_TEXT)
        let markdown = format === 'markdown' && (textFormat === MARKDOWN_TEXT || textFormat === AUTO_TEXT)
        this.$beginEdit()
        if((rich || markdown) !== this.$rich) this.$setRich(rich || markdown)
        this.$markdown = markdown
        if(format === 'html' && !plainOnly) RT.setDocumentHtml(this.$doc, text)
        else if(format === 'markdown' && !plainOnly) M.setMarkdown(this.$doc, text, this.$fontInfo())
        else RT.setPlainText(this.$doc, text)
        this.$styled = !rich && !markdown && !plainOnly && format !== 'plain'
        if(!this.$v[C.BASEURL] && base) this.$write(C.BASEURL, base)
        this.$commitEdit(this.$doc, 0, 0, false)
    }

    // the line moves of TextSelection (QTextCursor Up, Down, StartOfLine, EndOfLine) by the layout
    $lineMoves(){
        if(!this.$drawsRich()){
            let layout = this.$layout()
            let lines = layout.lines
            let at = (pos) => this.$lineIndexOf(lines, pos)
            let column = (pos) => pos - lines[at(pos)].start
            return {
                start: (pos) => lines[at(pos)].start,
                end: (pos) => lines[at(pos)].start + lines[at(pos)].text.length,
                up: (pos) => at(pos) > 0 ? Math.min(lines[at(pos) - 1].start + column(pos), lines[at(pos) - 1].start + lines[at(pos) - 1].text.length) : -1,
                down: (pos) => at(pos) < lines.length - 1 ? Math.min(lines[at(pos) + 1].start + column(pos), lines[at(pos) + 1].start + lines[at(pos) + 1].text.length) : -1,
            }
        }
        let { layout, layouter } = this.$richLayout()
        let all = []
        for(let block of layout.blocks) for(let line of block.lines) all.push(line)
        let lineAt = (pos) => all.indexOf(L.lineOf(layout, pos).line)
        let end = (line) => {
            let last = line.items[line.items.length - 1]
            return last && last.kind === 'break' ? last.start : line.end
        }
        let across = (pos, step) => {
            let i = lineAt(pos)
            let target = all[i + step]
            if(!target) return -1
            return L.xToCursor(layouter, target, L.cursorToX(layouter, all[i], pos))
        }
        return {
            start: (pos) => all[lineAt(pos)].start,
            end: (pos) => end(all[lineAt(pos)]),
            up: (pos) => across(pos, -1),
            down: (pos) => across(pos, 1),
        }
    }

    // the document, made at its first use (the base class completes a TextEdit made outside a component already in its
    // constructor)
    get $doc(){
        return this.$document || (this.$document = RT.createDocument())
    }

    $write(id, value){
        let C = TextEdit
        if(id === C.TEXTFORMAT) return this.$setTextFormat(value)
        let old = this.$v[id]
        super.$write(id, value)
        if(id === Item.WIDTH && old !== this.$v[id] && (this.$v[C.WRAPMODE] || this.$drawsRich())) this.$updateImplicitSize()
    }

    // QQuickTextControlPrivate::setContent: a new text puts the cursor at the start
    $cursorAfterSetText(){
        return 0
    }

    // QQuickTextEditPrivate::updateMouseCursorShape (a TextInput sets no cursor): a link shows the pointing hand
    $cursor(){
        if(this.$drawsRich() && this.$v[TextEdit.HOVEREDLINK]) return QtEnums.PointingHandCursor
        return this.$v[TextEdit.READONLY] && !this.$v[TextEdit.SELECTBYMOUSE] ? QtEnums.ArrowCursor : QtEnums.IBeamCursor
    }

    // ---- the document ----

    // how a text is read in the text format (QQuickTextEdit::setText): 'html', 'markdown' or 'plain'
    $kindFor(text){
        let format = this.$v[TextEdit.TEXTFORMAT]
        if(format === RICH_TEXT || (format === AUTO_TEXT && RT.mightBeRichText(text))) return 'html'
        return format === MARKDOWN_TEXT ? 'markdown' : 'plain'
    }

    // the text as the document has it, one character for each position: the raw text of a rich text (block separators
    // between blocks), the plain text of a plain one
    $content(){
        if(!this.$doc) return super.$content()
        if(this.$raw === null || this.$raw === undefined) this.$raw = this.$rich ? this.$doc.rawText() : this.$doc.plainText()
        return this.$raw
    }

    $layoutText(){
        return this.$rich ? this.$content() : super.$layoutText()
    }

    // the document changed: its text, its layout and its view are made again
    $docChanged(){
        this.$raw = null
        this.$docVersion = (this.$docVersion || 0) + 1
        this.$viewKey = null
    }

    // the default font of the document (QTextDocument::defaultFont is the TextEdit's font)
    $fontInfo(){
        let font = this.$v[TextEdit.FONT]
        return font ? font.$info() : DEFAULT_FONT
    }

    // QQuickTextEdit::text: the HTML of a rich text (QTextDocument::toHtml), the Markdown of a Markdown text
    // (toMarkdown), the plain text of a plain one
    $textOfDocument(){
        if(this.$markdown) return M.toMarkdown(this.$doc)
        if(this.$rich) return RT.toHtml(this.$doc, F.fontFormat(this.$fontInfo()))
        return this.$doc.plainText()
    }

    // the format of the cursor after the text was set (QQuickTextControlPrivate::setContent: cursor.setCharFormat of the
    // format before), until the cursor moves
    $cursorCharFormat(position){
        if(this.$insertionFormat && position === this.$insertionAt) return this.$insertionFormat
        return F.without(new TextCursor(this.$doc, position).charFormat(), 'objectType')
    }

    // QQuickTextEdit::setText: QTextDocument::setHtml, setMarkdown or setPlainText (no undo step, the history cleared);
    // the cursor at the start. Before componentComplete text is the string set
    $setText(value, force = false){
        let C = TextEdit
        value = value === undefined || value === null ? '' : String(value)
        if(!this.$doc) return super.$setText(value)
        let kind = this.$kindFor(value)
        let rich = kind !== 'plain', markdown = kind === 'markdown'
        if(!force && rich === this.$rich && markdown === !!this.$markdown && this.$v[C.TEXT] === value && (this.$f & engine.COMPLETED)) return
        let oldContent = this.$content()
        let oldText = this.$v[C.TEXT]
        let insertion = this.$cursorCharFormat(this.$v[C.CURSORPOSITION])
        if(rich !== this.$rich) this.$setRich(rich)
        this.$markdown = markdown
        this.$styled = false
        this.$setContent(kind, value)
        this.$docChanged()
        let text = (this.$f & engine.COMPLETED) ? this.$textOfDocument() : value
        this.$editing = true
        try {
            this.$write(C.TEXT, text)
        } finally {
            this.$editing = false
        }
        if(text === oldText) this.$textChanged()
        this.$updateUndoFlags()
        this.$applySelection(0, 0, true)
        this.$insertionFormat = insertion
        this.$insertionAt = 0
        if(this.$content() !== oldContent) engine.markDirty(this, TEXT)
    }

    // QQuickTextControlPrivate::setContent: the document of the text read as 'html', 'markdown' or 'plain'
    $setContent(kind, value){
        if(kind === 'html') RT.setDocumentHtml(this.$doc, value)
        else if(kind === 'markdown'){
            M.setMarkdown(this.$doc, value, this.$fontInfo())
            RT.updateContentDirection(this.$doc)
        } else RT.setPlainText(this.$doc, value)
    }

    // a rich text shown, or a plain one again (the view goes)
    $setRich(rich){
        this.$rich = rich
        this.$docChanged()
        if(rich) return
        this.$insertionFormat = null
        this.$setHoveredLink('')
        if(this.$view){
            this.$view.remove()
            this.$view = null
        }
        if(this.$caret){
            this.$caret.remove()
            this.$caret = null
        }
        engine.markDirty(this, TEXT | STYLE)
    }

    // QQuickTextEdit::setTextFormat: the text converted from the format it was read in (the source) to the one it is
    // read in now (the destination): a rich or Markdown text shown as plain shows its HTML or Markdown, a plain text made
    // rich (Markdown) is read as HTML (Markdown), HTML made Markdown is its Markdown (toMarkdown), Markdown made rich its
    // HTML (toHtml). AutoText keeps the format of the text (of the file textDocument loaded), and reads a text that looks
    // like HTML as rich text. Before componentComplete only the format the text is read in changes
    $setTextFormat(format){
        let C = TextEdit
        let old = this.$v[C.TEXTFORMAT]
        if(format === old) return
        let text = this.$v[C.TEXT]
        let wasRich = this.$rich && !this.$markdown, wasMarkdown = !!this.$markdown
        if(!(this.$f & engine.COMPLETED)){
            let rich = format === RICH_TEXT || (format === AUTO_TEXT && (wasRich || RT.mightBeRichText(text)))
            let markdown = format === MARKDOWN_TEXT
            super.$write(C.TEXTFORMAT, format)
            if(rich !== wasRich || markdown !== wasMarkdown) this.$setText(text, true)
            return
        }
        let detected = this.$quickDocument ? this.$quickDocument.$detectedFormat : null
        let source = old === PLAIN_TEXT ? PLAIN_TEXT : wasRich ? RICH_TEXT : wasMarkdown ? MARKDOWN_TEXT
            : (detected === null && RT.mightBeRichText(text)) ? RICH_TEXT : PLAIN_TEXT
        let destination = format
        if(format === AUTO_TEXT){
            if(detected === 'markdown' || (detected === null && source === MARKDOWN_TEXT)) destination = MARKDOWN_TEXT
            else if(detected === 'html' || (detected === null && (source === RICH_TEXT || RT.mightBeRichText(text)))) destination = RICH_TEXT
            else destination = PLAIN_TEXT
        }
        // a conversion between Markdown and HTML makes the text again
        let regenerate = format !== PLAIN_TEXT && source !== destination
        let content = null
        if(destination === PLAIN_TEXT && (source === RICH_TEXT || source === MARKDOWN_TEXT)) content = ['plain', text]
        else if(destination === RICH_TEXT && source === MARKDOWN_TEXT) content = ['html', RT.toHtml(this.$doc, F.fontFormat(this.$fontInfo()))]
        else if(destination === RICH_TEXT && source === PLAIN_TEXT) content = ['html', regenerate ? this.$doc.plainText() : text]
        else if(destination === MARKDOWN_TEXT && source === RICH_TEXT) content = ['markdown', M.toMarkdown(this.$doc)]
        else if(destination === MARKDOWN_TEXT && source === PLAIN_TEXT) content = ['markdown', regenerate ? this.$doc.plainText() : text]
        super.$write(C.TEXTFORMAT, format)
        let rich = destination === RICH_TEXT || destination === MARKDOWN_TEXT
        let markdown = destination === MARKDOWN_TEXT
        if(rich !== this.$rich) this.$setRich(rich)
        this.$markdown = markdown
        if(!content && !regenerate) return
        let oldText = text
        if(content){
            this.$styled = false
            this.$setContent(content[0], content[1])
        }
        this.$docChanged()
        let next = this.$textOfDocument()
        this.$editing = true
        try {
            this.$write(C.TEXT, next)
        } finally {
            this.$editing = false
        }
        if(next === oldText) this.$textChanged()
        this.$updateUndoFlags()
        if(content) this.$applySelection(0, 0, true)
        if(this.$quickDocument) this.$quickDocument.$updateModified()
        engine.markDirty(this, TEXT)
        if(this.$drawsRich()) this.$updateImplicitSize()
    }

    $componentComplete(){
        super.$componentComplete()
        // QQuickTextEdit::componentComplete: the text set before is the document now, text is the document's text
        let text = this.$textOfDocument()
        if(text !== this.$v[TextEdit.TEXT]){
            this.$editing = true
            try {
                this.$write(TextEdit.TEXT, text)
            } finally {
                this.$editing = false
            }
        }
        if(this.$drawsRich()) this.$updateImplicitSize()
    }

    // ---- edits and the undo history (the document's) ----

    // the steps of the history are the document's (QTextDocument undo stack)
    $recordUndo(){
        this.$redoStack = []
    }

    $updateUndoFlags(){
        let C = TextEdit
        this.$write(C.CANUNDO, this.$doc.isUndoAvailable())
        this.$write(C.CANREDO, this.$doc.isRedoAvailable())
    }

    $state(){
        return { content: this.$doc, anchor: this.$anchor(), cursor: this.$v[TextEdit.CURSORPOSITION] }
    }

    // an edit of the document: the text before it kept to see what changed
    $beginEdit(){
        this.$before = { content: this.$content(), text: this.$v[TextEdit.TEXT] }
    }

    // [start, end) replaced by inserted at the cursor (QTextCursor::insertText with the cursor's char format); a single
    // character removed by Backspace or Delete is QTextCursor::deletePreviousChar / deleteChar (a step the next joins)
    $spliced(start, end, inserted){
        let C = TextEdit
        this.$beginEdit()
        let doc = this.$doc
        let cursor
        let kind = this.$editKind
        if(!inserted && (kind === 'backspace' || kind === 'delete') && this.$v[C.SELECTIONSTART] === this.$v[C.SELECTIONEND]){
            cursor = RT.deleteChar(doc, kind === 'backspace' ? end : start, kind === 'backspace')
        } else {
            let format = inserted ? this.$cursorCharFormat(this.$v[C.CURSORPOSITION]) : undefined
            // the selection itself: from its anchor to the cursor (a selection over table cells is the cells between them)
            let anchor = start, position = end
            if(start !== end && start === this.$v[C.SELECTIONSTART] && end === this.$v[C.SELECTIONEND]){
                anchor = this.$anchor()
                position = this.$v[C.CURSORPOSITION]
            }
            cursor = RT.replaceText(doc, anchor, position, inserted, format)
        }
        return { content: doc, cursor }
    }

    // the document changed by an edit: text is its text now; textEdited unless edited is false
    $commitEdit(content, anchor, cursor, edited = true){
        if(!(content instanceof TextDocument)) return super.$commitEdit(content, anchor, cursor, edited)
        let C = TextEdit
        let before = this.$before || { content: this.$content(), text: this.$v[C.TEXT] }
        this.$before = null
        this.$docChanged()
        let text = this.$textOfDocument()
        let changed = text !== before.text || this.$content() !== before.content
        this.$editing = true
        try {
            if(changed){
                this.$write(C.TEXT, text)
                if(text === before.text) this.$textChanged()
            }
            this.$applySelection(anchor, cursor, true)
        } finally {
            this.$editing = false
        }
        this.$lastEditCursor = this.$v[C.CURSORPOSITION]
        this.$updateUndoFlags()
        if(changed) engine.markDirty(this, TEXT)
        if(this.$quickDocument) this.$quickDocument.$updateModified()
        if(this.$cursorSelection) this.$cursorSelection.$sync()
        if(changed && edited) engine.emitArgs(this, C.SIGNAL_textEdited, [])
        return changed
    }

    // Backspace and Delete do not remove the characters of a table's frame (QTextCursorPrivate::canDelete)
    $userEdit(start, end, inserted, kind){
        if(!inserted && (kind === 'backspace' || kind === 'delete') && end - start === 1 && this.$doc.charFormatAt(start).table) return false
        this.$editKind = kind
        try {
            return super.$userEdit(start, end, inserted, kind)
        } finally {
            this.$editKind = null
        }
    }

    $applySelection(anchor, cursor, toDom){
        super.$applySelection(anchor, cursor, toDom)
        if(this.$cursorSelection && !this.$cursorSelection.$syncing) this.$cursorSelection.$sync()
        // the format of the cursor goes when the cursor moves
        if(this.$insertionFormat && (cursor !== this.$insertionAt || anchor !== cursor)) this.$insertionFormat = null
        if(this.$rich){
            // QQuickTextEdit::selectedText of a rich text: QTextCursor::selectedText (the cells of a selection over a table)
            let C = TextEdit
            let text = RT.selectedText(this.$doc, this.$anchor(), this.$v[C.CURSORPOSITION])
            if(text !== this.$v[C.SELECTEDTEXT]){
                this.$selecting = true
                try {
                    this.$write(C.SELECTEDTEXT, text)
                } finally {
                    this.$selecting = false
                }
            }
            engine.markDirty(this, STYLE)
        }
    }

    // QQuickTextEdit::insert: plain text by QTextCursor::insertText, a rich text (RichText, or AutoText that looks like
    // HTML, which makes the TextEdit rich) by insertHtml; the cursor after the position moves on
    insert(position, text){
        let C = TextEdit
        text = String(text)
        if(!(this.$rich && !this.$markdown) && this.$v[C.TEXTFORMAT] === AUTO_TEXT && RT.mightBeRichText(text)){
            this.$beginEdit()
            if(!this.$rich) this.$setRich(true)
            this.$markdown = false
        }
        if(position < 0 || position >= this.$doc.characterCount()){
            this.$before = null
            return
        }
        if(!this.$before) this.$beginEdit()
        let before = this.$doc.length()
        if(this.$rich && !this.$markdown) RT.insertHtml(this.$doc, position, text)
        else if(this.$markdown) this.$insertMarkdown(position, text)
        else RT.insertText(this.$doc, position, text)
        let added = this.$doc.length() - before
        let shift = (p) => p >= position ? p + added : p
        this.$commitEdit(this.$doc, shift(this.$anchor()), shift(this.$v[C.CURSORPOSITION]))
    }

    // QTextCursor::insertMarkdown at position
    $insertMarkdown(position, text){
        let cursor = new TextCursor(this.$doc, position)
        M.insertMarkdown(cursor, text, this.$fontInfo())
        RT.updateContentDirection(this.$doc)
    }

    // QQuickTextEdit::remove: QTextCursor::removeSelectedText (over a table's edge the whole table)
    remove(start, end){
        let length = this.$doc.length()
        start = Math.max(0, Math.min(start, length))
        end = Math.max(0, Math.min(end, length))
        if(start > end) [start, end] = [end, start]
        if(start === end) return
        this.$beginEdit()
        let removed = RT.removeRange(this.$doc, start, end)
        if(removed){
            start = removed.start
            end = removed.end
        }
        let size = this.$doc.length()
        let shift = removed ? (p) => p <= start ? p : p - (Math.min(p, end) - start) : (p) => Math.min(p, size)
        this.$commitEdit(this.$doc, shift(this.$anchor()), shift(this.$v[TextEdit.CURSORPOSITION]))
    }

    // QQuickTextControl::clear: the whole document selected and removed (an undo step), no textEdited
    clear(){
        if(!this.$doc.length()) return
        this.$beginEdit()
        RT.removeRange(this.$doc, 0, this.$doc.length())
        this.$commitEdit(this.$doc, 0, 0, false)
    }

    // QQuickTextEdit::append: a new block at the end (the text itself when the document is empty), the text as HTML when
    // the format is RichText or the text looks like HTML in AutoText; the cursor at the end stays at the end
    append(text){
        let C = TextEdit
        text = String(text)
        let format = this.$v[C.TEXTFORMAT]
        let rich = format === RICH_TEXT || (format === AUTO_TEXT && RT.mightBeRichText(text))
        this.$beginEdit()
        let end = this.$doc.length()
        if(!rich && format === MARKDOWN_TEXT){
            // QTextCursor::insertMarkdown in a new block at the end, one undo step
            let doc = this.$doc
            doc.beginEditBlock(0)
            let cursor = new TextCursor(doc, doc.length())
            if(!doc.isEmpty()) cursor.insertBlock()
            M.insertMarkdown(cursor, text, this.$fontInfo())
            doc.endEditBlock()
            RT.updateContentDirection(doc)
        } else RT.append(this.$doc, text, rich)
        let atEnd = (p) => p === end ? this.$doc.length() : p
        this.$commitEdit(this.$doc, atEnd(this.$anchor()), atEnd(this.$v[C.CURSORPOSITION]), false)
    }

    // QTextDocument::undo / redo with the control's cursor: the cursor where the step changed the text
    $undoRedo(undo){
        this.$beginEdit()
        let position = undo ? this.$doc.undo() : this.$doc.redo()
        let cursor = position >= 0 ? position : Math.min(this.$v[TextEdit.CURSORPOSITION], this.$doc.length())
        let anchor = position >= 0 ? position : Math.min(this.$anchor(), this.$doc.length())
        this.$lastEdit = null
        this.$commitEdit(this.$doc, anchor, cursor, false)
    }

    undo(){
        if(!this.$doc.isUndoAvailable()) return
        this.$undoRedo(true)
    }

    redo(){
        if(!this.$doc.isRedoAvailable()) return
        this.$undoRedo(false)
    }

    // the text of the range: plain text (QTextDocumentFragment::toPlainText), HTML of a rich text (toHtml), Markdown of
    // a Markdown text (toMarkdown)
    getFormattedText(start, end){
        let max = this.$doc.length()
        start = Math.max(0, Math.min(start, max))
        end = Math.max(0, Math.min(end, max))
        if(!this.$rich) return plainOf(this.$content().slice(Math.min(start, end), Math.max(start, end)))
        if(this.$markdown) return M.toMarkdown(fragmentOf(this.$doc, start, end))
        return RT.formattedText(this.$doc, start, end)
    }

    // QQuickTextEdit::linkAt: the link of the character at the point (QQuickTextControl::anchorAt at x + topPadding,
    // y + leftPadding, as Qt passes them)
    linkAt(x, y){
        if(!this.$drawsRich()) return ''
        let position = this.$hitTest(Number(x) + this.$padding('TOPPADDING'), Number(y) + this.$padding('LEFTPADDING'))
        return position < 0 ? '' : RT.anchorAt(this.$doc, position)
    }

    // QQuickTextControl::copy of a rich text: the plain text of the selection
    copy(){
        if(!this.$rich) return super.copy()
        let text = this.$v[TextEdit.SELECTEDTEXT]
        if(text) this.$clipboardWrite(plainOf(text))
    }

    cut(){
        if(!this.$rich) return super.cut()
        let start = this.$v[TextEdit.SELECTIONSTART], end = this.$v[TextEdit.SELECTIONEND]
        if(this.$v[TextEdit.READONLY] || start === end) return
        this.$clipboardWrite(plainOf(this.$content().slice(start, end)))
        this.$userEdit(start, end, '', null)
    }

    // ---- layout of a rich text ----

    // the layout of the document (QTextDocumentLayout): at the width of the text (width minus padding) when the width
    // is set, else unwrapped; natural: always unwrapped
    $richLayout(natural = false){
        let C = TextEdit
        let font = this.$fontInfo()
        let left = this.$padding('LEFTPADDING'), right = this.$padding('RIGHTPADDING')
        let explicitWidth = !!(this.$explicit & 1)
        let textWidth = !natural && explicitWidth ? Math.max(0, this.$v[Item.WIDTH] - left - right) : Infinity
        let wrap = this.$v[C.WRAPMODE] || 0
        let alignment = this.$v[C.EFFECTIVEHORIZONTALALIGNMENT] || 1
        let margin = this.$v[C.TEXTMARGIN] || 0
        let key = [natural, font.family, font.pixelSize, font.pointSize, font.sizeUnit, font.weight, font.italic, font.letterSpacing, font.wordSpacing,
            font.capitalization, textWidth, wrap, alignment, margin, this.$v[C.TABSTOPDISTANCE], this.$imageVersion || 0].join('\u0000')
        let cache = natural ? this.$naturalCache : this.$richCache
        if(cache && cache.doc === this.$doc && cache.key === key && cache.version === this.$docVersion) return cache
        let options = {
            font, width: textWidth, wrapMode: textWidth === Infinity ? 0 : wrap, alignment, margin,
            tabStop: this.$v[C.TABSTOPDISTANCE], measure,
            alignWidth: explicitWidth ? Math.max(0, this.$v[Item.WIDTH] - left - right) : 0,
            imageSize: (name) => this.$imageSizes ? this.$imageSizes.get(name) : null,
            imageVersion: this.$imageVersion || 0,
            // the layouts of the blocks that did not change are taken from the layout before
            blockCache: this.$blockCache || (this.$blockCache = new Map()),
        }
        let layouter = new L.Layouter(this.$doc, options)
        let layout = layouter.layout()
        // without a width set and without wrapping the text is aligned in its own width (document idealWidth)
        if(!natural && !explicitWidth && alignment !== 1){
            options.alignWidth = layout.width + 2 * margin
            layouter = new L.Layouter(this.$doc, options)
            layout = layouter.layout()
        }
        let result = { doc: this.$doc, key, layout, layouter, version: this.$docVersion }
        if(natural) this.$naturalCache = result
        else this.$richCache = result
        return result
    }

    // the offset of the document in the item (QQuickTextEditPrivate xoff, yoff)
    $richOffset(layout){
        let C = TextEdit
        let top = this.$padding('TOPPADDING'), bottom = this.$padding('BOTTOMPADDING')
        let height = this.$richHeight(layout)
        let y = top + alignedY(height, this.$v[Item.HEIGHT] - top - bottom, this.$v[C.VERTICALALIGNMENT])
        return { x: this.$padding('LEFTPADDING'), y }
    }

    // the height of the document: the font's height when it is empty (QQuickTextEdit::updateSize)
    $richHeight(layout){
        if(this.$doc.isEmpty()) return Math.ceil(measure('', this.$fontInfo()).height)
        return layout.height
    }

    // QQuickTextEdit::updateSize
    $updateImplicitSize(){
        if(!this.$drawsRich()) return super.$updateImplicitSize()
        let C = TextEdit
        let { layout } = this.$richLayout()
        let natural = (this.$explicit & 1) ? this.$richLayout(true).layout : layout
        let margin = layout.margin
        let idealWidth = layout.width + 2 * margin
        let height = this.$richHeight(layout)
        let left = this.$padding('LEFTPADDING'), right = this.$padding('RIGHTPADDING')
        let top = this.$padding('TOPPADDING'), bottom = this.$padding('BOTTOMPADDING')
        engine.markDirty(this, TEXT)
        this.$write(C.CONTENTWIDTH, idealWidth)
        this.$write(C.CONTENTHEIGHT, height)
        this.$write(C.PAINTEDWIDTH, idealWidth)
        this.$write(C.PAINTEDHEIGHT, height)
        this.$write(C.LINECOUNT, layout.lineCount)
        this.$write(Item.IMPLICITWIDTH, natural.width + 2 * margin + left + right)
        this.$write(Item.IMPLICITHEIGHT, height + top + bottom)
        let first = layout.blocks[0] && layout.blocks[0].lines[0]
        let baseline = first ? first.ascent : measure('', this.$fontInfo()).ascent
        this.$write(Item.BASELINEOFFSET, baseline + this.$richOffset(layout).y + margin)
        this.$updateCursorRectangle()
    }

    // QQuickTextControl::cursorRect at pos, in the item
    $caretRect(pos){
        if(!this.$drawsRich()) return super.$caretRect(pos)
        let { layout, layouter } = this.$richLayout()
        let offset = this.$richOffset(layout)
        let { line } = L.lineOf(layout, pos)
        let x = L.cursorToX(layouter, line, pos)
        let width = 1
        if(this.$v[TextEdit.OVERWRITEMODE]){
            width = pos < line.end ? L.cursorToX(layouter, line, pos + 1) - x : layouter.width(' ', layouter.font(this.$doc.blockCharFormat(this.$doc.blockIndexAt(pos))))
        }
        return new QRectF(offset.x + x, offset.y + line.top, width, line.height)
    }

    // QQuickTextEdit::positionAt: the nearest position (QTextDocumentLayout::hitTest, FuzzyHit)
    $positionAtPoint(x, y, onCharacter){
        if(!this.$drawsRich()) return super.$positionAtPoint(x, y, onCharacter)
        let { layout, layouter } = this.$richLayout()
        let offset = this.$richOffset(layout)
        x -= offset.x
        y -= offset.y
        let lines = []
        for(let block of layout.blocks) for(let line of block.lines) lines.push(line)
        if(!lines.length) return 0
        // the lines at the height of the point (the cells of a table side by side), the one nearest across; else the
        // nearest line above the point
        let distance = (l) => x < l.x ? l.x - x : x > l.x + l.naturalWidth ? x - l.x - l.naturalWidth : 0
        let line = null
        for(let l of lines){
            if(y < l.top || y >= l.top + l.lineHeight) continue
            if(!line || distance(l) < distance(line)) line = l
        }
        if(!line){
            line = lines[0]
            for(let l of lines) if(l.top <= y && (l.top > line.top || (l.top === line.top && distance(l) < distance(line)))) line = l
        }
        return L.xToCursor(layouter, line, x)
    }

    // the position of the character at the point in the document's coordinates, -1 outside the text (ExactHit)
    $hitTest(x, y){
        let { layout, layouter } = this.$richLayout()
        for(let block of layout.blocks){
            for(let line of block.lines){
                if(y < line.top || y >= line.top + line.lineHeight) continue
                for(let item of line.items){
                    if(x < item.x || x >= item.x + item.width || item.kind === 'break') continue
                    if(item.kind !== 'word' && item.kind !== 'space') return item.start
                    for(let n = 1; n <= item.text.length; n++){
                        if(x < L.cursorToX(layouter, line, item.start + n)) return item.start + n - 1
                    }
                    return item.end - 1
                }
            }
        }
        return -1
    }

    $updateCursorRectangle(){
        super.$updateCursorRectangle()
        if(this.$drawsRich()) engine.markDirty(this, STYLE)
    }

    // ---- the mouse over a rich text ----

    $selectsByMouse(){
        return !!this.$drawsRich()
    }

    $positionAtClient(clientX, clientY){
        if(!this.$drawsRich()) return super.$positionAtClient(clientX, clientY)
        let rect = this.$input.getBoundingClientRect()
        return this.$positionAtPoint(clientX - rect.left, clientY - rect.top, false)
    }

    // a double click selects the word, a triple click the block (QQuickTextControlPrivate::mousePressEvent)
    $multiClick(position, clicks){
        if(!this.$drawsRich()) return
        let content = this.$content()
        if(clicks === 2){
            let isWord = (c) => c !== undefined && /[\p{L}\p{N}\p{M}_]/u.test(c)
            let start = position, end = position
            if(!isWord(content[position]) && isWord(content[position - 1])) start = end = position - 1
            if(isWord(content[start])){
                while(start > 0 && isWord(content[start - 1])) start--
                while(end < content.length && isWord(content[end])) end++
            }
            this.$applySelection(start, end, true)
            return
        }
        let b = this.$doc.blockIndexAt(position)
        let start = this.$doc.blockStart(b)
        this.$applySelection(start, start + this.$doc.blockLength(b) - 1, true)
    }

    // a press on a link remembers it: its release on the same link activates it (QQuickTextControlPrivate::
    // mouseReleaseEvent, LinksAccessibleByMouse)
    $pressed(event){
        this.$pressedLink = this.$drawsRich() ? this.$linkAtClient(event.clientX, event.clientY) : ''
    }

    $mouseUp(event){
        let link = this.$pressedLink
        this.$pressedLink = ''
        if(!link || !this.$drawsRich() || (this.$f & engine.DESTROYED)) return
        if(this.$v[TextEdit.SELECTIONSTART] !== this.$v[TextEdit.SELECTIONEND]) return
        if(this.$linkAtClient(event.clientX, event.clientY) === link) engine.emitArgs(this, TextEdit.SIGNAL_linkActivated, [link])
    }

    $linkAtClient(clientX, clientY){
        if(!this.$input || !this.$input.getBoundingClientRect) return ''
        let rect = this.$input.getBoundingClientRect()
        let { layout } = this.$richLayout()
        let offset = this.$richOffset(layout)
        let position = this.$hitTest(clientX - rect.left - offset.x, clientY - rect.top - offset.y)
        return position < 0 ? '' : RT.anchorAt(this.$doc, position)
    }

    // QQuickTextEdit::q_linkHovered: hoveredLink and linkHovered
    $setHoveredLink(link){
        if(this.$v[TextEdit.HOVEREDLINK] === link) return
        this.$write(TextEdit.HOVEREDLINK, link)
        engine.emitArgs(this, TextEdit.SIGNAL_linkHovered, [link])
        if(this.$input && this.$input.style) this.$input.style.cursor = link ? 'pointer' : ''
    }

    // ---- the keys ----

    // the keys QQuickTextControlPrivate::keyPressEvent edits with: Return a new block, Shift+Return a line separator,
    // Backspace at a block's start out of a list or an indent, Backspace and Delete as deletePreviousChar / deleteChar;
    // the cursor moves by the characters, in a rich text by the layout too. selectByKeyboard false: no selection by keys
    $keyEvent(event, kind){
        if(kind !== 'pressed' || !this.$doc) return super.$keyEvent(event, kind)
        let C = TextEdit
        let K = QtEnums
        let mods = event.modifiers & ~0x20000000
        let shift = !!(mods & K.ShiftModifier)
        let ctrl = !!(mods & (K.ControlModifier | K.MetaModifier))
        let alt = !!(mods & K.AltModifier)
        let readOnly = this.$v[C.READONLY]
        let start = this.$v[C.SELECTIONSTART], end = this.$v[C.SELECTIONEND]
        let cursor = this.$v[C.CURSORPOSITION]
        let key = event.key
        let selectByKeyboard = this.$selectByKeyboard()
        if((key === K.Key_Return || key === K.Key_Enter) && !ctrl && !alt){
            if(readOnly) return 0
            if(shift) this.$typed(start, end, '\u2028', null)
            else {
                this.$beginEdit()
                let position = RT.insertBlockAt(this.$doc, this.$anchor(), cursor)
                this.$commitEdit(this.$doc, position, position)
            }
            return ACCEPTED
        }
        if(key === K.Key_Backspace && !(mods & ~K.ShiftModifier)){
            if(readOnly) return ACCEPTED
            if(start === end){
                if(cursor === this.$doc.blockStart(this.$doc.blockIndexAt(cursor))){
                    this.$beginEdit()
                    if(RT.backspaceAtBlockStart(this.$doc, cursor)){
                        this.$commitEdit(this.$doc, cursor, cursor)
                        return ACCEPTED
                    }
                    this.$before = null
                }
                if(cursor === 0) return ACCEPTED
                let content = this.$content()
                let from = cursor - 1
                if(from > 0 && content[from] >= '\udc00' && content[from] <= '\udfff' && content[from - 1] >= '\ud800' && content[from - 1] <= '\udbff') from--
                this.$userEdit(from, cursor, '', 'backspace')
            } else this.$userEdit(start, end, '', null)
            return ACCEPTED
        }
        if(key === K.Key_Delete && !mods){
            if(readOnly) return ACCEPTED
            if(start === end){
                let content = this.$content()
                if(cursor >= content.length) return ACCEPTED
                let to = cursor + 1
                if(to < content.length && content[cursor] >= '\ud800' && content[cursor] <= '\udbff' && content[to] >= '\udc00' && content[to] <= '\udfff') to++
                this.$userEdit(cursor, to, '', 'delete')
            } else this.$userEdit(start, end, '', null)
            return ACCEPTED
        }
        if(shift && !selectByKeyboard && (key === K.Key_Left || key === K.Key_Right || key === K.Key_Up || key === K.Key_Down
            || key === K.Key_Home || key === K.Key_End || key === K.Key_PageUp || key === K.Key_PageDown)) return ACCEPTED
        if(ctrl && !alt && key === 0x41){
            if(!selectByKeyboard) return 0
            this.selectAll()
            return ACCEPTED
        }
        let moving = key === K.Key_Up || key === K.Key_Down || key === K.Key_Home || key === K.Key_End || key === K.Key_PageUp || key === K.Key_PageDown
        if(moving && !alt && this.$drawsRich()){
            let to = this.$moveByLayout(key, ctrl, cursor)
            if(to === null) return super.$keyEvent(event, kind)
            if(shift) this.moveCursorSelection(to, 0)
            else this.$applySelection(to, to, true)
            return ACCEPTED
        }
        // Left and Right by the characters of the document (a selection collapses to its side)
        if((key === K.Key_Left || key === K.Key_Right) && !alt){
            let content = this.$content()
            let to
            if(ctrl){
                let isWord = (c) => c !== undefined && /[\p{L}\p{N}\p{M}_]/u.test(c)
                to = cursor
                if(key === K.Key_Left){
                    while(to > 0 && !isWord(content[to - 1])) to--
                    while(to > 0 && isWord(content[to - 1])) to--
                } else {
                    while(to < content.length && isWord(content[to])) to++
                    while(to < content.length && !isWord(content[to])) to++
                }
            } else if(!shift && start !== end) to = key === K.Key_Left ? start : end
            else {
                to = cursor + (key === K.Key_Left ? -1 : 1)
                let isLow = (c) => c >= '\udc00' && c <= '\udfff', isHigh = (c) => c >= '\ud800' && c <= '\udbff'
                if(key === K.Key_Left && to > 0 && isLow(content[to]) && isHigh(content[to - 1])) to--
                if(key === K.Key_Right && to < content.length && isLow(content[to]) && isHigh(content[to - 1])) to++
            }
            to = Math.max(0, Math.min(content.length, to))
            if(shift) this.moveCursorSelection(to, 0)
            else this.$applySelection(to, to, true)
            return ACCEPTED
        }
        // without a native control (no document: node) the text typed
        if(!this.$input){
            let text = event.text || ''
            if(text && !ctrl && !alt && text >= ' ' && text !== '\x7f'){
                if(readOnly) return 0
                this.$typed(start, end, text, 'insert')
                return ACCEPTED
            }
        }
        return super.$keyEvent(event, kind)
    }

    // selectByKeyboard: by default true unless read-only (QQuickTextEdit::selectByKeyboard)
    $selectByKeyboard(){
        let C = TextEdit
        let value = this.$v[C.SELECTBYKEYBOARD]
        return value === null || value === undefined ? !this.$v[C.READONLY] : !!value
    }

    // QTextCursor::movePosition with the layout: Up / Down to the line above / below at the same x, Home / End to the
    // line's ends (Ctrl: the document's), Page Up / Down by the height of the item
    $moveByLayout(key, ctrl, cursor){
        let K = QtEnums
        let { layout, layouter } = this.$richLayout()
        let lines = []
        for(let block of layout.blocks) for(let line of block.lines) lines.push(line)
        let { line } = L.lineOf(layout, cursor)
        let index = lines.indexOf(line)
        if(index < 0) return null
        if(key === K.Key_Home) return ctrl ? 0 : line.start
        if(key === K.Key_End){
            if(ctrl) return this.$doc.length()
            let last = line.items[line.items.length - 1]
            return last && last.kind === 'break' ? last.start : line.end
        }
        if(this.$goalX === undefined || this.$goalCursor !== cursor) this.$goalX = L.cursorToX(layouter, line, cursor)
        let target = index
        if(key === K.Key_Up) target = index - 1
        else if(key === K.Key_Down) target = index + 1
        else {
            let page = Math.max(1, this.$v[Item.HEIGHT] - this.$padding('TOPPADDING') - this.$padding('BOTTOMPADDING'))
            let y = line.top + (key === K.Key_PageUp ? -page : page)
            target = lines.findIndex(l => y < l.top + l.lineHeight)
            if(target < 0) target = lines.length - 1
        }
        let position
        if(target < 0) position = 0
        else if(target >= lines.length) position = this.$doc.length()
        else position = L.xToCursor(layouter, lines[target], this.$goalX)
        this.$goalCursor = position
        return position
    }

    // the browser is about to edit the control: the edit is the document's (the typed text in the cursor's format, a
    // paste of HTML as rich text unless the format is PlainText); an input method's composition is the control's
    $beforeInput(event){
        let type = event.inputType || ''
        if(type === 'historyUndo' || type === 'historyRedo' || event.isComposing || type === 'insertCompositionText') return super.$beforeInput(event)
        event.preventDefault()
        if(this.$v[TextEdit.READONLY]) return
        let start = this.$v[TextEdit.SELECTIONSTART], end = this.$v[TextEdit.SELECTIONEND]
        if((type === 'insertFromPaste' || type === 'insertFromDrop') && event.dataTransfer){
            let html = event.dataTransfer.getData('text/html')
            if(html && this.$v[TextEdit.TEXTFORMAT] !== PLAIN_TEXT) return this.$insertHtmlAtCursor(html)
        }
        if(type === 'insertParagraph'){
            this.$beginEdit()
            let position = RT.insertBlockAt(this.$doc, this.$anchor(), this.$v[TextEdit.CURSORPOSITION])
            this.$commitEdit(this.$doc, position, position)
            return
        }
        if(type === 'insertLineBreak') return this.$typed(start, end, '\u2028', null)
        if(type.startsWith('insert')) return this.$typed(start, end, this.$insertedData(event, type), type === 'insertText' ? 'insert' : null)
        if(type.startsWith('delete')) return this.$deleted(start, end, type)
    }

    // QQuickTextControl::insertFromMimeData with acceptRichText: the HTML over the selection
    $insertHtmlAtCursor(html){
        this.$beginEdit()
        let cursor = new TextCursor(this.$doc, this.$anchor())
        cursor.setPosition(this.$v[TextEdit.CURSORPOSITION], true)
        insertFragment(cursor, fragmentFromHtml(html))
        RT.updateContentDirection(this.$doc)
        this.$commitEdit(this.$doc, cursor.position, cursor.position)
    }

    // an input method composed into the control: the change of its text is the document's edit
    $nativeInput(){
        let input = this.$input
        this.$pending = null
        let old = this.$content()
        let value = input.value
        if(value === old) return
        let head = 0
        while(head < old.length && head < value.length && old[head] === value[head]) head++
        let tail = 0
        while(tail < old.length - head && tail < value.length - head && old[old.length - 1 - tail] === value[value.length - 1 - tail]) tail++
        this.$beginEdit()
        let inserted = value.slice(head, value.length - tail)
        let format = this.$cursorCharFormat(head)
        let cursor = RT.replaceText(this.$doc, head, old.length - tail, inserted, format)
        this.$commitEdit(this.$doc, cursor, cursor)
    }

    // QQuickTextEdit::isRightToLeft: the first strong character of the range
    isRightToLeft(start, end){
        return super.isRightToLeft(start, end)
    }

    // QQuickTextEdit::inputMethodQuery: what an input method asks of the TextEdit (Qt::InputMethodQuery)
    inputMethodQuery(query, argument){
        let C = TextEdit
        let Q = QtEnums
        switch(query){
            case Q.ImEnabled: return true
            case Q.ImCursorRectangle: return this.$v[C.CURSORRECTANGLE]
            case Q.ImCursorPosition: return this.$v[C.CURSORPOSITION] - this.$doc.blockStart(this.$doc.blockIndexAt(this.$v[C.CURSORPOSITION]))
            case Q.ImAnchorPosition: return this.$anchor() - this.$doc.blockStart(this.$doc.blockIndexAt(this.$anchor()))
            case Q.ImSurroundingText: return this.$doc.blockText(this.$doc.blockIndexAt(this.$v[C.CURSORPOSITION]))
            case Q.ImCurrentSelection: return this.$v[C.SELECTEDTEXT]
            case Q.ImMaximumTextLength: return null
            case Q.ImHints: return this.$v[C.INPUTMETHODHINTS] || 0
            case Q.ImReadOnly: return !!this.$v[C.READONLY]
            case Q.ImFont: return this.$v[C.FONT]
            case Q.ImAbsolutePosition: return this.$v[C.CURSORPOSITION]
            case Q.ImTextBeforeCursor: return this.$content().slice(Math.max(0, this.$v[C.CURSORPOSITION] - (argument > 0 ? argument : 1024)), this.$v[C.CURSORPOSITION])
            case Q.ImTextAfterCursor: return this.$content().slice(this.$v[C.CURSORPOSITION], this.$v[C.CURSORPOSITION] + (argument > 0 ? argument : 1024))
        }
        return null
    }

    // ---- painting a rich text ----

    $paint(dirty){
        super.$paint(dirty)
        let input = this.$input
        if(!input || !this.$drawsRich()) return
        let C = TextEdit
        let s = input.style
        // the control only takes the keys and the input methods; the document is drawn by the view
        s.color = 'transparent'
        s.caretColor = 'transparent'
        s.setProperty('--jq2-selection', 'transparent')
        s.setProperty('--jq2-selected-text', 'transparent')
        s.whiteSpace = 'pre'
        if(!(dirty & (TEXT | STYLE | bits.GEOMETRY)) && this.$view) return
        if(!this.$view && typeof document !== 'undefined'){
            let view = document.createElement('div')
            view.className = 'jq2-rich'
            view.style.cssText = 'position:absolute;left:0;top:0;width:0;height:0;overflow:visible;pointer-events:none;'
            this.$dom.insertBefore(view, input)
            this.$view = view
            this.$listenLinks()
        }
        if(!this.$view) return
        let { layout, layouter } = this.$richLayout()
        let offset = this.$richOffset(layout)
        let start = this.$v[C.SELECTIONSTART], end = this.$v[C.SELECTIONEND]
        let showSelection = start !== end && (this.$v[Item.ACTIVEFOCUS] || this.$v[C.PERSISTENTSELECTION])
        let color = this.$v[C.COLOR], selectionColor = this.$v[C.SELECTIONCOLOR], selectedTextColor = this.$v[C.SELECTEDTEXTCOLOR]
        let key = [showSelection ? start : -1, showSelection ? end : -1, color, selectionColor, selectedTextColor].join(',')
        this.$view.style.left = offset.x + 'px'
        this.$view.style.top = offset.y + 'px'
        if(key !== this.$viewKey || this.$viewLayout !== layout){
            this.$viewKey = key
            this.$viewLayout = layout
            require('./TextDocument/View').render(this.$view, {
                doc: this.$doc, layout, layouter, color, selectionColor, selectedTextColor,
                selectionStart: showSelection ? start : 0, selectionEnd: showSelection ? end : 0,
                width: Math.max(0, this.$v[Item.WIDTH] - this.$padding('LEFTPADDING') - this.$padding('RIGHTPADDING')),
                cursorToX: (line, pos) => L.cursorToX(layouter, line, pos),
                resolveUrl: (name) => this.$resolveImage(name),
                onImageLoad: (name, img) => this.$imageLoaded(name, img),
            })
        }
        this.$paintCaret()
    }

    // the cursor of the TextEdit: blinking at the cursor rectangle while it shows (no delegate, not read-only)
    $paintCaret(){
        let C = TextEdit
        let show = this.$v[C.CURSORVISIBLE] && !this.$v[C.READONLY] && !this.$cursorItem
        if(!show){
            if(this.$caret) this.$caret.style.display = 'none'
            return
        }
        if(!this.$caret){
            if(typeof document === 'undefined') return
            let caret = document.createElement('div')
            caret.className = 'jq2-rich-caret'
            caret.style.cssText = 'position:absolute;pointer-events:none;'
            this.$dom.insertBefore(caret, this.$input)
            this.$caret = caret
            if(!document.getElementById('jq2-rich-style')){
                let style = document.createElement('style')
                style.id = 'jq2-rich-style'
                style.textContent = '@keyframes jq2-rich-blink { 0%, 49% { opacity: 1 } 50%, 100% { opacity: 0 } } .jq2-rich-caret { animation: jq2-rich-blink 1s step-end infinite; }'
                document.head.appendChild(style)
            }
        }
        let rect = this.$v[C.CURSORRECTANGLE] || this.$caretRect(this.$v[C.CURSORPOSITION])
        let s = this.$caret.style
        s.display = ''
        s.left = rect.x + 'px'
        s.top = rect.y + 'px'
        s.width = Math.max(1, rect.width) + 'px'
        s.height = rect.height + 'px'
        s.background = toCss(this.$v[C.COLOR])
    }

    // the mouse over the control: the link under it (hoveredLink, linkHovered); the clipboard of a rich text
    $listenLinks(){
        let input = this.$input
        if(!input || this.$linksListened) return
        this.$linksListened = true
        input.addEventListener('mousemove', (event) => {
            if(!this.$drawsRich() || (this.$f & engine.DESTROYED)) return
            engine.batch(() => this.$setHoveredLink(this.$linkAtClient(event.clientX, event.clientY)))
        })
        input.addEventListener('mouseleave', () => {
            if(!this.$drawsRich() || (this.$f & engine.DESTROYED)) return
            engine.batch(() => this.$setHoveredLink(''))
        })
        // plain text and the HTML of the selection (QQuickTextControl::createMimeDataFromSelection)
        let clip = (event, cut) => {
            if(!this.$rich || !event.clipboardData) return
            let start = this.$v[TextEdit.SELECTIONSTART], end = this.$v[TextEdit.SELECTIONEND]
            if(start === end) return
            event.preventDefault()
            event.clipboardData.setData('text/plain', plainOf(this.$v[TextEdit.SELECTEDTEXT]))
            event.clipboardData.setData('text/html', RT.formattedText(this.$doc, start, end))
            if(cut && !this.$v[TextEdit.READONLY]) engine.batch(() => this.$userEdit(start, end, '', null))
        }
        input.addEventListener('copy', (event) => clip(event, false))
        input.addEventListener('cut', (event) => clip(event, true))
        // a paste of HTML is rich text (QQuickTextControl::insertFromMimeData with acceptRichText)
        input.addEventListener('paste', (event) => {
            if(!this.$rich || !event.clipboardData || this.$v[TextEdit.READONLY]) return
            let html = event.clipboardData.getData('text/html')
            if(!html) return
            event.preventDefault()
            engine.batch(() => this.$insertHtmlAtCursor(html))
        })
    }

    // the url of an image of the document: relative to baseUrl
    $resolveImage(name){
        let base = this.$v[TextEdit.BASEURL]
        if(!name || /^[a-z][a-z0-9+.-]*:/i.test(name) || !base) return name
        try {
            return new URL(name, String(base)).href
        } catch(e){
            return name
        }
    }

    // an image loaded: its natural size lays the document out again
    $imageLoaded(name, img){
        if(this.$f & engine.DESTROYED) return
        if(!this.$imageSizes) this.$imageSizes = new Map()
        let size = { width: img.naturalWidth, height: img.naturalHeight }
        let old = this.$imageSizes.get(name)
        if(old && old.width === size.width && old.height === size.height) return
        this.$imageSizes.set(name, size)
        this.$imageVersion = (this.$imageVersion || 0) + 1
        engine.batch(() => this.$updateImplicitSize())
    }
}

defineType(TextEdit, {
    props: Object.assign({}, common, {
        wrapMode: { kind: 'int', effect: relayout },
        textFormat: 'int',
        lineCount: { kind: 'int', initial: 1, readonly: true },
        paintedWidth: { kind: 'real', readonly: true },
        paintedHeight: { kind: 'real', readonly: true },
        hoveredLink: { kind: 'string', readonly: true },
        baseUrl: 'var',
        tabStopDistance: { kind: 'real', initial: 80, effect: relayout },
        textMargin: { kind: 'real', effect: relayout },
        // null: as Qt, true unless read-only
        selectByKeyboard: { kind: 'var', initial: null, get(){ engine.track(this, TextEdit.SIGNAL_selectByKeyboardChanged); return this.$selectByKeyboard() } },
        cursorSelection: { kind: 'var', readonly: true, get(){ return this.$selection() } },
        textDocument: { kind: 'var', readonly: true, get(){ return this.$textDocument() } },
        // Text.QtRendering / NativeRendering / CurveRendering: the browser draws the text either way
        renderType: { kind: 'int', initial: 0 },
    }),
    signals: ['editingFinished', 'textEdited', 'linkActivated(link)', 'linkHovered(link)'],
})

Object.assign(TextEdit, { AlignLeft: 0x1, AlignRight: 0x2, AlignHCenter: 0x4, AlignJustify: 0x8, AlignTop: 0x20, AlignBottom: 0x40, AlignVCenter: 0x80 }, { NoWrap: 0, WordWrap: 1, WrapAnywhere: 3, Wrap: 4 }, { PlainText: 0, RichText: 1, AutoText: 2, MarkdownText: 3 })
Object.assign(TextEdit, { SelectCharacters: 0, SelectWords: 1 })
Object.assign(TextEdit, { QtRendering: 0, NativeRendering: 1, CurveRendering: 2 })

// read-only in Qt, writable in the base class
makeReadonly(TextEdit, 'implicitWidth')
makeReadonly(TextEdit, 'implicitHeight')

module.exports = { TextEdit }
