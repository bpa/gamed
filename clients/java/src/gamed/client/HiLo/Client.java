package gamed.client.HiLo;

import java.io.IOException;
import java.net.Socket;
import java.io.InputStream;
import java.io.OutputStream;
import gamed.Server;

/**
 *
 * @author bruce
 */

public class Client {
    public static final int GUESS = 7;
    public static final int GIVEUP = 8;
    public static final int TOO_LOW = 9;
    public static final int TOO_HIGH = 10;
    public static final int CORRECT = 11;
    
    Socket sock;
    Server server;
    InputStream input;
    OutputStream output;
    
    public Client(gamed.Server s) {
        server = s;
        sock = s.getSocket();
        try {
            input = sock.getInputStream();
            output = sock.getOutputStream();
        } catch (IOException e) {
            server.handleIOException(e);
        }
    }
    
    public void quit() {
        byte cmd[] = {GIVEUP, 0, 0 ,0};
        try {
            output.write(cmd);
            output.flush();
            input.read(cmd, 0, 4);
        } catch (IOException e) {
            server.handleIOException(e);
        }
    }
    
    public Result guess(byte g) {
        byte cmd[] = { GUESS, g, 0, 0 };
        try {
            output.write(cmd);
            output.flush();
            input.read(cmd, 0, 4);
            System.out.format("%d %d %d %d\n",cmd[0],cmd[1],cmd[2],cmd[3]);
            switch (cmd[0]) {
                case TOO_LOW:
                    return Result.TOO_LOW;
                case TOO_HIGH:
                    return Result.TOO_HIGH;
                default:
                    return Result.CORRECT;
            }
        } catch (IOException e) {
            server.handleIOException(e);
        }
        return Result.CORRECT;
    }
}
