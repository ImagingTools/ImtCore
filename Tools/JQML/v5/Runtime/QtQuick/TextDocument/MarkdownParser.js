// A Markdown parser that reports what it finds the way md4c (the parser Qt 6.12 uses) does: blocks entered and left,
// spans entered and left, text of a type. CommonMark with the GitHub extensions Qt turns on (tables, strikethrough,
// task lists, permissive autolinks) and md4c's underline (_text_). Reference links use their definitions.
const { ENTITIES } = require('./Entities')

// MD_BLOCKTYPE, MD_SPANTYPE, MD_TEXTTYPE
const BLOCK = { DOC: 0, QUOTE: 1, UL: 2, OL: 3, LI: 4, HR: 5, H: 6, CODE: 7, HTML: 8, P: 9, TABLE: 10, THEAD: 11, TBODY: 12, TR: 13, TH: 14, TD: 15 }
const SPAN = { EM: 0, STRONG: 1, A: 2, IMG: 3, CODE: 4, DEL: 5, U: 9 }
const TEXT = { NORMAL: 0, NULLCHAR: 1, BR: 2, SOFTBR: 3, ENTITY: 4, CODE: 5, HTML: 6 }
const ALIGN = { DEFAULT: 0, LEFT: 1, CENTER: 2, RIGHT: 3 }

const isBlankLine = (s) => /^[ \t]*$/.test(s)
const PUNCTUATION = /[!-\/:-@\[-`{-~\p{P}\p{S}]/u
const isSpaceChar = (c) => c === undefined || /\s/u.test(c)

// the columns of the leading spaces and tabs of a line (a tab to the next multiple of 4), and the rest
function leading(line, column = 0){
    let i = 0, col = column
    while(i < line.length && (line[i] === ' ' || line[i] === '\t')){
        col = line[i] === '\t' ? col + 4 - (col % 4) : col + 1
        i++
    }
    return { indent: col - column, index: i }
}

// the line without n columns of indentation (a tab only partly taken leaves its spaces)
function removeIndent(line, n, column = 0){
    let i = 0, col = column
    while(i < line.length && col - column < n && (line[i] === ' ' || line[i] === '\t')){
        let next = line[i] === '\t' ? col + 4 - (col % 4) : col + 1
        if(next - column > n){
            // a tab partly consumed: its rest as spaces
            return ' '.repeat(next - column - n) + line.slice(i + 1)
        }
        col = next
        i++
    }
    return line.slice(i)
}

const HTML_BLOCK_TAGS = new Set(['address', 'article', 'aside', 'base', 'basefont', 'blockquote', 'body', 'caption', 'center', 'col',
    'colgroup', 'dd', 'details', 'dialog', 'dir', 'div', 'dl', 'dt', 'fieldset', 'figcaption', 'figure', 'footer', 'form', 'frame',
    'frameset', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'head', 'header', 'hr', 'html', 'iframe', 'legend', 'li', 'link', 'main', 'menu',
    'menuitem', 'nav', 'noframes', 'ol', 'optgroup', 'option', 'p', 'param', 'search', 'section', 'summary', 'table', 'tbody', 'td',
    'tfoot', 'th', 'thead', 'title', 'tr', 'track', 'ul'])

// the kind of HTML block a line starts (CommonMark 4.6), 0 for none; para: a paragraph is open (kind 7 cannot interrupt it)
function htmlBlockStart(text, para){
    if(/^<(script|pre|style|textarea)(\s|>|$)/i.test(text)) return 1
    if(text.startsWith('<!--')) return 2
    if(text.startsWith('<?')) return 3
    if(/^<![A-Za-z]/.test(text)) return 4
    if(text.startsWith('<![CDATA[')) return 5
    let m = /^<\/?([A-Za-z][A-Za-z0-9-]*)(\s|\/?>|$)/.exec(text)
    if(m && HTML_BLOCK_TAGS.has(m[1].toLowerCase())) return 6
    if(!para && (/^<[A-Za-z][A-Za-z0-9-]*(\s+[A-Za-z_:][\w.:-]*(\s*=\s*([^\s"'=<>`]+|'[^']*'|"[^"]*"))?)*\s*\/?>\s*$/.test(text)
        || /^<\/[A-Za-z][A-Za-z0-9-]*\s*>\s*$/.test(text))) return 7
    return 0
}

function htmlBlockEnds(kind, text){
    if(kind === 1) return /<\/(script|pre|style|textarea)>/i.test(text)
    if(kind === 2) return text.includes('-->')
    if(kind === 3) return text.includes('?>')
    if(kind === 4) return text.includes('>')
    if(kind === 5) return text.includes(']]>')
    return false
}

