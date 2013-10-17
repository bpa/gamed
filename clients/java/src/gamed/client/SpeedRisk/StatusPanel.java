package gamed.client.SpeedRisk;

import gamed.Player;
import gamed.Server;
import java.awt.BorderLayout;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.beans.PropertyChangeListener;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class StatusPanel extends JPanel
{
	private PlayerPanel phaseBG = new PlayerPanel();
	private JLabel phaseLabel = new JLabel("Waiting for players");
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
		phaseLabel.setOpaque(false);
		reserveLabel.setOpaque(false);
		phaseBG.add(phaseLabel, BorderLayout.CENTER);
		add(phaseBG);
		add(reserveLabel);
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
		this.phaseBG.setRenderer(player.renderer);
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

	void updatePlayers(Player[] players, PropertyChangeListener display)
	{
		Set<Integer> seen = new HashSet();
		for (Player p : players)
		{
			seen.add(p.id);
			RiskPlayer riskPlayer = get(p.id);
			riskPlayer.setPlayerName(p.name);
			riskPlayer.renderer.setTheme(server, p.theme, display);
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
		phaseBG.removeAll();
		phaseBG.add(phaseLabel, BorderLayout.CENTER);
		if (player != null)
		{
			phaseLabel.setForeground(player.renderer.textColor);
			reserveLabel.setForeground(player.renderer.textColor);
			if (player.renderer.icon != null)
				phaseBG.add(new JLabel(new ImageIcon(player.renderer.icon)), BorderLayout.EAST);
		}
		add(phaseLabel);
		add(reserveLabel);
		adjustSize();
	}

	@Override
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		player.renderer.renderBackground(g, 0, 0, getWidth(), getHeight());
	}

	void mediaReady()
	{
		for (RiskPlayer riskPlayer : players.values())
		{
			riskPlayer.mediaReady();
		}
		reAddPlayers();
	}
}
