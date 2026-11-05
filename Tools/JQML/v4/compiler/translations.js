// Qt translation catalogs (.ts) embedded into a compiled application. Every file is grouped by its
// `<TS language="...">` value and merged into one dictionary per language; a string that exists twice
// keeps the later value. qsTr() of the engine looks the text up in the dictionary of engine.language.

import { readdirSync, readFileSync, statSync } from 'node:fs';
import path from 'node:path';
import { directoryExists } from './config.js';

function decodeXmlText(text) {
	let value = String(text);
	const cdata = [];
	value = value.replace(/<!\[CDATA\[([\s\S]*?)\]\]>/g, (_, inner) => {
		cdata.push(inner);
		return `\u0000CDATA${cdata.length - 1}\u0000`;
	});
	value = value
		.replace(/&#x([0-9a-fA-F]+);/g, (_, hex) => String.fromCodePoint(parseInt(hex, 16)))
		.replace(/&#(\d+);/g, (_, dec) => String.fromCodePoint(Number(dec)))
		.replace(/&lt;/g, '<')
		.replace(/&gt;/g, '>')
		.replace(/&quot;/g, '"')
		.replace(/&apos;/g, "'")
		.replace(/&amp;/g, '&');
	return value.replace(/\u0000CDATA(\d+)\u0000/g, (_, index) => cdata[Number(index)]);
}

// { source text -> translation } of one .ts file; of a plural message the first form is used.
export function parseTsCatalog(xml) {
	const dict = {};
	const source = String(xml).replace(/<!--[\s\S]*?-->/g, '');
	const messageRe = /<message\b[^>]*>([\s\S]*?)<\/message>/g;
	let messageMatch;
	while ((messageMatch = messageRe.exec(source))) {
		const body = messageMatch[1];
		const sourceMatch = body.match(/<source\b[^>]*>([\s\S]*?)<\/source>/);
		const translationMatch = body.match(/<translation\b[^>]*>([\s\S]*?)<\/translation>/);
		if (!sourceMatch || !translationMatch) continue;
		let translation = translationMatch[1];
		const forms = [];
		const formRe = /<numerusform\b[^>]*>([\s\S]*?)<\/numerusform>/g;
		let formMatch;
		while ((formMatch = formRe.exec(translation))) forms.push(decodeXmlText(formMatch[1]));
		translation = forms.length ? forms[0] : decodeXmlText(translation);
		dict[decodeXmlText(sourceMatch[1])] = translation;
	}
	return dict;
}

// The language of a catalog: the `language` attribute of <TS>, else the `xx_YY` suffix of the file name.
export function translationLanguage(xml, fileName) {
	const match = String(xml).match(/<TS\b[^>]*\blanguage\s*=\s*"([^"]+)"/);
	if (match && match[1]) return decodeXmlText(match[1]);
	const stem = fileName.slice(0, -3);
	const fromName = stem.match(/(?:^|_)([A-Za-z]{2,3}_[A-Za-z]{2})$/);
	return fromName ? fromName[1] : '';
}

// Returns { catalogs: { language: dict }, fileCount }; problems are reported through `warn`.
export function loadTranslationCatalogs(dirs, warn = console.warn) {
	const catalogs = {};
	let fileCount = 0;
	for (const dirPath of dirs) {
		if (!dirPath || String(dirPath).indexOf('${') >= 0) {
			warn(`translation_dir was not resolved: ${dirPath}`);
			continue;
		}
		if (!directoryExists(dirPath)) {
			warn(`translation_dir was not found: ${dirPath}`);
			continue;
		}
		const names = readdirSync(dirPath).filter((name) => name.length > 3 && name.endsWith('.ts')).sort();
		for (const name of names) {
			const filePath = path.join(dirPath, name);
			if (!statSync(filePath).isFile()) continue;
			const xml = readFileSync(filePath, 'utf8');
			const language = translationLanguage(xml, name);
			if (!language) {
				warn(`translation file has no language: ${filePath}`);
				continue;
			}
			if (!catalogs[language]) catalogs[language] = {};
			Object.assign(catalogs[language], parseTsCatalog(xml));
			fileCount++;
		}
	}
	return { catalogs, fileCount };
}
