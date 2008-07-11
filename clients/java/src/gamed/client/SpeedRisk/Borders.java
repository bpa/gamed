package gamed.client.SpeedRisk;

/**
 *
 * @author bruce
 */
public class Borders {
    boolean border_table[] = new boolean[42*42];
    private static final int EASTERN_US          =  0;
    private static final int NORTHWEST_TERRITORY =  1;
    private static final int WESTERN_US          =  2;
    private static final int ONTARIO             =  3;
    private static final int CENTRAL_AMERICA     =  4;
    private static final int ALBERTA             =  5;
    private static final int GREENLAND           =  6;
    private static final int ALASKA              =  7;
    private static final int QUEBEC              =  8;
    private static final int BRAZIL              =  9;
    private static final int VENEZUELA           = 10;
    private static final int ARGENTINA           = 11;
    private static final int PERU                = 12;
    private static final int ICELAND             = 13;
    private static final int SOUTHERN_EUROPE     = 14;
    private static final int UKRAINE             = 15;
    private static final int SCANDINAVIA         = 16;
    private static final int GREAT_BRITAIN       = 17;
    private static final int WESTERN_EUROPE      = 18;
    private static final int NORTHERN_EUROPE     = 19;
    private static final int EGYPT               = 20;
    private static final int CONGO               = 21;
    private static final int MADAGASCAR          = 22;
    private static final int SOUTH_AFRICA        = 23;
    private static final int EAST_AFRICA         = 24;
    private static final int NORTH_AFRICA        = 25;
    private static final int AFGHANISTAN         = 26;
    private static final int MONGOLIA            = 27;
    private static final int URAL                = 28;
    private static final int JAPAN               = 29;
    private static final int IRKUTSK             = 30;
    private static final int INDIA               = 31;
    private static final int SIAM                = 32;
    private static final int YAKUTSK             = 33;
    private static final int SIBERIA             = 34;
    private static final int CHINA               = 35;
    private static final int KAMCHATKA           = 36;
    private static final int MIDDLE_EAST         = 37;
    private static final int NEW_GUINEA          = 38;
    private static final int INDONESIA           = 39;
    private static final int WESTERN_AUSTRALIA   = 40;
    private static final int EASTERN_AUSTRALIA   = 41;

    public Borders() {
        set_border(ALASKA, KAMCHATKA);
        set_border(ALASKA, NORTHWEST_TERRITORY);
        set_border(ALASKA, ALBERTA);
        set_border(NORTHWEST_TERRITORY, ALBERTA);
        set_border(NORTHWEST_TERRITORY, GREENLAND);
        set_border(NORTHWEST_TERRITORY, ONTARIO);
        set_border(GREENLAND, ICELAND);
        set_border(GREENLAND, QUEBEC);
        set_border(GREENLAND, ONTARIO);
        set_border(ALBERTA, ONTARIO);
        set_border(ALBERTA, WESTERN_US);
        set_border(ONTARIO, QUEBEC);
        set_border(ONTARIO, EASTERN_US);
        set_border(ONTARIO, WESTERN_US);
        set_border(QUEBEC, EASTERN_US);
        set_border(WESTERN_US, EASTERN_US);
        set_border(WESTERN_US, CENTRAL_AMERICA);
        set_border(EASTERN_US, CENTRAL_AMERICA);
        set_border(CENTRAL_AMERICA, VENEZUELA);
        set_border(VENEZUELA, BRAZIL);
        set_border(VENEZUELA, PERU);
        set_border(PERU, BRAZIL);
        set_border(PERU, ARGENTINA);
        set_border(BRAZIL, NORTH_AFRICA);
        set_border(BRAZIL, ARGENTINA);
        set_border(ICELAND, SCANDINAVIA);
        set_border(ICELAND, GREAT_BRITAIN);
        set_border(SCANDINAVIA, UKRAINE);
        set_border(SCANDINAVIA, NORTHERN_EUROPE);
        set_border(SCANDINAVIA, GREAT_BRITAIN);
        set_border(UKRAINE, URAL);
        set_border(UKRAINE, AFGHANISTAN);
        set_border(UKRAINE, MIDDLE_EAST);
        set_border(UKRAINE, SOUTHERN_EUROPE);
        set_border(UKRAINE, NORTHERN_EUROPE);
        set_border(GREAT_BRITAIN, NORTHERN_EUROPE);
        set_border(GREAT_BRITAIN, WESTERN_EUROPE);
        set_border(NORTHERN_EUROPE, SOUTHERN_EUROPE);
        set_border(NORTHERN_EUROPE, WESTERN_EUROPE);
        set_border(WESTERN_EUROPE, SOUTHERN_EUROPE);
        set_border(WESTERN_EUROPE, NORTH_AFRICA);
        set_border(SOUTHERN_EUROPE, MIDDLE_EAST);
        set_border(SOUTHERN_EUROPE, EGYPT);
        set_border(SOUTHERN_EUROPE, NORTH_AFRICA);
        set_border(NORTH_AFRICA, EGYPT);
        set_border(NORTH_AFRICA, EAST_AFRICA);
        set_border(NORTH_AFRICA, CONGO);
        set_border(EGYPT, MIDDLE_EAST);
        set_border(EGYPT, EAST_AFRICA);
        set_border(EAST_AFRICA, MIDDLE_EAST);
        set_border(EAST_AFRICA, MADAGASCAR);
        set_border(EAST_AFRICA, SOUTH_AFRICA);
        set_border(EAST_AFRICA, CONGO);
        set_border(CONGO, SOUTH_AFRICA);
        set_border(SOUTH_AFRICA, MADAGASCAR);
        set_border(URAL, SIBERIA);
        set_border(URAL, CHINA);
        set_border(URAL, AFGHANISTAN);
        set_border(SIBERIA, YAKUTSK);
        set_border(SIBERIA, IRKUTSK);
        set_border(SIBERIA, MONGOLIA);
        set_border(SIBERIA, CHINA);
        set_border(YAKUTSK, KAMCHATKA);
        set_border(YAKUTSK, IRKUTSK);
        set_border(KAMCHATKA, JAPAN);
        set_border(KAMCHATKA, MONGOLIA);
        set_border(KAMCHATKA, IRKUTSK);
        set_border(IRKUTSK, MONGOLIA);
        set_border(AFGHANISTAN, CHINA);
        set_border(AFGHANISTAN, INDIA);
        set_border(AFGHANISTAN, MIDDLE_EAST);
        set_border(MONGOLIA, JAPAN);
        set_border(MONGOLIA, CHINA);
        set_border(CHINA, INDIA);
        set_border(CHINA, SIAM);
        set_border(MIDDLE_EAST, INDIA);
        set_border(INDIA, SIAM);
        set_border(SIAM, INDONESIA);
        set_border(INDONESIA, NEW_GUINEA);
        set_border(INDONESIA, WESTERN_AUSTRALIA);
        set_border(NEW_GUINEA, WESTERN_AUSTRALIA);
        set_border(NEW_GUINEA, EASTERN_AUSTRALIA);
        set_border(WESTERN_AUSTRALIA, EASTERN_AUSTRALIA);
    }
    
    private void set_border(int a, int b) {
        border_table[a * 42 + b] = true;
        border_table[b * 42 + a] = true;
    }

    public boolean borders(int c1, int c2) {
        return border_table[c1 * 42 + c2];
    }
}
