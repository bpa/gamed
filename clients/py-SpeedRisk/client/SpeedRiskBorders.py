BORDER_MAP = {
    'ALASKA': {'KAMCHATKA': 1, 'NORTHWEST TERRITORY': 1, 'ALBERTA': 1},
    'NORTHWEST TERRITORY': {'ALBERTA': 1, 'GREENLAND': 1, 'ONTARIO': 1},
    'GREENLAND': {'ICELAND': 1, 'QUEBEC': 1, 'ONTARIO': 1},
    'ALBERTA': {'ONTARIO': 1, 'WESTERN US': 1},
    'ONTARIO': {'QUEBEC': 1, 'EASTERN US': 1, 'WESTERN US': 1},
    'QUEBEC': {'EASTERN US': 1},
    'WESTERN US': {'EASTERN US': 1, 'CENTRAL AMERICA': 1},
    'EASTERN US': {'CENTRAL AMERICA': 1},
    'CENTRAL AMERICA': {'VENEZUELA': 1},
    'VENEZUELA': {'BRAZIL': 1, 'PERU': 1},
    'PERU': {'BRAZIL': 1, 'ARGENTINA': 1},
    'BRAZIL': {'NORTH AFRICA': 1, 'ARGENTINA': 1},
    'ICELAND': {'SCANDINAVIA': 1, 'GREAT BRITAIN': 1},
    'SCANDINAVIA': {'UKRAINE': 1, 'NORTHERN EUROPE': 1, 'GREAT BRITAIN': 1},
    'UKRAINE': {'URAL': 1, 'AFGHANISTAN': 1, 'MIDDLE EAST': 1, 'SOUTHERN EUROPE': 1, 'NORTHERN EUROPE': 1},
    'GREAT BRITAIN': {'NORTHERN EUROPE': 1, 'WESTERN EUROPE': 1},
    'NORTHERN EUROPE': {'SOUTHERN EUROPE': 1, 'WESTERN EUROPE': 1},
    'WESTERN EUROPE': {'SOUTHERN EUROPE': 1, 'NORTH AFRICA': 1},
    'SOUTHERN EUROPE': {'MIDDLE EAST': 1, 'EGYPT': 1, 'NORTH AFRICA': 1},
    'NORTH AFRICA': {'EGYPT': 1, 'EAST AFRICA': 1, 'CONGO': 1},
    'EGYPT': {'MIDDLE EAST': 1, 'EAST AFRICA': 1},
    'EAST AFRICA': {'MIDDLE EAST': 1, 'MADAGASCAR': 1, 'SOUTH AFRICA': 1, 'CONGO': 1},
    'CONGO': {'SOUTH AFRICA': 1},
    'SOUTH AFRICA': {'MADAGASCAR': 1},
    'URAL': {'SIBERIA': 1, 'CHINA': 1, 'AFGHANISTAN': 1},
    'SIBERIA': {'YAKUTSK': 1, 'IRKUTSK': 1, 'MONGOLIA': 1, 'CHINA': 1},
    'YAKUTSK': {'KAMCHATKA': 1, 'IRKUTSK': 1},
    'KAMCHATKA': {'JAPAN': 1, 'MONGOLIA': 1, 'IRKUTSK': 1},
    'IRKUTSK': {'MONGOLIA': 1},
    'AFGHANISTAN': {'CHINA': 1, 'INDIA': 1, 'MIDDLE EAST': 1},
    'MONGOLIA': {'JAPAN': 1, 'CHINA': 1},
    'CHINA': {'INDIA': 1, 'SIAM': 1},
    'MIDDLE EAST': {'INDIA': 1},
    'INDIA': {'SIAM': 1},
    'SIAM': {'INDONESIA': 1},
    'INDONESIA': {'NEW GUINEA': 1, 'WESTERN AUSTRALIA': 1},
    'NEW GUINEA': {'WESTERN AUSTRALIA': 1, 'EASTERN AUSTRALIA': 1},
    'WESTERN AUSTRALIA': {'EASTERN AUSTRALIA': 1},
}

for k in BORDER_MAP.keys():
    for c in BORDER_MAP[k].keys():
        if not BORDER_MAP.has_key(c):
            BORDER_MAP[c] = {}
        BORDER_MAP[c][k] = 1

def borders(c1, c2):
    return BORDER_MAP[c1.name].has_key(c2.name)
