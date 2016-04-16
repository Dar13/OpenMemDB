/* 
 * Java application that connects to voltdb and measures the execution time
 * 
 */

import org.voltdb.*;
import org.voltdb.client.*;
import java.util.ArrayList;

public class connector
{
    private String[] serverName;
    private int[] serverPort;
    private org.voltdb.client.Client db;

    //generate a list of client connections to connect to the cluster host
    public void generateServers(int size)
    {
        //admin port necessary to connect to servers and offset is port increment
        int adminPort = 7001;
        int offset = 1000;

        serverName = new String[size];
        serverPort = new int[size];
        for(int i = 0; i < size; i++)
        {
            serverName[i] = "localhost";
            serverPort[i] = adminPort + i*offset;
        }
    }

    //establish a connection to the cluster host
    public void init(int size)
    {
        try {

            //initalize voltdb client and server list
            db = ClientFactory.createClient();
            generateServers(size);

            //connect nodes to host
            for(int i = 0; i < size; i++)
                db.createConnection(serverName[i], serverPort[i]);

        } catch(Exception e)
        {
            System.out.println("Failed to connect to a server");
            e.printStackTrace();
        }
    }

    //runs sql statements that are predefined from file, different from generating sql statements
    public long run(String batch)
    {
        //batch is a batch of sql statements
        try
        {
            long start = System.currentTimeMillis();
            db.callProcedure("@AdHoc", batch);
            long stop = System.currentTimeMillis();
            long executeTime = stop - start;

            return executeTime;
        } catch(Exception e)
        {
            System.out.println("Could not execute AdHoc command");
            e.printStackTrace();
            return -1;
        }
    }

    //perform concurrent writes to the database, assuming the connection has been made
    public long runThread(ArrayList<String> batch, int threadCount)
    {
        thread[] list = new thread[threadCount];

        try {

            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                thread spawn = new thread(db, batch);
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
