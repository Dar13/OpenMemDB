//Stored procedure for selects
//This procedure is necessary since AdHoc is super slow

import org.voltdb.*;

public class find extends VoltProcedure
{
    public final SQLStmt find = new SQLStmt(
    "SELECT TestT0.B FROM TestT0 WHERE TestT0.B=?;");

    public VoltTable[] run(long value) throws VoltAbortException
    {
        voltQueueSQL(find, value);
        return voltExecuteSQL();
    }
}
