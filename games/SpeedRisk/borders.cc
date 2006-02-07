#include <strings.h>
#include <stdint.h>
#include <SpeedRisk/board.h>

#define set_border(a, b) border_table[a][b] = 1; border_table[b][a] = 1;
int8_t border_table[SR_NUM_COUNRIES][SR_NUM_COUNRIES];
bool border_table_built = 0;

void build_border_table() {
    if (border_table_built) {
        return;
    }
    bzero(&border_table[0][0], sizeof(border_table));

// North America
    set_border(SR_ALASKA, SR_KAMCHATKA);
    set_border(SR_ALASKA, SR_NORTHWEST_TERRITORY);
    set_border(SR_ALASKA, SR_ALBERTA);

    set_border(SR_NORTHWEST_TERRITORY, SR_ALBERTA);
    set_border(SR_NORTHWEST_TERRITORY, SR_GREENLAND);
    set_border(SR_NORTHWEST_TERRITORY, SR_ONTARIO);

    set_border(SR_GREENLAND, SR_ICELAND);
    set_border(SR_GREENLAND, SR_QUEBEC);
    set_border(SR_GREENLAND, SR_ONTARIO);

    set_border(SR_ALBERTA, SR_ONTARIO);
    set_border(SR_ALBERTA, SR_WESTERN_US);

    set_border(SR_ONTARIO, SR_QUEBEC);
    set_border(SR_ONTARIO, SR_EASTERN_US);
    set_border(SR_ONTARIO, SR_WESTERN_US);
    
    set_border(SR_QUEBEC, SR_EASTERN_US);

    set_border(SR_WESTERN_US, SR_EASTERN_US);
    set_border(SR_WESTERN_US, SR_CENTRAL_AMERICA);

    set_border(SR_EASTERN_US, SR_CENTRAL_AMERICA);

    set_border(SR_CENTRAL_AMERICA, SR_VENEZUELA);

// South America
    set_border(SR_VENEZUELA, SR_BRAZIL);
    set_border(SR_VENEZUELA, SR_PERU);

    set_border(SR_PERU, SR_BRAZIL);
    set_border(SR_PERU, SR_ARGENTINA);

    set_border(SR_BRAZIL, SR_NORTH_AFRICA);
    set_border(SR_BRAZIL, SR_ARGENTINA);

// Europe
    set_border(SR_ICELAND, SR_SCANDINAVIA);
    set_border(SR_ICELAND, SR_GREAT_BRITAIN);

    set_border(SR_SCANDINAVIA, SR_UKRAINE);
    set_border(SR_SCANDINAVIA, SR_NORTHERN_EUROPE);
    set_border(SR_SCANDINAVIA, SR_GREAT_BRITAIN);
    
    set_border(SR_UKRAINE, SR_URAL);
    set_border(SR_UKRAINE, SR_AFGHANISTAN);
    set_border(SR_UKRAINE, SR_MIDDLE_EAST);
    set_border(SR_UKRAINE, SR_SOUTHERN_EUROPE);
    set_border(SR_UKRAINE, SR_NORTHERN_EUROPE);

    set_border(SR_GREAT_BRITAIN, SR_NORTHERN_EUROPE);
    set_border(SR_GREAT_BRITAIN, SR_WESTERN_EUROPE);

    set_border(SR_NORTHERN_EUROPE, SR_SOUTHERN_EUROPE);
    set_border(SR_NORTHERN_EUROPE, SR_WESTERN_EUROPE);

    set_border(SR_WESTERN_EUROPE, SR_SOUTHERN_EUROPE);
    set_border(SR_WESTERN_EUROPE, SR_NORTH_AFRICA);

    set_border(SR_SOUTHERN_EUROPE, SR_MIDDLE_EAST);
    set_border(SR_SOUTHERN_EUROPE, SR_EGYPT);
    set_border(SR_SOUTHERN_EUROPE, SR_NORTH_AFRICA);

// Africa
    set_border(SR_NORTH_AFRICA, SR_EGYPT);
    set_border(SR_NORTH_AFRICA, SR_EAST_AFRICA);
    set_border(SR_NORTH_AFRICA, SR_CONGO);

    set_border(SR_EGYPT, SR_MIDDLE_EAST);
    set_border(SR_EGYPT, SR_EAST_AFRICA);

    set_border(SR_EAST_AFRICA, SR_MIDDLE_EAST);
    set_border(SR_EAST_AFRICA, SR_MADAGASCAR);
    set_border(SR_EAST_AFRICA, SR_SOUTH_AFRICA);
    set_border(SR_EAST_AFRICA, SR_CONGO);

    set_border(SR_CONGO, SR_SOUTH_AFRICA);

    set_border(SR_SOUTH_AFRICA, SR_MADAGASCAR);

// Asia
  set_border(SR_URAL, SR_SIBERIA);
  set_border(SR_URAL, SR_CHINA);
  set_border(SR_URAL, SR_AFGHANISTAN);

  set_border(SR_SIBERIA, SR_YAKUTSK);
  set_border(SR_SIBERIA, SR_IRKUTSK);
  set_border(SR_SIBERIA, SR_MONGOLIA);
  set_border(SR_SIBERIA, SR_CHINA);

  set_border(SR_YAKUTSK, SR_KAMCHATKA);
  set_border(SR_YAKUTSK, SR_IRKUTSK);

  set_border(SR_KAMCHATKA, SR_JAPAN);
  set_border(SR_KAMCHATKA, SR_MONGOLIA);
  set_border(SR_KAMCHATKA, SR_IRKUTSK);

  set_border(SR_IRKUTSK, SR_MONGOLIA);

  set_border(SR_AFGHANISTAN, SR_CHINA);
  set_border(SR_AFGHANISTAN, SR_INDIA);
  set_border(SR_AFGHANISTAN, SR_MIDDLE_EAST);

  set_border(SR_MONGOLIA, SR_JAPAN);
  set_border(SR_MONGOLIA, SR_CHINA);

  set_border(SR_CHINA, SR_INDIA);
  set_border(SR_CHINA, SR_SIAM);

  set_border(SR_MIDDLE_EAST, SR_INDIA);

  set_border(SR_INDIA, SR_SIAM);

  set_border(SR_SIAM, SR_INDONESIA);

// Australia
  set_border(SR_INDONESIA, SR_NEW_GUINEA);
  set_border(SR_INDONESIA, SR_WESTERN_AUSTRALIA);

  set_border(SR_NEW_GUINEA, SR_WESTERN_AUSTRALIA);
  set_border(SR_NEW_GUINEA, SR_EASTERN_AUSTRALIA);

  set_border(SR_WESTERN_AUSTRALIA, SR_EASTERN_AUSTRALIA);
  
  border_table_built = true;
}

bool borders(int c1, int c2) {
    if (c1 < SR_NUM_COUNRIES && c1 >=0 &&
        c2 < SR_NUM_COUNRIES && c2 >=0) {
            return border_table[c1][c2];
    }
    else {
        return false;
    }
}
