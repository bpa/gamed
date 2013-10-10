package gamed.client.SpeedRisk;

import gamed.Server;
import gamed.client.MediaDownloader;
import gamed.client.MediaRequestor;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

public class PlayerRenderer implements MediaRequestor
{
	String name = null;
	Properties properties = null;
	BufferedImage background = null;
	Image icon = null;
	int x_offset, y_offset, armies_x, armies_y;

	synchronized void setTheme(Server server, String theme)
	{
		if (theme.equals(name))
			return;

		name = theme;
		new Thread(new Theme(server, theme)).start();
	}

	public Iterable<String> getMediaRequests()
	{
		ArrayList images = new ArrayList(2);
		if (background == null)
			images.add(String.format("/themes/%s/%s", name, properties.getProperty("background-image")));
		if (icon == null)
			images.add(String.format("/themes/%s/%s", name, properties.getProperty("icon-image")));
		return images;
	}

	public void mediaCompleted(String request, Image img)
	{
		if (request.equals(properties.getProperty("background-image")))
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
				image.setRGB(i, j, (v & 0xff000000) | (p & 0x00FFFFFF));
			}
		}
	}

	public void renderBackground(Graphics g, int x, int y, int w, int h)
	{
		if (background == null)
			return;

		int dstX = 0;
		while (dstX < w)
		{
			int dstY = 0;
			int srcX = (dstX + x) % background.getWidth();
			int width = Math.min(background.getWidth() - srcX, w - dstX);
			while (dstY < h)
			{
				int srcY = (dstY + y) % background.getHeight();
				int height = Math.min(background.getHeight() - srcY, w - dstY);
				g.drawImage(background, dstX, dstY, dstX + width, dstY + height, srcX, srcY, width, height, null);
				dstY += height;
			}
			dstX += width;
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

	private PlayerRenderer ref()
	{
		return this;
	}

	private class Theme implements Runnable
	{
		final Server server;
		final String theme;

		public Theme(Server server, String theme)
		{
			this.server = server;
			this.theme = theme;
		}

		public void run()
		{
			try
			{
				URL url = new URL(server.getDocumentBase(), String.format("/themes/%s/theme.properties", theme));
				URLConnection connection = url.openConnection();
				properties = new Properties();
				InputStream inputStream = connection.getInputStream();
				properties.load(inputStream);
				String bgImage = properties.getProperty("background-image");
				if (bgImage == null)
					background = createImage(parseColor(properties.getProperty("background-color", "000")), false);
				String iconImage = properties.getProperty("icon-image");
				if (iconImage == null)
					icon = createImage(parseColor(properties.getProperty("image-color", "000")), true);
				List<MediaRequestor> list = new ArrayList();
				list.add(ref());
				MediaDownloader mediaDownloader = new MediaDownloader(server, list);
				mediaDownloader.execute();
			}
			catch (MalformedURLException ex)
			{
				Logger.getLogger(PlayerRenderer.class.getName()).log(Level.SEVERE, null, ex);
			}
			catch (IOException ex)
			{
				Logger.getLogger(PlayerRenderer.class.getName()).log(Level.SEVERE, null, ex);
			}
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

		private int parseColor(String color)
		{
			int colorNumber = 0;
			if (color == null)
				return colorNumber;

			if (color.startsWith("#"))
				color = color.substring(1);

			if (color.length() != 3)
				return Integer.parseInt(color, 16);

			StringBuilder sb = new StringBuilder();
			sb.append(color.charAt(0)).append(color.charAt(0));
			sb.append(color.charAt(1)).append(color.charAt(1));
			sb.append(color.charAt(2)).append(color.charAt(2));
			return Integer.parseInt(sb.toString(), 16);
		}
	}
}