// the cells of a table row: the text between the pipes (escaped pipes stay)
function splitRow(text){
    let s = text.trim()
    if(s.startsWith('|')) s = s.slice(1)
    if(s.endsWith('|') && !s.endsWith('\\|')) s = s.slice(0, -1)
    let cells = []
    let cell = ''
    let code = 0
    for(let i = 0; i < s.length; i++){
        let c = s[i]
        if(c === '\\' && s[i + 1] === '|'){
            cell += '|'
            i++
            continue
        }
        if(c === '`') code ^= 1
        if(c === '|' && !code){
            cells.push(cell.trim())
            cell = ''
            continue
        }
        cell += c
    }
    cells.push(cell.trim())
    return cells
}

function delimiterRow(text){
    if(!/\|/.test(text) && !/^\s*:?-+:?\s*$/.test(text)) return null
    let cells = splitRow(text)
    if(!cells.length) return null
    let aligns = []
    for(let c of cells){
        if(!/^:?-+:?$/.test(c)) return null
        aligns.push(c.startsWith(':') && c.endsWith(':') ? ALIGN.CENTER : c.endsWith(':') ? ALIGN.RIGHT : c.startsWith(':') ? ALIGN.LEFT : ALIGN.DEFAULT)
    }
    return aligns
}

// ---- blocks ----

class BlockParser {
    constructor(text){
        this.lines = text.replace(/\r\n?/g, '\n').split('\n')
        if(this.lines.length && this.lines[this.lines.length - 1] === '') this.lines.pop()
        this.root = { type: 'doc', children: [], open: true }
        this.refs = new Map()
    }

    lastChild(node){
        return node.children && node.children.length ? node.children[node.children.length - 1] : null
    }

    // the deepest open container and its open leaf
    tip(){
        let node = this.root
        for(;;){
            let last = this.lastChild(node)
            if(!last || !last.open) return node
            if(last.type === 'quote' || last.type === 'list' || last.type === 'li') node = last
            else return last
        }
    }

    close(node){
        node.open = false
        if(node.children) for(let c of node.children) if(c.open) this.close(c)
        if(node.type === 'p') this.finishParagraph(node)
    }

