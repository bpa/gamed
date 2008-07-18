package gamed;

import javax.swing.JPanel;
/**
 *
 * @author bruce
 */
public abstract class Game extends JPanel {
    abstract public void start();
    abstract public void stop();
}
