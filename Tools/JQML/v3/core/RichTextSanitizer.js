// Rich text of a Text item is rebuilt from the subset of HTML that Qt supports, so the page never gets markup it could run.
// Only allowed tags and attributes are written back, with escaped values; everything else is dropped or shown as text.

// Tags of the Qt rich text subset (doc.qt.io, "Supported HTML Subset")
const allowedTags = new Set([
    'a', 'address', 'b', 'big', 'blockquote', 'br', 'center', 'cite', 'code', 'dd', 'dfn', 'div', 'dl', 'dt', 'em', 'font',
    'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'hr', 'i', 'img', 'kbd', 'li', 'nobr', 'ol', 'p', 'pre', 's', 'samp', 'small',
    'span', 'strong', 'sub', 'sup', 'table', 'tbody', 'td', 'tfoot', 'th', 'thead', 'tr', 'tt', 'u', 'ul', 'var',
])

const voidTags = new Set(['br', 'hr', 'img'])

// Tags whose content is no text to show either
const droppedWithContent = new Set([
    'script', 'style', 'iframe', 'object', 'embed', 'applet', 'noscript', 'noembed', 'noframes', 'template', 'textarea',
    'title', 'head', 'svg', 'math', 'select', 'xmp', 'plaintext', 'frameset', 'audio', 'video', 'canvas',
])

const commonAttributes = ['style', 'align', 'dir', 'title']
const tagAttributes = {
    a: ['href', 'name'],
    font: ['color', 'size', 'face'],
    img: ['src', 'width', 'height', 'alt'],
    table: ['border', 'bgcolor', 'cellspacing', 'cellpadding', 'width', 'height'],
    tr: ['bgcolor', 'valign'],
    td: ['bgcolor', 'width', 'height', 'colspan', 'rowspan', 'valign', 'nowrap'],
    th: ['bgcolor', 'width', 'height', 'colspan', 'rowspan', 'valign', 'nowrap'],
    ol: ['type', 'start'],
    ul: ['type'],
    pre: ['width'],
}

const allowedStyleProperties = new Set([
    'color', 'background-color', 'font-family', 'font-size', 'font-style', 'font-weight', 'font-variant', 'text-decoration',
    'text-align', 'text-indent', 'text-transform', 'vertical-align', 'white-space', 'line-height', 'letter-spacing',
    'word-spacing', 'margin', 'margin-top', 'margin-bottom', 'margin-left', 'margin-right', 'padding', 'padding-top',
    'padding-bottom', 'padding-left', 'padding-right', 'border-width', 'border-style', 'border-color',
])

const urlAttributes = new Set(['href', 'src'])
const allowedLinkSchemes = new Set(['http', 'https', 'mailto', 'qrc'])
const allowedImageDataTypes = /^data:image\/(png|gif|jpeg|jpg|webp|bmp);/

const namedEntities = {amp: '&', lt: '<', gt: '>', quot: '"', apos: "'", nbsp: ' '}

function escapeText(text){
    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
}

