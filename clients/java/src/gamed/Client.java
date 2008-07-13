package gamed;
import java.io.IOException;
import java.net.*;

/**
 *
 * @author bruce
 */
public class Client {
    public static final byte CMD_NOP = 0;
    public static final byte CMD_INVALID = 1;
    public static final byte CMD_ERROR = 2;
    public static final byte CMD_CHAT = 3;
    public static final byte CMD_PLAYER = 4;
    public static final byte CMD_GAME = 5;
    public static final byte CMD_ADMIN = 6;
   
    public static final byte ERR_GAME_FULL = 0;
    public static final byte ERR_NO_GAME = 1;
    public static final byte ERR_IN_GAME = 2;
    
    public static final byte CMD_RENAME = 0;
    
    public static final byte CMD_LIST_GAMES = 0;
    public static final byte CMD_LIST_GAME_INSTANCES = 1;
    public static final byte CMD_CREATE_GAME = 2;
    public static final byte CMD_JOIN_GAME = 3;
    public static final byte CMD_LIST_PLAYERS = 4;
    public static final byte CMD_QUIT_GAME = 5;
    
    public Socket socket;
    private java.io.InputStream input;
    private java.io.OutputStream output;
        
    public Client(String host, String user) {
        try {
            socket = new Socket(host, 7483);
            input = socket.getInputStream();
            output = socket.getOutputStream();
            output.flush();
            byte userBytes[] = user.getBytes();
            int len = userBytes.length;
            byte cmd[] = new byte[len+4];
            cmd[0] = CMD_PLAYER;
            cmd[1] = CMD_RENAME;
            cmd[2] = (byte)(0xff & (len >> 8));
            cmd[3] = (byte)(0xff & len);
            for (int i=0; i<len; i++) {
                cmd[i+4] = userBytes[i];
            }
            output.write(cmd);
            output.flush();
        } catch (IOException e) {
            System.err.println("Couldn't create new Socket");
            return;
        }
    }
    
    public String[] list_games() {
        byte cmd[] = { CMD_GAME, CMD_LIST_GAMES, 0, 0};
        byte info[];
        String ret[];
        try {
            output.write(cmd);
            output.flush();
            input.read(cmd, 0, 4);
            int z = (((cmd[2] & 0xff) << 8) | (cmd[3] & 0xff));
            info = new byte[z];
            input.read(info, 0, z);
            String parse = new String(info);
            ret = parse.trim().split("\n");
        } catch (IOException e) {
            ret = new String[0];
        }
        return ret;
    }
    
    public String[] list_instances(String game) {
        byte cmd[] = new byte[4];
        byte info[];
        String ret[];
        try {
            sendCommand(CMD_GAME, CMD_LIST_GAME_INSTANCES, game);
            input.read(cmd, 0, 4);
            int z = (((cmd[2] & 0xff) << 8) | (cmd[3] & 0xff));
            info = new byte[z];
            input.read(info, 0, z);
            String parse = new String(info);
            ret = parse.trim().split(":");
        } catch (IOException e) {
            ret = new String[0];
        }
        return ret;
    }
    
    public boolean createGame(String game, String name) {
        byte cmd[] = new byte[4];
        try {
            sendCommand(CMD_GAME, CMD_CREATE_GAME, game+":"+name);
            input.read(cmd, 0, 4);
            if (cmd[0] == CMD_GAME) {
                return true;
            }
        } catch (IOException e) {
            // TODO figure out what to do here
        }
        return false;
    }
    
    public boolean joinGame(String game, String name) {
        byte cmd[] = new byte[4];
        try {
            sendCommand(CMD_GAME, CMD_JOIN_GAME, game+":"+name);
            input.read(cmd, 0, 4);
            if (cmd[0] == CMD_GAME) {
                return true;
            }
        } catch (IOException e) {
            // TODO figure out what to do here
        }
        return false;
    }
    
    private void sendCommand(byte command, byte subcommand, String value) throws IOException {
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
    
    public void quitGame() throws IOException {
        byte cmd[] = { CMD_GAME, CMD_QUIT_GAME, 0, 0 };
        output.write(cmd);
        input.read(cmd, 0, 4);
    }
    
    public void quit() {
        try {
            socket.close();
        } catch (java.io.IOException e){}
    }
    
}
