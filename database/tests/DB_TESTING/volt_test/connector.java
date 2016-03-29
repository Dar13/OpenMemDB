/* 
 * Java application that connects to voltdb and measures the execution time
 * 
 */

import org.voltdb.*;
import org.voltdb.client.*;

public class connector
{
    private String server;
    private org.voltdb.client.Client db;

    public connector(String server)
    {
        this.server = server;
    }

    //establish a connection to the server
    public void init()
    {
        try {

            db = ClientFactory.createClient();
            db.createConnection(server);
        } catch(Exception e)
        {
            System.out.println("Could not create connection to voltdb");
            e.printStackTrace();
        }
    }

    //perform concurrent writes to the database, assuming the connection has been made
    public long insert(int threadCount)
    {
        thread[] list = new thread[threadCount];

        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                thread spawn = new thread(db, i);
                list[i] = spawn;
            }

            //execute threads
            long start = System.currentTimeMillis();
            for(int j = 0; j < threadCount; j++)
            {
                list[j].start('i');
            }
            long stop = System.currentTimeMillis();
            long executeTime = stop - start;

            return executeTime;

        } catch(Exception e)
        {
            e.printStackTrace();
            return -1; 
        }
    }

    /*
    //prints out database using a select query on test tables
    public void print()
    {
        VoltTable[] results;
        try {
            db.callProcedure("Print");
        } catch(Exception e)
        {
            e.printStackTrace();
        }
    }
    */

    public void close()
    {
        try {
            db.drain();
            db.close();
        } catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}
