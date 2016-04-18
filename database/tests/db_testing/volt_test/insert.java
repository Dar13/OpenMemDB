//Stored procedure for selects
//This procedure is necessary since AdHoc is super slow

import org.voltdb.*;

public class insert extends VoltProcedure
{
    public final SQLStmt insert = new SQLStmt(
    "INSERT INTO TestT0 VALUES (?,?);");

    public VoltTable[] run(long date, int value) throws VoltAbortException
    {
        voltQueueSQL(insert, date, value);
        return voltExecuteSQL();
    }
}
