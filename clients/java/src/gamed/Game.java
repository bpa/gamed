package gamed;

import javax.swing.JPanel;
/**
 *
 * @author bruce
 */
public abstract class Game extends JPanel {
    public abstract void joinedGame();
    public abstract void renamePlayer(Player player);
    public abstract void updatePlayers(Player[] players);
    public abstract void handleGameData(byte[] data);
}