function escapeAttribute(value){
    return escapeText(value).replace(/"/g, '&quot;')
}

function decodeEntities(value){
    return value.replace(/&(#x[0-9a-f]+|#[0-9]+|[a-z]+);?/gi, (match, entity)=>{
        if(entity[0] === '#'){
            const code = entity[1] === 'x' || entity[1] === 'X' ? parseInt(entity.slice(2), 16) : parseInt(entity.slice(1), 10)
            return Number.isFinite(code) && code > 0 && code <= 0x10ffff ? String.fromCodePoint(code) : ''
        }

        const decoded = namedEntities[entity.toLowerCase()]
        return decoded !== undefined ? decoded : match
    })
}

// Text keeps its entities, a bare '&' becomes one
function sanitizeText(text){
    return text.replace(/&(?!(#x[0-9a-f]+|#[0-9]+|[a-z][a-z0-9]*);)/gi, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
}

function isAllowedUrl(attributeName, value){
    // Browsers ignore whitespace and control characters inside a scheme
    const compact = decodeEntities(value).replace(/[\u0000- \u007f-\u009f]/g, '').toLowerCase()
    const scheme = compact.match(/^([a-z][a-z0-9+.-]*):/)
    if(!scheme){
        return true
    }

    if(attributeName === 'src' && allowedImageDataTypes.test(compact)){
        return true
    }

    return allowedLinkSchemes.has(scheme[1])
}

function sanitizeStyle(value){
    const declarations = []
    for(const declaration of decodeEntities(value).split(';')){
        const separator = declaration.indexOf(':')
        if(separator < 0){
            continue
        }

        const property = declaration.slice(0, separator).trim().toLowerCase()
        const propertyValue = declaration.slice(separator + 1).trim()
        if(!allowedStyleProperties.has(property) || !propertyValue){
            continue
        }

        // No resource loads, script or escapes inside a value
        if(/url\s*\(|expression|javascript|vbscript|behavior|@import|\\|\/\*|[<>]/i.test(propertyValue)){
            continue
        }

        declarations.push(`${property}: ${propertyValue}`)
    }

    return declarations.join('; ')
}

function sanitizeAttributes(tagName, attributes){
    const allowed = (tagAttributes[tagName] || []).concat(commonAttributes)
    const written = new Set()
    let result = ''

    for(const attribute of attributes){
        if(written.has(attribute.name) || allowed.indexOf(attribute.name) < 0){
            continue
        }

        let value = attribute.value
        if(urlAttributes.has(attribute.name) && !isAllowedUrl(attribute.name, value)){
            continue
        }

        if(attribute.name === 'style'){
            value = sanitizeStyle(value)
            if(!value){
                continue
            }
        }
        else{
            value = decodeEntities(value)
        }

        written.add(attribute.name)
        result += ` ${attribute.name}="${escapeAttribute(value)}"`
    }

    return result
}

// Reads the tag starting at html[start] === '<'; null when it is no tag but text
function readTag(html, start){
    const match = /^<(\/?)([a-zA-Z][a-zA-Z0-9]*)/.exec(html.slice(start, start + 64))
    if(!match){
        return null
    }

    let index = start + match[0].length
    const attributes = []

    while(index < html.length){
        while(index < html.length && /[\s\/]/.test(html[index])){
            index++
        }

        if(index >= html.length){
            break
        }

        if(html[index] === '>'){
            return {isClosing: match[1] === '/', name: match[2].toLowerCase(), attributes, end: index + 1}
        }

        const nameMatch = /^[^\s\/>"'=]+/.exec(html.slice(index))
        if(!nameMatch){
            // A stray quote or '=' is skipped like a browser does
            index++
            continue
        }

        const attributeName = nameMatch[0].toLowerCase()
        index += nameMatch[0].length

        let lookahead = index
        while(lookahead < html.length && /\s/.test(html[lookahead])){
            lookahead++
        }

        let value = ''
        if(html[lookahead] === '='){
            index = lookahead + 1
            while(index < html.length && /\s/.test(html[index])){
                index++
            }

            const quote = html[index]
            if(quote === '"' || quote === "'"){
                const closing = html.indexOf(quote, index + 1)
                if(closing < 0){
                    return {isClosing: match[1] === '/', name: match[2].toLowerCase(), attributes, end: html.length}
                }

                value = html.slice(index + 1, closing)
                index = closing + 1
            }
            else{
                const unquoted = /^[^\s>]*/.exec(html.slice(index))[0]
                value = unquoted
                index += unquoted.length
            }
        }

        attributes.push({name: attributeName, value})
    }

    // An unterminated tag ends the input, as in a browser
    return {isClosing: match[1] === '/', name: match[2].toLowerCase(), attributes, end: html.length}
}

function skipPastClosingTag(html, start, tagName){
    const pattern = new RegExp(`</${tagName}[\\s/>]`, 'ig')
    pattern.lastIndex = start
    const match = pattern.exec(html)
    if(!match){
        return html.length
    }

    const close = html.indexOf('>', match.index)
    return close < 0 ? html.length : close + 1
}

function sanitize(html){
    if(html === undefined || html === null){
        return ''
    }

    html = String(html)
    let result = ''
    let index = 0

    while(index < html.length){
        const tagStart = html.indexOf('<', index)
        if(tagStart < 0){
            result += sanitizeText(html.slice(index))
            break
        }

        result += sanitizeText(html.slice(index, tagStart))

        // Comments, doctypes and processing instructions are no content
        if(html.startsWith('<!--', tagStart)){
            const end = html.indexOf('-->', tagStart + 4)
            index = end < 0 ? html.length : end + 3
            continue
        }

        if(html[tagStart + 1] === '!' || html[tagStart + 1] === '?'){
            const end = html.indexOf('>', tagStart)
            index = end < 0 ? html.length : end + 1
            continue
        }

        const tag = readTag(html, tagStart)
        if(!tag){
            result += '&lt;'
            index = tagStart + 1
            continue
        }

        index = tag.end

        if(droppedWithContent.has(tag.name)){
            if(!tag.isClosing){
                index = skipPastClosingTag(html, index, tag.name)
            }
            continue
        }

        if(!allowedTags.has(tag.name)){
            continue
        }

        if(tag.isClosing){
            if(!voidTags.has(tag.name)){
                result += `</${tag.name}>`
            }
            continue
        }

        result += `<${tag.name}${sanitizeAttributes(tag.name, tag.attributes)}>`
    }

    return result
}

module.exports = {
    sanitize,
}
