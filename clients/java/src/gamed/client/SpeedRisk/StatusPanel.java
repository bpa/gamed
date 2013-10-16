package gamed.client.SpeedRisk;

import gamed.Player;
import gamed.Server;
import java.awt.BorderLayout;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class StatusPanel extends JPanel
{
	private JPanel phaseBG = new JPanel(new BorderLayout());
	private JLabel phaseLabel = new JLabel("Waiting for players");
	private JPanel reserveBG = new JPanel(new BorderLayout());
	private JLabel reserveLabel = new JLabel("0 armies in reserve");
	private final Map<Integer, RiskPlayer> players = new ConcurrentHashMap<Integer, RiskPlayer>();
	private RiskPlayer player = null;
	private final RiskBoard board;
	private final Server server;

	public StatusPanel(RiskBoard board, Server server)
	{
		this.board = board;
		this.server = server;
		GridLayout gridLayout = new GridLayout(2, 1);
		setLayout(gridLayout);
		phaseBG.add(phaseLabel, BorderLayout.CENTER);
		add(phaseBG);
		reserveBG.add(reserveLabel, BorderLayout.CENTER);
		add(reserveBG);
		adjustSize();
	}

	private void adjustSize()
	{
		FontMetrics fontMetrics = getFontMetrics(getFont());
		int height = fontMetrics.getHeight();
		int columns = ((GridLayout) getLayout()).getRows();
		setBounds(10, board.height - height * columns - 10, 150, height * columns);
	}

	void setPhase(String phase)
	{
		phaseLabel.setText(phase);
	}

	void setReserve(int reserve)
	{
		reserveLabel.setText(reserve + " armies in reserve");
	}

	public void setOwner(RiskPlayer player)
	{
		this.player = player;
		int height = super.getGraphics().getFontMetrics().getHeight();
		player.renderer.renderBackground(phaseBG.getGraphics(), phaseBG.getX(), phaseBG.getY(), 150, height);
		player.renderer.renderBackground(reserveBG.getGraphics(), reserveBG.getX(), reserveBG.getY(), 150, height);
	}

	public RiskPlayer get(int playerId)
	{
		RiskPlayer riskPlayer = players.get(playerId);
		if (riskPlayer == null)
		{
			riskPlayer = new RiskPlayer(playerId, new PlayerRenderer());
			players.put(playerId, riskPlayer);
			reAddPlayers();
		}
		return riskPlayer;
	}

	void resetReady()
	{
		for (RiskPlayer riskPlayer : players.values())
		{
			riskPlayer.setReady(false);
		}
	}

	void updatePlayers(Player[] players)
	{
		Set<Integer> seen = new HashSet();
		for (Player p : players)
		{
			seen.add(p.id);
			RiskPlayer riskPlayer = get(p.id);
			riskPlayer.setPlayerName(p.name);
			riskPlayer.renderer.setTheme(server, p.theme);
		}
		for (Integer id : this.players.keySet())
		{
			if (!seen.contains(id))
				this.players.remove(id);
		}
		reAddPlayers();
	}

	private void reAddPlayers()
	{
		List<RiskPlayer> list = new ArrayList(players.size());
		for (RiskPlayer riskPlayer : players.values())
		{
			if (riskPlayer.playing)
				list.add(riskPlayer);
		}
		Collections.sort(list);
		int items = 2 + list.size();
		removeAll();
		((GridLayout) getLayout()).setRows(items);
		for (RiskPlayer p : list)
		{
			add(p);
		}
		add(phaseBG);
		add(reserveBG);
		adjustSize();
	}

	@Override
	protected void paintComponent(Graphics g)
	{
		player.renderer.renderBackground(g, getX(), getY(), getWidth(), getHeight());

		for (RiskPlayer riskPlayer : players.values())
		{
			riskPlayer.repaint();
		}
	}
}
