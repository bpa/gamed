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

	synchronized void setTheme(Server server, String theme, PropertyChangeListener display)
	{
		if (theme.equals(name))
			return;

		System.out.println("Setting theme to " + theme);
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
						byte o = (byte) (p >> 24);
						o += 128;
						p = (o << 24) | (p & 0x00FFFFFF);
					}
					image.setRGB(i, j, p);
				}
			}
		}

		if ((background.getRGB(0, 0) >> 24) < 192)
			addBorderEffect(image, w, h, 0xFF000000, -1);
		else
			addBorderEffect(image, w, h, 0x01000000, 1);
	}

	private void addBorderEffect(BufferedImage image, int w, int h, int start, int step)
	{
		int i = 0;
		int[] stack = new int[4096];
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				if (x == 0 || y == 0 || x == (w - 1) || y == (h - 1) || image.getRGB(x, y) == 0)
				{
					i = addAdjacentPixels(stack, i, x, y, w, h, image, start, step);
				}
				while (i > 0)
				{
					i -= 3;
					i = addAdjacentPixels(stack, i, stack[i], stack[i + 1], w, h, image, stack[i + 2], step);
				}
			}
		}
	}

	private int addAdjacentPixels(int[] stack, int i, int x, int y, int w, int h, BufferedImage image, int start, int step)
	{
		int next;
		if (step > 0)
			next = start << 1;
		else
			next = start >> 1;

		if (next < 0x01000000)
			return i;
			
		i = addPixel(stack, i, x, y - 1, w, h, image, next, step);
		i = addPixel(stack, i, x - 1, y, w, h, image, next, step);
		i = addPixel(stack, i, x, y + 1, w, h, image, next, step);
		i = addPixel(stack, i, x + 1, y, w, h, image, next, step);
		return i;
	}

	private int addPixel(int[] stack, int i, int x, int y, int w, int h, BufferedImage image, int start, int step)
	{
		if (x < 0 || y < 0 || x == w || y == h)
			return i;

		int v = image.getRGB(x, y);
		if (v == 0)
			return i;

		int opacity = v & 0xFF000000;
		if (step > 0)
			if (opacity >= start)
				return i;
		else
			if (opacity <= start)
				return i;
		
		image.setRGB(x, y, (v & 0x00FFFFFF) | start);
		stack[i] = x;
		stack[i + 1] = y;
		stack[i + 2] = start;
		return i + 3;
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
	}

	@Override
	public String toString()
	{
		return name;
	}

	private static class T
	{
		private final int x;
		private final int y;
		private final int opacity;

		public T(int x, int y, int opacity)
		{
			this.x = x;
			this.y = y;
			this.opacity = opacity;
		}
	}
}
