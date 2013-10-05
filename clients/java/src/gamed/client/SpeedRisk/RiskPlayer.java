package gamed.client.SpeedRisk;

import gamed.Player;
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import javax.swing.JCheckBox;

public class RiskPlayer extends Player
{
    public final int color;
    public boolean ready = false;
    public Image token;
    public JCheckBox playerDisplay;

    public RiskPlayer(int color, int tokenColor, int id, String name)
    {
        super(id, name);
        this.color = color;
        
        BufferedImage i = new BufferedImage(18, 15, BufferedImage.TYPE_4BYTE_ABGR);
        Graphics2D g = i.createGraphics();
        g.setColor(new Color((tokenColor | 0xDD000000), true));
        g.fill3DRect(0, 0, 18, 15, true);
    }
}
