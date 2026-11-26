// The CSS of Qt's rich text (qcssparser.cpp of Qt 6.12): the declarations of a style attribute and of the style sheets
// of a <style>, their values (Value types of QCss::Parser::parseTerm), the selectors QCss::StyleSelector matches and the
// parts of QCss::ValueExtractor the HTML parser uses (box, border, font, background, colors).
const { parseColor, colorName } = require('../../Core/color')

// ---- names (properties[] and values[] of qcssparser.cpp) ----

const PROPERTIES = new Set(['-qt-background-role', '-qt-block-indent', '-qt-fg-texture-cachekey', '-qt-foreground',
    '-qt-line-height-type', '-qt-list-indent', '-qt-list-number-prefix', '-qt-list-number-suffix', '-qt-paragraph-type',
    '-qt-stroke-color', '-qt-stroke-dasharray', '-qt-stroke-dashoffset', '-qt-stroke-linecap', '-qt-stroke-linejoin',
    '-qt-stroke-miterlimit', '-qt-stroke-width', '-qt-style-features', '-qt-table-type', '-qt-user-state', 'accent-color',
    'alternate-background-color', 'background', 'background-attachment', 'background-clip', 'background-color',
    'background-image', 'background-origin', 'background-position', 'background-repeat', 'border', 'border-bottom',
    'border-bottom-color', 'border-bottom-left-radius', 'border-bottom-right-radius', 'border-bottom-style',
    'border-bottom-width', 'border-collapse', 'border-color', 'border-image', 'border-left', 'border-left-color',
    'border-left-style', 'border-left-width', 'border-radius', 'border-right', 'border-right-color', 'border-right-style',
    'border-right-width', 'border-style', 'border-top', 'border-top-color', 'border-top-left-radius',
    'border-top-right-radius', 'border-top-style', 'border-top-width', 'border-width', 'bottom', 'color', 'float', 'font',
    'font-family', 'font-kerning', 'font-size', 'font-style', 'font-variant', 'font-weight', 'height', 'icon', 'image',
    'image-position', 'left', 'letter-spacing', 'line-height', 'list-style', 'list-style-type', 'margin', 'margin-bottom',
    'margin-left', 'margin-right', 'margin-top', 'max-height', 'max-width', 'min-height', 'min-width', 'outline',
    'outline-bottom-left-radius', 'outline-bottom-right-radius', 'outline-color', 'outline-offset', 'outline-radius',
    'outline-style', 'outline-top-left-radius', 'outline-top-right-radius', 'outline-width', 'padding', 'padding-bottom',
    'padding-left', 'padding-right', 'padding-top', 'page-break-after', 'page-break-before', 'placeholder-text-color',
    'position', 'right', 'selection-background-color', 'selection-color', 'spacing', 'subcontrol-origin',
    'subcontrol-position', 'text-align', 'text-decoration', 'text-decoration-color', 'text-indent', 'text-transform',
    'text-underline-style', 'top', 'vertical-align', 'white-space', 'width', 'word-spacing'])

const KNOWN_VALUES = new Set(['accent', 'active', 'alternate-base', 'always', 'auto', 'base', 'beveljoin', 'bold', 'bottom',
    'bright-text', 'button', 'button-text', 'center', 'circle', 'dark', 'dashed', 'decimal', 'disabled', 'disc', 'dot-dash',
    'dot-dot-dash', 'dotted', 'double', 'flatcap', 'groove', 'highlight', 'highlighted-text', 'inset', 'italic', 'large',
    'left', 'light', 'line-through', 'link', 'link-visited', 'lower-alpha', 'lower-roman', 'lowercase', 'medium', 'mid',
    'middle', 'midlight', 'miterjoin', 'native', 'no-role', 'none', 'normal', 'nowrap', 'oblique', 'off', 'on', 'outset',
    'overline', 'placeholder-text', 'pre', 'pre-line', 'pre-wrap', 'ridge', 'right', 'roundcap', 'roundjoin', 'selected',
    'shadow', 'small-caps', 'solid', 'square', 'squarecap', 'sub', 'super', 'svgmiterjoin', 'text', 'tooltip-base',
    'tooltip-text', 'top', 'transparent', 'underline', 'upper-alpha', 'upper-roman', 'uppercase', 'wave', 'window',
    'window-text', 'x-large', 'xx-large'])

// isInheritable (qcssparser.cpp)
const INHERITABLE = new Set(['font', 'font-kerning', 'font-family', 'font-size', 'font-style', 'font-weight', 'text-indent',
    'white-space', 'list-style-type', 'list-style', 'text-align', 'font-variant', 'text-transform', 'line-height',
    'letter-spacing', 'word-spacing'])

