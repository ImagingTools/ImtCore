#!/usr/bin/env python3
"""
Figma Token Processor Test
A standalone Python implementation to test the Figma token conversion logic
"""

import json
import sys
from pathlib import Path


def resolve_reference(token_value, tokens_root):
    """Resolve token references like {color.primary}"""
    if not (token_value.startswith('{') and token_value.endswith('}')):
        return token_value
    
    # Remove braces
    reference = token_value[1:-1].strip()
    
    # Navigate the token tree
    parts = reference.split('.')
    current = tokens_root
    
    for part in parts:
        if isinstance(current, dict) and part in current:
            current = current[part]
        else:
            return token_value  # Can't resolve, return original
    
    # Extract value
    if isinstance(current, dict):
        if 'value' in current:
            value = current['value']
        elif '$value' in current:
            value = current['$value']
        else:
            return token_value
        
        # Recursively resolve if the value is also a reference
        if isinstance(value, str) and value.startswith('{') and value.endswith('}'):
            return resolve_reference(value, tokens_root)
        return value
    elif isinstance(current, str):
        if current.startswith('{') and current.endswith('}'):
            return resolve_reference(current, tokens_root)
        return current
    
    return token_value


def extract_colors(tokens_json):
    """Extract color palette from Figma tokens"""
    color_palette = {}
    
    # Find color section
    color_keys = ['color', 'colors', 'colour', 'colours']
    colors_section = None
    
    for key in color_keys:
        if key in tokens_json:
            colors_section = tokens_json[key]
            break
    
    if not colors_section:
        colors_section = tokens_json
    
    # Recursively extract colors
    def extract_recursive(obj, prefix=''):
        for key, value in obj.items():
            if isinstance(value, dict):
                # Check if this is a token with value
                if 'value' in value or '$value' in value:
                    token_value = value.get('value') or value.get('$value')
                    
                    # Resolve references
                    token_value = resolve_reference(token_value, tokens_json)
                    
                    color_name = f"{prefix} {key}" if prefix else key
                    color_palette[color_name] = token_value
                else:
                    # Recurse into nested object
                    new_prefix = f"{prefix} {key}" if prefix else key
                    extract_recursive(value, new_prefix)
            elif isinstance(value, str):
                # Direct color value
                if value.startswith('#') or value.startswith('rgb'):
                    color_name = f"{prefix} {key}" if prefix else key
                    color_palette[color_name] = value
    
    extract_recursive(colors_section)
    return color_palette


def extract_fonts(tokens_json):
    """Extract fonts from Figma tokens"""
    fonts = {}
    
    # Find typography section
    font_keys = ['typography', 'font', 'fonts', 'fontFamily', 'fontFamilies']
    fonts_section = None
    
    for key in font_keys:
        if key in tokens_json:
            fonts_section = tokens_json[key]
            break
    
    if not fonts_section:
        return fonts
    
    for font_name, font_value in fonts_section.items():
        if not isinstance(font_value, dict):
            continue
        
        # Extract font properties
        value_obj = font_value.get('value', font_value)
        if not isinstance(value_obj, dict):
            continue
        
        font_def = {}
        
        if 'fontFamily' in value_obj:
            font_def['Family'] = value_obj['fontFamily']
        
		if 'fontSize' in value_obj:
            size_str = str(value_obj['fontSize']).replace('px', '').strip()
            try:
                # Convert directly to int to avoid float precision issues
                font_def['PointSize'] = int(size_str) if size_str.isdigit() else int(float(size_str))
            except (ValueError, TypeError):
                font_def['PointSize'] = 11
        
        if 'fontWeight' in value_obj:
            weight = value_obj['fontWeight']
            if isinstance(weight, str):
                font_def['Bold'] = weight in ['bold', '700']
            elif isinstance(weight, (int, float)):
                font_def['Bold'] = int(weight) >= 700
        
        if 'fontStyle' in value_obj:
            font_def['Italic'] = value_obj['fontStyle'] == 'italic'
        
        # Set defaults
        font_def.setdefault('Bold', False)
        font_def.setdefault('Italic', False)
        font_def.setdefault('PointSize', 11)
        
        fonts[font_name] = font_def
    
    return fonts


