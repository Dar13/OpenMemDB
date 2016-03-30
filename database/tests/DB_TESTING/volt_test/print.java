import org.voltdb.*;

/*
 * A custom made procedure to print out all the contents of a table
 */
public class print extends VoltProcedure
{
    public final SQLStmt grabAll = new SQLStmt(
        "SELECT ID FROM TEST"
    );

    public VoltTable[] run()
    {
        voltQueueSQL(grabAll);
        return voltExecuteSQL();
    }
}
