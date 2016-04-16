/* 
 * Java application that connects to voltdb and measures the execution time
 * 
 */

import org.voltdb.*;
import org.voltdb.client.*;

public class connector
{
    private class server
    {
        String name;
        int port;
    }

    private String host;
    private server[] nodes;
    private org.voltdb.client.Client db;

    public connector(String host)
    {
        this.host = host;
    }

    //TODO: Test if node client port is what Client.createConnection method requires
    //generate a list of client connections to connect to the cluster host
    public void generateServers(int size)
    {
        int hostPort = 7010;
        int offset = 1000;

        nodes = new server[size];
        for(int i = 0; i < size; i++)
        {
            nodes[i].name = "localhost";
            nodes[i].port = hostPort+i*offset;
        }
    }

    //establish a connection to the cluster host
    public void init()
    {
        try {

            //TODO: Make 64 not hardcoded but dynammic
            //initalize voltdb client and server list
            db = ClientFactory.createClient();
            generateServers(64);

            //connect nodes to host
            for(int i = 0; i < nodes.length; i++)
                db.createConnection(nodes[i].name, nodes[i].port);

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

    public void print(ClientResponse response)
    {
        //response would need to check if the custom made procedure returns success

        VoltTable results[];
        //db.callProcedure("Print");
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
