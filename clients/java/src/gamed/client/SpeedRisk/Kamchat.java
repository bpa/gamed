/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package gamed.client.SpeedRisk;

import java.awt.Graphics;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.image.BufferedImage;
/**
 *
 * @author bruce
 */
public class Kamchat extends Country {
    
    private int x2;
    private int y2;
    public Image overlay2;
    private BufferedImage img2;
    private Rectangle bounds2;

    public Kamchat(int img_x, int img_y, int label_x, int label_y) {
        x = img_x;
        y = img_y;
        lx = label_x;
        ly = label_y;
        x2 = 0;
        y2 = 57;
    }
    
    public void init() {
        super.init();
        img2 = makeBufferedImage(overlay2);
        bounds2 = new Rectangle(x2, y2, img2.getWidth(), img2.getHeight());
    }
    
    public void paint(Graphics g) {
        g.drawImage(img, x, y, null);
        g.drawImage(img2, x2, y2, null);
    }

    public void setSelected(boolean selected) {
        isSelected = selected;
        if (isSelected) {
            colorSelectedImage(img);
            colorSelectedImage(img2);
        } else {
            colorImage(img);
            colorImage(img2);
        }
    }

    public boolean contains(java.awt.Point p) {
        if (super.contains(p)) {
            return true;
        }
        if (bounds2.contains(p)) {
            int ix = p.x - x2;
            int iy = p.y - y2;
            int color = img2.getRGB(ix, iy);
            if ((color & 0xff000000) != 0) {
                return true;
            }
        }
        return false;
    }

}