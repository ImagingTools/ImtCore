// Run: node --test tests/unit
// TextEdit with MarkdownText as QQuickTextEdit with a QTextDocument (qtbase 6.12): Markdown read into a document
// (md4c's events as QTextMarkdownImporter takes them) and written back (QTextMarkdownWriter), the raw text of the
// document, its HTML after textFormat became RichText (setHtml(toHtml())), and HTML written as Markdown
// (textFormat MarkdownText after RichText: setMarkdownText(toMarkdown())).
// The expected values are what Qt 6.12 gives (qml -platform offscreen, TextEdit with font.family Arial and
// font.pixelSize 12, one TextEdit for all the cases: the front matter of a document stays with it).
// Left out: <code> of HTML written as Markdown, which the offscreen platform does not see as a fixed pitch font (its
// fixed font "monospace" is no font there; a platform's fixed font, as the browser's monospace, is one).
const test = require('node:test')
const assert = require('node:assert/strict')
const { TextDocument } = require('../../Runtime/QtQuick/TextDocument/Document')
const { TextCursor } = require('../../Runtime/QtQuick/TextDocument/Cursor')
const RT = require('../../Runtime/QtQuick/TextDocument/RichText')
const F = require('../../Runtime/QtQuick/TextDocument/Formats')
const M = require('../../Runtime/QtQuick/TextDocument/Markdown')
const MD = require('../../Runtime/QtQuick/TextDocument/MarkdownParser')

const FONT = { family: 'Arial', pixelSize: 12, pointSize: 9, sizeUnit: 'px', weight: 400 }
const body = (html) => html.slice(html.indexOf('<body'))

