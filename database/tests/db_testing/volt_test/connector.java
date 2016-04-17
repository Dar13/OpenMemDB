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

public class connector
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
            int port = 12002;
            ClientConfig config = new ClientConfig("advent","xyzzy");
            config.setProcedureCallTimeout(90*10000);
            config.setConnectionResponseTimeout(180 * 10000);
            db = ClientFactory.createClient(config);
            generateServers(size, port);

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
        ArrayList<List<String>> split = splitBatch(batch, threadCount);
        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                thread spawn = new thread(db, split.get(i));
                //print(split.get(i));
                list[i] = spawn;
            }

            //execute threads
            long start = System.currentTimeMillis();
            for(int j = 0; j < threadCount; j++)
            {
                list[j].start('r');
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

    public long runProcedure(String procedure, int dataSize, int threadCount)
    {
        thread[] list = new thread[threadCount];
        ArrayList<String> date_tmp = genDates(dataSize);
        ArrayList<Integer> value_tmp = genValues(dataSize);

        ArrayList<List<String>> date = splitBatch(date_tmp, threadCount);
        ArrayList<List<Integer>> value = splitIntArray(value_tmp, threadCount);

        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                thread spawn = null;
                if(procedure == "insert")
                {
                    spawn = new thread(db, "insert", date.get(i), value.get(i));
                }
                if(procedure == "select")
                {
                    spawn = new thread(db, "select", value.get(i));
                }
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

    private ArrayList<String> genDates(int size)
    {
        ArrayList<String> dates = new ArrayList<String>(); 
        String base = "-04-12;";
        int baseYear = 2016;
        for(int i = 0; i < size; i++)
        {
            dates.add(Integer.toString(baseYear).concat(base));
            baseYear++;
        }
        return dates;
    }

    private ArrayList<Integer> genValues(int size)
    {
        ArrayList<Integer> values = new ArrayList<Integer>();
        for(int i = 0; i < size; i++)
        {
            values.add(i);
        }

        return values;
    }

    private void print(List<String> list)
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

    //splits sql statements evenly to threads
    private ArrayList<List<Integer>> splitIntArray(ArrayList<Integer> batch, int divide)
    {
        if(divide == 0)
            System.out.println("Cannot divide by zero");

        int remainder = batch.size()%divide;
        int quotient = batch.size()/divide;
        ArrayList<List<Integer>> splitArray = new ArrayList<List<Integer>>();
        int previous = 0;
        int next = quotient;

        //split array into sublists according to their ranges
        for(int i = 0; i < divide-1; i++)
        {
            List<Integer> temp = batch.subList(previous, next);
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
