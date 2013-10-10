package gamed.client.SpeedRisk;

public class UltimateBoard extends RiskBoard
{
	public static final Country[] COUNTRIES = new Country[]
	{
	};

    public UltimateBoard()
    {
        super("images/ultimate/world.png", null, 1200, 720);
    }

	private static boolean[][] BORDERS = new boolean[][] {
	};

    @Override
    public boolean borders(int c1, int c2)
    {
        if (c1 >= COUNTRIES.length || c2 >= COUNTRIES.length)
			return false;
		return BORDERS[c1][c2];
    }
}
