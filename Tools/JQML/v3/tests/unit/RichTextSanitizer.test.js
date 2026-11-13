// Run: node --test tests/unit
const test = require('node:test')
const assert = require('node:assert/strict')
const {sanitize} = require('../../core/RichTextSanitizer')

const allowedTags = new Set([
    'a', 'address', 'b', 'big', 'blockquote', 'br', 'center', 'cite', 'code', 'dd', 'dfn', 'div', 'dl', 'dt', 'em', 'font',
    'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'hr', 'i', 'img', 'kbd', 'li', 'nobr', 'ol', 'p', 'pre', 's', 'samp', 'small',
    'span', 'strong', 'sub', 'sup', 'table', 'tbody', 'td', 'tfoot', 'th', 'thead', 'tr', 'tt', 'u', 'ul', 'var',
])

// Every tag of the output is an allowed one with quoted, handler-free attributes
function assertSafe(output){
    const tagPattern = /<(\/?)([^\s>\/]*)([^>]*)>/g
    let match
    while((match = tagPattern.exec(output)) !== null){
        assert.ok(allowedTags.has(match[2]), `unexpected tag <${match[2]}> in ${output}`)
        const unquoted = match[3].replace(/="[^"]*"/g, '')
        const attributes = unquoted.trim().split(/\s+/).filter(Boolean)
        for(const attribute of attributes){
            assert.ok(!/^on/i.test(attribute), `event handler ${attribute} in ${output}`)
        }
        assert.ok(!/[='"]/.test(unquoted), `unquoted attribute in ${output}`)
    }
    assert.ok(!/javascript:/i.test(output), `script URL in ${output}`)
}

const attacks = [
    '<img src=x onerror=alert(1)>',
    '<b>x</b><img src="x" onerror="alert(1)">',
    '<IMG SRC=x ONERROR=alert(1)>',
    '<img src=x onerror=alert(1)',
    '<script>alert(1)</script>ok',
    '<scr<script>ipt>alert(1)</script>',
    '<a href="javascript:alert(1)">x</a>',
    '<a href=" JaVaScRiPt:alert(1)">x</a>',
    '<a href="jav&#x09;ascript:alert(1)">x</a>',
    '<a href="jav&#97;script:alert(1)">x</a>',
    '<a href="data:text/html,<script>alert(1)</script>">x</a>',
    '<svg onload=alert(1)><circle/></svg>t',
    '<math><mtext><img src=x onerror=alert(1)></mtext></math>',
    '<iframe src="https://example.com"></iframe>',
    '<details open ontoggle=alert(1)>y</details>',
    '<b title=\'a" onmouseover="alert(1)\'>t</b>',
    '<b onmouseover = "alert(1)" >t</b>',
    '<span style="background:url(javascript:alert(1))">t</span>',
    '<p style="color: red; behavior: url(x.htc)">t</p>',
    '<!--<img src=x onerror=alert(1)>-->t',
    '<style>*{color:red}</style>t',
    '<form action="https://evil"><input autofocus onfocus=alert(1)></form>',
    '<a/href="javascript:alert(1)">x</a>',
    '<img src="x"/onerror="alert(1)">',
]

for(const attack of attacks){
    test(`neutralizes ${attack}`, ()=>{
        assertSafe(sanitize(attack))
    })
}

test('keeps the Qt rich text formatting', ()=>{
    assert.equal(sanitize('<b>bold</b> <i>it</i><br><font color="red" size="3">r</font>'),
                 '<b>bold</b> <i>it</i><br><font color="red" size="3">r</font>')
    assert.equal(sanitize('<a href="https://imagingtools.de/x?a=1&amp;b=2">link</a>'),
                 '<a href="https://imagingtools.de/x?a=1&amp;b=2">link</a>')
    assert.equal(sanitize('<p align="center" style="color: #ff0000; font-weight: bold">t</p>'),
                 '<p align="center" style="color: #ff0000; font-weight: bold">t</p>')
    assert.equal(sanitize('<table border="1"><tr><td colspan="2">c</td></tr></table>'),
                 '<table border="1"><tr><td colspan="2">c</td></tr></table>')
})

test('drops markup that could run and keeps its harmless text', ()=>{
    assert.equal(sanitize('<b>x</b><img src="x" onerror="alert(1)">'), '<b>x</b><img src="x">')
    assert.equal(sanitize('<script>alert(1)</script>ok'), 'ok')
    assert.equal(sanitize('<a href="javascript:alert(1)">x</a>'), '<a>x</a>')
    assert.equal(sanitize('<details open ontoggle=alert(1)>y</details>'), 'y')
    assert.equal(sanitize('<svg onload=alert(1)><circle/></svg>t'), 't')
    assert.equal(sanitize('<span style="background:url(x); color: blue">t</span>'), '<span style="color: blue">t</span>')
})

test('shows text characters as text', ()=>{
    assert.equal(sanitize('a < b && c > d'), 'a &lt; b &amp;&amp; c &gt; d')
    assert.equal(sanitize('&lt;b&gt; stays text'), '&lt;b&gt; stays text')
    assert.equal(sanitize('5 <3'), '5 &lt;3')
    assert.equal(sanitize('line\rnext'), 'line\rnext')
})

test('allows only image data URLs', ()=>{
    assert.equal(sanitize('<img src="data:image/png;base64,AAAA">'), '<img src="data:image/png;base64,AAAA">')
    assert.equal(sanitize('<img src="data:image/svg+xml;base64,AAAA">'), '<img>')
    assert.equal(sanitize('<a href="data:image/png;base64,AAAA">x</a>'), '<a>x</a>')
})

test('treats a missing text as empty', ()=>{
    assert.equal(sanitize(undefined), '')
    assert.equal(sanitize(null), '')
    assert.equal(sanitize(42), '42')
})
