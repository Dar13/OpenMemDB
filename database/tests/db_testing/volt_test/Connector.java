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

    /*
    //Does not use threads, runs sql statement parameter
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
        } catch(ProcCallException e)
        {
            System.out.println("Voltdb error");
            e.printStackTrace();
            return -1;
        } catch(NoConnectionsException e)
        {
            System.out.println("Client not found");
            e.printStackTrace();
            return -1;
        } catch(IOException e)
        {
            System.out.println("Network problem");
            e.printStackTrace();
            return -1;
        }
    }

    //Does use threads, perform sql statements to the database, assuming the connection has been made
    public long runThread(ArrayList<String> batch, int threadCount)
    {
        ArrayList<Thread> list = new ArrayList<Thread>();
        ArrayList<List<String>> split = splitBatch(batch, threadCount);
        final CyclicBarrier gate = new CyclicBarrier(threadCount+1);

        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                Runnable spawn = new Worker(db, split.get(i), gate);
                list.add(new Thread(spawn));
            }

            //execute threads
            list.forEach(Thread::start);
            long start;
            try
            {
                start = System.nanoTime();
                gate.await();
            } catch(BrokenBarrierException e)
            {
                e.printStackTrace();
                return -1;
            }

            for (Thread t: list)
            {
                t.join();
            }

            long stop = System.nanoTime();
            long executeTime = stop - start;

            return executeTime;

        } catch(Exception e)
        {
            e.printStackTrace();
            return -1; 
        }
    }

    //Stored Procedure method, different from AdHoc usage
    //generates its own sql statements and splits them up to each thread
    public long runProcedure(String procedure, int numSQLStmt, int threadCount)
    {
        ArrayList<Thread> list = new ArrayList<Thread>();
        ArrayList<String> dates = genDates(numSQLStmt);
        ArrayList<Long> values = genValues(numSQLStmt);
        final CyclicBarrier gate = new CyclicBarrier(threadCount+1);

        ArrayList<List<String>> date = splitBatch(dates, threadCount);
        ArrayList<List<Long>> value = splitLongArray(values, threadCount);

        try {
            //spawn threads
            for(int i = 0; i < threadCount; i++)
            {
                Runnable spawn = null;
                if(procedure.equalsIgnoreCase("insert"))
                {
                    spawn = new Worker(db, "insert", date.get(i), value.get(i), gate);
                }
                if(procedure.equalsIgnoreCase("select"))
                {
                    spawn = new Worker(db, "select", value.get(i), gate);
                }

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
            e.printStackTrace();
            return -1;
        }
    }
    */
    
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
            e.printStackTrace();
            return -1;
        }
    }

    //parse file contents with mixed sql instructions
    //grabs insert data, type of instruction in order, and select data
    private static ArrayList<Long> grabInsertData(ArrayList<String> content)
    {
        ArrayList<Long> insertData = new ArrayList<Long>();

        //parse each sql instruction and determine if it is SELECT OR INSERT
        for(int i = 0; i < content.size(); i++)
        {
            //if(content.get(i).contains("SELECT"))
            if(content.get(i).contains("INSERT"))
            {
                //remove string data around insert data
                //this is a static sql instruction with exception to the value in second column
                String frontStatement = "INSERT INTO TestT0 VALUES ('2016-04-12',";
                String backStatement = ");";
                String sqlStmt = content.get(i);
                sqlStmt = sqlStmt.replace(frontStatement,"");
                sqlStmt = sqlStmt.replace(backStatement,"");

                System.out.println(sqlStmt);
                long data = Long.valueOf(sqlStmt);
                insertData.add(data);
            }
        }

        return insertData;
    }

    private ArrayList<String> genDates(int size)
    {
        ArrayList<String> dates = new ArrayList<String>(); 
        String base = "2016-04-12;";
        //int baseYear = 2016;
        
        for(int i = 0; i < size; i++)
        {
            dates.add(base);
        //    dates.add(Integer.toString(baseYear).concat(base));
        //    baseYear++;
        }
        return dates;
    }

    private ArrayList<Long> genValues(int size)
    {
        ArrayList<Long> values = new ArrayList<Long>();
        for(long i = 0; i < size; i++)
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
    private ArrayList<List<Long>> splitLongArray(ArrayList<Long> batch, int divide)
    {
        if(divide == 0)
            System.out.println("Cannot divide by zero");

        int remainder = batch.size()%divide;
        int quotient = batch.size()/divide;
        ArrayList<List<Long>> splitArray = new ArrayList<List<Long>>();
        int previous = 0;
        int next = quotient;

        //split array into sublists according to their ranges
        for(int i = 0; i < divide-1; i++)
        {
            List<Long> temp = batch.subList(previous, next);
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
