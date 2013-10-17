package gamed.client.SpeedRisk;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JLabel;

public class RiskPlayer extends PlayerPanel implements Comparable<RiskPlayer>, ActionListener
{
    public final int id;
    public JCheckBox playerDisplay = new JCheckBox();
	public boolean playing = false;

    public RiskPlayer(int id, PlayerRenderer renderer)
    {
        this.id = id;
        this.renderer = renderer;
        playerDisplay.setFocusable(false);
        playerDisplay.setRequestFocusEnabled(false);
        playerDisplay.setRolloverEnabled(false);
        playerDisplay.addActionListener(this);
		playerDisplay.setOpaque(false);
        setLayout(new BorderLayout());
        add(playerDisplay, BorderLayout.CENTER);
    }

    void setReady(boolean b)
    {
        playerDisplay.setSelected(b);
    }

    void setPlayerName(String name)
    {
        playerDisplay.setText(name);
		playing = true;
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

	@Override
	public String toString()
	{
		StringBuilder sb = new StringBuilder();
		sb.append(id).append(": ");
		sb.append(playerDisplay.getText()).append(" (");
		sb.append(renderer).append(")");
		return sb.toString();
	}

	void mediaReady()
	{
		removeAll();
		playerDisplay.setForeground(renderer.textColor);
		add(playerDisplay, BorderLayout.CENTER);
		if (renderer.icon != null)
			add(new JLabel(new ImageIcon(renderer.icon)), BorderLayout.EAST);
	}
}
