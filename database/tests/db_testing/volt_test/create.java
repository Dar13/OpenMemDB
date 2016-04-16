import org.voltdb.*;

public class create extends VoltProcedure
{
    String sqlStmt;
    public create(String sqlStmt)
    {
        this.sqlStmt = sqlStmt;
    }

    public final SQLStmt createTable = new SQLStmt(sqlStmt);
    public void run () throws VoltAbortException
    {
        voltQueueSQL(createTable);
    }
}
