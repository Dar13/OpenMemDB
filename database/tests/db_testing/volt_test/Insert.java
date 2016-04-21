//Stored procedure for selects
//This procedure is necessary since AdHoc is super slow

import org.voltdb.*;

public class Insert extends VoltProcedure
{
    public final SQLStmt Insert = new SQLStmt(
    "INSERT INTO TestT0 VALUES (?,?);");

    public VoltTable[] run(long value, long date) throws VoltAbortException
    {
        voltQueueSQL(Insert, value, date);
        return voltExecuteSQL();
    }
}
