package gamed.client.SpeedRisk;

import gamed.client.MediaRequestor;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.util.ArrayList;

public class PlayerRenderer implements MediaRequestor
{
    final String bgFile;
    final int bgColor;
    final String iconFile;
    final int iconColor;
    BufferedImage background;
    Image icon;

    public PlayerRenderer(String bgFile, int bgColor, String iconFile, int iconColor)
    {
        this.bgFile = bgFile;
        this.bgColor = bgColor;
        this.iconFile = iconFile;
        this.iconColor = iconColor;
        if (iconFile == null)
            icon = createImage(iconColor, true);
        if (bgFile == null)
            background = createImage(bgColor, false);
    }

    private BufferedImage createImage(int color, boolean icon)
    {
        BufferedImage i = new BufferedImage(18, 15, BufferedImage.TYPE_4BYTE_ABGR);
        Graphics2D g = i.createGraphics();
        g.setColor(new Color((color | 0xDD000000), true));
        if (icon)
            g.fill3DRect(0, 0, 18, 15, true);
        else
            g.fillRect(0, 0, 18, 15);
        return i;
    }

    public Iterable<String> getMediaRequests()
    {
        ArrayList images = new ArrayList(2);
        if (background == null)
            images.add(bgFile);
        if (icon == null)
            images.add(iconFile);
        return images;
    }

    public void mediaCompleted(String request, Image img)
    {
        if (bgFile.equals(request))
            background = makeBufferedImage(img);
        else
            icon = img;
    }

    public void renderCountry(BufferedImage image, int x, int y)
    {
        if (background == null)
            return;

        int w = image.getWidth();
        int h = image.getHeight();
        int bgw = background.getWidth();
        int bgh = background.getHeight();
        int p, v;
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                v = image.getRGB(i, j);
                p = background.getRGB((i + x) % bgw, (j + y) % bgh);
                image.setRGB(i, j, (v & 0xff000000) | p);
            }
        }
    }

    public void renderBackground(Graphics g, int x, int y, int w, int h)
    {
        int dstX = 0;
        int dstY = 0;
        while (dstX < w && dstY < h)
        {
            int srcX = (dstX + x) % background.getWidth();
            int srcY = (dstY + y) % background.getHeight();
            int width = Math.min(background.getWidth() - srcX, w - dstX);
            int height = Math.min(background.getHeight()- srcY, w - dstY);
            g.drawImage(background, dstX, dstY, width, height, srcX, srcY, width, height, null);
            dstX += width;
            dstY += height;
        }
    }

    public void renderIcon(Graphics g, int x, int y, int armies)
    {
        g.drawImage(icon, x, y, null);
        g.setColor(Color.BLACK);
        g.drawString(Integer.toString(armies), x + 2, y + 12);
    }

    protected BufferedImage makeBufferedImage(Image o)
    {
        int w = o.getWidth(null);
        if (w == -1)
        {
            return null;
        }
        BufferedImage i = new BufferedImage(w, o.getHeight(null), BufferedImage.TYPE_4BYTE_ABGR);
        Graphics2D g = i.createGraphics();
        g.drawImage(o, 0, 0, null);
        return i;
    }
}
