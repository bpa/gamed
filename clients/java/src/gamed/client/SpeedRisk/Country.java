package gamed.client.SpeedRisk;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.Graphics;
import java.awt.Graphics2D;
/**
 *
 * @author bruce
 */
public class Country {
    protected static int[] country_colors = { 
        0xb22222, // Firebrick red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x191970, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };
    protected static int[] token_colors = {
        0xcd5c5c, // Indian red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x1e90ff, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };
    Image overlay;
    protected BufferedImage img;
    protected int owner;
    protected int armies;
    protected int x;
    protected int y;
    protected int lx;
    protected int ly;
    
    protected Country() {}
    
    public Country(int img_x, int img_y, int label_x, int label_y) {
        x = img_x;
        y = img_y;
        lx = label_x;
        ly = label_y;
    }
    
    public void setOwner(int o) {
        if (img == null) {
            img = makeBufferedImage(overlay);
        }
        owner = o;
        int w = img.getWidth();
        int h = img.getHeight();
        int v;
        for (int i=0; i<w; i++) {
            for (int j=0; j<h; j++) {
                v = img.getRGB(i, j);
                img.setRGB(i, j, (v & 0xff000000) | country_colors[o]);
            }
        }
    }
  
    public void set(int owner, int armies) {
        setOwner(owner);
        this.armies = armies;
    }
    
    public void paint(Graphics g) {
        if (img == null) {
//            if (overlay == null) { return; }
            img = makeBufferedImage(overlay);
//            if (img == null) { return; }
        }
        g.drawImage(img, x, y, null);
    }

    protected BufferedImage makeBufferedImage(Image o) {
        int w = o.getWidth(null);
        if (w == -1) {
            return null;
        }
        BufferedImage i = new BufferedImage(w, o.getHeight(null), BufferedImage.TYPE_4BYTE_ABGR);
        Graphics2D g = i.createGraphics();
        g.drawImage(overlay, 0, 0, null);
        return i;
    }
}
