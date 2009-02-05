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
    public static final byte CMD_GAME_MESSAGE = 6;
    
    public Socket socket;
    private GameListing login;
    private java.io.InputStream input;
    private java.io.OutputStream output;
        
    /**
     * I suppose I shouldn't connect and rename the client, but its sort of
     * part of the nonexistant login.
     * @todo Implement a real login function and have that called instead
     * @todo Bug #11, update CMD_RENAME
     * @param host
     * @param user
     */
    public Client(String host, String user, GameListing login) {
        this.login = login;
        user = user.replace(":", ";");
        try {
            socket = new Socket(host, 7483);
            input = socket.getInputStream();
            output = socket.getOutputStream();
            sendCommand(CMD_PLAYER, CMD_RENAME, user);
        } catch (IOException e) {
            System.err.println("Couldn't create new Socket");
            close();
            return;
        }
    }
    
    public void askForAvailableGames() {
        sendCommand(CMD_GAME, CMD_LIST_GAMES, null);
    }
    
    public void askForGameInstances(String game) {
        sendCommand(CMD_GAME, CMD_LIST_GAME_INSTANCES, game);
    }
    
    public void askForPlayerList() {
        sendCommand(CMD_GAME, CMD_LIST_PLAYERS, null);
    }
    
    public void createGame(String game, String name) {
        // TODO revisit serialization
        game = game.replace(":", ";");
        name = name.replace(":", ";");
        sendCommand(CMD_GAME, CMD_CREATE_GAME, game+":"+name);
    }
    
    public void joinGame(String game, String name) {
        game = game.replace(":", ";");
        name = name.replace(":", ";");
        sendCommand(CMD_GAME, CMD_JOIN_GAME, game+":"+name);
    }
       
    private void sendCommand(byte command, byte subcommand, String value) {
        if (socket == null) { return; }
        int len = 0;
        byte valueBytes[];
        if (value == null) {
            valueBytes = new byte[0];
        }
        else {
            valueBytes = value.getBytes();
        }
        len = valueBytes.length;
        byte cmd[] = new byte[len + 4];
        cmd[0] = command;
        cmd[1] = subcommand;
        cmd[2] = (byte) (0xff & (len >> 8));
        cmd[3] = (byte) (0xff & len);
        for (int i = 0; i < len; i++) {
            cmd[i + 4] = valueBytes[i];
        }
        try {
            output.write(cmd);
            output.flush();
        }
        catch (IOException e) {
            close();
        }

    }

    public void sendGameData(byte[] data) {
       if (socket == null) { return; }
       if (data == null || data.length == 0) { return; }
       byte[] msg = new byte[4];
       msg[0] = CMD_GAME;
       msg[1] = CMD_GAME_MESSAGE;
       msg[2] = (byte) (0xff & (data.length >> 8));
       msg[3] = (byte) (0xff & data.length);
       try {
           output.write(msg);
           output.write(data);
           output.flush();
       }
       catch(IOException e) {
           close();
       }
    }
    
    /**
     * Sends a command to the server to exit the current game
     */
    public void quit() {
        sendCommand(CMD_GAME, CMD_QUIT_GAME,  null);
    }
    
    private void close() {
        if (socket != null) {
            try {
                socket.shutdownOutput();
                socket.shutdownInput();
                socket.close();
            } catch (IOException e) {}
            finally {
                socket = null;
            }
        }
        input = null;
        output = null;
    }
    
    /**
     * Does a blocking read on the socket and deals with the result.  On error, it resets the connection
     * @param game
     * @return false on IOException
     */
    public boolean poll(Game game) {
        if (socket == null) { return false; }
        byte[] cmd = new byte[4];
        try {
            int read = input.read(cmd);
            if (read==0) { return true; }
            if (read==-1) { 
                close();
                return false;
            }
        }
        catch (IOException e) {
            close();
            return false;
        }
        switch(cmd[0]) {
            case CMD_NOP:
            case CMD_INVALID:
                break;
            case CMD_ERROR:
                switch(cmd[1]) {
                    case ERR_GAME_FULL:
                        login.handleErrorMessage("Game is Full");
                        break;
                    case ERR_NO_GAME:
                        login.handleErrorMessage("No game by that name exists");
                        break;
                    case ERR_IN_GAME:
                        break;
                }
                break;
            case CMD_CHAT:
                break;
            case CMD_PLAYER:
                switch (cmd[1]) {
                    case CMD_RENAME:
                        handlePlayerUpdate(cmd, game);
                        break;
                }
                break;
            case CMD_GAME:
                switch(cmd[1]) {
                    case CMD_LIST_GAMES:
                        handleListGames(cmd);
                        break;
                    case CMD_LIST_GAME_INSTANCES:
                        handleListInstances(cmd);
                        break;
                    case CMD_CREATE_GAME:
                    case CMD_JOIN_GAME:
                        login.handleStartGame();
                        sendCommand(CMD_GAME, CMD_LIST_PLAYERS, null);
                        break;
                    case CMD_LIST_PLAYERS:
                        handleListPlayers(cmd, game);
                        break;
                    case CMD_QUIT_GAME:
                        login.handleGameOver();
                        break;
                    case CMD_GAME_MESSAGE:
                        handleGameMessage(cmd, game);
                        break;
                }
            break;
        }
        return true;
    }
    
    private byte[] getRemainingMessage(byte[] cmd) {
        int z = (((cmd[2] & 0xff) << 8) | (cmd[3] & 0xff));
        byte[] info = new byte[z];
        if (z == 0) {
            return info;
        }
        try {
            input.read(info);
        }
        catch (IOException e) {
            close();
            return null;
        }
        return info;
    }
    
    /**
     * This could use some better serialization
     * @param cmd
     */
    private void handleListGames(byte[] cmd) {
        byte[] info = getRemainingMessage(cmd);
        if (info != null) {
            String parse = new String(info);
            String[] g = parse.trim().split("\n");
            GameInstance[] games = new GameInstance[(int)(g.length)];
            for (int i=0; i<games.length; i++) {
                games[i] = new GameInstance(g[i]);
            }
            login.updateAvailableGames(games);
        }
    }

    private void handleListInstances(byte[] cmd) {
        byte[] info = getRemainingMessage(cmd);
        if (info != null) {
            String parse = new String(info);
            String[] instances = parse.trim().split(":");
            login.updateGameInstances(instances);
        }
    }
    
    private void handleGameMessage(byte[] cmd, Game game) {
        if (game == null) { return; }
        byte[] msg = getRemainingMessage(cmd);
        if (msg != null && msg.length > 0) {
            game.handleGameData(msg);
        }
    }
    
    /**
     * This could use serialization help as well
     * @param cmd
     */
    private void handleListPlayers(byte[] cmd, Game game) {
        if (game == null) { return; }
        byte[] msg = getRemainingMessage(cmd);
        if (msg != null) {
            String list = new String(msg);
            String[] instances = list.trim().split(":");
            int playing = (int)(instances.length/2);
            Player[] players = new Player[playing];
            for (int i=0; i<players.length; i++) {
                players[i] = new Player(Integer.parseInt(instances[i*2+1]),instances[i*2+2]);
            }
            game.updatePlayers(players);
        }
    }
    
    private void handlePlayerUpdate(byte[] cmd, Game game) {
        if (game == null) { return; }
        byte[] msg = getRemainingMessage(cmd);
        if (msg != null) {
            String[] instances = msg.toString().trim().split(":");
            Player player = new Player(Integer.parseInt(instances[0]), instances[1]);
            game.renamePlayer(player);
        }
    }
}
