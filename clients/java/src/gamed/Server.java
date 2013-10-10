package gamed;

import java.awt.Image;
import java.net.URL;

/**
 * The purpose of this is to define the interface a plugin can use
 * to communicate with the server.  Some of this isn't strictly communication,
 * for example getImage()  its just convenient to not have to poke around
 * in the main class.
 * @author bruce
 */
public interface Server {
    public void quitGame();
    public void sendGameData(byte[] data);
    public void askForPlayerList();
    public Image getImage(String name);
	public URL getDocumentBase();
}
