package gamed.client.SpeedRisk;

import java.util.ArrayList;
import java.util.List;
import static gamed.client.SpeedRisk.ClassicBoard.Territories.*;

/**
 *
 * @author bruce
 */
public class ClassicBoard extends RiskBoard
{
    static final boolean BORDER_TABLE[] = new boolean[42 * 42];
    static final List<Country> COUNTRIES = new ArrayList(42);
    protected static int[] country_colors =
    {
        0xb22222, // Firebrick red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x191970, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };
    protected static int[] token_colors =
    {
        0xcd5c5c, // Indian red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x1e90ff, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };

    enum Territories
    {
        EASTERN_US(133, 114, 157, 131),
        NORTHWEST_TERRITORY(79, 52, 103, 65),
        WESTERN_US(100, 112, 117, 123),
        ONTARIO(150, 84, 154, 97),
        CENTRAL_AMERICA(114, 148, 140, 168),
        ALBERTA(87, 84, 109, 92),
        GREENLAND(99, 1, 187, 38),
        ALASKA(12, 49, 44, 62),
        QUEBEC(180, 76, 191, 92),
        BRAZIL(196, 198, 227, 219),
        VENEZUELA(180, 184, 194, 191),
        ARGENTINA(189, 240, 197, 267),
        PERU(177, 199, 197, 228),
        ICELAND(282, 65, 282, 62),
        SOUTHERN_EUROPE(332, 113, 348, 118),
        UKRAINE(362, 29, 388, 84),
        SCANDINAVIA(333, 50, 348, 66),
        GREAT_BRITAIN(306, 88, 306, 92),
        WESTERN_EUROPE(304, 108, 310, 119),
        NORTHERN_EUROPE(325, 90, 339, 99),
        EGYPT(340, 143, 358, 151),
        CONGO(341, 189, 353, 213),
        MADAGASCAR(403, 231, 400, 236),
        SOUTH_AFRICA(349, 242, 359, 251),
        EAST_AFRICA(362, 166, 379, 190),
        NORTH_AFRICA(293, 136, 317, 167),
        AFGHANISTAN(409, 75, 428, 109),
        MONGOLIA(484, 98, 517, 109),
        URAL(432, 43, 443, 78),
        JAPAN(560, 120, 569, 131),
        IRKUTSK(482, 81, 523, 85),
        INDIA(433, 142, 456, 156),
        SIAM(489, 157, 495, 167),
        YAKUTSK(519, 42, 546, 62),
        SIBERIA(455, 8, 483, 58),
        CHINA(451, 124, 491, 133),
        KAMCHATKA(new Kamchat(560, 32, 601, 60)),
        MIDDLE_EAST(372, 127, 398, 147),
        NEW_GUINEA(562, 208, 573, 211),
        INDONESIA(498, 174, 520, 199),
        WESTERN_AUSTRALIA(529, 228, 542, 247),
        EASTERN_AUSTRALIA(569, 227, 572, 248);

        private Territories(Country country)
        {
            COUNTRIES.add(country);
        }

        private Territories(int x, int y, int lx, int ly)
        {
            int id = COUNTRIES.size();
            COUNTRIES.add(id, new Country(id, String.format("images/classic/c%d.png", id), x, y, lx, ly));
        }
    }

    static
    {
        setBorder(ALASKA, KAMCHATKA);
        setBorder(ALASKA, NORTHWEST_TERRITORY);
        setBorder(ALASKA, ALBERTA);
        setBorder(NORTHWEST_TERRITORY, ALBERTA);
        setBorder(NORTHWEST_TERRITORY, GREENLAND);
        setBorder(NORTHWEST_TERRITORY, ONTARIO);
        setBorder(GREENLAND, ICELAND);
        setBorder(GREENLAND, QUEBEC);
        setBorder(GREENLAND, ONTARIO);
        setBorder(ALBERTA, ONTARIO);
        setBorder(ALBERTA, WESTERN_US);
        setBorder(ONTARIO, QUEBEC);
        setBorder(ONTARIO, EASTERN_US);
        setBorder(ONTARIO, WESTERN_US);
        setBorder(QUEBEC, EASTERN_US);
        setBorder(WESTERN_US, EASTERN_US);
        setBorder(WESTERN_US, CENTRAL_AMERICA);
        setBorder(EASTERN_US, CENTRAL_AMERICA);
        setBorder(CENTRAL_AMERICA, VENEZUELA);
        setBorder(VENEZUELA, BRAZIL);
        setBorder(VENEZUELA, PERU);
        setBorder(PERU, BRAZIL);
        setBorder(PERU, ARGENTINA);
        setBorder(BRAZIL, NORTH_AFRICA);
        setBorder(BRAZIL, ARGENTINA);
        setBorder(ICELAND, SCANDINAVIA);
        setBorder(ICELAND, GREAT_BRITAIN);
        setBorder(SCANDINAVIA, UKRAINE);
        setBorder(SCANDINAVIA, NORTHERN_EUROPE);
        setBorder(SCANDINAVIA, GREAT_BRITAIN);
        setBorder(UKRAINE, URAL);
        setBorder(UKRAINE, AFGHANISTAN);
        setBorder(UKRAINE, MIDDLE_EAST);
        setBorder(UKRAINE, SOUTHERN_EUROPE);
        setBorder(UKRAINE, NORTHERN_EUROPE);
        setBorder(GREAT_BRITAIN, NORTHERN_EUROPE);
        setBorder(GREAT_BRITAIN, WESTERN_EUROPE);
        setBorder(NORTHERN_EUROPE, SOUTHERN_EUROPE);
        setBorder(NORTHERN_EUROPE, WESTERN_EUROPE);
        setBorder(WESTERN_EUROPE, SOUTHERN_EUROPE);
        setBorder(WESTERN_EUROPE, NORTH_AFRICA);
        setBorder(SOUTHERN_EUROPE, MIDDLE_EAST);
        setBorder(SOUTHERN_EUROPE, EGYPT);
        setBorder(SOUTHERN_EUROPE, NORTH_AFRICA);
        setBorder(NORTH_AFRICA, EGYPT);
        setBorder(NORTH_AFRICA, EAST_AFRICA);
        setBorder(NORTH_AFRICA, CONGO);
        setBorder(EGYPT, MIDDLE_EAST);
        setBorder(EGYPT, EAST_AFRICA);
        setBorder(EAST_AFRICA, MIDDLE_EAST);
        setBorder(EAST_AFRICA, MADAGASCAR);
        setBorder(EAST_AFRICA, SOUTH_AFRICA);
        setBorder(EAST_AFRICA, CONGO);
        setBorder(CONGO, SOUTH_AFRICA);
        setBorder(SOUTH_AFRICA, MADAGASCAR);
        setBorder(URAL, SIBERIA);
        setBorder(URAL, CHINA);
        setBorder(URAL, AFGHANISTAN);
        setBorder(SIBERIA, YAKUTSK);
        setBorder(SIBERIA, IRKUTSK);
        setBorder(SIBERIA, MONGOLIA);
        setBorder(SIBERIA, CHINA);
        setBorder(YAKUTSK, KAMCHATKA);
        setBorder(YAKUTSK, IRKUTSK);
        setBorder(KAMCHATKA, JAPAN);
        setBorder(KAMCHATKA, MONGOLIA);
        setBorder(KAMCHATKA, IRKUTSK);
        setBorder(IRKUTSK, MONGOLIA);
        setBorder(AFGHANISTAN, CHINA);
        setBorder(AFGHANISTAN, INDIA);
        setBorder(AFGHANISTAN, MIDDLE_EAST);
        setBorder(MONGOLIA, JAPAN);
        setBorder(MONGOLIA, CHINA);
        setBorder(CHINA, INDIA);
        setBorder(CHINA, SIAM);
        setBorder(MIDDLE_EAST, INDIA);
        setBorder(INDIA, SIAM);
        setBorder(SIAM, INDONESIA);
        setBorder(INDONESIA, NEW_GUINEA);
        setBorder(INDONESIA, WESTERN_AUSTRALIA);
        setBorder(NEW_GUINEA, WESTERN_AUSTRALIA);
        setBorder(NEW_GUINEA, EASTERN_AUSTRALIA);
        setBorder(WESTERN_AUSTRALIA, EASTERN_AUSTRALIA);
    }

    public ClassicBoard()
    {
        super("images/classic/world_map_relief.png", COUNTRIES, 650, 375);
    }

    private static void setBorder(Enum a, Enum b)
    {
        BORDER_TABLE[a.ordinal() * 42 + b.ordinal()] = true;
        BORDER_TABLE[b.ordinal() * 42 + a.ordinal()] = true;
    }

    @Override
    public boolean borders(int c1, int c2)
    {
        return BORDER_TABLE[c1 * 42 + c2];
    }
}
