package gamed;

import java.net.Socket;
import java.io.IOException;
import java.awt.Image;
import java.net.URL;

/**
 *
 * @author bruce
 */
public interface Server {
    void quitGame();
    void handleIOException(IOException e);
    Socket getSocket();
    Image getImage(String name);
}
