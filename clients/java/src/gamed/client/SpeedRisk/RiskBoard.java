package gamed.client.SpeedRisk;

import gamed.client.MediaRequestor;
import java.awt.Graphics;
import java.awt.Image;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

public abstract class RiskBoard implements MediaRequestor
{
    final String background;
    final Country[] countries;
    final int width, height;
    Image bg = null;

    public RiskBoard(String background, Country[] countries, int width, int height)
    {
        this.background = background;
        this.countries = countries;
        this.width = width;
        this.height = height;
    }

    public abstract boolean borders(int c1, int c2);

    public Collection<MediaRequestor> getMediaRequestors()
    {
        List<MediaRequestor> requestors = new ArrayList(countries.length + 1);
        requestors.add(this);
        requestors.addAll(Arrays.asList(countries));
        return requestors;
    }

    void paintComponent(Graphics g)
    {
        if (bg == null)
        {
            g.clearRect(0, 0, width, height);
        }
        else
        {
            g.drawImage(bg, 0, 0, null);
            for (Country c : countries)
            {
                c.paint(g);
                c.paintIcon(g);
            }
        }
    }

    public Iterable<String> getMediaRequests()
    {
        List<String> requests = new ArrayList(1);
        requests.add(background);
        return requests;
    }

    public void mediaCompleted(String request, Image img)
    {
        bg = img;
    }
}
