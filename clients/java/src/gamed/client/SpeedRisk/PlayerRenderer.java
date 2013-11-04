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

public class PlayerRenderer
{
	public Theme theme = new Theme(null, null, null);

	synchronized void setTheme(Server server, String name, PropertyChangeListener display)
	{
		if (name.equals(theme.name))
			return;

		theme = new Theme(server, display, name);
		new Thread(theme).start();
	}

	public BufferedImage renderCountry(BufferedImage mask, int x, int y, boolean selected)
	{
		if (theme.background == null)
			return null;

		int w = mask.getWidth();
		int h = mask.getHeight();
		int bgw = theme.background.getWidth();
		int bgh = theme.background.getHeight();
		BufferedImage image = new BufferedImage(w, h, BufferedImage.TYPE_4BYTE_ABGR);

		long pre = 0;
		long post = 0;
		long a = theme.background.getRGB(0,0) & 0xFFFFFFFFL;
		if (selected)
		{
			pre = 0x80000000L;
			a = a ^ pre;
		}

		if (a < 0xC0000000L)
		{
			pre = pre ^ 0xFF000000L;
			post = 0xFF000000L;
		}

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				a = mask.getRGB(i, j) & 0xFFFFFFFFL;
				if (a == 0)
					continue;

				long p = theme.background.getRGB((i + x) % bgw, (j + y) % bgh) & 0xFFFFFFFFL;
				int color = (int) p & 0xFFFFFF;
				p = ((p ^ pre) * ((a >> 24) & 0xFF)) / 0xFF;
				image.setRGB(i, j, (int) ((p ^ post) & 0xFF000000) | color);
			}
		}

		return image;
	}

	public void renderBackground(Graphics g, int x, int y, int w, int h)
	{
		if (theme.background == null)
			return;

		g.setColor(Color.BLACK);
		g.fillRect(0, 0, w, h);
		int dstX = 0;
		while (dstX < w)
		{
			int dstY = 0;
			int srcX = (dstX + x) % theme.background.getWidth();
			int width = Math.min(theme.background.getWidth() - srcX, w - dstX);
			while (dstY < h)
			{
				int srcY = (dstY + y) % theme.background.getHeight();
				int height = Math.min(theme.background.getHeight() - srcY, w - dstY);
				g.drawImage(theme.background, dstX, dstY, dstX + width, dstY + height, srcX, srcY, width, height, null);
				dstY += height;
			}
			dstX += width;
		}
	}

	public void renderIcon(Graphics g, int x, int y, int armies)
	{
		if (theme.icon == null)
			return;

		int xCenter = x - (theme.icon.getWidth(null) / 2);
		int yCenter = y - (theme.icon.getHeight(null) / 2);
		int x2 = x + 2 + theme.x_offset;
		int y2 = y + 12 + theme.y_offset;
		g.drawImage(theme.icon, xCenter, yCenter, null);
		g.setColor(theme.textColor);
		g.drawString(Integer.toString(armies), x2, y2);
	}

	public class Theme implements Runnable, MediaRequestor
	{
		final Server server;
		final PropertyChangeListener display;
		final String name;
		Properties properties = null;
		BufferedImage background = null;
		Image icon = null;
		Color textColor = Color.BLACK;
		int x_offset, y_offset, armies_x, armies_y;

		public Theme(Server server, PropertyChangeListener display, String theme)
		{
			this.server = server;
			this.display = display;
			this.name = theme;
		}

		public void run()
		{
			try
			{
				URL url = new URL(server.getDocumentBase(), String.format("resources/themes/%s/theme.properties", name));
				URLConnection connection = url.openConnection();
				properties = new Properties();
				InputStream inputStream = connection.getInputStream();
				properties.load(inputStream);
				String bgImage = properties.getProperty("background-image");
				if (bgImage == null)
				{
					background = createImage(parseColor(properties.getProperty("background-color", "000")), false);
				}
				String iconImage = properties.getProperty("icon-image");
				if (iconImage == null)
					icon = createImage(parseColor(properties.getProperty("icon-color", "000")), true);
				textColor = new Color(parseColor(properties.getProperty("text-color", "000")));
				x_offset = intProperty("text-x", 0);
				y_offset = intProperty("text-y", 0);
				List<MediaRequestor> list = new ArrayList();
				list.add(this);
				MediaDownloader mediaDownloader = new MediaDownloader(server, list);
				mediaDownloader.addPropertyChangeListener(display);
				mediaDownloader.execute();
			}
			catch (MalformedURLException ex)
			{
				System.err.println(ex.getMessage());
				Logger.getLogger(PlayerRenderer.class.getName()).log(Level.SEVERE, null, ex);
			}
			catch (IOException ex)
			{
				System.err.println(ex.getMessage());
				Logger.getLogger(PlayerRenderer.class.getName()).log(Level.SEVERE, null, ex);
			}
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
			}
			else
			{
				icon = img;
			}
		}

		private BufferedImage createImage(int color, boolean icon)
		{
			BufferedImage i = new BufferedImage(18, 15, BufferedImage.TYPE_4BYTE_ABGR);
			Graphics2D g = i.createGraphics();
			g.setColor(new Color(color, true));
			if (icon)
				g.fill3DRect(0, 0, 18, 15, true);
			else
				g.fillRect(0, 0, 18, 15);
			return i;
		}

		private int parseColor(String color)
		{
			try
			{
				int colorNumber = 0;
				if (color == null)
					return colorNumber;

				if (color.startsWith("#"))
					color = color.substring(1);

				if (color.length() == 3)
				{
					StringBuilder sb = new StringBuilder();
					sb.append(color.charAt(0)).append(color.charAt(0));
					sb.append(color.charAt(1)).append(color.charAt(1));
					sb.append(color.charAt(2)).append(color.charAt(2));
					color = sb.toString();
				}

				if (color.length() == 8)
				{
					int high = Integer.parseInt(color.substring(0, 4), 16);
					int low = Integer.parseInt(color.substring(5), 16);
					return (high << 16) | low;
				}

				return 0xFF000000 | Integer.parseInt(color.toString(), 16);
			}
			catch (NumberFormatException e)
			{
				return 0;
			}
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

	@Override
	public String toString()
	{
		return theme.name;
	}
}
