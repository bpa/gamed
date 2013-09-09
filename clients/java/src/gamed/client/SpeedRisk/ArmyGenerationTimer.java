package gamed.client.SpeedRisk;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JProgressBar;
import javax.swing.Timer;

public class ArmyGenerationTimer implements ActionListener
{
	Timer timer;
	JProgressBar armyGenerationProgress;
	int duration;
	long target;

	public ArmyGenerationTimer(JProgressBar armyGenerationProgress)
	{
		this.armyGenerationProgress = armyGenerationProgress;
	}

	public void set(int b)
	{
		if (timer == null)
		{
			timer = new Timer(50, this);
			timer.setInitialDelay(50);
			timer.start();
		}
		duration = b * 1000;
		armyGenerationProgress.setValue(0);
		armyGenerationProgress.setMaximum(duration);
		target = System.currentTimeMillis() + duration;
		armyGenerationProgress.setString(String.format("%.1f s", duration / 1000d));
	}

	public void stop()
	{
		if (timer != null)
			timer.stop();
	}

	public void actionPerformed(ActionEvent ae)
	{
		long left = target - System.currentTimeMillis();
		if (left < 0)
			left = 0;
		int completed = duration - (int) left;
		armyGenerationProgress.setValue(completed);
		armyGenerationProgress.setString(String.format("%.1f s", left / 1000d));
	}
}
