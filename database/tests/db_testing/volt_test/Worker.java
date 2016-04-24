import java.util.*;
import java.io.*;
import java.util.concurrent.*;
import org.voltdb.client.*;
import org.voltdb.*;

public class Worker implements Runnable
{
    private org.voltdb.client.Client myApp;
    private List<String> batch_list;
    private CyclicBarrier gate;
    private VoltTable[] results;

    // each thread uses voltdb database, sql batch instructions
    public Worker(org.voltdb.client.Client myApp, List<String> batch, CyclicBarrier gate)
    {
        this.myApp = myApp;
        this.batch_list = batch;
        this.gate = gate;
    }

    //Special run Method to run batch sql statements, it parse the batch and determine which stored procedure is used
    @Override
    public void run()
    {
        ClientResponse response = null;
        try
        {
            //wait for gate controlled by main
            try
            {
                this.gate.await();
            } catch(InterruptedException e)
            {
                e.printStackTrace();
            } catch(BrokenBarrierException e)
            {
                e.printStackTrace();
            }

            //AdHoc executes multiple sql statements each seperated by ; threads their own instructions
            //myApp.callProcedure("@AdHoc", sqlStmt);
            // date and value are lists of all of the arguments that this thread should run the stored procedure with
            for(int j = 0; j < batch_list.size(); j++)
            {
                // grab sql statement from batch
                String flag = batch_list.get(j);
                long value = 0;

                if (flag.contains("INSERT")) {
                    // Leaving date static as the epoch
                    value = grabInsertData(flag);
                    response = myApp.callProcedure("Insert", value, 2);
                    if(response.getStatus() != ClientResponse.SUCCESS)
                    {
                        throw new RuntimeException(response.getStatusString());
                    }
                } else if (flag.contains("SELECT")) {
                    try
                    {
                        run_everywhere();
                    } catch(Exception e)
                    {
                        e.printStackTrace();
                    }
                }
            }

        } catch (NoConnectionsException e)
        {
            System.out.println("Couldn't connect to server");
            e.printStackTrace();
        } catch(IOException e)
        {
            e.printStackTrace();
        } catch(ProcCallException e)
        {
            System.out.println("Procedure exception");
            e.printStackTrace();
        }
    }

    private void run_everywhere() throws Exception
    {
        VoltTable results[] = myApp.callProcedure("@GetPartitionKeys", "INTEGER").getResults();
        VoltTable keys = results[0];

        for(int k = 0; k < keys.getRowCount(); k++)
        {
            long key = keys.fetchRow(k).getLong(1);
            VoltTable table = myApp.callProcedure("SelectB", key).getResults()[0];
            //System.out.println("Partition "+key+"row count = " + table.fetchRow(0).getLong(0));
        }
    }

    //useful for AdHoc since it takes in a multiple SQL statements as a String
    private String arrayToString(List<String> array)
    {
        String convertArray = "";
        for(int i = 0; i < array.size(); i++)
        {
            convertArray = convertArray.concat(array.get(i));
        }

        return convertArray;
    }

    //grabs insert data from sql statement, type of instruction in order, and select data
    private long grabInsertData(String sqlStmt)
    {
        long insertData = 0;
        if(sqlStmt.contains("INSERT"))
        {
            //remove string data around insert data
            //this is a static sql instruction with exception to the value in second column
            String frontStatement = "INSERT INTO TestT0 VALUES ('2016-04-12',";
            String backStatement = ");";
            sqlStmt = sqlStmt.replace(frontStatement,"");
            sqlStmt = sqlStmt.replace(backStatement,"");

            insertData = Long.valueOf(sqlStmt);
        }

        return insertData;
    }

}

