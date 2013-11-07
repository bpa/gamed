package gamed.client.SpeedRisk;

import gamed.Player;
import gamed.Server;
import java.awt.BorderLayout;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.beans.PropertyChangeListener;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;

public class StatusPanel extends JPanel implements KeyEventDispatcher
{
	private final PlayerPanel phaseBG = new PlayerPanel();
	private final JLabel phaseLabel = new JLabel("Waiting for players");
	private final JLabel reserveLabel = new JLabel("0 armies in reserve");
	private Phase phase = Phase.WAITING_FOR_PLAYERS;
	private RiskPlayer player = null;
	private ArmyGenerationTimer armyGenerationTimer;
	private final JProgressBar armyGenerationProgress = new JProgressBar();
	private final Map<Integer, RiskPlayer> players = new ConcurrentHashMap<Integer, RiskPlayer>();
	private final RiskBoard board;
	private final Server server;
	private final PropertyChangeListener display;
	private final JButton quitButton = new JButton("Quit Game");
	private final JComboBox defenderPreference = new JComboBox(new String[]
	{
		"Leave 1", "Leave 2", "Leave 3", "Leave 4", "Leave 5", "Leave 6", "Leave 7", "Leave 8", "Leave 9", "Leave 10"
	});

	public StatusPanel(RiskBoard board, final Server server, PropertyChangeListener display)
	{
		quitButton.setDefaultCapable(false);
		quitButton.setFocusable(false);
		quitButton.setRequestFocusEnabled(false);
		quitButton.addActionListener(new java.awt.event.ActionListener()
		{
			public void actionPerformed(java.awt.event.ActionEvent evt)
			{
				server.quitGame();
			}
		});
		this.board = board;
		this.server = server;
		this.display = display;
		GridLayout gridLayout = new GridLayout(4, 1);
		setLayout(gridLayout);
		add(defenderPreference);
		phaseLabel.setOpaque(false);
		reserveLabel.setOpaque(false);
		phaseBG.add(phaseLabel, BorderLayout.CENTER);
		add(phaseBG);
		add(reserveLabel);
		add(quitButton);
		adjustSize();
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(this);
	}

	private void adjustSize()
	{
		FontMetrics fontMetrics = getFontMetrics(getFont());
		int height = fontMetrics.getHeight();
		int columns = ((GridLayout) getLayout()).getRows();
		setBounds(10, board.height - height * columns - 10, 150, height * columns);
	}

	void setPhase(Phase phase)
	{
		this.phase = phase;
		phaseLabel.setText(phase.label);
		switch (phase)
		{
			case AT_WAR:
				armyGenerationTimer = new ArmyGenerationTimer(armyGenerationProgress);
				reAddPlayers();
				break;
			case GAME_OVER:
				armyGenerationTimer.stop();
				reAddPlayers();
				break;
		}
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

	void updatePlayers(Player[] players)
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
		int items = 4 + list.size();
		if (phase == Phase.AT_WAR)
			items++;
		removeAll();
		((GridLayout) getLayout()).setRows(items);
		if (phase == Phase.AT_WAR)
			add(armyGenerationProgress);
		add(defenderPreference);
		for (RiskPlayer p : list)
		{
			add(p);
		}
		phaseBG.removeAll();
		phaseBG.add(phaseLabel, BorderLayout.CENTER);
		if (player != null)
		{
			phaseLabel.setForeground(player.renderer.theme.textColor);
			reserveLabel.setForeground(player.renderer.theme.textColor);
			if (player.renderer.theme.icon != null)
				phaseBG.add(new JLabel(new ImageIcon(player.renderer.theme.icon)), BorderLayout.EAST);
		}
		add(phaseBG);
		add(reserveLabel);
		add(quitButton);
		adjustSize();
	}

	@Override
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		if (player != null)
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

	int getDefenders()
	{
		return defenderPreference.getSelectedIndex() + 1;
	}

	public boolean dispatchKeyEvent(KeyEvent ke)
	{
		if (ke.getID() == KeyEvent.KEY_PRESSED)
		{
			char keyChar = ke.getKeyChar();
			if (keyChar == '0')
				defenderPreference.setSelectedIndex(9);
			else if ('1' <= keyChar && keyChar <= '9')
				defenderPreference.setSelectedIndex(keyChar - '1');
			else
				return false;
			return true;
		}
		return false;
	}

	void setArmyGenerationTime(byte b)
	{
		armyGenerationTimer.set(b);
	}

	public enum Phase
	{
		WAITING_FOR_PLAYERS("Waiting for players"),
		PLACING_ARMIES("Placing Armies"),
		AT_WAR("At War"),
		GAME_OVER("Game Over");
		public final String label;

		private Phase(String label)
		{
			this.label = label;
		}
	}
}
