package gamed;

/**
 * @author bruce
 */
public class GameInstance {
    String name;
    String version;
    int instances;
    public GameInstance(String data) {
        String[] pieces = data.split(":");
        name = pieces[0];
        version = pieces[1];
        instances = Integer.parseInt(pieces[2]);
    }
    
    public String toString() {
        return String.format("%s(%d)", name, instances);
    }
}
