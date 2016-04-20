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
    private final ArrayList<String> starterInserts;
    private final ArrayList<String> mixedCommands;

    public Commands(List<String> commands) {
        this.createCommands = getCommands(commands, "CREATE");
        this.insertCommands = getCommands(commands, "INSERT");
        this.selectCommands = getCommands(commands, "SELECT");
        this.starterInserts = getBeginingInserts(commands);
        this.mixedCommands = getMixedCommands(commands);
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

    public ArrayList<String> getStarterInserts(){return starterInserts;}

    public ArrayList<String> getMixedCommands() {
        return mixedCommands;
    }

    private ArrayList<String> getCommands(List<String> commands, String start) {
        return commands.stream().filter(s -> s.startsWith(start)).collect(Collectors.toCollection(ArrayList::new));
    }

    private ArrayList<String> getBeginingInserts(List<String> commands) {
        ArrayList<String> toReturn = new ArrayList<>();
        // Progress up to insert statements
        int i = 0;
        while (!commands.get(i).equalsIgnoreCase("insert") && i != commands.size()) {
            i++;
        }

        // Add all insert statements until you get to mix
        while (!commands.get(i).equalsIgnoreCase("mixed") && i != commands.size()) {
            // Test
            if (commands.get(i).startsWith("INSERT")) {
                toReturn.add(commands.get(i));
            }
            ++i;
        }

        return toReturn;
    }

    private ArrayList<String> getMixedCommands(List<String> commands) {
        ArrayList<String> toReturn = new ArrayList<>();
        // Move up to mixed statement
        int i = 0;
        while (!commands.get(i).equalsIgnoreCase("mixed") && i != commands.size()) {
            ++i;
        }

        // Move passed mixed word
        ++i;

        // Add all remaining commands to toReturn
        while (i < commands.size()) {
            if (commands.get(i).equalsIgnoreCase("end")) {
                break;
            }
            toReturn.add(commands.get(i));
            i++;
        }

        return toReturn;
    }
}
