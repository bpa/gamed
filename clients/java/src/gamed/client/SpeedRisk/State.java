package gamed.client.SpeedRisk;

/**
 *
 * @author bruce
 */
public class State {

    public static final int CMD_NOP = 0;
    public static final int CMD_INVALID = 1;
    public static final int CMD_ERROR = 2;
    public static final int CMD_CHAT = 3;
    public static final int CMD_PLAYER = 4;
    public static final int CMD_GAME = 5;
    public static final int CMD_ADMIN = 6;
    public static final int PLAYER_JOIN = 7;
    public static final int MESSAGE = 8;
    public static final int ERROR = 9;
    public static final int READY = 10;
    public static final int NOTREADY = 11;
    public static final int START_PLACING = 12;
    public static final int BEGIN = 13;
    public static final int MOVE = 14;
    public static final int ATTACK = 15;
    public static final int PLACE = 16;
    public static final int GET_ARMIES = 17;
    public static final int ATTACK_RESULT = 18;
    public static final int MOVE_RESULT = 19;
    public static final int GAME_STATUS = 20;
    public static final int PLAYER_STATUS = 21;
    public static final int COUNTRY_STATUS = 22;
    public static final int DEFEAT = 23;
    public static final int VICTORY = 24;
    public static final int PLAYER_QUIT = 25;

    private gamed.Server server;
    public Country[] countries;
    
    public State(gamed.Server s) {
        server = s;
    }
}
