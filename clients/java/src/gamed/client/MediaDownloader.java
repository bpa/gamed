package gamed.client;

import gamed.Server;
import gamed.client.SpeedRisk.Kamchat;
import gamed.client.risk.Country;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.swing.JProgressBar;

public class MediaDownloader extends JProgressBar
{
    private final ExecutorService executor = Executors.newFixedThreadPool(1);
    private final Server server;

    public MediaDownloader(Server server)
    {
        this.server = server;
    }

    public void load(Country country)
    {
        executor.submit(new CountryLoader(country));
    }

    private class CountryLoader implements Runnable
    {
        private final Country country;

        public CountryLoader(Country country)
        {
            this.country = country;
        }

        public void run()
        {
            country.overlay = server.getImage(country.imageFile);
            if (country instanceof Kamchat)
                ((Kamchat) country).overlay2 = server.getImage("images/classic/c36b.png");
        }
    }
}
