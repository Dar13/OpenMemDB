import java.util.*;
import java.io.*;
import java.util.concurrent.*;
import org.voltdb.client.*;
import org.voltdb.types.TimestampType;

public class thread implements Runnable
{
    private org.voltdb.client.Client myApp;
    private List<String> batch;
    private List<Integer> value;
    private List<String> date;
    private String flag;
    private CyclicBarrier gate;

    public thread(org.voltdb.client.Client myApp, List<String> batch, CyclicBarrier gate)
    {
        this.myApp = myApp;
        this.batch = batch;
        this.gate = gate;
    }

    public thread(org.voltdb.client.Client myApp, String flag, List<String> date, List<Integer> value)
    {
        this.myApp = myApp;
        this.date = date;
        this.value = value;
        this.flag = flag;
    }

    public thread(org.voltdb.client.Client myApp, String flag, List<Integer> value)
    {
        this.myApp = myApp;
        this.value = value;
        this.flag = flag;
    }

    //Special Run Method to run Stored Procedures
    public void runProc()
    {
        //Calls procedure using client and uses stored procedure to do so
        for(int i = 0; i < date.size(); i++)
        {
            try 
            {
                if(flag == "insert")
                {
                    TimestampType convert = new TimestampType(date.get(i));
                    myApp.callProcedure("insert", convert.getTime(), value.get(i));
                }
                else if(flag == "select")
                    myApp.callProcedure("find", value.get(i));
            } catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    //Special run Method to run AdHoc quieries
    @Override
    public void run()
    {
        String sqlStmt = arrayToString(batch);
        System.out.println(sqlStmt);
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
            myApp.callProcedure("@AdHoc", sqlStmt);

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
}