def convert_figma_to_theme(figma_tokens):
    """Convert Figma tokens to .theme format"""
    
    # Extract colors and fonts
    color_palette = extract_colors(figma_tokens)
    fonts = extract_fonts(figma_tokens)
    
    # Build theme structure
    theme = {
        "ColorPalette": color_palette,
        "Style": {
            "Name": "Figma",
            "IconTemplateList": [
                {
                    "TemplateIconColor": "#D9D9D9",
                    "IconColor": {
                        "Normal": "primary" if "primary" in color_palette else "text primary",
                        "OffActive": "primary" if "primary" in color_palette else "text primary",
                        "OffDisabled": "text disabled" if "text disabled" in color_palette else "text secondary",
                        "OffNormal": "primary" if "primary" in color_palette else "text primary",
                        "OffSelected": "accent" if "accent" in color_palette else "primary",
                        "OnActive": "primary" if "primary" in color_palette else "text primary",
                        "OnDisabled": "text disabled" if "text disabled" in color_palette else "text secondary",
                        "OnNormal": "primary" if "primary" in color_palette else "text primary",
                        "OnSelected": "accent" if "accent" in color_palette else "primary"
                    }
                }
            ]
        }
    }
    
    # Add fonts if present
    if fonts:
        theme["Style"]["Fonts"] = fonts
    
    # Add basic color groups
    active_colors = {}
    
    # Map common semantic tokens
    color_map = {
        "Background": ["background light", "background", "surface"],
        "Foreground": ["text primary", "text", "foreground"],
        "Text": ["text primary", "text", "primary"],
        "Base": ["surface", "background light", "background"],
        "Button": ["surface", "background light"],
        "ButtonText": ["text primary", "text"],
        "Highlight": ["accent", "primary"]
    }
    
    for target_key, possible_keys in color_map.items():
        for key in possible_keys:
            if key in color_palette:
                active_colors[target_key] = key
                break
    
    if active_colors:
        theme["Style"]["ActiveColors"] = active_colors
        theme["Style"]["InactiveColors"] = active_colors
        theme["Style"]["DisabledActiveColors"] = active_colors
        theme["Style"]["DisabledInActiveColors"] = active_colors
    
    return theme


def main():
    if len(sys.argv) < 3:
        print("Usage: python3 test_figma_processor.py <figma-tokens.json> <output.theme>")
        sys.exit(1)
    
    input_file = Path(sys.argv[1])
    output_file = Path(sys.argv[2])
    
    if not input_file.exists():
        print(f"Error: Input file not found: {input_file}")
        sys.exit(1)
    
    print(f"Reading Figma tokens from: {input_file}")
    
    with open(input_file, 'r') as f:
        figma_tokens = json.load(f)
    
    print("Converting to .theme format...")
    theme = convert_figma_to_theme(figma_tokens)
    
    print(f"Writing output to: {output_file}")
    with open(output_file, 'w') as f:
        json.dump(theme, f, indent=2)
    
    print("\n=== Conversion Summary ===")
    print(f"Colors extracted: {len(theme['ColorPalette'])}")
    if 'Fonts' in theme['Style']:
        print(f"Fonts extracted: {len(theme['Style']['Fonts'])}")
    print(f"\nOutput saved to: {output_file}")
    
    # Preview
    print("\n=== Color Palette Preview ===")
    for i, (name, value) in enumerate(list(theme['ColorPalette'].items())[:10]):
        print(f"  {name}: {value}")
    if len(theme['ColorPalette']) > 10:
        print(f"  ... and {len(theme['ColorPalette']) - 10} more")
    
    print("\n✓ Conversion successful!")


if __name__ == '__main__':
    main()
