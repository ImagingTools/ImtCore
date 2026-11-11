// A reply of the size of the real one: `rows` rows of the table and as many measurements of 36 values.
export function makeReply(rows) {
	const lab = { __typename: 'Lab', L: 62, a: -44, b: -50 };
	const preview = { __typename: 'RgbColorHex', Color: '#00adee' };
	const colorant = (name) => ({ __typename: 'ColorantItem', colorantName: name, colorPreview: { __typename: 'ColorantProperties', chroma: 66.6, hue: 228.6, lab, preview } });
	const colorants = ['Cyan', 'Magenta', 'Yellow', 'Black'].map(colorant);
	const tableRows = [];
	const measurements = [];
	for (let i = 0; i < rows; i++) {
		const id = `id-${i}`;
		const index = [(i % 10) / 10, (i % 7) / 10, (i % 5) / 10, 0];
		tableRows.push({ __typename: 'TableViewRow', id, inactive: false, index, measurementIndex: i });
		measurements.push({
			__typename: 'MeasurementItem',
			measurementId: id,
			value: { __typename: 'MeasurementValue', index, value: Array.from({ length: 36 }, (_, k) => 0.1 + k / 100) },
			view: { __typename: 'MeasurementValueView', chroma: 33.9, hue: 120.5, lab, preview },
		});
	}
	return {
		__typename: 'MeasurementViewData',
		measurements: {
			__typename: 'MeasurementContainer',
			colorSpecification: {
				__typename: 'SpectralColorMeasurementSpecification',
				aperture: '',
				backing: 'Substrate',
				calibrationStandard: 'XRGA',
				deviceInfo: { __typename: 'DeviceInfo', deviceClass: 'NotSet', deviceModel: 'i1', devicePolarization: 'NotSet', filterType: 'NotSet', illuminationType: 'Incandescent', manufacturer: 'X-Rite', serialNumber: '1' },
				measurementType: 'Reflectance',
				spectrumInfo: { __typename: 'SpectrumInfo', end: 730, start: 380, step: 10 },
				spectrumType: 'Reflective',
			},
			colorantList: colorants,
			conditionList: [{ __typename: 'Condition', conditionId: 'M0', measurementList: measurements }],
		},
		table: {
			__typename: 'MeasurementTableViewData',
			colorantList: colorants,
			conditions: [{ __typename: 'TableViewCondition', id: 'M0', rows: tableRows }],
			inkSetPreview: [preview, preview, preview, preview],
			type: 'Press',
		},
	};
}
