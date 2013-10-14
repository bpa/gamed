package gamed;

public class Player
{
	public String name;
	public int id;
	public String theme;
	private final String[] THEMES = new String[]
	{
		"biohazard", "chess", "radioactive", "circuitboard", "trees", "duck"
	};

	public Player(int id, String name)
	{
		this.name = name;
		this.id = id;
		this.theme = THEMES[id];
	}
}
