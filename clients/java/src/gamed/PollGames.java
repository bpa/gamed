package gamed;

class PollGames extends Thread {
    private Client client;
    private volatile boolean running;
    
    PollGames(Client client) {
        this.client = client;
    }

    @Override
    public void run() {
        running = true;
        while(running) {
            try {
                client.askForAvailableGames();
                sleep(3000);
            }
            catch (java.lang.InterruptedException e) {
                running = false;
            }
        }
    }
    
    public void halt() {
        running = false;
    }
}
