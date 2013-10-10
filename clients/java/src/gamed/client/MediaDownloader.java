package gamed.client;

import gamed.Server;
import java.awt.Image;
import java.awt.image.ImageObserver;
import java.util.Collection;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicInteger;
import javax.swing.SwingWorker;

public class MediaDownloader extends SwingWorker implements ImageObserver
{
    private final Server server;
    private final Collection<MediaRequestor> mediaRequestors;
    AtomicInteger total = new AtomicInteger(0);
    AtomicInteger completed = new AtomicInteger(0);
    ConcurrentMap<Image, Callback> callbacks = new ConcurrentHashMap<Image, Callback>();

    public MediaDownloader(Server server, Collection<MediaRequestor> mediaRequestors)
    {
        this.server = server;
        this.mediaRequestors = mediaRequestors;
    }

    @Override
    protected Object doInBackground()
    {
        for (MediaRequestor mediaRequestor : mediaRequestors)
        {
            for (String request : mediaRequestor.getMediaRequests())
            {
                total.incrementAndGet();
                Image image = server.getImage(request);
                callbacks.put(image, new Callback(mediaRequestor, request));
                image.getWidth(this);
            }
        }
        while (completed.get() < total.get())
        {
            try
            {
                Thread.sleep(100);
            }
            catch (java.lang.InterruptedException e)
            {
                return null;
            }
        }
        return null;
    }

    public boolean imageUpdate(Image image, int infoflags, int x, int y, int width, int height)
    {
		if ((infoflags & ImageObserver.ERROR) > 0)
		{
			Callback callback = callbacks.get(image);
			System.err.println("Error: " + callback.media);
			setProgress(completed.incrementAndGet() * 100 / total.get());
			return false;
		}
        if ((infoflags & ImageObserver.ALLBITS) == 0)
            return true;

        Callback callback = callbacks.get(image);
        setProgress(completed.incrementAndGet() * 100 / total.get());
			if ((infoflags & ImageObserver.ERROR) == 0)
				callback.requestor.mediaCompleted(callback.media, image);
        return false;
    }

    private static class Callback
    {
        public final MediaRequestor requestor;
        public final String media;

        public Callback(MediaRequestor requestor, String media)
        {
            this.requestor = requestor;
            this.media = media;
        }
    }
}