// the colors of the palette roles a palette(<role>) value names (QPalette of the Basic style, light)
const PALETTE = {
    'window-text': '#000000', 'button': '#efefef', 'light': '#ffffff', 'midlight': '#cacaca', 'dark': '#9f9f9f',
    'mid': '#b8b8b8', 'text': '#000000', 'bright-text': '#ffffff', 'button-text': '#000000', 'base': '#ffffff',
    'window': '#efefef', 'shadow': '#767676', 'highlight': '#308cc6', 'highlighted-text': '#ffffff', 'link': '#0000ff',
    'link-visited': '#ff00ff', 'alternate-base': '#f7f7f7', 'tooltip-base': '#ffffdc', 'tooltip-text': '#000000',
    'placeholder-text': '#000000', 'accent': '#308cc6',
}

// ---- the scanner (qcssscanner.cpp) ----

const isNameStart = (c) => c !== undefined && /[A-Za-z_\u0080-￿]/.test(c)
const isNameChar = (c) => c !== undefined && /[A-Za-z0-9_\-\u0080-￿]/.test(c)
const isDigit = (c) => c !== undefined && c >= '0' && c <= '9'
const isSpace = (c) => c === ' ' || c === '\t' || c === '\n' || c === '\r' || c === '\f'

// tokens: { t: type, v: text }; the types: S, IDENT, STRING, HASH, NUMBER, PERCENTAGE, LENGTH, FUNCTION, ATKEYWORD,
// and the characters themselves ('{', '}', ':', ';', ',', '/', '+', '-', '>', '*', '.', '[', ']', '=', '~', '|', '!', '(', ')')
function scan(text){
    let tokens = []
    let i = 0, n = text.length
    let readName = () => {
        let s = ''
        while(i < n){
            let c = text[i]
            if(c === '\\' && i + 1 < n){
                // an escape: a hexadecimal code point or the character itself
                let m = /^[0-9a-fA-F]{1,6}\s?/.exec(text.slice(i + 1, i + 8))
                if(m){
                    s += String.fromCodePoint(parseInt(m[0], 16))
                    i += 1 + m[0].length
                } else {
                    s += text[i + 1]
                    i += 2
                }
            } else if(isNameChar(c)){
                s += c
                i++
            } else break
        }
        return s
    }
    while(i < n){
        let c = text[i]
        if(isSpace(c)){
            while(i < n && isSpace(text[i])) i++
            tokens.push({ t: 'S', v: ' ' })
            continue
        }
        if(c === '/' && text[i + 1] === '*'){
            let end = text.indexOf('*/', i + 2)
            i = end < 0 ? n : end + 2
            continue
        }
        if(c === '"' || c === "'"){
            let quote = c
            let s = ''
            i++
            while(i < n && text[i] !== quote){
                if(text[i] === '\\' && i + 1 < n){
                    if(text[i + 1] === '\n'){ i += 2; continue }
                    let m = /^[0-9a-fA-F]{1,6}\s?/.exec(text.slice(i + 1, i + 8))
                    if(m){
                        s += String.fromCodePoint(parseInt(m[0], 16))
                        i += 1 + m[0].length
                        continue
                    }
                    s += text[i + 1]
                    i += 2
                    continue
                }
                s += text[i++]
            }
            i++
            tokens.push({ t: 'STRING', v: s })
            continue
        }
        if(c === '#' && isNameChar(text[i + 1])){
            i++
            tokens.push({ t: 'HASH', v: '#' + readName() })
            continue
        }
        if(isDigit(c) || (c === '.' && isDigit(text[i + 1]))){
            let start = i
            while(i < n && isDigit(text[i])) i++
            if(text[i] === '.' && isDigit(text[i + 1])){
                i++
                while(i < n && isDigit(text[i])) i++
            }
            let number = text.slice(start, i)
            if(text[i] === '%'){
                i++
                tokens.push({ t: 'PERCENTAGE', v: number + '%' })
            } else if(isNameStart(text[i]) || (text[i] === '-' && isNameStart(text[i + 1]))){
                tokens.push({ t: 'LENGTH', v: number + readName() })
            } else tokens.push({ t: 'NUMBER', v: number })
            continue
        }
        if(isNameStart(c) || (c === '-' && (isNameStart(text[i + 1]) || text[i + 1] === '-')) || c === '\\'){
            let name = readName()
            if(text[i] === '('){
                i++
                tokens.push({ t: 'FUNCTION', v: name + '(' })
            } else tokens.push({ t: 'IDENT', v: name })
            continue
        }
        if(c === '@' && isNameStart(text[i + 1])){
            i++
            tokens.push({ t: 'ATKEYWORD', v: '@' + readName() })
            continue
        }
        tokens.push({ t: c, v: c })
        i++
    }
    return tokens
}

// ---- the parser (QCss::Parser) ----

function Parser(text){
    this.tokens = scan(text)
    this.index = 0
}

