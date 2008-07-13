/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package gamed.client.SpeedRisk;

import java.awt.Graphics;
import java.awt.Image;
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

    public Kamchat(int img_x, int img_y, int label_x, int label_y) {
        x = img_x;
        y = img_y;
        lx = label_x;
        ly = label_y;
        x2 = 0;
        y2 = 57;
    }
    
    public void paint(Graphics g) {
        /*
        if (img == null) {
            if (overlay == null) { return; }
            img = makeBufferedImage(overlay);
            if (img == null) { return; }
        }
        if (img2 == null) {
            if (overlay2 == null) { return; }
            img2 = makeBufferedImage(overlay2);
            if (img2 == null) { return; }
        }
         */
        g.drawImage(img, x, y, null);
        g.drawImage(img2, x2, y2, null);
    }
}
