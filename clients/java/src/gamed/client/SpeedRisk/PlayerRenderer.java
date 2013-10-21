package gamed.client.SpeedRisk;

import gamed.Server;
import gamed.client.MediaDownloader;
import gamed.client.MediaRequestor;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.beans.PropertyChangeListener;
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
	Color textColor = Color.BLACK;
	int x_offset, y_offset, armies_x, armies_y;
	boolean backgroundGenerated = true;

/*
	public PlayerRenderer()
	{
		background = new BufferedImage(1, 1, BufferedImage.TYPE_4BYTE_ABGR);
		background.setRGB(0, 0, 0xff000000);
	}
*/

	synchronized void setTheme(Server server, String theme, PropertyChangeListener display)
	{
		if (theme.equals(name))
			return;

		name = theme;
		new Thread(new Theme(server, display, theme)).start();
	}

	public Iterable<String> getMediaRequests()
	{
		ArrayList images = new ArrayList(2);
		if (background == null)
			images.add(String.format("resources/themes/%s/%s", name, properties.getProperty("background-image")));
		if (icon == null)
			images.add(String.format("resources/themes/%s/%s", name, properties.getProperty("icon-image")));
		return images;
	}

	public void mediaCompleted(String request, Image img)
	{
		String property = properties.getProperty("background-image");
		if (property != null && request.endsWith(property))
		{
			background = makeBufferedImage(img);
			backgroundGenerated = false;
		}
		else
		{
			icon = img;
		}
	}

	public void renderCountry(BufferedImage image, int x, int y, boolean selected)
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
				if (image.getRGB(i, j) != 0)
				{
					p = background.getRGB((i + x) % bgw, (j + y) % bgh);
					if (selected)
					{
						p = p & 0x00FFFFFF;
						p = p | 0x90000000;
					}
					image.setRGB(i, j, p);
				}
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
		if (icon == null)
			return;

		int xCenter = x - (icon.getWidth(null) / 2);
		int yCenter = y - (icon.getHeight(null) / 2);
		int x2 = x + 2 + x_offset;
		int y2 = y + 12 + y_offset;
		g.drawImage(icon, xCenter, yCenter, null);
		g.setColor(textColor);
		g.drawString(Integer.toString(armies), x2, y2);
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
		final PropertyChangeListener display;
		final String theme;

		public Theme(Server server, PropertyChangeListener display, String theme)
		{
			this.server = server;
			this.display = display;
			this.theme = theme;
		}

		public void run()
		{
			try
			{
				URL url = new URL(server.getDocumentBase(), String.format("resources/themes/%s/theme.properties", theme));
				URLConnection connection = url.openConnection();
				properties = new Properties();
				InputStream inputStream = connection.getInputStream();
				properties.load(inputStream);
				String bgImage = properties.getProperty("background-image");
				if (bgImage == null)
					background = createImage(parseColor(properties.getProperty("background-color", "000")), false);
				String iconImage = properties.getProperty("icon-image");
				if (iconImage == null)
					icon = createImage(parseColor(properties.getProperty("icon-color", "000")), true);
				textColor = new Color(parseColor(properties.getProperty("text-color", "000")));
				x_offset = intProperty("text-x", 0);
				y_offset = intProperty("text-y", 0);
				List<MediaRequestor> list = new ArrayList();
				list.add(ref());
				MediaDownloader mediaDownloader = new MediaDownloader(server, list);
				mediaDownloader.addPropertyChangeListener(display);
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

		private int intProperty(String property, int i)
		{
			try
			{
				return Integer.parseInt(properties.getProperty(property));
			}
			catch (NumberFormatException e)
			{
				return i;
			}
		}
	}

	@Override
	public String toString()
	{
		return name;
	}
}