    // a paragraph's link reference definitions taken out
    finishParagraph(p){
        let text = p.lines.join('\n')
        for(;;){
            let m = /^ {0,3}\[((?:[^\]\\]|\\.){1,999})\]:[ \t]*\n?[ \t]*(<[^>\n]*>|\S+)(?:(?:[ \t]+|[ \t]*\n[ \t]*)("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*'|\((?:[^)\\]|\\.)*\)))?[ \t]*(?:\n|$)/.exec(text)
            if(!m) break
            let label = m[1].trim().replace(/\s+/g, ' ').toLowerCase()
            let dest = m[2].startsWith('<') ? m[2].slice(1, -1) : m[2]
            let title = m[3] ? m[3].slice(1, -1) : ''
            if(!this.refs.has(label)) this.refs.set(label, { dest: unescape(dest), title: unescape(title) })
            text = text.slice(m[0].length)
        }
        p.text = text
        if(!text.trim()) p.empty = true
    }

    parse(){
        for(let raw of this.lines) this.line(raw)
        this.close(this.root)
        return this.root
    }

    line(raw){
        let line = raw
        let column = 0
        // the open containers the line continues
        let node = this.root
        let matchedAll = true
        let lastMatched = this.root
        let path = []
        for(;;){
            let last = this.lastChild(node)
            if(!last || !last.open || !(last.type === 'quote' || last.type === 'list' || last.type === 'li')){
                break
            }
            if(last.type === 'quote'){
                let { indent, index } = leading(line, column)
                if(indent <= 3 && line[index] === '>'){
                    let rest = line.slice(index + 1)
                    column += indent + 1
                    if(rest[0] === ' ') rest = rest.slice(1), column++
                    else if(rest[0] === '\t') rest = removeIndent(rest, 1, column), column++
                    line = rest
                    node = last
                    lastMatched = last
                    continue
                }
                matchedAll = false
                break
            }
            if(last.type === 'list'){
                // a list goes on as long as something follows in it: a new item, or a line of its last item
                node = last
                lastMatched = last
                continue
            }
            // a list item: blank, or indented as far as its content
            if(isBlankLine(line)){
                if(last.children.length || last.startedBlank === false){
                    node = last
                    lastMatched = last
                    continue
                }
                matchedAll = false
                break
            }
            let { indent } = leading(line, column)
            if(indent >= last.contentIndent){
                line = removeIndent(line, last.contentIndent, column)
                column += last.contentIndent
                node = last
                lastMatched = last
                continue
            }
            matchedAll = false
            break
        }
        let container = lastMatched
        let tip = this.tip()
        // a fenced code block or an HTML block goes on in its container
        if(tip.type === 'code' && tip.open && tip.fenced && this.isIn(tip, container)){
            let { indent, index } = leading(line)
            let rest = line.slice(index)
            let close = new RegExp('^' + (tip.fence === '`' ? '`' : '~') + '{' + tip.fenceLength + ',}[ \\t]*$')
            if(indent <= 3 && close.test(rest)){
                tip.open = false
                return
            }
            tip.lines.push(removeIndent(line, tip.fenceIndent))
            return
        }
        if(tip.type === 'html' && tip.open && this.isIn(tip, container)){
            if(tip.kind >= 6 && isBlankLine(line)){
                tip.open = false
            } else {
                tip.lines.push(line)
                if(htmlBlockEnds(tip.kind, line)) tip.open = false
                return
            }
        }
        // lazy continuation of a paragraph
        let blank = isBlankLine(line)
        if(!matchedAll && tip.type === 'p' && tip.open && !blank && !this.startsBlock(line, true)){
            tip.lines.push(line.replace(/^[ \t]+/, ''))
            return
        }
        // the containers not continued are closed
        this.closeAfter(container)
        // new containers
        let parent = container
        // a list none of whose items goes on: the block after it is its parent's (a new item is its own)
        let leaveList = () => {
            if(parent.type !== 'list') return
            this.close(parent)
            parent = parent.parent
        }
        for(;;){
            let { indent, index } = leading(line, column)
            let rest = line.slice(index)
            if(indent >= 4 && !(this.lastChild(parent) && this.lastChild(parent).type === 'p' && this.lastChild(parent).open)) break
            if(rest[0] === '>'){
                leaveList()
                this.closeLeaf(parent)
                let quote = { type: 'quote', children: [], open: true }
                this.add(parent, quote)
                let r = rest.slice(1)
                column += indent + 1
                if(r[0] === ' ') r = r.slice(1), column++
                line = r
                parent = quote
                continue
            }
            let item = this.listMarker(rest, indent, parent)
            if(item){
                if(parent.type === 'list' && !(parent.ordered === item.ordered && parent.mark === item.mark)) leaveList()
                if(parent.type === 'list'){
                    let list = parent
                    let prev = this.lastChild(list)
                    if(prev && prev.open) this.close(prev)
                    let li = { type: 'li', children: [], open: true, contentIndent: indent + item.width, startedBlank: item.blankStart, parent: list }
                    list.children.push(li)
                    if(list.blankBeforeItem) list.loose = true
                    list.blankBeforeItem = false
                    column += indent + item.width
                    line = item.rest
                    parent = li
                    if(item.blankStart) return
                    continue
                }
                this.closeLeaf(parent)
                // the list it goes into: the open one of the same kind, else a new one
                let last = this.lastChild(parent)
                let list
                if(last && last.type === 'list' && last.open && last.ordered === item.ordered && last.mark === item.mark){
                    list = last
                } else {
                    if(last && last.open) this.close(last)
                    list = { type: 'list', ordered: item.ordered, mark: item.mark, start: item.start, children: [], open: true, tight: true }
                    this.add(parent, list)
                }
                let prev = this.lastChild(list)
                if(prev && prev.open) this.close(prev)
                let li = { type: 'li', children: [], open: true, contentIndent: indent + item.width, startedBlank: item.blankStart, parent: list }
                list.children.push(li)
                if(list.blankBeforeItem) list.loose = true
                list.blankBeforeItem = false
                column += indent + item.width
                line = item.rest
                parent = li
                if(item.blankStart) return
                continue
            }
            break
        }
        // the leaf of the line (a blank line leaves a list open)
        if(parent.type === 'list'){
            if(isBlankLine(line)){
                this.markBlank(parent)
                return
            }
            leaveList()
        }
        this.leaf(parent, line, column)
    }

    isIn(node, container){
        // whether node is a child of container's subtree as its open leaf
        let tip = container
        let last = this.lastChild(tip)
        return last === node
    }

    add(parent, node){
        let last = this.lastChild(parent)
        if(last && last.open) this.close(last)
        node.parent = parent
        parent.children.push(node)
    }

    closeLeaf(parent){
        let last = this.lastChild(parent)
        if(last && last.open && !(last.type === 'quote' || last.type === 'list' || last.type === 'li')) this.close(last)
    }

    // close the open children below container
    closeAfter(container){
        let last = this.lastChild(container)
        if(last && last.open && (last.type === 'quote' || last.type === 'list' || last.type === 'li')){
            // keep a list open for a following item: closed when another block starts
            this.close(last)
        }
    }

    // a list marker at the start of rest: { ordered, mark, start, width, rest, blankStart }
    listMarker(rest, indent, parent){
        let m = /^([*+-])([ \t]+|$)/.exec(rest)
        let ordered = false, mark, start = 1, markerLength
        if(m){
            // a thematic break is no list item
            if(/^([*-])[ \t]*(\1[ \t]*){2,}$/.test(rest)) return null
            mark = m[1]
            markerLength = 1
        } else {
            m = /^(\d{1,9})([.)])([ \t]+|$)/.exec(rest)
            if(!m) return null
            ordered = true
            mark = m[2]
            start = parseInt(m[1], 10)
            markerLength = m[1].length + 1
        }
        let after = rest.slice(markerLength)
        let blankStart = isBlankLine(after)
        // a paragraph is interrupted only by an item that is not empty, ordered starting at 1
        let last = this.lastChild(parent)
        let interrupting = last && last.type === 'p' && last.open
        if(interrupting && (blankStart || (ordered && start !== 1))) return null
        let { indent: spaces } = leading(after, indent + markerLength)
        let width
        if(blankStart) width = markerLength + 1
        else if(spaces >= 5) width = markerLength + 1
        else width = markerLength + spaces
        let content = blankStart ? '' : removeIndent(after, spaces >= 5 ? 1 : spaces, indent + markerLength)
        return { ordered, mark, start, width, rest: content, blankStart }
    }

