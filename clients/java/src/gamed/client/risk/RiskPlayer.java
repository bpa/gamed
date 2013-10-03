package gamed.client.risk;

import gamed.Player;
import javax.swing.JCheckBox;

public class RiskPlayer extends Player
{
    public boolean ready = false;
    public int inGameId = 0;
    private JCheckBox playerDisplay;
    
    public RiskPlayer(int id, String name)
    {
        super(id, name);
    }
}
