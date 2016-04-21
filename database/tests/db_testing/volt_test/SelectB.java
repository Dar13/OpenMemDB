//Stored procedure for selects
//This procedure is necessary since AdHoc is super slow

import org.voltdb.*;

public class SelectB extends VoltProcedure
{
    public final SQLStmt SelectB = new SQLStmt(
    "SELECT TestT0.B FROM TestT0;");

    public VoltTable[] run() throws VoltAbortException
    {
        voltQueueSQL(SelectB);
        return voltExecuteSQL();
    }
}

