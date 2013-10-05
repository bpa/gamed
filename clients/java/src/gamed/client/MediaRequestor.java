package gamed.client;

import java.awt.Image;

public interface MediaRequestor
{
    
    public Iterable<String> getMediaRequests();

    public void mediaCompleted(String request, Image img);
}
