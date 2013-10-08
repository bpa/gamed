package gamed.client.SpeedRisk;

import gamed.Player;
import java.awt.BorderLayout;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.util.*;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class StatusPanel extends JPanel
{
    private JPanel phaseBG = new JPanel(new BorderLayout());
    private JLabel phaseLabel = new JLabel("Waiting for players");
    private JPanel reserveBG = new JPanel(new BorderLayout());
    private JLabel reserveLabel = new JLabel("0 armies in reserve");
    private Map<Integer, RiskPlayer> players = new HashMap();
    private RiskPlayer player = null;
    private final RiskBoard board;

    public StatusPanel(RiskBoard board)
    {
        this.board = board;
        GridLayout gridLayout = new GridLayout(2, 1, 0, 3);
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
        int height = fontMetrics.getHeight() + 3;
        int columns = ((GridLayout) getLayout()).getColumns();
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
            riskPlayer = new RiskPlayer(playerId, board.renderers[playerId]);
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
            riskPlayer.setName(p.name);
        }
        Set<Integer> existing = this.players.keySet();
        existing.removeAll(seen);
        if (!existing.isEmpty())
        {
            for (Integer integer : existing)
            {
                this.players.remove(integer);
            }
            reAddPlayers();
        }
    }

    private void reAddPlayers()
    {
        List<RiskPlayer> list = new ArrayList(players.size());
        list.addAll(players.values());
        Collections.sort(list);
        int items = 2 + list.size();
        removeAll();
        ((GridLayout) getLayout()).setColumns(items);
        for (RiskPlayer p : list)
        {
            add(p.playerDisplay);
        }
        add(phaseBG);
        add(reserveBG);
        adjustSize();
    }
}
