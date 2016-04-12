import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * Created by mike on 4/11/16.
 */
public class Commands {
    private final ArrayList<String> createCommands;
    private final ArrayList<String> insertCommands;
    private final ArrayList<String> selectCommands;

    public Commands(List<String> commands) {
        this.createCommands = getCommands(commands, "CREATE");
        this.insertCommands = getCommands(commands, "INSERT");
        this.selectCommands = getCommands(commands, "SELECT");
    }

    public ArrayList<String> getCreateCommands() {
        return createCommands;
    }

    public ArrayList<String> getInsertCommands() {
        return insertCommands;
    }

    public ArrayList<String> getSelectCommands() {
        return selectCommands;
    }

    private ArrayList<String> getCommands(List<String> commands, String start) {
        return commands.stream().filter(s -> s.startsWith(start)).collect(Collectors.toCollection(ArrayList::new));
    }
}