const QT_MARKDOWN = [
    { md: "plain text",
      out: "plain text\n\n",
      raw: "plain text",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">plain text</p></body></html>" },
    { md: "# Heading 1\n\n## Heading 2\n\n###### H6",
      out: "# Heading 1\n\n## Heading 2\n\n###### H6\n\n",
      raw: "Heading 1\u2029Heading 2\u2029H6",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">Heading 1</span></h1>\n<h2 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">Heading 2</span></h2>\n<h6 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">H6</span></h6></body></html>" },
    { md: "Setext\n======\n\nSub\n---",
      out: "# Setext\n\n## Sub\n\n",
      raw: "Setext\u2029Sub",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">Setext</span></h1>\n<h2 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">Sub</span></h2></body></html>" },
    { md: "*em* and **strong** and ***both***",
      out: "*em* and **strong** and ***both***\n\n",
      raw: "em and strong and both",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">em</span> and <span style=\" font-weight:700;\">strong</span> and <span style=\" font-weight:700; font-style:italic;\">both</span></p></body></html>" },
    { md: "_under_ and __strong__ and ~~del~~",
      out: "_under_ and _strong_ and ~~del~~\n\n",
      raw: "under and strong and del",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" text-decoration: underline;\">under</span> and <span style=\" text-decoration: underline;\">strong</span> and <span style=\" text-decoration: line-through;\">del</span></p></body></html>" },
    { md: "`code` and ``co`de``",
      out: "`code` and ``co\\`de``\n\n",
      raw: "code and co`de",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">code</span> and <span style=\" font-family:'monospace';\">co`de</span></p></body></html>" },
    { md: "a\nb",
      out: "a b\n\n",
      raw: "a b",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a b</p></body></html>" },
    { md: "line  \nbreak\\\nagain",
      out: "line\n\nbreak\n\nagain\n\n",
      raw: "line\u2029break\u2029again",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">line</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">break</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">again</p></body></html>" },
    { md: "- one\n- two\n- three",
      out: "- one\n- two\n- three\n",
      raw: "one\u2029two\u2029three",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">three</li></ul></body></html>" },
    { md: "* star\n+ plus",
      out: "* star\n+ plus\n",
      raw: "star\u2029plus",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul type=\"circle\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">star</li></ul>\n<ul type=\"square\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">plus</li></ul></body></html>" },
    { md: "1. first\n2. second\n\n3) third",
      out: "1.  first\n2.  second\n3)  third\n",
      raw: "first\u2029second\u2029third",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">first</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">second</li></ol>\n<ol start=\"3\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1; -qt-list-number-suffix: ')';\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">third</li></ol></body></html>" },
    { md: "5. five\n6. six",
      out: "5.  five\n6.  six\n",
      raw: "five\u2029six",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol start=\"5\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">five</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">six</li></ol></body></html>" },
    { md: "- [ ] todo\n- [x] done",
      out: "- [ ] todo\n- [x] done\n",
      raw: "todo\u2029done",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li class=\"unchecked\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">todo</li>\n<li class=\"checked\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">done</li></ul></body></html>" },
    { md: "- a\n  - nested\n    - deeper\n- b",
      out: "- a\n  - nested\n    - deeper\n- b\n",
      raw: "a\u2029nested\u2029deeper\u2029b",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 2;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">nested\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 3;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">deeper</li></ul></li></ul></li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul></body></html>" },
    { md: "- loose\n\n- list",
      out: "- loose\n- list\n",
      raw: "loose\u2029list",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">loose</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">list</li></ul></body></html>" },
    { md: "- item\n\n  continuation paragraph",
      out: "- item\n\n  continuation paragraph\n\n",
      raw: "item\u2029continuation paragraph",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">item</li></ul>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:1; text-indent:0px;\">continuation paragraph</p></body></html>" },
    { md: "> quote\n> more\n\n> > nested",
      out: "> quote more\n> \n> > nested\n\n",
      raw: "quote more\u2029nested",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">quote more</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:80px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">nested</p></body></html>" },
    { md: "```\ncode\n  block\n```",
      out: "```\ncode\n  block\n```\n\n",
      raw: "code\u2029  block",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">code</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">  block</span></p></body></html>" },
    { md: "```js\nlet x = 1\n\nlet y\n```",
      out: "```js\nlet x = 1\n\nlet y\n```\n\n",
      raw: "let x = 1\u2029\u2029let y",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">let x = 1</span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'monospace';\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">let y</span></p></body></html>" },
    { md: "~~~\ntilde\n~~~",
      out: "~~~\ntilde\n~~~\n\n",
      raw: "tilde",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">tilde</span></p></body></html>" },
    { md: "    indented\n    code",
      out: "    indented\n    code\n\n",
      raw: "indented\u2029code",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">indented</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">code</span></p></body></html>" },
    { md: "---",
      out: "",
      raw: "\u2029",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<hr /></body></html>" },
    { md: "a\n\n***\n\nb",
      out: "a\n\n- - -\nb\n\n",
      raw: "a\u2029\u2029b",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</p>\n<hr />\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p></body></html>" },
    { md: "[link](http://x.y) and [titled](http://a.b \"Title\")",
      out: "[link](http://x.y) and [titled](http://a.b \"Title\")\n\n",
      raw: "link and titled",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://x.y\"><span style=\" text-decoration: underline; color:#0000ff;\">link</span></a> and <a href=\"http://a.b\" title=\"Title\"><span style=\" text-decoration: underline; color:#0000ff;\">titled</span></a></p></body></html>" },
    { md: "<http://auto.link> and www.example.com and mail@example.com",
      out: "<http://auto.link> and [www.example.com](http://www.example.com) and \n[mail@example.com](mailto:mail@example.com)\n\n",
      raw: "http://auto.link and www.example.com and mail@example.com",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://auto.link\"><span style=\" text-decoration: underline; color:#0000ff;\">http://auto.link</span></a> and <a href=\"http://www.example.com\"><span style=\" text-decoration: underline; color:#0000ff;\">www.example.com</span></a> and <a href=\"mailto:mail@example.com\"><span style=\" text-decoration: underline; color:#0000ff;\">mail@example.com</span></a></p></body></html>" },
    { md: "![alt text](img.png) and ![](i.png \"t\")",
      out: "![alt text](img.png) and \n\n",
      raw: "\ufffc and ",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\"img.png\" alt=\"alt text\" /> and </p></body></html>" },
    { md: "| a | b |\n|---|:-:|\n| 1 | 2 |\n| 3 | 4 |",
      out: "\n|a|b|\n|-|-|\n|1|2|\n|3|4|\n\n",
      raw: "\ufdd0a\ufdd0b\ufdd01\ufdd02\ufdd03\ufdd04\ufdd1",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; border-collapse:collapse;\" cellspacing=\"2\" cellpadding=\"4\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">b</span></p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1</p></td>\n<td>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2</p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3</p></td>\n<td>\n<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">4</p></td></tr></table></body></html>" },
    { md: "| left | right |\n|:-----|------:|\n| x | y |",
      out: "\n|left|right|\n|----|-----|\n|x   |y    |\n\n",
      raw: "\ufdd0left\ufdd0right\ufdd0x\ufdd0y\ufdd1",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; border-collapse:collapse;\" cellspacing=\"2\" cellpadding=\"4\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">left</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">right</span></p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</p></td>\n<td>\n<p align=\"right\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y</p></td></tr></table></body></html>" },
    { md: "\\*not em\\* and \\# hash",
      out: "\\*not em* and # hash\n\n",
      raw: "*not em* and # hash",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">*not em* and # hash</p></body></html>" },
    { md: "&amp; &lt; &copy; &#65;",
      out: "& < \u00a9 A\n\n",
      raw: "& < \u00a9 A",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">&amp; &lt; \u00a9 A</p></body></html>" },
    { md: "<b>html</b> inline",
      out: "**html** inline\n\n",
      raw: "html inline",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">html</span> inline</p></body></html>" },
    { md: "<div>\nblock html\n</div>",
      out: "block html \n\n",
      raw: "block html ",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">block html </p></body></html>" },
    { md: "para one\n\npara two\n\npara three",
      out: "para one\n\npara two\n\npara three\n\n",
      raw: "para one\u2029para two\u2029para three",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">para one</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">para two</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">para three</p></body></html>" },
    { md: "**bold *nested em* bold**",
      out: "**bold *nested em* bold**\n\n",
      raw: "bold nested em bold",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">bold </span><span style=\" font-weight:700; font-style:italic;\">nested em</span><span style=\" font-weight:700;\"> bold</span></p></body></html>" },
    { md: "a * not em * b and snake_case_word",
      out: "a * not em * b and snake_case_word\n\n",
      raw: "a * not em * b and snake_case_word",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a * not em * b and snake_case_word</p></body></html>" },
    { md: "# Heading with *em*",
      out: "# Heading with em\n\n",
      raw: "Heading with em",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">Heading with </span><span style=\" font-size:xx-large; font-weight:700; font-style:italic;\">em</span></h1></body></html>" },
    { md: "1. one\n   - inner\n2. two",
      out: "1.  one\n  - inner\n2.  two\n",
      raw: "one\u2029inner\u2029two",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 2;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">inner</li></ul></li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two</li></ol></body></html>" },
    { md: "text with trailing spaces   ",
      out: "text with trailing spaces\n\n",
      raw: "text with trailing spaces",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">text with trailing spaces</p></body></html>" },
    { md: "---\ntitle: x\n---\nbody",
      out: "---\ntitle: x\n---\nbody\n\n",
      raw: "body",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">body</p></body></html>" },
    { md: "- a\n- b\n\nafter",
      out: "---\ntitle: x\n---\n- a\n- b\n\nafter\n\n",
      raw: "a\u2029b\u2029after",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">after</p></body></html>" },
    { md: "> - quoted item\n> - another\n>\n> para in quote",
      out: "> - quoted item\n> - another\n> \n> para in quote\n\n",
      raw: "quoted item\u2029another\u2029para in quote",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">quoted item</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">another</li></ul>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">para in quote</p></body></html>" },
    { md: "- item\n\n  ```\n  code in item\n  ```\n- next",
      out: "- item\n\n  ```\n  code in item\n  ```\n- next\n",
      raw: "item\u2029code in item\u2029next",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">item</li>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:1; text-indent:0px;\"><span style=\" font-family:'monospace';\">code in item</span></p>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">next</li></ul></body></html>" },
    { md: "| *em* | `code` |\n|---|---|\n| **b** | [l](u) |",
      out: "\n|em|code|\n|--|----|\n|**b**|[l](u)|\n\n",
      raw: "\ufdd0em\ufdd0code\ufdd0b\ufdd0l\ufdd1",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; border-collapse:collapse;\" cellspacing=\"2\" cellpadding=\"4\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">em</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">code</span></p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">b</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"u\"><span style=\" text-decoration: underline; color:#0000ff;\">l</span></a></p></td></tr></table></body></html>" },
    { md: "| a | b | c |\n|---|---|---|\n| x | | |\n| | y | |",
      out: "\n|a|b|c|\n|-|-|-|\n|x  |||\n| |y| |\n\n",
      raw: "\ufdd0a\ufdd0b\ufdd0c\ufdd0x\ufdd0\ufdd0y\ufdd0\ufdd1",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; border-collapse:collapse;\" cellspacing=\"2\" cellpadding=\"4\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">b</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">c</span></p></td></tr>\n<tr>\n<td colspan=\"3\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">x</p></td></tr>\n<tr>\n<td></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">y</p></td>\n<td></td></tr></table></body></html>" },
    { md: "| a | b |\n|---|---|\n| only one |",
      out: "\n|a       |b|\n|--------|-|\n|only one| |\n\n",
      raw: "\ufdd0a\ufdd0b\ufdd0only one\ufdd0\ufdd1",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; border-collapse:collapse;\" cellspacing=\"2\" cellpadding=\"4\">\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a</span></p></td>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">b</span></p></td></tr>\n<tr>\n<td>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">only one</p></td>\n<td></td></tr></table></body></html>" },
    { md: "This is a very long paragraph of text that should certainly be wrapped by the markdown writer because it is longer than eighty columns of text.",
      out: "This is a very long paragraph of text that should certainly be wrapped by the\nmarkdown writer because it is longer than eighty columns of text.\n\n",
      raw: "This is a very long paragraph of text that should certainly be wrapped by the markdown writer because it is longer than eighty columns of text.",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">This is a very long paragraph of text that should certainly be wrapped by the markdown writer because it is longer than eighty columns of text.</p></body></html>" },
    { md: "- a list item with a very long text that goes on and on past the eighty column limit of the markdown writer",
      out: "- a list item with a very long text that goes on and on past the eighty column\n  limit of the markdown writer\n",
      raw: "a list item with a very long text that goes on and on past the eighty column limit of the markdown writer",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a list item with a very long text that goes on and on past the eighty column limit of the markdown writer</li></ul></body></html>" },
    { md: "[ref link][r] and [r]\n\n[r]: http://ref.example \"Ref Title\"",
      out: "[ref link](http://ref.example \"Ref Title\") and \n[r](http://ref.example \"Ref Title\")\n\n",
      raw: "ref link and r",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://ref.example\" title=\"Ref Title\"><span style=\" text-decoration: underline; color:#0000ff;\">ref link</span></a> and <a href=\"http://ref.example\" title=\"Ref Title\"><span style=\" text-decoration: underline; color:#0000ff;\">r</span></a></p></body></html>" },
    { md: "Trailing hashes ##\n\n## Heading ##",
      out: "Trailing hashes ##\n\n## Heading\n\n",
      raw: "Trailing hashes ##\u2029Heading",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Trailing hashes ##</p>\n<h2 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">Heading</span></h2></body></html>" },
    { md: "Multi line\nsetext heading\n==============",
      out: "# Multi line setext heading\n\n",
      raw: "Multi line setext heading",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">Multi line setext heading</span></h1></body></html>" },
    { md: "*emph with `code` inside* and **bold [link](http://x) inside**",
      out: "*emph with `code` inside* and **bold [link](http://x) inside**\n\n",
      raw: "emph with code inside and bold link inside",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-style:italic;\">emph with </span><span style=\" font-family:'monospace';\">code</span><span style=\" font-style:italic;\"> inside</span> and <span style=\" font-weight:700;\">bold </span><a href=\"http://x\"><span style=\" font-weight:700; text-decoration: underline; color:#0000ff;\">link</span></a><span style=\" font-weight:700;\"> inside</span></p></body></html>" },
    { md: "a\\\nb\\\nc",
      out: "a\n\nb\n\nc\n\n",
      raw: "a\u2029b\u2029c",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</p></body></html>" },
    { md: "- a  \n  b",
      out: "- a\n- b\n",
      raw: "a\u2029b",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul></body></html>" },
    { md: "1. one\n\n   para of one\n\n2. two",
      out: "1.  one\n\n    para of one\n\n2.  two\n",
      raw: "one\u2029para of one\u2029two",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">one</li>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:1; text-indent:0px;\">para of one</p>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">two</li></ol></body></html>" },
    { md: "- [ ] task one\n  - [x] nested done\n- plain",
      out: "- [ ] task one\n  - [x] nested done\n- plain\n",
      raw: "task one\u2029nested done\u2029plain",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li class=\"unchecked\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">task one\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 2;\">\n<li class=\"checked\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">nested done</li></ul></li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">plain</li></ul></body></html>" },
    { md: "+ a\n+ b\n\n- c",
      out: "+ a\n+ b\n- c\n",
      raw: "a\u2029b\u2029c",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul type=\"square\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul>\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</li></ul></body></html>" },
    { md: "<span>inline</span> html and <i>italic</i>",
      out: "inline html and *italic*\n\n",
      raw: "inline html and italic",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">inline html and <span style=\" font-style:italic;\">italic</span></p></body></html>" },
    { md: "<!-- comment -->\ntext",
      out: "text\n\n",
      raw: "text",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">text</p></body></html>" },
    { md: "http://example.com/path?q=1. And www.x.org, end",
      out: "<http://example.com/path?q=1>. And [www.x.org](http://www.x.org), end\n\n",
      raw: "http://example.com/path?q=1. And www.x.org, end",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://example.com/path?q=1\"><span style=\" text-decoration: underline; color:#0000ff;\">http://example.com/path?q=1</span></a>. And <a href=\"http://www.x.org\"><span style=\" text-decoration: underline; color:#0000ff;\">www.x.org</span></a>, end</p></body></html>" },
    { md: "\\# not heading\n\n1\\. not list",
      out: "\\# not heading\n\n1\\. not list\n\n",
      raw: "# not heading\u20291. not list",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># not heading</p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1. not list</p></body></html>" },
    { md: "# heading with `code` and **bold**",
      out: "# heading with `code` and bold\n\n",
      raw: "heading with code and bold",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">heading with </span><span style=\" font-family:'monospace'; font-size:xx-large; font-weight:700;\">code</span><span style=\" font-size:xx-large; font-weight:700;\"> and bold</span></h1></body></html>" },
    { md: "***\n---\n___",
      out: "- - -\n- - -\n",
      raw: "\u2029\u2029\u2029",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n<hr />\n<hr />\n<hr /></body></html>" },
    { md: "text with <br> break",
      out: "text with \n\n",
      raw: "text with ",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">text with </p></body></html>" },
    { md: "a *b **c** d* e",
      out: "a *b **c** d* e\n\n",
      raw: "a b c d e",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a <span style=\" font-style:italic;\">b </span><span style=\" font-weight:700; font-style:italic;\">c</span><span style=\" font-style:italic;\"> d</span> e</p></body></html>" },
    { md: "**a *b* c**",
      out: "**a *b* c**\n\n",
      raw: "a b c",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:700;\">a </span><span style=\" font-weight:700; font-style:italic;\">b</span><span style=\" font-weight:700;\"> c</span></p></body></html>" },
    { md: "~~strike **bold** strike~~",
      out: "~~strike **bold** strike~~\n\n",
      raw: "strike bold strike",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" text-decoration: line-through;\">strike </span><span style=\" font-weight:700; text-decoration: line-through;\">bold</span><span style=\" text-decoration: line-through;\"> strike</span></p></body></html>" },
    { md: "_under *em* under_",
      out: "_under *em* under_\n\n",
      raw: "under em under",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" text-decoration: underline;\">under </span><span style=\" font-style:italic; text-decoration: underline;\">em</span><span style=\" text-decoration: underline;\"> under</span></p></body></html>" },
    { md: "> quote\nlazy continuation",
      out: "> quote lazy continuation\n\n",
      raw: "quote lazy continuation",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">quote lazy continuation</p></body></html>" },
    { md: "> # heading in quote\n> text",
      out: "# heading in quote\n\n> text\n\n",
      raw: "heading in quote\u2029text",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h1 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700;\">heading in quote</span></h1>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:40px; margin-right:40px; -qt-block-indent:0; text-indent:0px;\">text</p></body></html>" },
    { md: "```python\ndef f():\n    return 1\n```\n\nafter code",
      out: "```python\ndef f():\n    return 1\n```\nafter code\n\n",
      raw: "def f():\u2029    return 1\u2029after code",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">def f():</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">    return 1</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">after code</p></body></html>" },
    { md: "    code\n\n    more code\n\ntext",
      out: "    code\n    \n    more code\n\ntext\n\n",
      raw: "code\u2029\u2029more code\u2029text",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">code</span></p>\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'monospace';\"><br /></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'monospace';\">more code</span></p>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">text</p></body></html>" },
    { md: "Hello &mdash; world &nbsp; &unknown; &#x41;",
      out: "Hello \u2014 world \u00a0 \\&unknown; A\n\n",
      raw: "Hello \u2014 world \u00a0 &unknown; A",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Hello \u2014 world \u00a0 &amp;unknown; A</p></body></html>" },
    { md: "![img *alt*](a.png \"title\")",
      out: "![img ](a.png \"title\")![alt](a.png \"title\")\n\n",
      raw: "\ufffc\ufffc",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\"a.png\" alt=\"img \" title=\"title\" /><img src=\"a.png\" alt=\"alt\" title=\"title\" /></p></body></html>" },
    { md: "[link with `code`](http://c.d)",
      out: "[link with ](http://c.d)[code](http://c.d)\n\n",
      raw: "link with code",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://c.d\"><span style=\" text-decoration: underline; color:#0000ff;\">link with </span></a><a href=\"http://c.d\"><span style=\" font-family:'monospace'; text-decoration: underline; color:#0000ff;\">code</span></a></p></body></html>" },
    { md: "line one\nline two\nline three",
      out: "line one line two line three\n\n",
      raw: "line one line two line three",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">line one line two line three</p></body></html>" },
    { md: "* a\n\n  b\n* c",
      out: "* a\n\n  b\n\n* c\n",
      raw: "a\u2029b\u2029c",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul type=\"circle\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</li>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:1; text-indent:0px;\">b</p>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</li></ul></body></html>" },
    { md: "10. ten\n11. eleven",
      out: "10. ten\n11. eleven\n",
      raw: "ten\u2029eleven",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol start=\"10\" style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">ten</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">eleven</li></ol></body></html>" },
    { md: "1) paren\n2) paren two",
      out: "1)  paren\n2)  paren two\n",
      raw: "paren\u2029paren two",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1; -qt-list-number-suffix: ')';\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">paren</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">paren two</li></ol></body></html>" },
    { md: "- a\n- b\n1. c\n2. d",
      out: "- a\n- b\n1.  c\n2.  d\n",
      raw: "a\u2029b\u2029c\u2029d",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">a</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">b</li></ul>\n<ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">c</li>\n<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">d</li></ol></body></html>" },
    { md: "Setext\n---\ntext after",
      out: "## Setext\n\ntext after\n\n",
      raw: "Setext\u2029text after",
      body: "<body style=\" font-family:'Arial'; font-size:12px; font-weight:400; font-style:normal;\">\n<h2 style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:x-large; font-weight:700;\">Setext</span></h2>\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">text after</p></body></html>" },
]

const QT_FROM_HTML = [
    { html: "<p>plain <b>bold</b> <i>italic</i> <u>under</u> <s>strike</s></p>",
      md: "plain **bold** *italic* _under_ ~~strike~~\n\n",
      raw: "plain bold italic under strike" },
    { html: "<h2>Head</h2><p>para</p>",
      md: "## Head\n\npara\n\n",
      raw: "Head\u2029para" },
    { html: "<ul><li>one</li><li>two<ul><li>inner</li></ul></li></ul>",
      md: "- one\n- two\n  - inner\n",
      raw: "one\u2029two\u2029inner" },
    { html: "<ol start=\"4\"><li>four</li><li>five</li></ol>",
      md: "4.  four\n5.  five\n",
      raw: "four\u2029five" },
    { html: "<p>a <a href=\"http://x.y\">link</a> and <a href=\"http://x.y\" title=\"T\">titled</a></p>",
      md: "a [link](http://x.y) and [titled](http://x.y \"T\")\n\n",
      raw: "a link and titled" },
    { html: "<pre>pre line 1\npre line 2</pre>",
      md: "```\npre line 1\n\npre line 2\n```\n\n",
      raw: "pre line 1\u2029\u2029pre line 2" },
    { html: "<blockquote>quoted</blockquote>",
      md: "> quoted\n\n",
      raw: "quoted" },
    { html: "<table border=1><tr><td>a</td><td>bb</td></tr><tr><td>ccc</td><td>d</td></tr></table>",
      md: "\n|a  |bb|\n|---|--|\n|ccc|d |\n\n",
      raw: "\ufdd0a\ufdd0bb\ufdd0ccc\ufdd0d\ufdd1" },
    { html: "<p>* star and # hash and 1. num</p><p>- dash</p>",
      md: "\\* star and # hash and 1. num\n\n\\- dash\n\n",
      raw: "* star and # hash and 1. num\u2029- dash" },
    { html: "<p>back\\slash and *stars* and [brackets] and `ticks` and &lt;tag&gt;</p>",
      md: "back\\\\slash and \\*stars* and \\[brackets] and \\`ticks` and \\<tag>\n\n",
      raw: "back\\slash and *stars* and [brackets] and `ticks` and <tag>" },
    { html: "<p>line<br/>break</p>",
      md: "line break\n\n",
      raw: "line break" },
    { html: "<hr/><p>after rule</p>",
      md: "- - -\nafter rule\n\n",
      raw: "\u2029\u2029after rule" },
    { html: "<p><img src=\"a.png\" alt=\"alt\" title=\"tt\"/></p>",
      md: "![alt](a.png \"tt\")\n\n",
      raw: "\ufffc" },
    { html: "<p><b>bold <i>both</i></b><i> italic</i></p>",
      md: "*bold both italic*\n\n",
      raw: "bold both italic" },
    { html: "<p>http://x.y</p><p><a href=\"http://x.y\">http://x.y</a></p>",
      md: "<http://x.y>\n\n<http://x.y>\n\n",
      raw: "http://x.y\u2029http://x.y" },
    { html: "<p>a very long paragraph with <b>bold words that go on and on and on and on</b> and then plain words that continue past the column limit</p>",
      md: "a very long paragraph with **bold words that go on and on and on and on** and\nthen plain words that continue past the column limit\n\n",
      raw: "a very long paragraph with bold words that go on and on and on and on and then plain words that continue past the column limit" },
    { html: "<ul><li>a</li></ul><p>para</p><ul><li>b</li></ul>",
      md: "- a\n\npara\n\n- b\n",
      raw: "a\u2029para\u2029b" },
]

test('Markdown into a document and back as Qt writes it (toMarkdown), its raw text and its HTML', () => {
    let doc = new TextDocument()
    QT_MARKDOWN.forEach((c, i) => {
        // the second half: another TextEdit (no front matter from before)
        if(i === 40) doc = new TextDocument()
        M.setMarkdown(doc, c.md, FONT)
        assert.equal(M.toMarkdown(doc), c.out, JSON.stringify(c.md))
        assert.equal(doc.rawText(), c.raw, JSON.stringify(c.md))
        // textFormat RichText: QTextDocument::setHtml(toHtml())
        let rich = new TextDocument()
        RT.setDocumentHtml(rich, RT.toHtml(doc, F.fontFormat(FONT)))
        assert.equal(body(RT.toHtml(rich, F.fontFormat(FONT))), c.body, JSON.stringify(c.md))
    })
})

test('HTML written as Markdown and read again (textFormat MarkdownText after RichText)', () => {
    let doc = new TextDocument()
    for(let c of QT_FROM_HTML){
        RT.setDocumentHtml(doc, c.html)
        M.setMarkdown(doc, M.toMarkdown(doc), FONT)
        assert.equal(M.toMarkdown(doc), c.md, c.html)
        assert.equal(doc.rawText(), c.raw, c.html)
    }
})

test('the parser reports blocks, spans and texts as md4c does', () => {
    let events = []
    let handler = {
        enterBlock: (t, d) => events.push('B' + t + (d.level ? ':' + d.level : '')),
        leaveBlock: (t) => events.push('/B' + t),
        enterSpan: (t) => events.push('S' + t),
        leaveSpan: (t) => events.push('/S' + t),
        text: (t, s) => events.push('T' + t + ':' + s),
    }
    MD.parse('# a *b*\n\n- x\n- y', handler)
    assert.deepEqual(events, ['B0', 'B6:1', 'T0:a ', 'S0', 'T0:b', '/S0', '/B6', 'B2', 'B4', 'T0:x', '/B4', 'B4', 'T0:y', '/B4', '/B2', '/B0'])
    events = []
    MD.parse('```js\nlet x\n```', handler)
    assert.deepEqual(events, ['B0', 'B7', 'T5:let x', 'T5:\n', '/B7', '/B0'])
})

test('a heading is bold and larger, a code span and a code block in the fixed font, a list item in its list', () => {
    let doc = new TextDocument()
    M.setMarkdown(doc, '# H\n\n`c` and\n\n    code\n\n- [x] item', FONT)
    assert.equal(doc.blockFormat(0).headingLevel, 1)
    assert.deepEqual([doc.charFormatAt(0).weight, doc.charFormatAt(0).sizeAdjustment], [700, 3])
    let code = doc.charFormatAt(doc.blockStart(1))
    assert.deepEqual([code.families, code.fixedPitch], [['monospace'], true])
    assert.ok(M.isMono(doc.charFormatAt(doc.blockStart(2))))
    assert.equal(doc.blockFormat(2).codeLanguage, '')
    let item = doc.blockFormat(3)
    assert.deepEqual([item.list.format.style, item.marker, item.indent], [F.LIST_DISC, 2, 0])
})

test('insertMarkdown at a cursor (QTextCursor::insertMarkdown) and the Markdown of a range (a fragment)', () => {
    let doc = new TextDocument()
    M.setMarkdown(doc, 'start', FONT)
    let cursor = new TextCursor(doc, 5)
    // the white space at the start of a paragraph is no text of it
    M.insertMarkdown(cursor, ' **bold**', FONT)
    assert.equal(M.toMarkdown(doc), 'start**bold**\n\n')
    assert.equal(cursor.position, 9)
    const { fragmentOf } = require('../../Runtime/QtQuick/TextDocument/Fragment')
    assert.equal(M.toMarkdown(fragmentOf(doc, 5, 9)), '**bold**\n\n')
})

test('setMarkdown leaves no undo step; the front matter stays with the document', () => {
    let doc = new TextDocument()
    M.setMarkdown(doc, '---\na: 1\n---\ntext', FONT)
    assert.equal(doc.isUndoAvailable(), false)
    assert.equal(doc.frontMatter, 'a: 1\n')
    M.setMarkdown(doc, 'other', FONT)
    assert.equal(M.toMarkdown(doc), '---\na: 1\n---\nother\n\n')
})

test('a paragraph has a margin of 2/3 of the font size in points (none for a font in pixels)', () => {
    let doc = new TextDocument()
    M.setMarkdown(doc, 'p', { family: 'Arial', pointSize: 12, sizeUnit: 'pt', weight: 400 })
    assert.deepEqual([doc.blockFormat(0).topMargin, doc.blockFormat(0).bottomMargin], [8, 8])
    M.setMarkdown(doc, 'p', FONT)
    assert.deepEqual([doc.blockFormat(0).topMargin, doc.blockFormat(0).bottomMargin], [0, 0])
})
