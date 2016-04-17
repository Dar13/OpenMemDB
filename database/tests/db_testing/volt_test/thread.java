import java.util.*;

public class thread extends Thread
{
    private org.voltdb.client.Client myApp;
    private List<String> batch;
    private List<Integer> value;
    private List<String> date;
    private String flag;
    public thread(org.voltdb.client.Client myApp, List<String> batch)
    {
        this.myApp = myApp;
        this.batch = batch;
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


    public void runProc()
    {
        //Calls procedure using client and uses stored procedure to do so
        for(int i = 0; i < date.size(); i++)
        {
            try {
                if(flag == "insert")
                    myApp.callProcedure("insert", date.get(i), value.get(i));
                else if(flag == "select")
                    myApp.callProcedure("find", value.get(i));
            } catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    public void runStmt()
    {
        String sqlStmt = arrayToString(batch);
        try
        {
            myApp.callProcedure("@AdHoc", sqlStmt);
        } catch(Exception e)
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


    public void start(char flag)
    {
        switch(flag)
        {
            //run insert method
            case 'i':
                {
                    runProc();
                    break;
                }
            case 'r':
                {
                    runStmt();
                    break;
                }
        }
    }
}

