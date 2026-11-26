const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// TextDocument (QQuickTextDocument of Qt 6.12): TextEdit.textDocument. source loads a file into the TextEdit (an .html /
// .htm file as HTML, an .md / .markdown file as Markdown, any other as plain text, unless the TextEdit's textFormat is
// PlainText), modified tells whether the user changed it since; save() and saveAs() write files, which a page cannot:
// a url that is no local file is NonLocalFileError, a local one WriteError
class TextDocument extends QtObject {
    constructor(parent = null){
        super(parent)
        // the TextEdit of the document
        this.$edit = parent
        this.$detectedFormat = null
    }

    $write(id, value){
        let C = TextDocument
        if(id === C.SOURCE) return this.$setSource(value)
        if(id === C.MODIFIED) return this.$setModified(!!value)
        return super.$write(id, value)
    }

    // QTextDocument::isModified: changed since it was loaded, saved or set
    $updateModified(){
        let modified = !!this.$edit.$doc.modified
        if(this.$v[TextDocument.MODIFIED] !== modified) super.$write(TextDocument.MODIFIED, modified)
    }

    // QTextDocument::setModified
    $setModified(modified){
        let doc = this.$edit.$doc
        doc.modified = modified
        doc.modifiedState = modified ? -1 : doc.undoState
        this.$updateModified()
    }

    $setStatus(status, error){
        let C = TextDocument
        super.$write(C.ERRORSTRING, error || '')
        super.$write(C.STATUS, status)
    }

    // QQuickTextDocument::setSource: a modified document keeps its text
    $setSource(url){
        let C = TextDocument
        url = url === undefined || url === null ? '' : String(url)
        if(url === this.$v[C.SOURCE]) return
        if(this.$edit.$doc.modified){
            console.warn('JQML: TextDocument: Existing document modified: you should save(), or set modified=false before setting a different source')
            return
        }
        super.$write(C.SOURCE, url)
        this.$load()
    }

    // the url of the source against the base url of the TextEdit or of the page
    $resolved(url){
        let base = this.$edit.$v[this.$edit.constructor.BASEURL]
        try {
            return new URL(url, base ? String(base) : (typeof location !== 'undefined' ? location.href : 'file:///')).href
        } catch(e){
            return url
        }
    }

    // QQuickTextDocumentPrivate::load: the file read (a request of the page), its format by its name
    $load(){
        let C = TextDocument
        let url = this.$v[C.SOURCE]
        if(!url) return
        let resolved = this.$resolved(url)
        let path = resolved.replace(/[?#].*$/, '')
        let format = /\.html?$/i.test(path) ? 'html' : /\.(md|markdown)$/i.test(path) ? 'markdown' : 'plain'
        if(typeof XMLHttpRequest === 'undefined'){
            this.$setStatus(C.ReadError, decodeURIComponent(path.replace(/^file:\/\//, '')) + ' does not exist')
            return
        }
        this.$setStatus(C.Loading, '')
        let request = new XMLHttpRequest()
        request.open('GET', resolved)
        request.onreadystatechange = () => {
            if(request.readyState !== 4 || (this.$f & engine.DESTROYED)) return
            let ok = request.status === 200 || (request.status === 0 && request.responseText)
            engine.batch(() => {
                if(!ok){
                    this.$setStatus(C.ReadError, decodeURIComponent(path.replace(/^file:\/\//, '')) + ' does not exist')
                    return
                }
                this.$detectedFormat = format
                this.$edit.$loadDocument(request.responseText, format, resolved.replace(/[^/]*$/, ''))
                this.$setStatus(C.Loaded, '')
                this.$setModified(false)
            })
        }
        request.send()
    }

    // QQuickTextDocument::save
    save(){
        this.$writeTo(this.$v[TextDocument.SOURCE])
    }

    // QQuickTextDocument::saveAs: only a local file
    saveAs(url){
        let C = TextDocument
        url = String(url)
        if(!/^file:/i.test(this.$resolved(url))){
            this.$setStatus(C.NonLocalFileError, 'Can only save to local files')
            return
        }
        this.$writeTo(url)
        if(url !== this.$v[C.SOURCE]) super.$write(C.SOURCE, url)
    }

    // QQuickTextDocumentPrivate::writeTo: a page has no file to write
    $writeTo(url){
        let path = decodeURIComponent(this.$resolved(url).replace(/^file:\/\//, '').replace(/[?#].*$/, ''))
        this.$setStatus(TextDocument.WriteError, 'Cannot save: ' + path + ': the page cannot write files')
    }
}

defineType(TextDocument, {
    props: {
        source: 'string',
        modified: 'bool',
        status: { kind: 'int', readonly: true },
        errorString: { kind: 'string', readonly: true },
    },
})

// QQuickTextDocument::Status
Object.assign(TextDocument, { Null: 0, Loading: 1, Loaded: 2, Saving: 3, Saved: 4, ReadError: 5, WriteError: 6, NonLocalFileError: 7 })

module.exports = { TextDocument }
