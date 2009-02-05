package gamed;

import java.awt.Image;

/**
 * The purpose of this is to define the interface a plugin can use
 * to communicate with the server.  Some of this isn't strictly communication,
 * for example getImage()  its just conventient to not have to poke around
 * in the main class.
 * @author bruce
 */
public interface Server {
    void quitGame();
    void sendGameData(byte[] data);
    void askForPlayerList();
    Image getImage(String name);
}
