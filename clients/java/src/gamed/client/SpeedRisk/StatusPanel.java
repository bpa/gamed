package gamed.client.SpeedRisk;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.GridLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class StatusPanel extends JPanel
{
    private JPanel phaseBG = new JPanel(new BorderLayout());
    private JLabel phaseLabel = new JLabel("Waiting for players");
    private JPanel reserveBG = new JPanel(new BorderLayout());
    private JLabel reserveLabel = new JLabel("0 armies in reserve");
    private final int boardHeight;

    public StatusPanel(int boardHeight)
    {
        this.boardHeight = boardHeight;
        GridLayout gridLayout = new GridLayout();
        setLayout(gridLayout);
        gridLayout.setColumns(2);
        phaseBG.add(phaseLabel, BorderLayout.CENTER);
        add(phaseBG);
        reserveBG.add(reserveLabel, BorderLayout.CENTER);
        add(reserveBG);
        adjustSize();
    }

    private void adjustSize()
    {
        int height = super.getGraphics().getFontMetrics().getHeight() + 3;
        int columns = ((GridLayout)getLayout()).getColumns();
        setBounds(10, boardHeight - height * columns - 10, 150, height * columns);
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
        phaseBG.setBackground(new Color(player.color));
        reserveBG.setBackground(new Color(player.color));
    }
}
