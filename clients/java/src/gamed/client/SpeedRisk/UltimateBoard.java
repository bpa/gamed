package gamed.client.SpeedRisk;

public class UltimateBoard extends RiskBoard
{
    public static PlayerRenderer[] RENDERERS = new PlayerRenderer[]
    {
        
    };
    public UltimateBoard()
    {
        super("images/ultimate/world.png", null, RENDERERS, 1200, 720);
    }

    @Override
    public boolean borders(int c1, int c2)
    {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
