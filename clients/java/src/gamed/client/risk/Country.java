package gamed.client.risk;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;

/**
 *
 * @author bruce
 */
public class Country
{
    protected static int[] country_colors =
    {
        0xb22222, // Firebrick red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x191970, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };
    protected static int[] token_colors =
    {
        0xcd5c5c, // Indian red
        0x7cfc00, // Lawn green
        0xffffff, // White
        0x1e90ff, // Dodger blue
        0xcd950c, // Dark goldenrod 3
        0xffbbff  // Plum 1
    };
    public Image overlay;
    public final int id;
    protected BufferedImage img;
    protected int owner;
    protected int armies;
    protected int x;
    protected int y;
    protected int lx;
    protected int ly;
    protected boolean isSelected;
    protected Rectangle bounds;
    protected Rectangle iconBounds;
    protected volatile boolean initialized = false;
    public final String imageFile;

    public Country(int id, String imageFile, int img_x, int img_y, int label_x, int label_y)
    {
        this.id = id;
        this.imageFile = imageFile;
        isSelected = false;
        x = img_x;
        y = img_y;
        lx = label_x;
        ly = label_y;
    }

    public void init()
    {
        img = makeBufferedImage(overlay);
        bounds = new Rectangle(x, y, img.getWidth(), img.getHeight());
        iconBounds = new Rectangle(lx, ly, 18, 15);
        initialized = true;
        colorImage(img);
    }

    public void setOwner(int o)
    {
        owner = o;
        setSelected(isSelected);
    }

    public void set(int owner, int armies)
    {
        this.owner = owner;
        this.armies = armies;
        setSelected(isSelected);
    }

    public void set(int owner, int armies, boolean selected)
    {
        this.owner = owner;
        this.armies = armies;
        setSelected(selected);
    }

    public void setSelected(boolean selected)
    {
        isSelected = selected;
        if (!initialized)
        {
            return;
        }
        if (isSelected)
        {
            colorSelectedImage(img);
        }
        else
        {
            colorImage(img);
        }
    }

    public boolean contains(java.awt.Point p)
    {
        if (iconBounds.contains(p))
        {
            return true;
        }
        if (bounds.contains(p))
        {
            int ix = p.x - x;
            int iy = p.y - y;
            int color = img.getRGB(ix, iy);
            if ((color & 0xff000000) != 0)
            {
                return true;
            }
        }
        return false;
    }

    public void paint(Graphics g)
    {
        g.drawImage(img, x, y, null);
    }

    public void paintIcon(Graphics g)
    {
        g.setColor(new Color((token_colors[owner] | 0xDD000000), true));
        g.fill3DRect(lx, ly, iconBounds.width, iconBounds.height, true);
        g.setColor(Color.BLACK);
        g.drawString(Integer.toString(armies), lx + 2, ly + 12);
    }

    protected void colorImage(BufferedImage image)
    {
        int w = image.getWidth();
        int h = image.getHeight();
        int v;
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                v = image.getRGB(i, j);
                image.setRGB(i, j, (v & 0xff000000) | country_colors[owner]);
            }
        }
    }

    protected void colorSelectedImage(BufferedImage image)
    {
        int w = image.getWidth();
        int h = image.getHeight();
        int v;
        for (int i = 0; i < w; i++)
        {
            for (int j = 0; j < h; j++)
            {
                v = image.getRGB(i, j);
                image.setRGB(i, j, v & 0xff000000);
            }
        }
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
