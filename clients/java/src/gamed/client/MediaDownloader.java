package gamed.client;

import gamed.Server;
import java.awt.Image;
import java.util.Collection;
import javax.swing.SwingWorker;

public class MediaDownloader extends SwingWorker
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
                Image image = server.getImage(request);
                mediaRequestor.mediaCompleted(request, image);
            }
            setProgress(completed / mediaRequestors.size());
        }
        return null;
    }
}
