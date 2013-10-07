package gamed.client.SpeedRisk;

import javax.swing.JCheckBox;

public class RiskPlayer implements Comparable<RiskPlayer>
{
    public final int id;
    public final PlayerRenderer renderer;
    public JCheckBox playerDisplay = new JCheckBox();

    public RiskPlayer(int id, PlayerRenderer renderer)
    {
        this.id = id;
        this.renderer = renderer;
    }

    void setReady(boolean b)
    {
        playerDisplay.setSelected(b);
    }

    void setName(String name)
    {
        playerDisplay.setText(name);
    }

    public int compareTo(RiskPlayer t)
    {
        return id - t.id;
    }
}
