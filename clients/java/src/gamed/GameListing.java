package gamed;

/**
 *
 * @author bruce
 */
public interface GameListing {
    public void updateAvailableGames(GameInstance[] games);
    public void updateGameInstances(String[] instances);
    public void handleStartGame();
    public void handleErrorMessage(String error);
    public void handleGameOver();
}
