import java.util.*;
import java.io.*;
import java.util.concurrent.*;
import org.voltdb.client.*;
import org.voltdb.types.TimestampType;

public class Worker implements Runnable
{
    private org.voltdb.client.Client myApp;
    private List<String> batch_list;
    private ArrayList<String> batch_array;
    private List<Long> value;
    private List<String> date;
    private String flag;
    private CyclicBarrier gate;

    //used for AdHoc procedure
    public Worker(org.voltdb.client.Client myApp, List<String> batch, CyclicBarrier gate)
    {
        this.myApp = myApp;
        this.batch_list = batch;
        this.gate = gate;
    }

    //used for mixed sql test
    public Worker(org.voltdb.client.Client myApp, ArrayList<String> batch, List<Long> value,CyclicBarrier gate )
    {
        this.myApp = myApp;
        this.batch_array = batch;
        this.gate = gate;
    }

    //used for insert test
    public Worker(org.voltdb.client.Client myApp, String flag, List<String> date, List<Long> value, CyclicBarrier gate )
    {
        this.myApp = myApp;
        this.date = date;
        this.value = value;
        this.flag = flag;
        this.gate = gate;
    }

    //used for select test
    public Worker(org.voltdb.client.Client myApp, String flag, List<Long> value, CyclicBarrier gate)
    {
        this.myApp = myApp;
        this.value = value;
        this.flag = flag;
        this.gate = gate;
    }

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

            // sql mixed batch must be parsed so each sql statement is executed using appropiate stored procedure
            for(int i = 0; i < batch_array.size(); i++)
            {
                String procedure = batch_array.get(i);
                long value = 0;
                if (procedure.contains("INSERT")) {
                    // Leaving date static as the epoch
                    value = grabInsertData(procedure);
                    response = myApp.callProcedure("insert", value, 2);
                    if(response.getStatus() != ClientResponse.SUCCESS)
                    {
                        throw new RuntimeException(response.getStatusString());
                    }
                } else if (procedure.contains("SELECT")) {
                    response = myApp.callProcedure("select", value);
                    if(response.getStatus() != ClientResponse.SUCCESS)
                    {
                        throw new RuntimeException(response.getStatusString());
                    }
                }
            }

        } catch (NoConnectionsException e)
        {
            e.printStackTrace();
        } catch(IOException e)
        {
            e.printStackTrace();
        } catch(ProcCallException e)
        {
            e.printStackTrace();
        }
    }


    /*
    //Special run Method to run sequential sql statements
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
            if (flag.equalsIgnoreCase("insert")) {
                for (int i = 0; i < date.size(); ++i) {
                    // Leaving date static as the epoch
                    response = myApp.callProcedure("insert", value.get(i), 2);
                    if(response.getStatus() != ClientResponse.SUCCESS)
                    {
                        throw new RuntimeException(response.getStatusString());
                    }
                }
            } else if (flag.equalsIgnoreCase("select")) {
                for (int i = 0; i < value.size(); ++i) {
                    response = myApp.callProcedure("find", value.get(i));
                    if(response.getStatus() != ClientResponse.SUCCESS)
                    {
                        throw new RuntimeException(response.getStatusString());
                    }
                } 
            }

        } catch (NoConnectionsException e)
        {
            e.printStackTrace();
        } catch(IOException e)
        {
            e.printStackTrace();
        } catch(ProcCallException e)
        {
            e.printStackTrace();
        }
    }
    */

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

            System.out.println(sqlStmt);
            insertData = Long.valueOf(sqlStmt);
        }

        return insertData;
    }

}

