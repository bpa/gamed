package gamed.client.SpeedRisk;

import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

public class RiskPlayer extends JPanel implements Comparable<RiskPlayer>, ActionListener
{
    public final int id;
    public final PlayerRenderer renderer;
    public JCheckBox playerDisplay = new JCheckBox();

    public RiskPlayer(int id, PlayerRenderer renderer)
    {
        this.id = id;
        this.renderer = renderer;
        playerDisplay.setFocusable(false);
        playerDisplay.setRequestFocusEnabled(false);
        playerDisplay.setRolloverEnabled(false);
        playerDisplay.addActionListener(this);
        setLayout(new BorderLayout());
        add(playerDisplay, BorderLayout.CENTER);
    }

    void setReady(boolean b)
    {
        playerDisplay.setSelected(b);
    }

    void setPlayerName(String name)
    {
        System.err.println(String.format("%d: Setting name to %s", id, name));
        playerDisplay.setText(name);
    }

    public int compareTo(RiskPlayer t)
    {
        return id - t.id;
    }

    public void actionPerformed(ActionEvent e)
    {
        playerDisplay.setSelected(!playerDisplay.isSelected());
    }

    @Override
    protected void paintComponent(Graphics g)
    {
        renderer.renderBackground(g, getX(), getY(), getWidth(), getHeight());
    }

    @Override
    public boolean equals(Object that)
    {
        if (that instanceof RiskPlayer)
            return this.id == ((RiskPlayer) that).id;
        return false;
    }

    @Override
    public int hashCode()
    {
        return id;
    }
}