Parser.prototype = {
    peek(){ return this.tokens[this.index] },
    test(type){
        let token = this.tokens[this.index]
        if(token && token.t === type){
            this.index++
            return true
        }
        return false
    },
    skipSpace(){ while(this.test('S')){} },
    lexem(){ return this.tokens[this.index - 1].v },
    // skips to after the token type, nested blocks included
    until(type){
        let depth = 0
        while(this.index < this.tokens.length){
            let t = this.tokens[this.index++].t
            if(depth === 0 && t === type) return true
            if(t === '{' || t === '(' || t === '[') depth++
            else if(t === '}' || t === ')' || t === ']'){
                if(depth === 0){
                    if(t === type) return true
                    this.index--
                    return false
                }
                depth--
            }
        }
        return false
    },
    // a value term (parseTerm), null when it is none
    parseTerm(){
        let token = this.peek()
        if(!token) return null
        let sign = ''
        if(token.t === '+' || token.t === '-'){
            let next = this.tokens[this.index + 1]
            if(!next || !(next.t === 'NUMBER' || next.t === 'PERCENTAGE' || next.t === 'LENGTH')) return null
            sign = token.t === '-' ? '-' : ''
            this.index++
            token = this.peek()
        }
        let value = null
        switch(token.t){
            case 'NUMBER': value = { type: 'Number', variant: Number(sign + token.v) }; break
            case 'PERCENTAGE': value = { type: 'Percentage', variant: sign + token.v.slice(0, -1) }; break
            case 'LENGTH': value = { type: 'Length', variant: sign + token.v }; break
            case 'STRING': value = { type: 'String', variant: token.v }; break
            case 'IDENT': {
                let lower = token.v.toLowerCase()
                value = KNOWN_VALUES.has(lower) ? { type: 'KnownIdentifier', variant: lower } : { type: 'Identifier', variant: token.v }
                break
            }
            case 'HASH': {
                let color = parseColor(token.v)
                if(!color) return undefined
                value = { type: 'Color', variant: colorName(color) }
                break
            }
            case 'FUNCTION': {
                this.index++
                let name = token.v.slice(0, -1)
                let start = this.index
                this.skipSpace()
                let argsStart = this.index
                this.index = start
                if(!this.until(')')) return undefined
                let args = this.tokens.slice(argsStart, this.index - 1).map(t => t.t === 'STRING' ? '"' + t.v + '"' : t.v).join('')
                this.skipSpace()
                if(name.toLowerCase() === 'url') return { type: 'Uri', variant: args.trim().replace(/^(['"])(.*)\1$/, '$2') }
                return { type: 'Function', variant: [name, args] }
            }
            default: return null
        }
        this.index++
        this.skipSpace()
        return value
    },
    // values of a declaration (parseExpr) up to ';', '}' or '!important'
    parseExpr(){
        let values = []
        for(;;){
            let token = this.peek()
            if(!token || token.t === ';' || token.t === '}' || token.t === '!') break
            if(token.t === ','){
                this.index++
                this.skipSpace()
                values.push({ type: 'TermOperatorComma', variant: ',' })
                continue
            }
            if(token.t === '/'){
                this.index++
                this.skipSpace()
                values.push({ type: 'TermOperatorSlash', variant: '/' })
                continue
            }
            let term = this.parseTerm()
            if(!term) return null
            values.push(term)
        }
        return values
    },
    // the declarations up to '}' or the end; one that does not parse is skipped to its ';'
    parseDeclarations(){
        let declarations = []
        for(;;){
            this.skipSpace()
            let token = this.peek()
            if(!token || token.t === '}') break
            if(token.t === ';'){
                this.index++
                continue
            }
            if(token.t !== 'IDENT'){
                this.until(';')
                continue
            }
            this.index++
            let property = token.v.toLowerCase()
            this.skipSpace()
            if(!this.test(':')){
                this.until(';')
                continue
            }
            this.skipSpace()
            let values = this.parseExpr()
            if(values === null){
                this.until(';')
                continue
            }
            let important = false
            if(this.test('!')){
                this.skipSpace()
                if(this.test('IDENT') && this.lexem().toLowerCase() === 'important') important = true
                this.skipSpace()
            }
            if(values.length) declarations.push(makeDeclaration(property, values, important))
        }
        return declarations
    },
    // a simple selector: element name and the ids, classes, attributes and pseudos that follow it
    parseBasicSelector(){
        let basic = { element: '', ids: [], attributes: [], pseudos: [], relation: 'none' }
        let any = false
        if(this.test('IDENT')){
            basic.element = this.lexem().toLowerCase()
            any = true
        } else if(this.test('*')){
            basic.element = '*'
            any = true
        }
        for(;;){
            if(this.test('HASH')){
                basic.ids.push(this.lexem().slice(1))
                any = true
            } else if(this.test('.')){
                if(!this.test('IDENT')) return null
                basic.attributes.push({ name: 'class', value: this.lexem(), op: 'includes' })
                any = true
            } else if(this.test('[')){
                this.skipSpace()
                if(!this.test('IDENT')) return null
                let attribute = { name: this.lexem().toLowerCase(), value: '', op: 'set' }
                this.skipSpace()
                if(this.test('=')) attribute.op = 'equal'
                else if(this.test('~') && this.test('=')) attribute.op = 'includes'
                else if(this.test('|') && this.test('=')) attribute.op = 'dash'
                if(attribute.op !== 'set'){
                    this.skipSpace()
                    if(!this.test('IDENT') && !this.test('STRING')) return null
                    attribute.value = this.lexem()
                    this.skipSpace()
                }
                if(!this.test(']')) return null
                basic.attributes.push(attribute)
                any = true
            } else if(this.test(':')){
                let negated = this.test('!')
                if(this.test('IDENT')) basic.pseudos.push({ name: this.lexem().toLowerCase(), negated })
                else if(this.test('FUNCTION')){
                    this.until(')')
                    basic.pseudos.push({ name: '?', negated })
                } else return null
                any = true
            } else break
        }
        return any ? basic : null
    },
    // a selector: basic selectors joined by a descendant (' '), child ('>') or sibling ('+') relation
    parseSelector(){
        let basics = []
        let relation = 'none'
        for(;;){
            let basic = this.parseBasicSelector()
            if(!basic) return null
            basic.relation = relation
            basics.push(basic)
            let sawSpace = false
            while(this.test('S')) sawSpace = true
            let token = this.peek()
            if(!token || token.t === ',' || token.t === '{') break
            if(token.t === '>' || token.t === '+'){
                relation = token.t === '>' ? 'child' : 'adjacent'
                this.index++
                this.skipSpace()
            } else if(sawSpace) relation = 'descendant'
            else return null
        }
        return basics
    },
    // the rules of a style sheet; @import, @media and other at-rules are skipped (none loads anything here)
    parseStyleSheet(){
        let rules = []
        let order = 0
        while(this.index < this.tokens.length){
            this.skipSpace()
            let token = this.peek()
            if(!token) break
            if(token.t === 'ATKEYWORD'){
                this.index++
                let name = token.v.toLowerCase()
                if(name === '@media'){
                    // @media screen { rules }: the rules of the screen medium
                    let media = []
                    while(this.peek() && this.peek().t !== '{'){
                        let t = this.tokens[this.index++]
                        if(t.t === 'IDENT') media.push(t.v.toLowerCase())
                    }
                    if(!this.test('{')) break
                    let start = this.index
                    this.until('}')
                    if(!media.length || media.includes('screen') || media.includes('all')){
                        let inner = new Parser('')
                        inner.tokens = this.tokens.slice(start, this.index - 1)
                        for(let rule of inner.parseStyleSheet()) rules.push(Object.assign(rule, { order: order++ }))
                    }
                } else {
                    while(this.peek() && this.peek().t !== ';' && this.peek().t !== '{') this.index++
                    if(this.test('{')) this.until('}')
                    else this.index++
                }
                continue
            }
            let selectors = []
            let failed = false
            for(;;){
                let selector = this.parseSelector()
                if(!selector){
                    failed = true
                    break
                }
                selectors.push(selector)
                this.skipSpace()
                if(!this.test(',')) break
                this.skipSpace()
            }
            if(failed || !this.test('{')){
                while(this.peek() && this.peek().t !== '{' && this.peek().t !== '}') this.index++
                if(this.test('{')) this.until('}')
                else this.index++
                continue
            }
            let declarations = this.parseDeclarations()
            this.test('}')
            rules.push({ selectors, declarations, order: order++ })
        }
        return rules
    },
}

function makeDeclaration(property, values, important){
    return { property, known: PROPERTIES.has(property), values, important, inheritable: INHERITABLE.has(property) }
}

// the declarations of a style attribute (QTextHtmlParserNode::parseStyleAttribute parses "* {value}")
function parseDeclarations(text){
    return new Parser(text).parseDeclarations()
}

function parseStyleSheet(text){
    return new Parser(text).parseStyleSheet()
}

// ---- matching (QCss::StyleSelector) ----

// the specificity of a selector: ids, attributes and pseudos, element names (QCss::Selector::specificity)
function specificity(selector){
    let a = 0, b = 0, c = 0
    for(let basic of selector){
        if(basic.element && basic.element !== '*') c++
        a += basic.ids.length
        b += basic.attributes.length + basic.pseudos.length
    }
    return a * 0x100 * 0x100 + b * 0x100 + c
}

// adapter: { name(node), attribute(node, name), parent(node), previousSibling(node), isNull(node) }
function basicMatches(basic, node, adapter, extraPseudo){
    if(basic.element && basic.element !== '*' && adapter.name(node) !== basic.element) return false
    for(let id of basic.ids) if(adapter.attribute(node, 'id') !== id) return false
    for(let a of basic.attributes){
        let value = adapter.attribute(node, a.name)
        if(value === null) return false
        if(a.op === 'equal' && value !== a.value) return false
        if(a.op === 'includes' && !value.split(/\s+/).includes(a.value)) return false
        if(a.op === 'dash' && !(value === a.value || value.startsWith(a.value + '-'))) return false
    }
    for(let p of basic.pseudos){
        let has = p.name === extraPseudo
        if(has === p.negated) return false
    }
    return true
}

function selectorMatches(selector, at, node, adapter, extraPseudo){
    let basic = selector[at]
    if(!basicMatches(basic, node, adapter, at === selector.length - 1 ? extraPseudo : null)) return false
    if(at === 0) return true
    if(basic.relation === 'child'){
        let parent = adapter.parent(node)
        return !adapter.isNull(parent) && selectorMatches(selector, at - 1, parent, adapter, null)
    }
    if(basic.relation === 'adjacent'){
        let sibling = adapter.previousSibling(node)
        return !adapter.isNull(sibling) && selectorMatches(selector, at - 1, sibling, adapter, null)
    }
    // descendant
    for(let parent = adapter.parent(node); !adapter.isNull(parent); parent = adapter.parent(parent)){
        if(selectorMatches(selector, at - 1, parent, adapter, null)) return true
    }
    return false
}

// the declarations of the rules of the style sheets that match the node, the more specific later (StyleSelector::
// declarationsForNode); a rule with a pseudo other than the extra one (":link" of an <a href>) does not match
function declarationsForNode(sheets, node, adapter, extraPseudo){
    let matched = []
    let sheetIndex = 0
    for(let sheet of sheets){
        for(let rule of sheet){
            let best = -1
            for(let selector of rule.selectors){
                if(selectorMatches(selector, selector.length - 1, node, adapter, extraPseudo)) best = Math.max(best, specificity(selector))
            }
            if(best >= 0) matched.push({ weight: best, sheet: sheetIndex, order: rule.order, declarations: rule.declarations })
        }
        sheetIndex++
    }
    matched.sort((x, y) => x.weight - y.weight || x.sheet - y.sheet || x.order - y.order)
    let declarations = []
    for(let m of matched) for(let d of m.declarations) declarations.push(d)
    // !important ones last
    return declarations.filter(d => !d.important).concat(declarations.filter(d => d.important))
}

// ---- values (ValueExtractor, Declaration) ----

// a color of a value (parseColorValue): Qt's color name, or null
function colorOf(value){
    if(!value) return null
    if(value.type === 'Identifier' || value.type === 'String'){
        let c = parseColor(String(value.variant))
        return c ? colorName(c) : 'invalid'
    }
    if(value.type === 'Color') return value.variant
    if(value.type === 'KnownIdentifier'){
        if(value.variant === 'transparent') return '#00000000'
        // a known name that is also a color name (none is, except via the palette function)
        let c = parseColor(value.variant)
        return c ? colorName(c) : 'invalid'
    }
    if(value.type !== 'Function') return 'invalid'
    let [name, args] = value.variant
    name = name.toLowerCase()
    if(name === 'palette'){
        let role = args.trim().toLowerCase()
        return PALETTE[role] || 'invalid'
    }
    let rgb = name.startsWith('rgb'), hsv = !rgb && name.startsWith('hsv'), hsl = !rgb && !hsv && name.startsWith('hsl')
    if(!rgb && !hsv && !hsl) return 'invalid'
    let hasAlpha = name.length === 4 && name[3] === 'a'
    if(name.length > 3 && !hasAlpha) return 'invalid'
    let parts = args.split(',').map(s => s.trim())
    if(parts.length !== (hasAlpha ? 4 : 3)) return 'invalid'
    let nums = []
    for(let i = 0; i < parts.length; i++){
        let p = parts[i]
        let percent = p.endsWith('%')
        let v = Number(percent ? p.slice(0, -1) : p)
        if(p === '' || Number.isNaN(v)) return 'invalid'
        if(percent && i < 3) v = v * ((rgb || i !== 0) ? 255 : 359) / 100
        nums.push(v)
    }
    let toInt = (v) => Math.round(v)
    let alpha = 255
    if(hasAlpha){
        let a = nums[3]
        alpha = Math.trunc(a) <= 1 ? Math.trunc(a * 255) : Math.trunc(a)
    }
    let r, g, b
    if(rgb){
        r = toInt(nums[0]); g = toInt(nums[1]); b = toInt(nums[2])
    } else {
        let h = toInt(nums[0]), s = toInt(nums[1]) / 255, l = toInt(nums[2]) / 255
        let rgbOf = hsl ? hslToRgb : hsvToRgb
        ;[r, g, b] = rgbOf(h, s, l)
    }
    let clamp = (v) => Math.max(0, Math.min(255, v))
    let hex = (v) => clamp(v).toString(16).padStart(2, '0')
    return (alpha >= 255 ? '#' : '#' + hex(alpha)) + hex(r) + hex(g) + hex(b)
}

function hsvToRgb(h, s, v){
    let c = v * s, hh = ((h % 360) + 360) % 360 / 60, x = c * (1 - Math.abs(hh % 2 - 1)), m = v - c
    let [r, g, b] = hh < 1 ? [c, x, 0] : hh < 2 ? [x, c, 0] : hh < 3 ? [0, c, x] : hh < 4 ? [0, x, c] : hh < 5 ? [x, 0, c] : [c, 0, x]
    return [Math.round((r + m) * 255), Math.round((g + m) * 255), Math.round((b + m) * 255)]
}

function hslToRgb(h, s, l){
    let c = (1 - Math.abs(2 * l - 1)) * s, hh = ((h % 360) + 360) % 360 / 60, x = c * (1 - Math.abs(hh % 2 - 1)), m = l - c / 2
    let [r, g, b] = hh < 1 ? [c, x, 0] : hh < 2 ? [x, c, 0] : hh < 3 ? [0, c, x] : hh < 4 ? [0, x, c] : hh < 5 ? [x, 0, c] : [c, 0, x]
    return [Math.round((r + m) * 255), Math.round((g + m) * 255), Math.round((b + m) * 255)]
}

// Declaration::colorValue: one value
function declarationColor(decl){
    if(decl.values.length !== 1) return 'invalid'
    return colorOf(decl.values[0])
}

// Declaration::brushValue: a solid brush of the color, null when none (gradients are not kept)
function declarationBrush(decl){
    if(decl.values.length !== 1) return null
    let color = colorOf(decl.values[0])
    return color && color !== 'invalid' ? color : null
}

// LengthData of a value: number and unit (px, ex, em, % or none)
function lengthData(value){
    let s = String(value.variant)
    let unit = 'none'
    let lower = s.toLowerCase()
    if(lower.endsWith('px')) unit = 'px'
    else if(lower.endsWith('ex')) unit = 'ex'
    else if(lower.endsWith('em')) unit = 'em'
    if(unit !== 'none') s = s.slice(0, -2)
    else if(value.type === 'Percentage') unit = '%'
    let number = parseFloat(s)
    return { number: Number.isNaN(number) ? 0 : number, unit }
}

// lengthValueFromData: em is the height of the font of the declarations, ex its x-height
function lengthFromData(data, font){
    let size = font && font.pixelSize > 0 ? font.pixelSize : font && font.pointSize > 0 ? font.pointSize * 96 / 72 : 16
    let scale = data.unit === 'ex' ? Math.round(size * 0.5) : data.unit === 'em' ? Math.ceil(size * 1.15) : 1
    let v = scale * data.number
    return v >= 0 ? Math.floor(v + 0.5) : -Math.floor(-v + 0.5)
}

// ValueExtractor::lengthValues: one to four lengths for the four edges (top, right, bottom, left)
function lengthValues(decl, font){
    let datas = []
    for(let i = 0; i < Math.min(decl.values.length, 4); i++) datas.push(lengthData(decl.values[i]))
    if(datas.length === 0) datas = [{ number: 0, unit: 'none' }]
    if(datas.length === 1) datas = [datas[0], datas[0], datas[0], datas[0]]
    else if(datas.length === 2) datas = [datas[0], datas[1], datas[0], datas[1]]
    else if(datas.length === 3) datas = [datas[0], datas[1], datas[2], datas[1]]
    return datas.map(d => lengthFromData(d, font))
}

const TOP = 0, RIGHT = 1, BOTTOM = 2, LEFT = 3

// ValueExtractor::extractFont: the font properties the declarations set (only those), and the size adjustment (-255: none)
function extractFont(declarations){
    let font = {}
    let adjustment = -255
    let setSize = (value) => {
        if(value.type === 'KnownIdentifier'){
            let map = { 'small': -1, 'medium': 0, 'large': 1, 'x-large': 2, 'xx-large': 3 }
            if(value.variant in map){
                adjustment = map[value.variant]
                return true
            }
            return false
        }
        if(value.type !== 'Length') return false
        let s = String(value.variant).toLowerCase()
        if(s.endsWith('pt')){
            let v = parseFloat(s.slice(0, -2))
            if(!Number.isNaN(v)){
                font.pointSize = Math.max(0, Math.min(v, (1 << 24) - 1))
                delete font.pixelSize
                return true
            }
        } else if(s.endsWith('px')){
            let v = parseFloat(s.slice(0, -2))
            if(!Number.isNaN(v)){
                font.pixelSize = Math.max(0, Math.min(Math.round(v), (1 << 24) - 1))
                delete font.pointSize
                return true
            }
        }
        return false
    }
    let setStyle = (value) => {
        if(value.type !== 'KnownIdentifier') return false
        if(value.variant === 'normal'){ font.italic = false; return true }
        if(value.variant === 'italic' || value.variant === 'oblique'){ font.italic = true; return true }
        return false
    }
    let setWeight = (value) => {
        if(value.type === 'KnownIdentifier'){
            if(value.variant === 'normal'){ font.weight = 400; return true }
            if(value.variant === 'bold'){ font.weight = 700; return true }
            return false
        }
        if(value.type !== 'Number') return false
        font.weight = Math.round(Math.max(0, Math.min(value.variant, 1001)))
        return true
    }
    let setFamilies = (values, start = 0) => {
        let families = []
        let family = ''
        let space = false
        for(let i = start; i < values.length; i++){
            let v = values[i]
            if(v.type === 'TermOperatorComma'){
                families.push(family)
                family = ''
                space = false
                continue
            }
            let str = String(v.variant)
            if(!str) break
            if(space) family += ' '
            family += str
            space = true
        }
        if(family) families.push(family)
        if(!families.length) return false
        font.families = families
        return true
    }
    for(let decl of declarations){
        if(!decl.values.length) continue
        let val = decl.values[0]
        switch(decl.property){
            case 'font-size': setSize(val); break
            case 'font-style': setStyle(val); break
            case 'font-weight': setWeight(val); break
            case 'font-family': setFamilies(decl.values); break
            case 'font-kerning':
                if(val.type === 'KnownIdentifier' && (val.variant === 'normal' || val.variant === 'none')) font.kerning = val.variant === 'normal'
                break
            case 'text-decoration':
                for(let v of decl.values){
                    if(v.type !== 'KnownIdentifier') continue
                    if(v.variant === 'underline') font.underline = true
                    else if(v.variant === 'overline') font.overline = true
                    else if(v.variant === 'line-through') font.strikeOut = true
                    else if(v.variant === 'none'){
                        font.underline = false
                        font.overline = false
                        font.strikeOut = false
                    }
                }
                break
            case 'font': {
                font.italic = false
                font.weight = 400
                adjustment = -255
                let i = 0
                while(i < decl.values.length && (setStyle(decl.values[i]) || setWeight(decl.values[i]))) i++
                if(i < decl.values.length) setSize(decl.values[i++])
                if(i < decl.values.length) setFamilies(decl.values, i)
                break
            }
            case 'font-variant':
                if(val.type === 'KnownIdentifier'){
                    if(val.variant === 'normal') font.capitalization = 0
                    else if(val.variant === 'small-caps') font.capitalization = 3
                }
                break
            case 'text-transform':
                if(val.type === 'KnownIdentifier'){
                    if(val.variant === 'none') font.capitalization = 0
                    else if(val.variant === 'uppercase') font.capitalization = 1
                    else if(val.variant === 'lowercase') font.capitalization = 2
                }
                break
            case 'letter-spacing': {
                let s = String(val.variant).toLowerCase()
                if(s.endsWith('em')){
                    let v = Number(s.slice(0, -2))
                    if(!Number.isNaN(v)) font.letterSpacing = { type: 'percent', value: (v + 1) * 100 }
                } else if(s.endsWith('px')){
                    let v = Number(s.slice(0, -2))
                    if(!Number.isNaN(v)) font.letterSpacing = { type: 'absolute', value: v }
                }
                break
            }
            case 'word-spacing': {
                let s = String(val.variant).toLowerCase()
                if(s.endsWith('px')){
                    let v = Number(s.slice(0, -2))
                    if(!Number.isNaN(v)) font.wordSpacing = v
                }
                break
            }
            default: continue
        }
    }
    return { font, adjustment }
}

// ValueExtractor::extractBox: margins and paddings (four edges each; an edge not set stays as it is)
function extractBox(declarations, margins, paddings){
    let { font } = extractFont(declarations)
    for(let decl of declarations){
        switch(decl.property){
            case 'padding-left': paddings[LEFT] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'padding-right': paddings[RIGHT] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'padding-top': paddings[TOP] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'padding-bottom': paddings[BOTTOM] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'padding': lengthValues(decl, font).forEach((v, i) => { paddings[i] = v }); break
            case 'margin-left': margins[LEFT] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'margin-right': margins[RIGHT] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'margin-top': margins[TOP] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'margin-bottom': margins[BOTTOM] = lengthFromData(lengthData(decl.values[0] || { variant: '0' }), font); break
            case 'margin': lengthValues(decl, font).forEach((v, i) => { margins[i] = v }); break
        }
    }
}

const BORDER_STYLES = ['none', 'dotted', 'dashed', 'solid', 'double', 'dot-dash', 'dot-dot-dash', 'groove', 'ridge', 'inset', 'outset']

// parseStyleValue: a border style name, or null
function borderStyleOf(value){
    if(!value || value.type !== 'KnownIdentifier') return null
    let index = BORDER_STYLES.indexOf(value.variant)
    return index < 0 ? (value.variant === 'native' ? 'native' : null) : value.variant
}

// ValueExtractor::borderValue: width, style and color of a "border" shorthand
function borderValue(decl, font){
    let result = { width: null, style: null, color: null }
    let values = decl.values
    let i = 0
    if(i < values.length && (values[i].type === 'Length' || values[i].type === 'Number')){
        result.width = lengthFromData(lengthData(values[i]), font)
        i++
    }
    if(i < values.length){
        let style = borderStyleOf(values[i])
        if(style){
            result.style = style
            i++
        }
    }
    if(i < values.length){
        let color = colorOf(values[i])
        if(color && color !== 'invalid') result.color = color
    }
    return result
}

// ValueExtractor::extractBorder: widths, colors and styles of the four edges; whether a border declaration was there
function extractBorder(declarations, widths, colors, styles){
    let { font } = extractFont(declarations)
    let hit = false
    let edges = { top: TOP, right: RIGHT, bottom: BOTTOM, left: LEFT }
    let four = (values, map) => {
        let list = values.slice(0, 4).map(map)
        if(list.length === 1) return [list[0], list[0], list[0], list[0]]
        if(list.length === 2) return [list[0], list[1], list[0], list[1]]
        if(list.length === 3) return [list[0], list[1], list[2], list[1]]
        return list
    }
    for(let decl of declarations){
        let p = decl.property
        let m = /^border-(top|right|bottom|left)(?:-(width|style|color))?$/.exec(p)
        if(m){
            let edge = edges[m[1]]
            if(!m[2]){
                let b = borderValue(decl, font)
                if(b.width !== null) widths[edge] = b.width
                if(b.style !== null) styles[edge] = b.style
                if(b.color !== null) colors[edge] = b.color
            } else if(m[2] === 'width') widths[edge] = lengthFromData(lengthData(decl.values[0]), font)
            else if(m[2] === 'style'){
                let s = borderStyleOf(decl.values[0])
                if(s) styles[edge] = s
            } else {
                let c = colorOf(decl.values[0])
                if(c && c !== 'invalid') colors[edge] = c
            }
            hit = true
            continue
        }
        if(p === 'border-width'){
            lengthValues(decl, font).forEach((v, i) => { widths[i] = v })
            hit = true
        } else if(p === 'border-style'){
            four(decl.values, borderStyleOf).forEach((v, i) => { if(v) styles[i] = v })
            hit = true
        } else if(p === 'border-color'){
            four(decl.values, colorOf).forEach((v, i) => { if(v && v !== 'invalid') colors[i] = v })
            hit = true
        } else if(p === 'border'){
            let b = borderValue(decl, font)
            for(let i = 0; i < 4; i++){
                if(b.width !== null) widths[i] = b.width
                if(b.style !== null) styles[i] = b.style
                if(b.color !== null) colors[i] = b.color
            }
            hit = true
        }
    }
    return hit
}

// ValueExtractor::extractBackground: the solid color and the image url of background / background-color /
// background-image
function extractBackground(declarations){
    let result = { brush: null, image: '' }
    for(let decl of declarations){
        if(!decl.values.length) continue
        let val = decl.values[0]
        if(decl.property === 'background-color') result.brush = declarationBrush(decl)
        else if(decl.property === 'background-image'){
            if(val.type === 'Uri') result.image = val.variant
        } else if(decl.property === 'background'){
            // parseShorthandBackgroundProperty: an url is the image, a color the brush
            result.brush = null
            for(let v of decl.values){
                if(v.type === 'Uri') result.image = v.variant
                else if(v.type === 'KnownIdentifier' && v.variant !== 'transparent') continue
                else {
                    let c = colorOf(v)
                    if(c && c !== 'invalid') result.brush = c
                }
            }
        }
    }
    return result
}

// Declaration::realValue / intValue with a unit (or none)
function realValue(decl, unit){
    if(decl.values.length !== 1) return null
    let v = decl.values[0]
    if(unit && v.type !== 'Length') return null
    let s = String(v.variant)
    if(unit){
        if(!s.toLowerCase().endsWith(unit)) return null
        s = s.slice(0, -unit.length)
    }
    if(s.trim() === '') return null
    let n = Number(s)
    return Number.isNaN(n) ? null : n
}

function intValue(decl, unit){
    let n = realValue(decl, unit)
    return n === null || !Number.isInteger(n) ? null : n
}

module.exports = {
    parseDeclarations, parseStyleSheet, declarationsForNode, makeDeclaration,
    colorOf, declarationColor, declarationBrush, lengthData, lengthFromData, lengthValues,
    extractFont, extractBox, extractBorder, extractBackground, borderStyleOf, realValue, intValue,
    TOP, RIGHT, BOTTOM, LEFT, PALETTE,
}