    // whether a line starts a block that interrupts a paragraph
    startsBlock(line, lazy){
        let { indent, index } = leading(line)
        if(indent >= 4) return false
        let rest = line.slice(index)
        if(rest[0] === '>') return true
        if(/^#{1,6}([ \t]|$)/.test(rest)) return true
        if(/^(`{3,}|~{3,})/.test(rest)) return true
        if(/^([*_-])[ \t]*(\1[ \t]*){2,}$/.test(rest)) return true
        if(htmlBlockStart(rest, true)) return true
        // a list item interrupts a paragraph when it is not empty and an ordered one starts at 1; a paragraph of a list
        // item the line does not continue (lazy) is no paragraph of the container the item may start in
        if(lazy ? /^([*+-]|\d{1,9}[.)])([ \t]|$)/.test(rest) : (/^[*+-][ \t]+\S/.test(rest) || /^1[.)][ \t]+\S/.test(rest))) return true
        return false
    }

    leaf(parent, line, column){
        let blank = isBlankLine(line)
        let last = this.lastChild(parent)
        let openP = last && last.type === 'p' && last.open ? last : null
        if(blank){
            if(openP) this.close(openP)
            if(last && last.type === 'code' && last.open && !last.fenced) last.lines.push('')
            // a blank line in a list makes it loose when something follows
            this.markBlank(parent)
            return
        }
        let { indent, index } = leading(line)
        let rest = line.slice(index)
        // indented code
        if(indent >= 4 && !openP){
            let code = last && last.type === 'code' && last.open && !last.fenced ? last : null
            if(!code){
                code = { type: 'code', fenced: false, lines: [], open: true, info: '' }
                this.add(parent, code)
            }
            code.lines.push(removeIndent(line, 4))
            return
        }
        if(last && last.type === 'code' && last.open && !last.fenced) this.close(last)
        // ATX heading
        let h = /^(#{1,6})([ \t]+|$)(.*)$/.exec(rest)
        if(h && indent <= 3){
            let text = h[3].replace(/[ \t]+#+[ \t]*$/, '').replace(/^#+[ \t]*$/, '').trim()
            this.add(parent, { type: 'h', level: h[1].length, text, open: false })
            return
        }
        // fenced code
        let f = /^(`{3,}|~{3,})(.*)$/.exec(rest)
        if(f && indent <= 3 && !(f[1][0] === '`' && f[2].includes('`'))){
            let info = f[2].trim()
            this.add(parent, { type: 'code', fenced: true, fence: f[1][0], fenceLength: f[1].length, fenceIndent: indent, info: unescape(info), lines: [], open: true })
            return
        }
        // HTML block
        let kind = indent <= 3 ? htmlBlockStart(rest, !!openP) : 0
        if(kind){
            let html = { type: 'html', kind, lines: [line], open: true }
            this.add(parent, html)
            if(htmlBlockEnds(kind, line)) html.open = false
            return
        }
        // setext heading underline
        if(openP && indent <= 3 && /^(=+|-+)[ \t]*$/.test(rest) && !(openP.table)){
            this.finishParagraph(openP)
            if(openP.text.trim()){
                parent.children.pop()
                this.add(parent, { type: 'h', level: rest[0] === '=' ? 1 : 2, text: openP.text.trim(), open: false })
                return
            }
        }
        // thematic break
        if(indent <= 3 && /^([*_-])[ \t]*(\1[ \t]*){2,}$/.test(rest)){
            this.add(parent, { type: 'hr', open: false })
            return
        }
        // a table: the paragraph's last line is the header, this line the delimiter row
        if(openP && openP.lines.length >= 1){
            let aligns = delimiterRow(line)
            if(aligns){
                let header = splitRow(openP.lines[openP.lines.length - 1])
                if(header.length === aligns.length){
                    openP.lines.pop()
                    if(!openP.lines.length) parent.children.pop()
                    else this.close(openP)
                    this.add(parent, { type: 'table', aligns, header, rows: [], open: true })
                    return
                }
            }
        }
        if(last && last.type === 'table' && last.open){
            if(/\|/.test(line) || true){
                last.rows.push(splitRow(line))
                return
            }
        }
        if(openP){
            openP.lines.push(rest)
            return
        }
        this.add(parent, { type: 'p', lines: [rest], open: true })
    }

    // a blank line after something in a list item: the list is loose if another item or block follows
    markBlank(parent){
        for(let node = parent; node; node = node.parentList){
            if(node.type === 'li') node.blankAfter = true
        }
        // the list of the deepest open item
        let list = this.openList(this.root)
        if(list) list.blankBeforeItem = true
    }

    openList(node){
        let found = null
        let walk = (n) => {
            let last = this.lastChild(n)
            if(!last || !last.open) return
            if(last.type === 'list') found = last
            if(last.type === 'quote' || last.type === 'list' || last.type === 'li') walk(last)
        }
        walk(node)
        return found
    }
}

function unescape(s){
    return s.replace(/\\([!-\/:-@\[-`{-~])/g, '$1').replace(/&(#\d+|#[xX][0-9a-fA-F]+|[A-Za-z][A-Za-z0-9]*);/g, (m, e) => {
        if(e[0] === '#'){
            let code = e[1] === 'x' || e[1] === 'X' ? parseInt(e.slice(2), 16) : parseInt(e.slice(1), 10)
            return String.fromCodePoint(code > 0x10ffff || code === 0 ? 0xfffd : code)
        }
        return Object.prototype.hasOwnProperty.call(ENTITIES, e) ? String.fromCharCode(ENTITIES[e]) : m
    })
}

// ---- inlines ----

// the inline content of a paragraph, a heading or a cell as nodes: text, code, html, entity, br, softbr, emph (em,
// strong, u, del), link, image
function parseInlines(text, refs){
    let nodes = []
    let delims = []
    let brackets = []
    let i = 0
    let buffer = ''
    let flush = () => {
        if(buffer){
            nodes.push({ t: 'text', s: buffer })
            buffer = ''
        }
    }
    while(i < text.length){
        let c = text[i]
        if(c === '\\'){
            let n = text[i + 1]
            if(n === '\n'){
                flush()
                nodes.push({ t: 'br' })
                i += 2
                while(text[i] === ' ' || text[i] === '\t') i++
                continue
            }
            if(n !== undefined && /[!-\/:-@\[-`{-~]/.test(n)){
                buffer += n
                i += 2
                continue
            }
            buffer += c
            i++
            continue
        }
        if(c === '\n'){
            // two spaces before it: a hard break
            let hard = /  +$/.test(buffer)
            buffer = buffer.replace(/ +$/, '')
            flush()
            nodes.push({ t: hard ? 'br' : 'softbr' })
            i++
            while(text[i] === ' ' || text[i] === '\t') i++
            continue
        }
        if(c === '`'){
            let run = /^`+/.exec(text.slice(i))[0]
            let close = -1
            let j = i + run.length
            while(j < text.length){
                let k = text.indexOf('`', j)
                if(k < 0) break
                let r = /^`+/.exec(text.slice(k))[0]
                if(r.length === run.length){
                    close = k
                    break
                }
                j = k + r.length
            }
            if(close >= 0){
                flush()
                let code = text.slice(i + run.length, close).replace(/\n/g, ' ')
                if(code.length > 2 && code[0] === ' ' && code[code.length - 1] === ' ' && code.trim()) code = code.slice(1, -1)
                nodes.push({ t: 'code', s: code })
                i = close + run.length
                continue
            }
            buffer += run
            i += run.length
            continue
        }
        if(c === '&'){
            let m = /^&(#\d{1,7}|#[xX][0-9a-fA-F]{1,6}|[A-Za-z][A-Za-z0-9]{1,31});/.exec(text.slice(i))
            if(m && (m[1][0] === '#' || Object.prototype.hasOwnProperty.call(ENTITIES, m[1]))){
                flush()
                nodes.push({ t: 'entity', s: m[0] })
                i += m[0].length
                continue
            }
            buffer += c
            i++
            continue
        }
        if(c === '<'){
            let rest = text.slice(i)
            let auto = /^<([A-Za-z][A-Za-z0-9+.-]{1,31}:[^<>\s]*)>/.exec(rest)
            let mail = /^<([A-Za-z0-9.!#$%&'*+\/=?^_`{|}~-]+@[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?(?:\.[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?)*)>/.exec(rest)
            if(auto || mail){
                flush()
                let target = auto ? auto[1] : mail[1]
                nodes.push({ t: 'link', href: auto ? target : 'mailto:' + target, title: '', children: [{ t: 'text', s: target }], auto: true })
                i += (auto || mail)[0].length
                continue
            }
            let html = /^(<[A-Za-z][A-Za-z0-9-]*(?:\s+[A-Za-z_:][\w.:-]*(?:\s*=\s*(?:[^\s"'=<>`]+|'[^']*'|"[^"]*"))?)*\s*\/?>|<\/[A-Za-z][A-Za-z0-9-]*\s*>|<!--[\s\S]*?-->|<\?[\s\S]*?\?>|<![A-Za-z][^>]*>|<!\[CDATA\[[\s\S]*?\]\]>)/.exec(rest)
            if(html){
                flush()
                nodes.push({ t: 'html', s: html[0] })
                i += html[0].length
                continue
            }
            buffer += c
            i++
            continue
        }
        if(c === '*' || c === '_' || c === '~'){
            let run = new RegExp('^\\' + c + '+').exec(text.slice(i))[0]
            let before = i > 0 ? text[i - 1] : undefined
            let after = text[i + run.length]
            let leftFlanking = !isSpaceChar(after) && (!PUNCTUATION.test(after) || isSpaceChar(before) || PUNCTUATION.test(before))
            let rightFlanking = !isSpaceChar(before) && (!PUNCTUATION.test(before) || isSpaceChar(after) || PUNCTUATION.test(after))
            let canOpen, canClose
            if(c === '_'){
                canOpen = leftFlanking && (!rightFlanking || PUNCTUATION.test(before || ''))
                canClose = rightFlanking && (!leftFlanking || PUNCTUATION.test(after || ''))
            } else {
                canOpen = leftFlanking
                canClose = rightFlanking
            }
            if(c === '~' && run.length > 2){
                buffer += run
                i += run.length
                continue
            }
            flush()
            let node = { t: 'text', s: run }
            nodes.push(node)
            delims.push({ node, c, count: run.length, original: run.length, canOpen, canClose, active: true })
            i += run.length
            continue
        }
        if(c === '!' && text[i + 1] === '['){
            flush()
            let node = { t: 'text', s: '![' }
            nodes.push(node)
            brackets.push({ node, image: true, index: nodes.length - 1, delimIndex: delims.length, active: true })
            i += 2
            continue
        }
        if(c === '['){
            flush()
            let node = { t: 'text', s: '[' }
            nodes.push(node)
            brackets.push({ node, image: false, index: nodes.length - 1, delimIndex: delims.length, active: true })
            i++
            continue
        }
        if(c === ']'){
            flush()
            let opener = brackets.pop()
            if(!opener || !opener.active){
                buffer += ']'
                i++
                continue
            }
            // an inline link (destination, title), a full, collapsed or shortcut reference
            let rest = text.slice(i + 1)
            let link = null
            let consumed = 0
            let m = /^\(\s*(<(?:[^<>\n\\]|\\.)*>|(?:[^\s()\\]|\\.|\((?:[^\s()\\]|\\.)*\))*)(?:\s+("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*'|\((?:[^()\\]|\\.)*\)))?\s*\)/.exec(rest)
            if(m){
                let dest = m[1].startsWith('<') ? m[1].slice(1, -1) : m[1]
                link = { href: unescape(dest), title: m[2] ? unescape(m[2].slice(1, -1)) : '' }
                consumed = m[0].length
            } else {
                let label = null
                let r = /^\[((?:[^\]\\]|\\.){0,999})\]/.exec(rest)
                if(r && r[1].trim()){
                    label = r[1]
                    consumed = r[0].length
                } else {
                    label = text.slice(opener.index >= 0 ? textIndexOf(nodes, opener) : 0, i)
                    label = nodesText(nodes.slice(opener.index + 1))
                    consumed = r ? r[0].length : 0
                }
                let def = refs.get(label.trim().replace(/\s+/g, ' ').toLowerCase())
                if(def) link = { href: def.dest, title: def.title }
                else consumed = 0
            }
            if(!link){
                buffer += ']'
                i++
                continue
            }
            // the bracket's content: emphasis inside it resolved first
            let inner = nodes.splice(opener.index + 1)
            let innerDelims = delims.splice(opener.delimIndex)
            processEmphasis(inner, innerDelims)
            nodes.pop()
            let node = opener.image
                ? { t: 'image', src: link.href, title: link.title, children: inner }
                : { t: 'link', href: link.href, title: link.title, children: inner }
            nodes.push(node)
            // links contain no links
            if(!opener.image) for(let b of brackets) if(!b.image) b.active = false
            i += 1 + consumed
            continue
        }
        buffer += c
        i++
    }
    flush()
    processEmphasis(nodes, delims)
    return autolinks(mergeText(nodes))
}

function nodesText(nodes){
    return nodes.map(n => n.s !== undefined ? n.s : n.children ? nodesText(n.children) : '').join('')
}

function textIndexOf(){ return 0 }

// CommonMark's process emphasis: closers matched with openers, the runs turned into emphasis nodes
function processEmphasis(nodes, delims){
    let closer = 0
    while(closer < delims.length){
        let d = delims[closer]
        if(!d.canClose || d.count === 0){
            closer++
            continue
        }
        let found = -1
        for(let o = closer - 1; o >= 0; o--){
            let op = delims[o]
            if(op.c !== d.c || !op.canOpen || op.count === 0) continue
            if(d.c !== '~' && (op.canClose || d.canOpen) && (op.original + d.original) % 3 === 0 && !(op.original % 3 === 0 && d.original % 3 === 0)) continue
            if(d.c === '~' && op.count !== d.count) continue
            found = o
            break
        }
        if(found < 0){
            closer++
            continue
        }
        let op = delims[found]
        let use = d.c === '~' ? d.count : (op.count >= 2 && d.count >= 2 ? 2 : 1)
        // md4c with MD_FLAG_UNDERLINE: an underscore underlines, one or two of them
        let kind = d.c === '~' ? 'del' : d.c === '_' ? 'u' : (use === 2 ? 'strong' : 'em')
        let start = nodes.indexOf(op.node), end = nodes.indexOf(d.node)
        let children = nodes.slice(start + 1, end)
        let emph = { t: 'emph', kind, children }
        nodes.splice(start + 1, end - start - 1, emph)
        op.count -= use
        d.count -= use
        op.node.s = op.node.s.slice(0, op.count)
        d.node.s = d.node.s.slice(0, d.count)
        // the delimiters between them are gone
        delims.splice(found + 1, closer - found - 1)
        closer = found + 1
        if(op.count === 0){
            nodes.splice(nodes.indexOf(op.node), 1)
            delims.splice(found, 1)
            closer--
        }
        if(d.count === 0){
            nodes.splice(nodes.indexOf(d.node), 1)
            delims.splice(closer, 1)
        }
    }
}

function mergeText(nodes){
    let out = []
    for(let n of nodes){
        if(n.children) n.children = mergeText(n.children)
        let last = out[out.length - 1]
        if(n.t === 'text' && !n.s) continue
        if(n.t === 'text' && last && last.t === 'text') last.s += n.s
        else out.push(n)
    }
    return out
}

// md4c's permissive autolinks: www., http(s)://, ftp:// and e-mail addresses in text
function autolinks(nodes){
    let out = []
    for(let n of nodes){
        if(n.children){
            if(n.t !== 'link' && n.t !== 'image') n.children = autolinks(n.children)
            out.push(n)
            continue
        }
        if(n.t !== 'text'){
            out.push(n)
            continue
        }
        let s = n.s
        let re = /(?:https?:\/\/|ftp:\/\/|www\.)[^\s<]*[^\s<?!.,:*_~)]|[A-Za-z0-9._+-]+@[A-Za-z0-9-]+(?:\.[A-Za-z0-9-]+)+/g
        let last = 0, m
        while((m = re.exec(s))){
            let before = s[m.index - 1]
            if(before !== undefined && /[A-Za-z0-9]/.test(before)) continue
            if(m.index > last) out.push({ t: 'text', s: s.slice(last, m.index) })
            let target = m[0]
            let href = target.includes('@') && !/^(https?|ftp):/.test(target) && !target.startsWith('www.') ? 'mailto:' + target : target.startsWith('www.') ? 'http://' + target : target
            out.push({ t: 'link', href, title: '', children: [{ t: 'text', s: target }], auto: true })
            last = m.index + m[0].length
        }
        if(last < s.length) out.push({ t: 'text', s: s.slice(last) })
    }
    return out
}

// ---- the events ----

function parse(text, handler){
    let parser = new BlockParser(text)
    let root = parser.parse()
    let refs = parser.refs
    let inlines = (s) => emitInlines(parseInlines(s, refs), handler)
    let walk = (node, tight) => {
        switch(node.type){
            case 'doc':
                handler.enterBlock(BLOCK.DOC, {})
                for(let c of node.children) walk(c, false)
                handler.leaveBlock(BLOCK.DOC, {})
                break
            case 'quote':
                handler.enterBlock(BLOCK.QUOTE, {})
                for(let c of node.children) walk(c, false)
                handler.leaveBlock(BLOCK.QUOTE, {})
                break
            case 'list': {
                let loose = node.loose || node.children.some((li, k) => li.blankAfter && (k < node.children.length - 1 || li.children.length > 1 && li.children.some((c, j) => j > 0)))
                let type = node.ordered ? BLOCK.OL : BLOCK.UL
                let detail = node.ordered ? { start: node.start, isTight: !loose, markDelimiter: node.mark } : { isTight: !loose, mark: node.mark }
                handler.enterBlock(type, detail)
                for(let li of node.children){
                    // a task: [ ] or [x] at the start of the first paragraph
                    let first = li.children[0]
                    let task = null
                    if(first && first.type === 'p' && /^\[[ xX]\](?:[ \t]|$)/.test(first.text)){
                        task = first.text[1]
                        first.text = first.text.slice(3).replace(/^[ \t]/, '')
                    }
                    handler.enterBlock(BLOCK.LI, { isTask: task !== null, taskMark: task || '' })
                    for(let c of li.children) walk(c, !loose)
                    handler.leaveBlock(BLOCK.LI, {})
                }
                handler.leaveBlock(type, detail)
                break
            }
            case 'p':
                if(node.empty) break
                if(!tight) handler.enterBlock(BLOCK.P, {})
                inlines(node.text.replace(/[ \t]+$/, ''))
                if(!tight) handler.leaveBlock(BLOCK.P, {})
                break
            case 'h':
                handler.enterBlock(BLOCK.H, { level: node.level })
                inlines(node.text)
                handler.leaveBlock(BLOCK.H, { level: node.level })
                break
            case 'hr':
                handler.enterBlock(BLOCK.HR, {})
                handler.leaveBlock(BLOCK.HR, {})
                break
            case 'code': {
                let lines = node.lines.slice()
                if(!node.fenced) while(lines.length && isBlankLine(lines[lines.length - 1])) lines.pop()
                let info = node.info || ''
                let detail = { info, lang: info.split(/\s+/)[0] || '', fenceChar: node.fenced ? node.fence : '' }
                handler.enterBlock(BLOCK.CODE, detail)
                for(let l of lines){
                    if(l) handler.text(TEXT.CODE, l)
                    handler.text(TEXT.CODE, '\n')
                }
                handler.leaveBlock(BLOCK.CODE, detail)
                break
            }
            case 'html':
                handler.enterBlock(BLOCK.HTML, {})
                for(let l of node.lines) handler.text(TEXT.HTML, l + '\n')
                handler.leaveBlock(BLOCK.HTML, {})
                break
            case 'table': {
                let columns = node.aligns.length
                handler.enterBlock(BLOCK.TABLE, { columns, headRows: 1, bodyRows: node.rows.length })
                handler.enterBlock(BLOCK.THEAD, {})
                handler.enterBlock(BLOCK.TR, {})
                for(let c = 0; c < columns; c++){
                    handler.enterBlock(BLOCK.TH, { align: node.aligns[c] })
                    inlines(node.header[c] || '')
                    handler.leaveBlock(BLOCK.TH, {})
                }
                handler.leaveBlock(BLOCK.TR, {})
                handler.leaveBlock(BLOCK.THEAD, {})
                if(node.rows.length){
                    handler.enterBlock(BLOCK.TBODY, {})
                    for(let row of node.rows){
                        handler.enterBlock(BLOCK.TR, {})
                        for(let c = 0; c < columns; c++){
                            handler.enterBlock(BLOCK.TD, { align: node.aligns[c] })
                            inlines(row[c] || '')
                            handler.leaveBlock(BLOCK.TD, {})
                        }
                        handler.leaveBlock(BLOCK.TR, {})
                    }
                    handler.leaveBlock(BLOCK.TBODY, {})
                }
                handler.leaveBlock(BLOCK.TABLE, {})
                break
            }
        }
    }
    walk(root, false)
}

function emitInlines(nodes, handler){
    for(let n of nodes){
        switch(n.t){
            case 'text': handler.text(n.s.includes('\u0000') ? TEXT.NULLCHAR : TEXT.NORMAL, n.s.replace(/\u0000/g, '\ufffd')); break
            case 'code':
                handler.enterSpan(SPAN.CODE, {})
                handler.text(TEXT.CODE, n.s)
                handler.leaveSpan(SPAN.CODE, {})
                break
            case 'entity': handler.text(TEXT.ENTITY, n.s); break
            case 'html': handler.text(TEXT.HTML, n.s); break
            case 'br': handler.text(TEXT.BR, '\n'); break
            case 'softbr': handler.text(TEXT.SOFTBR, '\n'); break
            case 'emph': {
                let type = n.kind === 'em' ? SPAN.EM : n.kind === 'strong' ? SPAN.STRONG : n.kind === 'u' ? SPAN.U : SPAN.DEL
                handler.enterSpan(type, {})
                emitInlines(n.children, handler)
                handler.leaveSpan(type, {})
                break
            }
            case 'link':
                handler.enterSpan(SPAN.A, { href: n.href, title: n.title, autolink: !!n.auto })
                emitInlines(n.children, handler)
                handler.leaveSpan(SPAN.A, {})
                break
            case 'image':
                handler.enterSpan(SPAN.IMG, { src: n.src, title: n.title })
                emitInlines(n.children, handler)
                handler.leaveSpan(SPAN.IMG, {})
                break
        }
    }
}

module.exports = { parse, BLOCK, SPAN, TEXT, ALIGN }
