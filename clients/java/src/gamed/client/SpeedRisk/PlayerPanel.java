package gamed.client.SpeedRisk;

import java.awt.BorderLayout;
import java.awt.Graphics;
import javax.swing.JPanel;

public class PlayerPanel extends JPanel
{
	protected PlayerRenderer renderer = null;

	public PlayerPanel()
	{
		super(new BorderLayout());
	}

	public void setRenderer(PlayerRenderer renderer)
	{
		this.renderer = renderer;
		repaint();
	}

	@Override
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		if (renderer != null)
			renderer.renderBackground(g, 0, 0, getWidth(), getHeight());
	}
}
