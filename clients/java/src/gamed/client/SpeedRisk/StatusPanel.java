package gamed.client.SpeedRisk;

import gamed.Player;
import gamed.Server;
import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import javax.swing.BoxLayout;
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
    private final Set<Integer> displayedPlayers = new HashSet();
    private final Server server;
    private final Display display;
    private final JButton quitButton = new JButton("Quit Game");
    private final JComboBox defenderPreference = new JComboBox(new String[]
    {
        "Leave 1", "Leave 2", "Leave 3", "Leave 4", "Leave 5", "Leave 6", "Leave 7", "Leave 8", "Leave 9", "Leave 10"
    });

    public StatusPanel(final Server server, Display display)
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
        this.server = server;
        this.display = display;
        BoxLayout boxLayout = new BoxLayout(this, BoxLayout.Y_AXIS);
        setLayout(boxLayout);
        add(defenderPreference);
        phaseLabel.setOpaque(false);
        reserveLabel.setOpaque(false);
        reserveLabel.setAlignmentX(CENTER_ALIGNMENT);
        phaseBG.add(phaseLabel, BorderLayout.CENTER);
        quitButton.setAlignmentX(CENTER_ALIGNMENT);
        add(phaseBG);
        add(reserveLabel);
        add(quitButton);
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(this);
    }

    void setPhase(Phase phase)
    {
        this.phase = phase;
        phaseLabel.setText(phase.label);
        switch (phase)
        {
            case AT_WAR:
                armyGenerationTimer = new ArmyGenerationTimer(armyGenerationProgress);
                add(armyGenerationProgress, 0);
                break;
            case GAME_OVER:
                armyGenerationTimer.stop();
                remove(armyGenerationProgress);
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
        Set<Integer> active = new HashSet();
        int ind = phase == Phase.AT_WAR ? 2 : 1;
        for (Player p : players)
        {
            active.add(p.id);
            RiskPlayer riskPlayer = get(p.id);
            riskPlayer.setPlayerName(p.name);
            riskPlayer.renderer.setTheme(server, p.theme, display);
            if (!displayedPlayers.contains(p.id))
            {
                displayedPlayers.add(p.id);
                add(riskPlayer, ind);
            }
        }
        for (Integer pid : displayedPlayers)
        {
            if (!active.contains(pid))
            {
                displayedPlayers.remove(pid);
                remove(this.players.get(pid));
            }
        }
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
