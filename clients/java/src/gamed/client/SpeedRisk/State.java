package gamed.client.SpeedRisk;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
/**
 *
 * @author bruce
 */
public class State {

    public static final byte CMD_NOP = 0;
    public static final byte CMD_INVALID = 1;
    public static final byte CMD_ERROR = 2;
    public static final byte CMD_CHAT = 3;
    public static final byte CMD_PLAYER = 4;
    public static final byte CMD_GAME = 5;
    public static final byte CMD_ADMIN = 6;
    public static final byte PLAYER_JOIN = 7;
    public static final byte MESSAGE = 8;
    public static final byte ERROR = 9;
    public static final byte READY = 10;
    public static final byte NOTREADY = 11;
    public static final byte START_PLACING = 12;
    public static final byte BEGIN = 13;
    public static final byte MOVE = 14;
    public static final byte ATTACK = 15;
    public static final byte PLACE = 16;
    public static final byte GET_ARMIES = 17;
    public static final byte ATTACK_RESULT = 18;
    public static final byte MOVE_RESULT = 19;
    public static final byte GAME_STATUS = 20;
    public static final byte PLAYER_STATUS = 21;
    public static final byte COUNTRY_STATUS = 22;
    public static final byte DEFEAT = 23;
    public static final byte VICTORY = 24;
    public static final byte PLAYER_QUIT = 25;

    private InputStream input;
    private OutputStream output;
    private gamed.Server server;
    public Country[] countries;
    public int reserve;
    public int player_id;
    public int playing;
    
    public State(gamed.Server s) {
        server = s;
        try {
            java.net.Socket socket = s.getSocket();
            input = socket.getInputStream();
            output = socket.getOutputStream();
            byte cmd[] = new byte[4];
            cmd[0] = READY;
            output.write(cmd);
        } catch (IOException e) {
            s.quitGame();
        }

        countries = new Country[42];
        countries[ 0] = new Country(133, 114, 157, 131); // EasternUS
        countries[ 1] = new Country( 79,  52, 103,  65); // NorthwestTerritory
        countries[ 2] = new Country(100, 112, 117, 123); // WesternUS
        countries[ 3] = new Country(150,  84, 154,  97); // Ontario
        countries[ 4] = new Country(114, 148, 140, 168); // CentralAmerica
        countries[ 5] = new Country( 87,  84, 109,  92); // Alberta
        countries[ 6] = new Country( 99,   1, 187,  38); // Greenland
        countries[ 7] = new Country( 12,  49,  44,  62); // Alaska
        countries[ 8] = new Country(180,  76, 191,  92); // Quebec
        countries[ 9] = new Country(196, 198, 227, 219); // Brazil
        countries[10] = new Country(180, 184, 194, 191); // Venezuela
        countries[11] = new Country(189, 240, 197, 267); // Argentina
        countries[12] = new Country(177, 199, 197, 228); // Peru
        countries[13] = new Country(282,  65, 282,  62); // Iceland
        countries[14] = new Country(332, 113, 348, 118); // SouthernEurope
        countries[15] = new Country(362,  29, 388,  84); // Ukraine
        countries[16] = new Country(333,  50, 348,  66); // Scandinavia
        countries[17] = new Country(306,  88, 306,  92); // GreatBritan
        countries[18] = new Country(304, 108, 310, 119); // WesternEurope
        countries[19] = new Country(325,  90, 339,  99); // NorthernEurope
        countries[20] = new Country(340, 143, 358, 151); // Egypt
        countries[21] = new Country(341, 189, 353, 213); // Congo
        countries[22] = new Country(403, 231, 400, 236); // Madagascar
        countries[23] = new Country(349, 242, 359, 251); // SouthAfrica
        countries[24] = new Country(362, 166, 379, 190); // EastAfrica
        countries[25] = new Country(293, 136, 317, 167); // NorthAfrica
        countries[26] = new Country(409,  75, 428, 109); // Afghanistan
        countries[27] = new Country(484,  98, 517, 109); // Mongolia
        countries[28] = new Country(432,  43, 443,  78); // Ural
        countries[29] = new Country(560, 120, 569, 131); // Japan
        countries[30] = new Country(482,  81, 523,  85); // Irkutsk
        countries[31] = new Country(433, 142, 456, 156); // India
        countries[32] = new Country(489, 157, 495, 167); // Siam
        countries[33] = new Country(519,  42, 546,  62); // Yakutsk
        countries[34] = new Country(455,   8, 483,  58); // Siberia
        countries[35] = new Country(451, 124, 491, 133); // China
        countries[36] = new Kamchat(560,  32, 601,  60); // Kamchatka
        countries[37] = new Country(372, 127, 398, 147); // MiddleEast
        countries[38] = new Country(562, 208, 573, 211); // NewGuinea
        countries[39] = new Country(498, 174, 520, 199); // Indonesia
        countries[40] = new Country(529, 228, 542, 247); // WesternAustralia
        countries[41] = new Country(569, 227, 572, 248); // EasternAustralia
    }
    
    public void tick() {
        byte cmd[] = new byte[4];
        try {
            input.read(cmd, 0, 4);
            switch(cmd[0]) {
                case CMD_GAME:
                    // TODO if its a CMD_LIST_PLAYERS, update players
                    break;
                case PLAYER_JOIN:
                    break;
                case READY:
                    break;
                case NOTREADY:
                    break;
                case START_PLACING:
                    // TODO do the right thing instead of this
                    cmd[0] = READY;
                    output.write(cmd);
                    break;
                case BEGIN:
                case MOVE:
                case ATTACK:
                case PLACE:
                    break;
                case GET_ARMIES:
                    reserve = cmd[3];
                    break;
                case ATTACK_RESULT:
                case MOVE_RESULT:
                    input.read(cmd, 0, 4);
                    countries[cmd[0]].set(cmd[1], cmd[2]);
                    input.read(cmd, 0, 4);
                    countries[cmd[0]].set(cmd[1], cmd[2]);
                    break;
                case GAME_STATUS:
                    for (int i=0; i<42; i++) {
                        input.read(cmd, 0, 4);
                        countries[cmd[0]].set(cmd[1], cmd[2]);
                    }
                    break;
                case PLAYER_STATUS:
                    player_id = cmd[1];
                    playing = cmd[2];
                    reserve = cmd[3];
                    break;
                case COUNTRY_STATUS:
                    input.read(cmd, 0, 4);
                    countries[cmd[0]].set(cmd[1], cmd[2]);
                    break;
                case DEFEAT:
                case VICTORY:
                case PLAYER_QUIT:
            }
        } catch (IOException e) {
            server.quitGame();
        }
    }
    
    private void Command(byte command, byte subcommand, String value) throws IOException {
        byte valueBytes[] = value.getBytes();
        int len = valueBytes.length;
        byte cmd[] = new byte[len+4];
        cmd[0] = command;
        cmd[1] = subcommand;
        cmd[2] = (byte)(0xff & (len >>8));
        cmd[3] = (byte)(0xff & len);
        for (int i=0; i<len; i++) {
            cmd[i+4] = valueBytes[i];
        }
        output.write(cmd);
        output.flush();
    }
}
