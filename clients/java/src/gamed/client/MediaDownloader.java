package gamed.client;

import gamed.Server;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.awt.image.ImageObserver;
import java.util.Collection;
import javax.swing.SwingWorker;

public class MediaDownloader extends SwingWorker implements ImageObserver
{
    private final Server server;
    private final Collection<MediaRequestor> mediaRequestors;

    public MediaDownloader(Server server, Collection<MediaRequestor> mediaRequestors)
    {
        this.server = server;
        this.mediaRequestors = mediaRequestors;
    }

    @Override
    protected Object doInBackground()
    {
        int completed = 0;
        for (MediaRequestor mediaRequestor : mediaRequestors)
        {
            for (String request : mediaRequestor.getMediaRequests())
            {
                System.err.println("Getting " + request);
                Image image = server.getImage(request);
                image.getWidth(this);
                mediaRequestor.mediaCompleted(request, image);
                setProgress(completed++ * 100 / mediaRequestors.size());
            }
        }
        return null;
    }

    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
    {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
