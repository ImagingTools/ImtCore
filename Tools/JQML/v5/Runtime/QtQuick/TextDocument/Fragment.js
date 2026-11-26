// QTextDocumentFragment and QTextCopyHelper (qtextdocumentfragment.cpp of Qt 6.12): a range of a document copied into a
// document of its own, a fragment inserted at a cursor, a fragment of HTML
const F = require('./Formats')
const { TextDocument, isBlockSeparator } = require('./Document')
const { HtmlImporter, IMPORT_TO_FRAGMENT } = require('./HtmlImporter')

class CopyHelper {
    // [start, end) of src into dst at insertPos
    constructor(src, start, end, dst, insertPos){
        this.src = src
        this.dst = dst
        this.start = start
        this.end = end
        this.insertPos = insertPos
        this.lists = new Map()
    }

    // convertFormatIndex: a list of the source is a new list of the destination (one for each)
    convert(format){
        if(!format.list) return format
        let list = this.lists.get(format.list)
        if(!list){
            list = this.dst.createListObject(format.list.format)
            this.lists.set(format.list, list)
        }
        return F.with_(format, { list })
    }

    appendFragment(pos, endPos){
        let src = this.src, dst = this.dst
        let { index, start } = src.fragmentAt(pos)
        let frag = src.fragments[index]
        let charFormat = this.convert(frag.format)
        let offset = Math.max(0, pos - start)
        let charsToCopy = Math.min(frag.size - offset, endPos - pos)
        let nextBlock = src.blockIndexAt(pos + 1)
        let nextBlockStart = src.blockStart(nextBlock)
        let blockFormat = null
        if(nextBlockStart === pos + 1){
            blockFormat = this.convert(src.blockFormat(nextBlock))
        } else if(pos === 0 && this.insertPos === 0){
            dst.setBlockFormat(0, this.convert(src.blockFormat(0)))
            dst.setCharFormat(-1, 1, this.convert(src.blockCharFormat(0)), false)
        }
        let txt = src.text.substr(frag.stringPosition + offset, charsToCopy)
        if(txt.length === 1 && isBlockSeparator(txt)){
            dst.insertBlock(this.insertPos, blockFormat || F.EMPTY, charFormat, txt)
            this.insertPos++
        } else {
            let srcList = src.blockFormat(nextBlock).list
            if(srcList){
                let dstBlock = dst.blockIndexAt(this.insertPos)
                if(!dst.blockFormat(dstBlock).list){
                    // a block of the source's block and char format so that the text is in a list as it should
                    dst.insertBlock(this.insertPos, this.convert(src.blockFormat(nextBlock)), this.convert(src.blockCharFormat(nextBlock)))
                    this.insertPos++
                }
            }
            dst.insertString(this.insertPos, txt, charFormat)
            let userState = src.blocks[nextBlock].userState
            if(userState !== -1) dst.blocks[dst.blockIndexAt(this.insertPos)].userState = userState
            this.insertPos += txt.length
        }
        return charsToCopy
    }

    copy(){
        let pos = this.start
        while(pos < this.end) pos += this.appendFragment(pos, this.end)
        return this.insertPos
    }
}

// QTextDocumentFragment(cursor): the selection [start, end) as a document
function fragmentOf(doc, start, end){
    let fragment = new TextDocument()
    if(start !== end) new CopyHelper(doc, Math.min(start, end), Math.max(start, end), fragment, 0).copy()
    return fragment
}

// QTextDocumentFragment::fromHtml
function fragmentFromHtml(html){
    let fragment = new TextDocument()
    new HtmlImporter(fragment, html, IMPORT_TO_FRAGMENT).import()
    return fragment
}

// QTextCursor::insertFragment: the selection removed, the fragment inserted; the position after it
function insertFragment(cursor, fragment){
    if(fragment.characterCount() <= 1) return
    cursor.doc.beginEditBlock()
    cursor.removeSelection()
    let end = new CopyHelper(fragment, 0, fragment.length(), cursor.doc, cursor.position).copy()
    cursor.doc.endEditBlock()
    cursor.position = cursor.anchor = end
    cursor.currentCharFormat = null
}

module.exports = { fragmentOf, fragmentFromHtml, insertFragment, CopyHelper }
