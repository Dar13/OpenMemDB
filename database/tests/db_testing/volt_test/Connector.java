/* 
 * Java application that connects to voltdb and measures the execution time
 * 
 */

import org.voltdb.*;
import org.voltdb.client.*;
import java.util.ArrayList;
import java.sql.*;
import java.io.*;
import java.util.List;
import java.util.concurrent.*;

public class Connector
{
    private String[] serverName;
    private int[] serverPort;
    private org.voltdb.client.Client db;
    private Connection conn;

    //generate a list of client connections to connect to the cluster host
    public void generateServers(int size, int port)
    {
        //admin port necessary to connect to servers and offset is port increment
        int offset = 6;

        serverName = new String[size];
        serverPort = new int[size];
        for(int i = 0; i < size; i++)
        {
            serverName[i] = "localhost";
            serverPort[i] = port + i*offset;
        }
    }

    //establish a connection to the cluster host
    public void init(int size)
    {
        try {

            //initalize voltdb client and server list
            int port = 21212;
            ClientConfig config = new ClientConfig("advent","xyzzy");
            config.setProcedureCallTimeout(90*10000);
            config.setConnectionResponseTimeout(180 * 10000);
            db = ClientFactory.createClient(config);
            generateServers(size, port);

            //connect nodes to host
            for(int i = 0; i < size; i++)
                db.createConnection("localhost", 21212);

        } catch(Exception e)
        {
            System.out.println("Failed to connect to a server");
            e.printStackTrace();
        }
    }
    
    //Stored Procedure method, different from AdHoc usage
    //reads in sql statements in a batch and splits them up to each thread
    public long runProcedure(ArrayList<String> batch, int threadCount)
    {
        ArrayList<Thread> list = new ArrayList<Thread>();
        final CyclicBarrier gate = new CyclicBarrier(threadCount+1);

        //list of sql instructions passed to each thread
        ArrayList<List<String>> mixedSQL = splitBatch(batch, threadCount);

        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                //System.out.println("Thread #"+i+ "is running");
                //for(int k = 0; k < mixedSQL.get(i).size(); k++)
                //    System.out.println(mixedSQL.get(i).get(k));
                Runnable spawn = null;
                spawn = new Worker(db, mixedSQL.get(i), gate);

                list.add(new Thread(spawn));
            }

            //execute threads
            long start = System.currentTimeMillis();
            list.forEach(Thread::start);
            try
            {
                gate.await();
            } catch(Exception e)
            {
                System.out.println("Gate could not invoke await()");
                e.printStackTrace();
            }

            for(Thread t:list)
            {
                t.join();
            }

            long stop = System.currentTimeMillis();
            long executeTime = stop - start;

            return executeTime;

        } catch(Exception e)
        {
            System.out.println("runProcedure() threw an error");
            e.printStackTrace();
            return -1;
        }
    }

    private void printList(List<String> list)
    {
        for(int i = 0; i < list.size(); i++)
        {
            System.out.println(list.get(i));
        }
    }

    //splits sql statements evenly to threads
    private ArrayList<List<String>> splitBatch(ArrayList<String> batch, int divide)
    {
        if(divide == 0)
            System.out.println("Cannot divide by zero");

        int remainder = batch.size()%divide;
        int quotient = batch.size()/divide;
        ArrayList<List<String>> splitArray = new ArrayList<List<String>>();
        int previous = 0;
        int next = quotient;

        //split array into sublists according to their ranges
        for(int i = 0; i < divide-1; i++)
        {
            List<String> temp = batch.subList(previous, next);
            splitArray.add(temp);
            previous = previous + quotient;
            next = next + quotient;
        }

        //last split has remainder added to end
        splitArray.add(batch.subList(previous, next+remainder));
        return splitArray;
    }

    public void close()
    {
        try {
            db.drain();
            db.close();
        } catch(Exception e)
        {
            System.out.println("Couldn't close connection");
            e.printStackTrace();
        }
    }

    public void shutdown()
    {
        try
        {
            db.callProcedure("@Shutdown");
        } catch(org.voltdb.client.ProcCallException e)
        {
            System.out.println("Database shutting down");
        } catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}
